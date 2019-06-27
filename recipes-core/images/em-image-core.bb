inherit em-image

DEFAULT_APPS = ""

TOOLCHAIN_HOST_TASK_append = " \
	nativesdk-protobuf-dev \
	nativesdk-protobuf-compiler \
	nativesdk-protobuf-c-compiler \
"

TOOLCHAIN_TARGET_TASK_append = " \
	libdaemon-dev \
	mosquitto-dev \
	protobuf-dev \
	protobuf-c-dev \
	jansson-dev \
	gupnp-dev \
	libsqlite3-dev \
	libmodbus-dev \
	libevdev-dev \
	libdeviceinfo-dev \
"
