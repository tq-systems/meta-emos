EM_IMAGE_NAME = "sdk"

EM_BUNDLE_SPEC = "\
    sdk.yml \
    common.yml \
    apps-mandatory.yml \
    apps-development.yml \
    "

EM_BUNDLE_VERSION = "${DISTRO_VERSION}"
EM_BUNDLE_VERSION[vardepsexclude] = "DATETIME"

inherit em-bundle
