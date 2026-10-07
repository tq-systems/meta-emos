AM_CPPFLAGS = $(SYSTEMD_CFLAGS)
AM_CFLAGS = -std=c99 -Wall -Wextra -Wpedantic
em_timeguard_LDADD = $(SYSTEMD_LIBS)

sbin_PROGRAMS = em-timeguard
em_timeguard_SOURCES = src/em-timeguard.c src/em-timeguard.h

TESTS = tests/test-cli.sh
EXTRA_DIST = tests/test-cli.sh
