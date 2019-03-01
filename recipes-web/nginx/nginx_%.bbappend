FILESEXTRAPATHS_prepend_emos := "${THISDIR}/files:"

SRC_URI_append_emos = " \
    file://nginx-keygen \
    file://nginx-keygen.conf \
    file://no_cache.conf \
    file://static_cache.conf \
    file://ssl.conf \
    file://nginx-log-setup \
    file://nginx-log-setup.conf \
"

RDEPENDS_${PN}_append_emos = " faketime openssl-bin"

do_install_append_emos() {
    install -d ${D}${sbindir}/
    install -m 0755 ${WORKDIR}/nginx-keygen ${D}${sbindir}
    install -m 0755 ${WORKDIR}/nginx-log-setup ${D}${sbindir}

    install -d ${D}${systemd_unitdir}/system/nginx.service.d/
    install -m 0644 ${WORKDIR}/nginx-keygen.conf ${D}${systemd_unitdir}/system/nginx.service.d/
    install -m 0644 ${WORKDIR}/nginx-log-setup.conf ${D}${systemd_unitdir}/system/nginx.service.d/

    install -m 0644 ${WORKDIR}/no_cache.conf ${D}${sysconfdir}/nginx/
    install -m 0644 ${WORKDIR}/static_cache.conf ${D}${sysconfdir}/nginx/
    install -m 0644 ${WORKDIR}/ssl.conf ${D}${sysconfdir}/nginx/
}

FILES_${PN}_append_emos = " ${systemd_unitdir}/system/nginx.service.d/"
