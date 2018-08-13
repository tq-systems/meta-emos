# Name convention: app recipe names must start with em-app-
def em_app_get_id(pn):
    import re
    return re.match(r'^em-app-(.*)$', pn).group(1)


APP_ID ?= "${@em_app_get_id('${PN}')}"

# This is the root directory for preinstalled apps
# They will be symlinked into /apps/installed automatically
# (where manually installed apps reside as well)
APP_INSTALL_ROOT = "/opt/apps/${APP_ID}"

APP_ROOT = "/apps/installed/${APP_ID}"
APP_DATA = "/data/apps/${APP_ID}"
FILES_${PN} = "${APP_INSTALL_ROOT}"
APP_LIC_DIR = "${APP_ROOT}/license"

DEPENDS_append = " jq-native"


em_app_compile_backend() {
    :
}
em_app_compile_frontend() {
    :
}
em_app_compile() {
    :
}

python do_compile() {
    bb.build.exec_func('em_app_compile_backend', d)
    bb.build.exec_func('em_app_compile_frontend', d)
    bb.build.exec_func('em_app_compile', d)
}
unset do_compile[export_func]


em_app_install_backend() {
    :
}
em_app_install_frontend() {
    :
}
em_app_install() {
    :
}

em_app_install_manifest() {
    jq -nc \
        --arg id '${APP_ID}' \
        --arg version '${PV}-${PR}' \
        --arg arch '${PACKAGE_ARCH}' \
        --arg name '${APP_NAME}' \
        --arg description '${DESCRIPTION}' \
        '{
            id: $id,
            version: $version,
            arch: $arch,
            name: $name,
            description: $description,
        }' \
        > ${D}${APP_ROOT}/manifest.json
}

# Moves the whole app into the dir for preinstalled apps
# (so it ends up at the correct location in the rootfs)
em_app_install_mv() {
    mkdir -p ${D}/opt/apps
    mv ${D}${APP_ROOT} ${D}/opt/apps/
    rmdir ${D}/apps/installed ${D}/apps
}

python do_install() {
    bb.build.exec_func('em_app_install_backend', d)
    bb.build.exec_func('em_app_install_frontend', d)
    bb.build.exec_func('em_app_install', d)
    bb.build.exec_func('em_app_install_manifest', d)
    bb.build.exec_func('em_app_install_mv', d)
}
unset do_install[export_func]


inherit package_empkg
