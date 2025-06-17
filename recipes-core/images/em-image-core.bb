inherit em-image

TOOLCHAIN_HOST_TASK:append = " \
	nativesdk-protobuf-dev \
	nativesdk-protobuf-compiler \
	nativesdk-protobuf-c-compiler \
	nativesdk-emit \
	nativesdk-clang \
	nativesdk-clang-tools \
	nativesdk-jansson-dev \
	nativesdk-libdeviceinfo-dev \
	nativesdk-libmodbus-dev \
	nativesdk-libsystemd-dev \
	nativesdk-mosquitto-dev \
	nativesdk-gcc \
	nativesdk-gcov \
	nativesdk-g++ \
	nativesdk-python3-gcovr \
"

TOOLCHAIN_TARGET_TASK:append = " \
	libdaemon-dev \
	mosquitto-dev \
	paho-mqtt-c-dev \
	protobuf-dev \
	protobuf-c-dev \
	jansson-dev \
	gupnp-dev \
	libsqlite3-dev \
	libmodbus-dev \
	libevdev-dev \
	libdeviceinfo-dev \
"
