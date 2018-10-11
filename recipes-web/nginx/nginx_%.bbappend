FILESEXTRAPATHS_prepend_emos := "${THISDIR}/files:"

SRC_URI_append_emos = " \
    file://no_cache.conf \
    file://static_cache.conf \
"

do_install_append_emos() {
    install -m 0644 ${WORKDIR}/no_cache.conf ${D}${sysconfdir}/nginx/
    install -m 0644 ${WORKDIR}/static_cache.conf ${D}${sysconfdir}/nginx/
}
