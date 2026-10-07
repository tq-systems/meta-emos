#ifndef EM_TIMEGUARD_H
#define EM_TIMEGUARD_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <systemd/sd-bus.h>
#include <systemd/sd-event.h>

#define BUS_NAME "com.tq_group.tq_em.timeguard1"
#define EM_TIMEGUARD_PREFIX "em-timeguard: "
#define EM_TIMEGUARD_STRING_CLOCKSTEP "clock-step"
#define EM_TIMEGUARD_STRING_LEAPSECOND "leap-second"
#define EM_TIMEGUARD_STRING_UNKNOWN "unknown"
#define EVENT_QUEUE_SIZE 64
#define INTERFACE "com.tq_group.tq_em.timeguard1.TimeGuard"
#define OBJECT_PATH "/com/tq_group/tq_em/timeguard1"
#define RECONNECT_CHECK_INTERVAL_MICROSECONDS (1 * 1000 * 1000)
#define TIMER_INTERVAL_SECONDS (24 * 60 * 60)


struct clock_event {
	char reason[32];
	int64_t realtime_ns;
	int64_t monotonic_ns;
	int64_t tai_ns;
	int64_t delta_ns;
	int64_t previous_offset_ns;
	int64_t current_offset_ns;
	int state;
	int timex_status;
};

struct timeguard {
	sd_event *event;
	sd_bus *bus;
	sd_event_source *timer_source;
	sd_event_source *signal_source;
	sd_event_source *interrupt_source;
	sd_event_source *reconnect_source;
	int timer_fd;
	bool verbose;
	int64_t previous_offset_ns;
	struct clock_event queue[EVENT_QUEUE_SIZE];
	size_t queue_head;
	size_t queue_count;
};

#endif
