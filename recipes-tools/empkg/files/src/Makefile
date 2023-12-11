CFLAGS += -std=c99 -Wall -Wextra -O2
CFLAGS += -D_GNU_SOURCE
LDLIBS += -ljansson -larchive -lcrypto -lacl

empkg : empkg.o \
	empkg_appdb.o \
	empkg_dbus.o \
	empkg_fops.o \
	empkg_fops_cp.o \
	empkg_fops_mkdir.o \
	empkg_helper.o \
	empkg_info.o \
	empkg_json.o \
	empkg_list.o \
	empkg_lock.o \
	empkg_log.o \
	empkg_register.o \
	empkg_status.o \
	empkg_tar.o \
	empkg_users.o \
	em-app-generator.o

all: empkg
clean :
	rm -f empkg
	rm -f *.o
