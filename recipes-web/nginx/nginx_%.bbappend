FILESEXTRAPATHS:prepend:emos := "${THISDIR}/files:"

SRC_URI:append:emos = " \
    file://nginx-emos.conf \
    file://nginx-prepare.service \
    file://no_cache.conf \
    file://static_cache.conf \
    file://ssl.conf \
    file://nginx-log-setup \
"

RDEPENDS:${PN}:append:emos = " emcfg"

do_install:append:emos() {
    install -d ${D}${sbindir}/
    install -m 0755 ${WORKDIR}/nginx-log-setup ${D}${sbindir}

    install -d ${D}${systemd_unitdir}/system/nginx.service.d/
    install -m 0644 ${WORKDIR}/nginx-prepare.service ${D}${systemd_unitdir}/system/
    install -m 0644 ${WORKDIR}/nginx-emos.conf ${D}${systemd_unitdir}/system/nginx.service.d/

    install -m 0644 ${WORKDIR}/no_cache.conf ${D}${sysconfdir}/nginx/
    install -m 0644 ${WORKDIR}/static_cache.conf ${D}${sysconfdir}/nginx/
    install -m 0644 ${WORKDIR}/ssl.conf ${D}${sysconfdir}/nginx/

    # Do not create /var/log/nginx, as it will fail due to our symlink setup
    sed -i '/\/log\//d' ${D}${sysconfdir}/tmpfiles.d/${BPN}.conf
}

FILES:${PN}:append:emos = "\
	${systemd_unitdir}/system/nginx-prepare.service \
	${systemd_unitdir}/system/nginx.service.d/ \
"
