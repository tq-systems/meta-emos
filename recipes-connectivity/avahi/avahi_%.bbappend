do_install:append () {
    sed -i '/\[Service\]/a \BindReadOnlyPaths=-/cfglog/apps/device-settings/avahi/services:/etc/avahi/services:norbind' \
	    "${D}/${systemd_system_unitdir}/avahi-daemon.service"
}
