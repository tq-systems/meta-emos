# em-timeguard

`em-timeguard` monitors discontinuous changes of `CLOCK_REALTIME` and reports
clock events as a signal on the system D-Bus.

## Runtime interface

| Item | Value |
| --- | --- |
| Bus | System bus |
| Bus name | `com.tq_group.tq_em.timeguard1` |
| Object path | `/com/tq_group/tq_em/timeguard1` |
| Interface | `com.tq_group.tq_em.timeguard1.TimeGuard` |
| Signal | `ClockEvent` |
| Signature | `sxxxxxxii` |

The interface description is installed as
`/usr/share/dbus-1/interfaces/com.tq_group.tq_em.timeguard1.xml`.

The signal is emitted after a `timerfd` using `CLOCK_REALTIME` is cancelled by
`TFD_TIMER_CANCEL_ON_SET`. This means that the signal reports an observed
discontinuous realtime-clock change. It does not identify the cause as NTP,
manual configuration, RTC restoration, or another source.

## ClockEvent message

The signature `sxxxxxxii` contains the following values, in this order:

| # | Name | D-Bus type | Meaning |
| ---: | --- | --- | --- |
| 1 | `reason` | `s` | `clock-step`, `leap-second`, or `unknown` (only `--simulate`) |
| 2 | `realtime_ns` | `x` | `CLOCK_REALTIME` in nanoseconds since the Unix epoch |
| 3 | `monotonic_ns` | `x` | `CLOCK_MONOTONIC` in nanoseconds |
| 4 | `tai_ns` | `x` | `CLOCK_TAI` in nanoseconds |
| 5 | `delta_offset_ns` | `x` | Current offset minus the previous observed offset; positive for a forward step, negative for a backward step |
| 6 | `previous_offset_ns` | `x` | Reference offset: the offset at the previous event, or at daemon start |
| 7 | `current_offset_ns` | `x` | Current offset, `realtime_ns - monotonic_ns` |
| 8 | `adjtimex_state` | `i` | Return state from read-only `adjtimex()`; `-1` on error |
| 9 | `timex_status` | `i` | `struct timex.status`, or `0` when `adjtimex()` failed |

All `x` values are signed 64-bit integers. Nanosecond arithmetic can
overflow if clocks are converted outside the supported `int64_t` range; the
receiver should validate values before further calculations.

### Event semantics

- The daemon records the current offset as reference at startup; every
  event becomes the new reference. No event is sent at startup, so the
  first event after a restart already has a real delta.
  Only the diagnostic `--simulate` event reports reason `unknown`, delta
  `0` and a `previous_offset_ns` equal to `current_offset_ns`.
  Clock slewing does not change the offset, because
  `CLOCK_REALTIME` and `CLOCK_MONOTONIC` are slewed alike.
- `clock-step` means a discontinuous realtime change was observed.
- `leap-second` is selected when `STA_INS`, `STA_DEL`, `TIME_OOP`, or
  `TIME_WAIT` is reported by `adjtimex()`. This is a kernel-state
  classification, not proof that a leap second has already occurred.
- `ntp-step` is never emitted. NTP provenance is not determined.
- A D-Bus disconnect temporarily queues up to 64 events. On overflow, the
  oldest event is discarded. Queued events are sent in order after reconnect.

## Observing messages

List the service name:

```sh
busctl list | grep com.tq_group.tq_em.timeguard1
```

Monitor all messages from the service:

```sh
busctl monitor com.tq_group.tq_em.timeguard1
```

Monitor only `ClockEvent` signals:

```sh
dbus-monitor \
  "type='signal',sender='com.tq_group.tq_em.timeguard1',\
interface='com.tq_group.tq_em.timeguard1.TimeGuard',member='ClockEvent'"
```

`busctl monitor` is the reliable runtime check. The installed XML file is a
contract/documentation file; the current daemon emits signals but does not
register a full `sd_bus_vtable`. Therefore `busctl tree` or `busctl introspect`
may not discover the object even while signals are received successfully.

## Receiver example using libsystemd

A C receiver can subscribe with a D-Bus match and read the message values in
the same order as the signature:

```c
static int clock_event_callback(sd_bus_message *message,
                                void *userdata, sd_bus_error *error)
{
    const char *reason;
    int64_t realtime_ns;
    int64_t monotonic_ns;
    int64_t tai_ns;
    int64_t delta_offset_ns;
    int64_t previous_offset_ns;
    int64_t current_offset_ns;
    int32_t adjtimex_state;
    int32_t timex_status;
    int ret;

    (void)userdata;
    (void)error;

    ret = sd_bus_message_read(message, "sxxxxxxii",
                               &reason, &realtime_ns, &monotonic_ns,
                               &tai_ns, &delta_offset_ns,
                               &previous_offset_ns, &current_offset_ns,
                               &adjtimex_state, &timex_status);
    if (ret < 0)
        return ret;

    /* Handle the event here. */
    return 0;
}
```

Register the match on a system-bus connection:

```c
sd_bus_add_match(bus, &slot,                                     \
    "type='signal',sender='com.tq_group.tq_em.timeguard1',"       \
    "path='/com/tq_group/tq_em/timeguard1',"                      \
    "interface='com.tq_group.tq_em.timeguard1.TimeGuard',"        \
    "member='ClockEvent'",                                        \
    clock_event_callback, NULL);
```

The receiver must also attach the bus to its event loop or process the bus
with `sd_bus_process()` and `sd_bus_wait()`.

## Receiver example using Go

The following example uses
[`github.com/godbus/dbus/v5`](https://pkg.go.dev/github.com/godbus/dbus/v5)
to receive and decode `ClockEvent` signals on the system bus:

```go
package main

import (
  "fmt"
  "log"

  "github.com/godbus/dbus/v5"
)

const (
  serviceName   = "com.tq_group.tq_em.timeguard1"
  objectPath    = dbus.ObjectPath("/com/tq_group/tq_em/timeguard1")
  interfaceName = "com.tq_group.tq_em.timeguard1.TimeGuard"
)

func main() {
  conn, err := dbus.SystemBus()
  if err != nil {
    log.Fatal(err)
  }
  defer conn.Close()

  matchOptions := []dbus.MatchOption{
    dbus.WithMatchSender(serviceName),
    dbus.WithMatchObjectPath(objectPath),
    dbus.WithMatchInterface(interfaceName),
    dbus.WithMatchMember("ClockEvent"),
  }
  if err := conn.AddMatchSignal(matchOptions...); err != nil {
    log.Fatal(err)
  }
  defer conn.RemoveMatchSignal(matchOptions...)

  signals := make(chan *dbus.Signal, 16)
  conn.Signal(signals)
  defer conn.RemoveSignal(signals)

  for signal := range signals {
    var (
      reason string
      realtimeNS, monotonicNS, taiNS int64
      deltaOffsetNS int64
      previousOffsetNS, currentOffsetNS int64
      adjtimexState, timexStatus int32
    )

    if err := dbus.Store(signal.Body,
      &reason, &realtimeNS, &monotonicNS, &taiNS,
      &deltaOffsetNS,
      &previousOffsetNS, &currentOffsetNS,
      &adjtimexState, &timexStatus); err != nil {
      log.Printf("invalid ClockEvent signal: %v", err)
      continue
    }

    fmt.Printf("reason=%s realtime_ns=%d "+
      "monotonic_ns=%d current_offset_ns=%d delta_offset_ns=%d\n",
      reason, realtimeNS, monotonicNS, currentOffsetNS,
      deltaOffsetNS)
  }
}
```

Create a small Go module and run the receiver with:

```sh
go mod init example.com/timeguard-receiver
go get github.com/godbus/dbus/v5
go run .
```

The receiving process needs access to the system bus but does not need to own
`com.tq_group.tq_em.timeguard1`. The `Store` call validates the number and order of
signal values against the expected D-Bus signature. A malformed or unrelated
message is logged and ignored.

## Service and permissions

The producer runs as the dedicated `em-timeguard` user. A receiver only needs
to read signals from the system bus; it does not need to own the producer's
bus name. The D-Bus policy is installed at
`/etc/dbus-1/system.d/com.tq_group.tq_em.timeguard1.conf`.

## Testing

Start a monitor on the target:

```sh
busctl monitor com.tq_group.tq_em.timeguard1
```

Then perform a controlled clock step only on an isolated test target or VM:

```sh
date -s '@1700000000'
```

Expected values include a changed `realtime_ns`, an almost continuously
increasing `monotonic_ns`, and a changed `current_offset_ns`. A receiver must not
interpret `clock-step` as `ntp-step` without an independent NTP provenance
source.
