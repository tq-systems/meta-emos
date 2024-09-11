EM_BOOTLOADER ?= "virtual/bootloader"
EM_CORE_IMAGE ?= "em-image-core"

EM_IMAGE_NAME ??= "invalid"
EM_BUNDLE_SPEC ?= "${EM_IMAGE_NAME}.yml"
EM_BUNDLE_SPEC_URI ?= "${@ ' '.join(['file://' + name for name in d.getVar('EM_BUNDLE_SPEC').split()])}"

EM_BUNDLE_MIGRATION_EXEC = "migration"
EM_BUNDLE_MIGRATION_URI = "file://${EM_BUNDLE_MIGRATION_EXEC}"
EM_BUNDLE_MIGRATION ?= "0"

EM_BUNDLE_VERSION ?= "${PV}"

TQ_DEVICE_TYPE ??= ""
TQ_DEVICE_SUBTYPE ??= ""
TQ_MANUFACTURER_ID ??= "0x5233"
TQ_PRODUCT_ID ??= ""

EM_BUNDLE_BOOTLOADER ??= ""

BUNDLE_BASENAME ??= "${EM_IMAGE_NAME}"
BUNDLE_NAME ??= "${BUNDLE_BASENAME}-${TQ_DEVICE_TYPE}-sw${EM_BUNDLE_VERSION}"
BUNDLE_NAME[vardepsexclude] = "DATETIME"
BUNDLE_LINK_NAME ??= "${BUNDLE_BASENAME}-${TQ_DEVICE_TYPE}"

# maximum bundle file size that is accepted by Updater app
BUNDLE_SIZE_MAX ?= "176160768"

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

SRC_URI = "${EM_BUNDLE_SPEC_URI} ${EM_BUNDLE_MIGRATION_URI}"

EMIT_DOWNLOAD_DIR = "${DL_DIR}/apps"
EMIT = "emit \
    --download-dir ${EMIT_DOWNLOAD_DIR} \
    --arch ${TUNE_PKGARCH} \
"

# python uses 4 spaces indentation
python emit_fetch_post() {
    download_dir = d.getVar('EMIT_DOWNLOAD_DIR')
    os.makedirs(download_dir, exist_ok=True)

    specs = d.getVar('EM_BUNDLE_SPEC_URI').split()
    fetcher = bb.fetch2.Fetch(specs, d)
    urls = fetcher.urls

    bundle_specs=''
    for url in urls:
        bundle_spec = fetcher.localpath(url)
        bundle_specs += ' --bundle-spec {}'.format(bundle_spec)

    lockfile = os.path.join(download_dir, 'emit.lock')
    lf = bb.utils.lockfile(lockfile)

    try:
        bb.fetch2.runfetchcmd('{} {} download'.format(
            d.getVar('EMIT'),
            bundle_specs,
        ), d)
    finally:
        bb.utils.unlockfile(lf)
}
do_fetch[depends] += "emit-native:do_populate_sysroot"
do_fetch[postfuncs] += "emit_fetch_post"

def opt_arg(d, name, varname):
    value = d.getVar(varname)
    if value:
        return f"--{name} '{value}'"
    else:
        return ''

def em_bundle_bootloaders(d):
    bootloaders = d.getVar('EM_BUNDLE_BOOTLOADER').split()
    deploydir = d.getVar('DEPLOY_DIR_IMAGE')
    ret = []

    for bootloader in bootloaders:
        file = os.path.join(deploydir, d.getVarFlag('EM_BUNDLE_BOOTLOADER', bootloader))
        ret.append(f"--bootloader '{bootloader}' '{file}'")

    return ' '.join(ret)

EMIT_ARGUMENTS = " \
    --build-dir '${B}/tmp' \
    --bundle-version '${EM_BUNDLE_VERSION}' \
    --machine '${MACHINE}' \
    --device-type '${TQ_DEVICE_TYPE}' \
    ${@ opt_arg(d, 'device-subtype', 'TQ_DEVICE_SUBTYPE') } \
    ${@ opt_arg(d, 'manufacturer-id', 'TQ_MANUFACTURER_ID') } \
    ${@ opt_arg(d, 'product-id', 'TQ_PRODUCT_ID') } \
    --rauc-key '${RAUC_KEY_FILE}' \
    --rauc-cert '${RAUC_CERT_FILE}' \
    --rauc-keyring '${RAUC_KEYRING_FILE}' \
    --core-image '${DEPLOY_DIR_IMAGE}/${EM_CORE_IMAGE}-${MACHINE}.tar' \
    ${@ em_bundle_bootloaders(d) } \
    --output-bundle '${B}/bundle.raucb' \
    --image-name '${EM_IMAGE_NAME}' \
"
EMIT_ARGUMENTS[vardeps] += "TQ_DEVICE_SUBTYPE TQ_MANUFACTURER_ID TQ_PRODUCT_ID EM_BUNDLE_BOOTLOADER"
EMIT_COMPRESSION_ARG = "${@ '--compression' if bb.utils.to_boolean(d.getVar('EM_BUNDLE_COMPRESSION')) else ''}"

do_bundle() {
    local specs=''
    for spec in ${EM_BUNDLE_SPEC}; do
        specs="${specs} --bundle-spec ${WORKDIR}/$spec"
    done

    local migration_args=''
    if [ ${EM_BUNDLE_MIGRATION} != "0" ]; then
        migration_args="--migration-exec ${WORKDIR}/${EM_BUNDLE_MIGRATION_EXEC}"
    fi

    ${EMIT} $specs ${EMIT_COMPRESSION_ARG} build ${EMIT_ARGUMENTS} $migration_args

    FILESIZE="$(stat -c%s "${B}/bundle.raucb")"
    if [ "${FILESIZE}" -gt "${BUNDLE_SIZE_MAX}" ]; then \
        echo >&2 "The bundle ${B}/bundle.raucb is too large: ${FILESIZE} (max. ${BUNDLE_SIZE_MAX})"
        exit 1;
    fi

}
do_bundle[depends] += "${EM_BOOTLOADER}:do_deploy ${EM_CORE_IMAGE}:do_image_complete"
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
