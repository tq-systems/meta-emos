EM_IMAGE_NAME = "sdk"

EM_BUNDLE_SPEC:append:imx8mn-egw = " egw.yml"

EM_BUNDLE_VERSION = "${DISTRO_VERSION}"
EM_BUNDLE_VERSION[vardepsexclude] = "DATETIME"

inherit em-bundle
