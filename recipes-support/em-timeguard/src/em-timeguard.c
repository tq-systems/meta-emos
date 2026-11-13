#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/timex.h>
#include <sys/timerfd.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#include <systemd/sd-bus.h>
#include <systemd/sd-event.h>

#include "em-timeguard.h"

/*
 * Log to stderr. Under systemd stderr is the journal, and the "<N>" prefix sets the journal
 * priority. Messages below LOG_WARNING need verbose mode; guard may be NULL for early errors.
 */
static void __attribute__((format(printf, 3, 4)))
log_message(const struct timeguard *guard, int priority, const char *format, ...)
{
	char message[256];
	char prefix[8] = "";
	va_list args;

	if (priority > LOG_WARNING && (!guard || !guard->verbose))
		return;

	va_start(args, format);
	vsnprintf(message, sizeof(message), format, args);
	va_end(args);

	if (getenv("JOURNAL_STREAM"))
		snprintf(prefix, sizeof(prefix), "<%d>", priority);

	fprintf(stderr, "%s%s%s\n", prefix, prefix[0] ? "" : EM_TIMEGUARD_PREFIX, message);
}

/* Log an unrecoverable error and make the event loop exit, so that systemd restarts the service. */
static int fatal_error(struct timeguard *guard, int error, const char *what)
{
	log_message(guard, LOG_ERR, "%s failed: %s", what, strerror(-error));
	sd_event_exit(guard->event, error);

	return 0;
}

/* Convert a timespec to signed nanoseconds. */
static int64_t timespec_to_ns(const struct timespec *value)
{
	return (int64_t)value->tv_sec * INT64_C(1000000000) + value->tv_nsec;
}

/* Read realtime and monotonic clocks, plus TAI if tai_ns is given. */
static int get_clock_values(int64_t *realtime_ns, int64_t *monotonic_ns,
							int64_t *tai_ns)
{
	struct timespec realtime;
	struct timespec monotonic;
	struct timespec tai;

	if (clock_gettime(CLOCK_REALTIME, &realtime) < 0 ||
	    clock_gettime(CLOCK_MONOTONIC, &monotonic) < 0)
		return -errno;

	*realtime_ns = timespec_to_ns(&realtime);
	*monotonic_ns = timespec_to_ns(&monotonic);

	if (tai_ns) {
		if (clock_gettime(CLOCK_TAI, &tai) < 0)
			return -errno;

		*tai_ns = timespec_to_ns(&tai);
	}

	return 0;
}

/* Classify an event using the current kernel leap-second state. */
static const char *event_reason(const struct timex *timex, int state)
{
	if ((timex->status & (STA_INS | STA_DEL)) != 0 ||
	     state == TIME_OOP || state == TIME_WAIT)
		return EM_TIMEGUARD_STRING_LEAPSECOND;

	return EM_TIMEGUARD_STRING_CLOCKSTEP;
}

/* Remember the current realtime-monotonic offset as the reference for the next event. */
static int record_offset_baseline(struct timeguard *guard)
{
	int64_t realtime_ns;
	int64_t monotonic_ns;
	int ret;

	ret = get_clock_values(&realtime_ns, &monotonic_ns, NULL);
	if (ret < 0)
		return ret;

	guard->previous_offset_ns = realtime_ns - monotonic_ns;

	return 0;
}

/* Queue an event, dropping the oldest when the fixed queue is full. */
static void queue_event(struct timeguard *guard, const struct clock_event *event)
{
	size_t index;

	if (guard->queue_count == EVENT_QUEUE_SIZE) {
		guard->queue_head = (guard->queue_head + 1) % EVENT_QUEUE_SIZE;
		guard->queue_count--;
	}

	index = (guard->queue_head + guard->queue_count) % EVENT_QUEUE_SIZE;
	guard->queue[index] = *event;
	guard->queue_count++;
}

/* Build and emit a ClockEvent signal, queueing it if D-Bus is unavailable. */
static int emit_clock_event(struct timeguard *guard, const char *reason)
{
	struct timex timex = {0};
	struct clock_event event = {0};
	int64_t realtime_ns;
	int64_t monotonic_ns;
	int64_t tai_ns;
	int64_t offset_ns;
	int64_t delta_ns = 0;
	int state;
	int ret;

	ret = get_clock_values(&realtime_ns, &monotonic_ns, &tai_ns);
	if (ret < 0)
		return ret;

	/* Calculate clock delta as difference between this and last offset */
	offset_ns = realtime_ns - monotonic_ns;
	/* Only on --simulate the reason is 'unknown' and should report no change. */
	if (strcmp(reason, EM_TIMEGUARD_STRING_UNKNOWN) == 0)
		guard->previous_offset_ns = offset_ns;
	delta_ns = offset_ns - guard->previous_offset_ns;

	state = adjtimex(&timex);
	if (state < 0) {
		log_message(guard, LOG_ERR, "adjtimex failed: %s", strerror(errno));
		state = -1;
		timex.status = 0;
	}

	if (strcmp(reason, EM_TIMEGUARD_STRING_CLOCKSTEP) == 0)
		reason = event_reason(&timex, state);

	ret = snprintf(event.reason, sizeof(event.reason), "%s", reason);
	if (ret < 0 || (size_t)ret >= sizeof(event.reason))
		log_message(guard, LOG_WARNING, "reason '%s' does not fit into the event", reason);

	event.realtime_ns = realtime_ns;
	event.monotonic_ns = monotonic_ns;
	event.delta_ns = delta_ns;
	event.previous_offset_ns = guard->previous_offset_ns;
	event.current_offset_ns = offset_ns;
	event.state = state;
	event.timex_status = timex.status;
	event.tai_ns = tai_ns;

	/* update previous offset */
	guard->previous_offset_ns = offset_ns;

	ret = sd_bus_emit_signal(guard->bus, OBJECT_PATH, INTERFACE, "ClockEvent",
				 "sxxxxxxii",
				 event.reason,
				 event.realtime_ns,
				 event.monotonic_ns,
				 event.tai_ns,
				 event.delta_ns,
				 event.previous_offset_ns,
				 event.current_offset_ns,
				 event.state,
				 event.timex_status);
	if (ret < 0) {
		queue_event(guard, &event);
		return 0;
	}

	log_message(guard, LOG_INFO, "emitted %s", reason);

	return 0;
}

/* Arm an absolute realtime timer that is cancelled by a clock step. */
static int arm_timer(struct timeguard *guard)
{
	struct itimerspec timer = {0};
	struct timespec now;

	if (clock_gettime(CLOCK_REALTIME, &now) < 0)
		return -errno;

	timer.it_value.tv_sec = now.tv_sec + TIMER_INTERVAL_SECONDS;
	timer.it_value.tv_nsec = now.tv_nsec;

	if (timerfd_settime(guard->timer_fd, TFD_TIMER_ABSTIME | TFD_TIMER_CANCEL_ON_SET,
						&timer, NULL) < 0)
		return -errno;

	return 0;
}

/* Handle timer cancellation as a clock step, then arm the next timer. */
static int timer_callback(sd_event_source *source, int fd, uint32_t revents,
						  void *userdata)
{
	struct timeguard *guard = userdata;
	uint64_t expirations;
	ssize_t bytes;
	int ret;

	// Unused
	(void)source;
	(void)revents;

	bytes = read(fd, &expirations, sizeof(expirations));
	if (bytes < 0 && errno != ECANCELED)
		return fatal_error(guard, -errno, "reading the timer");

	if (bytes < 0 && errno == ECANCELED) {
		ret = emit_clock_event(guard, EM_TIMEGUARD_STRING_CLOCKSTEP);
		if (ret < 0)
			return fatal_error(guard, ret, "emitting the clock event");
	}

	ret = arm_timer(guard);
	if (ret < 0)
		return fatal_error(guard, ret, "re-arming the timer");

	return 0;
}

/* Stop the event loop after SIGTERM or SIGINT. */
static int signal_callback(sd_event_source *source, const struct signalfd_siginfo *si,
						   void *userdata)
{
	struct timeguard *guard = userdata;

	// Unused
	(void)source;
	(void)si;

	return sd_event_exit(guard->event, 0);
}

/* Print supported command-line options. */
static void print_usage(const char *program)
{
	printf("Usage: %s [OPTIONS]\n"
		"  -f, --foreground	stay in the foreground\n"
		"  -v, --verbose	log emitted events\n"
		"  -d, --debug		enable verbose diagnostics\n"
		"  --simulate		emit one diagnostic event and exit\n"
		"  --version		show version\n"
		"  -h, --help		show this help\n", program);
}

/* Parse CLI flags and return an error for unsupported options. */
static int parse_options(int argc, char **argv, bool *verbose, bool *simulate)
{
	static const struct option options[] = {
		{"foreground", no_argument, NULL, 'f'},
		{"verbose", no_argument, NULL, 'v'},
		{"debug", no_argument, NULL, 'd'},
		{"simulate", no_argument, NULL, 1},
		{"version", no_argument, NULL, 2},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};
	int option;

	while ((option = getopt_long(argc, argv, "fvdh", options, NULL)) != -1) {
		switch (option) {
		case 'f':
			break;
		case 'v':
		case 'd':
			*verbose = true;
			break;
		case 1:
			*simulate = true;
			break;
		case 2:
			puts(EM_TIMEGUARD_PREFIX "1.0");
			exit(EXIT_SUCCESS);
		case 'h':
			print_usage(argv[0]);
			exit(EXIT_SUCCESS);
		default:
			return -EINVAL;
		}
	}
	return 0;
}

/* Connect to the system bus, own the service name, and flush queued events. */
static int setup_bus(struct timeguard *guard)
{
	int ret;

	if (guard->bus) {
		sd_bus_detach_event(guard->bus);
		sd_bus_close_unref(guard->bus);
		guard->bus = NULL;
	}

	ret = sd_bus_open_system(&guard->bus);
	if (ret < 0)
		return ret;

	sd_bus_set_exit_on_disconnect(guard->bus, 0);
	ret = sd_bus_request_name(guard->bus, BUS_NAME, 0);
	if (ret < 0)
		return ret;

	ret = sd_bus_attach_event(guard->bus, guard->event, 0);
	if (ret < 0)
		return ret;

	while (guard->queue_count > 0) {
		struct clock_event *event = &guard->queue[guard->queue_head];

		ret = sd_bus_emit_signal(guard->bus, OBJECT_PATH, INTERFACE, "ClockEvent",
					 "sxxxxxxii",
					  event->reason,
					  event->realtime_ns,
					  event->monotonic_ns,
					  event->tai_ns,
					  event->delta_ns,
					  event->previous_offset_ns,
					  event->current_offset_ns,
					  event->state,
					  event->timex_status);
		if (ret < 0)
			return ret;

		guard->queue_head = (guard->queue_head + 1) % EVENT_QUEUE_SIZE;
		guard->queue_count--;
	}

	return 0;
}

/* Retry D-Bus setup periodically after disconnects or failed sends. */
static int reconnect_callback(sd_event_source *source, uint64_t usec, void *userdata)
{
	struct timeguard *guard = userdata;
	int ret;

	// Unused
	(void)usec;

	/* One-shot sources are disabled after dispatch and must be re-armed explicitly. */
	ret = sd_event_source_set_time_relative(source, RECONNECT_CHECK_INTERVAL_MICROSECONDS);
	if (ret < 0)
		return fatal_error(guard, ret, "re-arming the reconnect timer");

	ret = sd_event_source_set_enabled(source, SD_EVENT_ONESHOT);
	if (ret < 0)
		return fatal_error(guard, ret, "enabling the reconnect timer");

	if (guard->queue_count == 0 && guard->bus && sd_bus_is_ready(guard->bus))
		return 0;

	ret = setup_bus(guard);
	if (ret < 0)
		log_message(guard, LOG_INFO, "D-Bus reconnect failed: %s", strerror(-ret));

	return 0;
}

/* Initialize timer, signal, and reconnect sources on the sd-event loop. */
static int setup_event_loop(struct timeguard *guard)
{
	sigset_t signals;
	int ret;

	ret = sd_event_default(&guard->event);
	if (ret < 0)
		return ret;

	guard->timer_fd = timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC | TFD_NONBLOCK);
	if (guard->timer_fd < 0)
		return -errno;

	ret = arm_timer(guard);
	if (ret < 0)
		return ret;

	ret = sd_event_add_io(guard->event, &guard->timer_source, guard->timer_fd,
						  EPOLLIN, timer_callback, guard);
	if (ret < 0)
		return ret;

	sigemptyset(&signals);
	sigaddset(&signals, SIGTERM);
	sigaddset(&signals, SIGINT);
	ret = sigprocmask(SIG_BLOCK, &signals, NULL);
	if (ret < 0)
		return -errno;

	ret = sd_event_add_signal(guard->event, &guard->signal_source, SIGTERM,
				  signal_callback, guard);
	if (ret < 0)
		return ret;

	ret = sd_event_add_signal(guard->event, &guard->interrupt_source, SIGINT,
				  signal_callback, guard);
	if (ret < 0)
		return ret;

	return sd_event_add_time(guard->event, &guard->reconnect_source,
				 CLOCK_MONOTONIC, 0, RECONNECT_CHECK_INTERVAL_MICROSECONDS,
				 reconnect_callback, guard);
}

/* Release all resources;
 * sd_event_source_unref(), sd_event_unref() and sd_bus_close_unref() do nothing if argument is NULL,
 * so freeing all references at once is safe even after a partial setup and also when called more than once. */
static void guard_free(struct timeguard *guard)
{
	/* cleanup in reverse order of creation:
	 * set by setup_bus()
	 */
	guard->bus = sd_bus_close_unref(guard->bus);

	/* set by setup_event_loop() */
	guard->reconnect_source = sd_event_source_unref(guard->reconnect_source);
	guard->interrupt_source = sd_event_source_unref(guard->interrupt_source);
	guard->signal_source = sd_event_source_unref(guard->signal_source);
	guard->timer_source = sd_event_source_unref(guard->timer_source);

	if (guard->timer_fd >= 0) {
		if (close(guard->timer_fd) < 0)
			log_message(guard, LOG_ERR, "closing the timer failed: %s", strerror(errno));
		guard->timer_fd = -1;
	}

	guard->event = sd_event_unref(guard->event);
}

/* Parse options, initialize runtime resources, then run until shutdown. */
int main(int argc, char **argv)
{
	struct timeguard guard = {.timer_fd = -1};
	bool simulate = false;
	int ret;

	ret = parse_options(argc, argv, &guard.verbose, &simulate);
	if (ret < 0) {
		log_message(NULL, LOG_ERR, "invalid command line, see --help");
		return EXIT_FAILURE;
	}

	ret = record_offset_baseline(&guard);
	if (ret < 0) {
		log_message(&guard, LOG_ERR, "clock read failed: %s", strerror(-ret));
		goto out;
	}

	if (simulate) {
		guard.verbose = true;
		ret = setup_event_loop(&guard);
		if (ret >= 0)
			ret = setup_bus(&guard);
		if (ret >= 0)
			ret = emit_clock_event(&guard, EM_TIMEGUARD_STRING_UNKNOWN);
		if (ret < 0)
			log_message(&guard, LOG_ERR, "simulation failed: %s", strerror(-ret));
		goto out;
	}

	ret = setup_event_loop(&guard);
	if (ret < 0) {
		log_message(&guard, LOG_ERR, "event setup failed: %s", strerror(-ret));
		goto out;
	}

	ret = setup_bus(&guard);
	if (ret < 0) {
		log_message(&guard, LOG_ERR, "D-Bus setup failed: %s", strerror(-ret));
		goto out;
	}

	log_message(&guard, LOG_INFO, "started");
	ret = sd_event_loop(guard.event);
	if (ret < 0)
		log_message(&guard, LOG_ERR, "event loop failed: %s", strerror(-ret));

out:
	guard_free(&guard);

	return ret < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
