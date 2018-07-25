FILESEXTRAPATHS_prepend_emos := "${THISDIR}/files:"

SRC_URI_append_emos = " \
    file://static_cache.conf \
"

do_install_append_emos() {
    install -m 0644 ${WORKDIR}/static_cache.conf ${D}${sysconfdir}/nginx/static_cache.conf
}
