inherit em-image-bundle

IMAGE_INSTALL += " \
	em-app-devel \
	em-app-sensors \
"

TOOLCHAIN_HOST_TASK_append = " \
	nativesdk-protobuf-dev \
	nativesdk-protobuf-compiler \
	nativesdk-protobuf-c-compiler \
"
