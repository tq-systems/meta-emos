EM_IMAGE_NAME = "emcore-qa"
EM_BUNDLE_COMPRESSION = "1"

EM_BUNDLE_SPEC = "\
    emcore-qa.yml \
    common.yml \
    apps-mandatory.yml \
    apps-development.yml \
    apps-integrable.yml \
    apps-core-qa.yml \
    "

EM_BUNDLE_VERSION = "${DISTRO_VERSION}"
EM_BUNDLE_VERSION[vardepsexclude] = "DATETIME"

inherit em-bundle
