EM_CORE_IMAGE ?= "em-image-core"

EM_IMAGE_NAME ??= "invalid"
EM_BUNDLE_SPEC ?= "${EM_IMAGE_NAME}.yml"
EM_BUNDLE_SPEC_URI ?= "${@ ' '.join(['file://' + name for name in d.getVar('EM_BUNDLE_SPEC').split()])}"

EM_BUNDLE_VERSION ?= "${PV}"

TQ_DEVICE_TYPE ??= ""

BUNDLE_BASENAME ??= "${EM_IMAGE_NAME}"
BUNDLE_NAME ??= "${BUNDLE_BASENAME}-${TQ_DEVICE_TYPE}-sw${EM_BUNDLE_VERSION}"
BUNDLE_NAME[vardepsexclude] = "DATETIME"
BUNDLE_LINK_NAME ??= "${BUNDLE_BASENAME}-${TQ_DEVICE_TYPE}"


do_patch[noexec] = "1"
do_configure[noexec] = "1"
do_compile[noexec] = "1"
do_install[noexec] = "1"
do_package[noexec] = "1"
do_populate_lic[noexec] = "1"
do_package_qa[noexec] = "1"
do_packagedata[noexec] = "1"
do_package_write_ipk[noexec] = "1"
do_package_write_deb[noexec] = "1"
do_package_write_rpm[noexec] = "1"

S = "${WORKDIR}"
B = "${WORKDIR}/build"

inherit python3native

DEPENDS = "emit-native ${EM_CORE_IMAGE}"

LICENSE = "MIT"

PACKAGE_ARCH = "${MACHINE_ARCH}"

SRC_URI = "${EM_BUNDLE_SPEC_URI}"

EMIT_DOWNLOAD_DIR = "${DL_DIR}/apps"
EMIT = "emit \
    --download-dir ${EMIT_DOWNLOAD_DIR} \
    --arch ${TUNE_PKGARCH} \
"


python emit_fetch_post() {
    download_dir = d.getVar('EMIT_DOWNLOAD_DIR')
    os.makedirs(download_dir, exist_ok=True)

    for uri in d.getVar('EM_BUNDLE_SPEC_URI').split():
        fetcher = bb.fetch2.Fetch([uri], d)
        url = fetcher.urls[0]
        bundle_spec = fetcher.localpath(url)

        lockfile = os.path.join(download_dir, 'emit.lock')
        lf = bb.utils.lockfile(lockfile)

        try:
            bb.fetch2.runfetchcmd('{} --bundle-spec {} download'.format(
                d.getVar('EMIT'),
                bundle_spec,
            ), d)
        finally:
            bb.utils.unlockfile(lf)
}
do_fetch[depends] += "emit-native:do_populate_sysroot"
do_fetch[postfuncs] += "emit_fetch_post"

do_bundle() {
    local specs=''
    for spec in ${EM_BUNDLE_SPEC}; do
        specs="${specs} --bundle-spec ${WORKDIR}/$spec"
    done
    ${EMIT} \
        $specs \
        build \
        --build-dir '${B}/tmp' \
        --bundle-version '${EM_BUNDLE_VERSION}' \
        --machine '${MACHINE}' \
        --device-type '${TQ_DEVICE_TYPE}' \
        --rauc-key '${RAUC_KEY_FILE}' \
        --rauc-cert '${RAUC_CERT_FILE}' \
        --rauc-keyring '${RAUC_KEYRING_FILE}' \
        --core-image '${DEPLOY_DIR_IMAGE}/${EM_CORE_IMAGE}-${MACHINE}.tar' \
        --output-bundle '${B}/bundle.raucb' \
        --image-name '${EM_IMAGE_NAME}'
}
do_bundle[depends] += "${EM_CORE_IMAGE}:do_image_complete"
do_bundle[dirs] = "${B}"

addtask bundle after do_configure before do_build

inherit deploy

do_deploy() {
	install -d ${DEPLOYDIR}
	install -m 0644 ${B}/bundle.raucb ${DEPLOYDIR}/${BUNDLE_NAME}.raucb
	ln -sf ${BUNDLE_NAME}.raucb ${DEPLOYDIR}/${BUNDLE_LINK_NAME}.raucb
}
do_deploy[cleandirs] = "${DEPLOYDIR}"

addtask deploy after do_bundle before do_build
