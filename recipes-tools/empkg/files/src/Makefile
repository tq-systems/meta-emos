CFLAGS += -std=c99 -Wall -Wextra -O2
CFLAGS += -D_GNU_SOURCE
LDLIBS += -ljansson -lcrypto

empkg : empkg.o \
	empkg_appdb.o \
	empkg_json.o \
	empkg_list.o \
	empkg_log.o \
	empkg_status.o \
	em-app-generator.o

all: empkg
clean :
	rm -f empkg
	rm -f *.o
