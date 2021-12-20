## [ 3.2.0-rc1 ] - 2021-12-20
### Added
- emcfg: add udev rule for rs485 symlinks in dev for em4xx
- emcfg: add handling of hidden ip address

## [3.1.0-rc4] - 2021-10-27
### Changed
- teridiand-fw is downgraded again because the new measuring board is not yet produced

## [3.1.0-rc3] - 2021-10-26
### Changed
- Nothing has changed between v3.1.0-rc2 and v3.1.0-rc3, created for keeping tags of em-build in sync

## [3.1.0-rc2] - 2021-10-26
### Changed
- emit: support new measuring board by updating teridiand and teridiand-fw

## [3.1.0-rc1] - 2021-09-13
### Added
- empkg: Add evaluation of the disabled flag in app manifest
### Changed
- Add prefix 'sw' for bundle version
- emos-upgrade: extend error and status handling

## [3.0.0] - 2021-06-29
### Changed
- Update app definitions for sdk bundle

## [3.0.0-rc6] - 2021-06-15
- Move reset of teridian meter registers to teridiand app, because SPI in EM4xx hardware is initialized with systemd

## [3.0.0-rc5] - 2021-06-07
### Added
- Add paho-mqtt-c to core-image and toolchain
- linux-imx-em: add ftdi_sio USB UART driver
### Changed
- linux-imx-em: further adaptions for em4xx hardware and update to latest version
- u-boot-imx-em: further adaptions for em4xx hardware and update to latest version
- Remove upnp-app from base.yml and add it to sdk.yml

## [3.0.0-rc4] - 2021-02-22
### Changed
- emcfg: add default rate limit to journald-debug.conf

## [3.0.0-rc3] - 2021-02-15
### Added
- introduce TQ_DEVICE_TYPE
- empkg: implement em-app-time and em-app-no-time target in em-app-generator
- empkg: introduce em-app-time and em-app-no-time systemd target
- emcfg: add timesync service/timer
- emcfg: add em-timesync script
- emcfg: add journald-debug.conf
### Changed
- libdeviceinfo: update to v1.4.0
- emcfg: em-timesync restarts all apps of em-app-time.target
- emcfg: replace em-timesync.timer with em-timesync-timer.service
- emcfg: em-timesync: handle RTC read failures

## [3.0.0-rc2] - 2020-12-11
### Changed
- emit, em-bundle-sdk: update all app versions and checksums

## [3.0.0-rc1] - 2020-11-19
### Added
- em4xx: add machine config
- linux-imx-em: add recipe
- u-boot-imx-em: add recipe
- emit: implement bootloader update for EM4xx
- trusted-firmware-a: add configuration for EM4xx
- firmware-imx-8m: import from meta-freescale
- layer.conf: add BBFILES_DYNAMIC for meta-arm
- em-network-config: add switch-based network configuration
- teridiand-config: add udev rule for /dev/teridian symlink
### Changed
- linux-em: enable CONFIG_I2C_CHARDEV
- Avoid dependency of kernel modules on kernel image
- distro: modernize selection of systemd
- linux-imx-em: more config cleanup
- u-boot-imx-em: update to latest version
- linux-imx-em: update to latest version
- libdeviceinfo: update to v1.3.0
- linux-imx-em: make config more similar to EM3x0 kernel
- emit: determine U-Boot filename based on machine
- emit: move hardware-specific parts of upgrade hook to separate file
- Extend recipes with EM4xx support
- Move imx28-blupdate dependency from em-image to machine configurations
- emit: use u-boot.sb symlink without machine name
- emit: check /etc/os-machine
- em-image: store MACHINE in /etc/os-machine
- empkg: add arch check (and related error handling)
- emit: allow for apps to override the default arch
- emit: annotate sha256 keys with CPU arch
- emit: switch to yaml.safe_load() to avoid load() deprecation warning
- u-boot-fslc: rename to u-boot-fslc-em
- u-boot-fslc: merge common U-Boot definitions into recipe
- u-boot-fslc-fw-utils: replace with libubootenv-bin
- u-boot-fslc: update to latest version
- emcfg: ensure that `emcfg update` is run before network setup
- linux-em: enable KSZ8863 switch and bridge support
- em-power-handler: add package
- linux-em: update to latest version
- linux-em: enable regulator support
- linux-em: update to 5.4
- linux-em: disable CONFIG_VT and framebuffer support
- linux-em: enable UCS namespaces
- linux-em-4.19: replace config with actual defconfig
- haveged: update to 1.9.13
- recipes-tools/imx28-blupdate: fetch source from https server
- treewide: update for compatiblity with Yocto Dunfell
- treewide: update for compatiblity with Yocto Zeus
- busybox: use ${BPN} instead of ${PN} where appropriate
### Fixed
- emos-upgrade: fix LIC_FILES_CHKSUM
- u-boot-fslc-em: fix UBOOT_INITIAL_ENV
### Removed
- machine: clean up variable assignments
- distro: remove obsolete commented settings
- systemd: remove invalid udev rules
- Remove micrel-switch-tool and micrel-netdev-daemon
- nginx: remove /var/log/nginx tmpfiles.d snippet
- nginx: remove obsolete systemd unit fixup

## [2.1.0-rc8] - 2020-11-05
### Added
- emit: add error messages
- emit: add support for allow-downgrade flag
### Changed
- emit: make check_version() return explicit

## [2.1.0-rc7] - 2020-09-18
### Changed
- archiver workaround: move classes/archiver.bbclass to classes/em-archiver.bbclass
- emcfg: change path of network presets

## [2.1.0-rc6] - 2020-09-15
### Added
- add classes/archiver.bbclass, copied from openembedded-core/dunfell

## [2.1.0-rc5] - 2020-07-17
### Removed
- emcfg: remove em-store-teridian-registers

## [2.1.0-rc4] - 2020-06-29
### Added
- emcfg: handle network presets

## [2.1.0-rc2] - 2020-05-27
### Added
- empkg: add command list
### Changed
- empkg: log app install to journal
- emcfg: add service partition
- systemd: adjust journald.conf

## [2.1.0-rc1] - 2020-04-16
### Added
- add tool em-flash-read and eol-led
- e2fsprogs: enable periodic filesystem checks
- emcfg: add file system checks
### Changed
- emcfg: add EOL status handling
- empkg: handle the --set-eol option
- emcfg: Use noatime mount option for all rw mounted partitions
- distro: disable RPM support of libsolv
- empkg: Create langs.json when updating www directory
- emcfg: set timer to 1 week
### Fixed
- em-flash-read: fix type
- em-bundle: fix fetch with latest bitbake 1.42
### Removed
- emit: remove deprecated force-install-same option
- systemd: remove patch that was applied upstream

## [2.0.1] - 2020-07-02
### Changed
- update web-application with adjusted legal texts

## [2.0.0] - 2020-02-22
### Changed
- set DISTRO_VERSION

## [2.0.0-rc9] - 2020-01-31
### Changed
- update apps

## [2.0.0-rc8] - 2020-01-28
### Changed
- increase version (keep the tag in sync in superior project context)

## [2.0.0-rc7] - 2020-01-27
### Changed
- increase version (keep the tag in sync in superior project context)

## [2.0.0-rc6] - 2020-01-27
### Added
- emit: add /run/ignore-compatible flag
### Changed
- nginx: set proxy_request_buffering to off
- emos-upgrade: emos-upgrade-status: use return instead of exit
- emit: prevent firmware downgrades
- emit: split compatible string for more detailed error messages
- emit: replace built-in compatible check with custom hook
- emos-upgrade: clear inactive partition after reboot
- emit: install images regardless of stored checksums
### Fixed
- emos-upgrade: fix slot name in log message
### Removed
- emos-upgrade: remove unused get_slot_saved

## [2.0.0-rc5] - 2020-01-08
### Changed
- set app versions for sdk 2.0.0 release candidate

## [2.0.0-rc4] - 2020-01-06
### Changed
- linux-em: enable high-resolution timers
- linux-em: update to latest em-4.19.x

## [2.0.0-rc3] - 2020-01-06
### Changed
- bundles/sdk.yml: update apps, set compatible
- Adjust README
- emit: extract branch name variable from URL template
- emit: allow referencing arbitrary variables from URL templates

## [2.0.0-rc2] - 2019-12-18
### Changed
- devtools/emit: adjust default URL

## [2.0.0-rc1] - 2019-10-25
### Added
- empkg: introduce em-app-before systemd target
- empkg: reimplement systemd generator in C
- emcfg: init: add helper for boot flag checks
- Introduce new systemd targets em-app.target and emos.target
- emcfg: add backup restore functionality
- emos-upgrade: add --no-reboot option
- em-image-core: add emit to toolchain
- em-bundle-sdk: add new SDK bundle recipe
- em-bundle.bbclass: add new bundle class based on emit
- emit: add "Energy Manager Image Tool"
- em-image-core: add minimal image
- libdeviceinfo: add recipe
- nginx: add WebSocket support
- Add security flags to yocto
- emos-upgrade-status: add sanity checks to emos-upgrade-status
- emos-upgrade: add reset command to status script
- emos-update: add locking to status script to prevent concurrent changes
- emcfg: add em-config-reset
- em-image-sdk: add Go toolchain to toolchain package
- em-image-sdk: add protobuf development files to toolchain package
- layer.conf: add LAYERSERIES_COMPAT
- emos-upgrade: add a sleep after first rootfs update
- emos-upgrade: add emos-upgrade-status
- emcfg: implement factory reset
- empkg: add support for the autostart manifest flag
- emcfg: add support for advanced NTP configuration
- nginx: add nginx-log-setup script
- nginx: add nginx-keygen script
- faketime: add package
- Add product-info JSON file
### Changed
- systemd: increase RuntimeWatchdogSec to 29s
- emcfg: init: do not allow the root password to be restored from backups
- emcfg: init: format /data when necessary after an interrupted backup restore
- empkg: allow starting and stopping all non-core apps using em-app.target
- libdeviceinfo: update to v1.2.1
- u-boot: update to latest git version
- linux-em: update to latest em-4.19.x version (4.19.65)
- emcfg: init: reorder factory reset to make it more robust
- emcfg: init: continue booting after factory reset
- nginx: move keygen and other preparation to a separate service
- emit: rauc: store statusfile on /update
- empkg: do not fail because of service start failures
- empkg: update D-Bus policies earlier, clean up obsolete files
- Move openssl-em.cnf and keygen script to emcfg
- emos.conf: do not set SDKPATH
- Adjust for conversion of various components to apps
- empkg: do not automatically start apps on install that set autostart to false
- micrel-netdev-daemon: replace gpio-wait with gpiomon from libgpiod-tools
- em-image.bbclass: include avahi-daemon and libavahi-client in all images
- avahi: do not advertise SSH and SFTP services
- jq: downgrade to 1.5
- systemd: update name of systemd-conf recipe
- Set layer compatibility to warrior
- emos-upgrade: reset upgrade status on errors
- emos-upgrade: simplify read logic in status script
- distro/emos.conf: use libmodbus 3.1.x
- emcfg: enable SSH server when enable_ssh=1 is set in u-boot environment
- systemd: set distro-specific append
- systemd-conf: set watchdog timeout
- em-image-*: explicitly include various library packages
- machine/em3x0: update serial console definition
- em-image.bbclass: always include libraries used by apps
- nginx: add openssl-bin to RDEPENDS
- linux-em: update config, add CGROUP_BPF
- Add volatile binds required by new systemd
- em-image.bbclass: allow SSH root login with Yocto Thud
- classes/em-product-info.bbclass: add manufacturer URL
- classes/em-product-info: add README for license files
- emcfg: em-init: reset Teridian registers on factory reset
- empkg: update to version 2
- emcfg: update to version 2
- dropbear: disable dropbear for systemd
- emos-upgrade-finalize: adapt for status
- emos-upgrade-finalize: adapt ConditionPathExists for status
- emos-upgrade: set update status
- emos-upgrade-finalize: do not use tmpfs for update
- micrel-netdev-daemon: adjust for new gpio-wait version
- em-image.bbclass: include timezone data in images
- openssl/nginx: use own config for ssl certificate generation
- em-product-info.bbclass: set use-bundle-signing-time RAUC option
- em-bundle.bbclass: set options required with latest meta-rauc
- Move RAUC configuration to product info package, add image variant to compatible string
- Move RAUC key defintions to distro config
- em-image-bundle: simplify specification of generated recipe names
- Update all TQSSLA license references to v1.0.2
- linux-em: use github source
- u-boot-flsc: use github source
- empkg: allow apps to provide dbus config snippets
- nodejs: update from 8.4 to 8.14
- emcfg: enable link-local (IPv4) address assignment
- emcfg: set net.ipv4.ip_nonlocal_bind to 1
- emcfg: do not change default hostname to lowercase
- gupnp: backport patch for Linux context manager
- micrel-netdev-daemon: set eth0 down when no switch port has a link
- linux-em: add CONFIG_IP_MULTICAST
- em-product-info: add software version to product-info.json
- mosquitto: listen on 127.0.0.1 only
- nginx.conf: log into systemd journal, suppress error 404
- nginx: only allow TLS 1.2
- nginx: enable HTTPS
- mosquitto: do not wait for network uplink on start
- mosquitto: update to 1.5.4
- uthash: update to 2.0.2
- imx28-blupdate: build with large file support
- emcfg: implement handler functions for reset button actions
- linux-em: add gpio-keys and evdev drivers for reset button
- micrel-netdev-led-daemon: use LICENSE for license filename
- classes/em-image: install u-boot to rootfs for license info
- distro/emos.conf: remove vardepsexclude for DATE
- em-init: mount filesystems as nodev,nosuid
- nginx: reduce cache time to 1 week, do not cache certain files
- empkg: include cache-busting query strings in apps.json
- linux-em: update to Linux 4.14.x
- netdev-led: adjust to Linux 4.14.x
- micrel-netdev-led-daemon: adjust to Linux 4.14.x
- emcfg: derive default hostname from product name and serial number
- emcfg: em-init: mount /run to make /var/lock available
- linux-em: enable kernel memory protection
- linux-em: unify kernel configuration
- busybox: add sha support for rootpw
- empkg: respect essential flag
- empkg: add periods to error messages
- classes/em-app: allow flagging an app as essential
- empkg: print builtin flag in package status
- empkg: change "preinstalled" term to "builtin"
- empkg: add status command without argument
- nginx: add generic proxy location for backends using UNIX sockets
- nginx: make sure that static locations always precede regular expressions
- nginx: set utf-8 charset
### Fixed
- nginx: fix path of kill command
- em-bundle.bbclass: fix permissions of deployed bundles
- empkg: fix creation of app cfglog directories
- empkg: fix creation of dbus directory for initial setup or factory reset
- Revert "empkg: fix creation of dbus directory for initial setup or factory reset"
- empkg: fix creation of dbus directory for initial setup or factory reset
- systemd: backport patch to fix systemd error message with newer kernels
- nginx: fix order of ExecStartPre commands
- Backport Yocto SDK build bugfix
- empkg: fix syntax errors in em-app-generator
- faketime: fix build with Yocto Thud
- u-boot-fslc:linux-em: fix fetch warnings
- u-boot-fslc:linux-em: fix typo in SRC_URI
- systemd: backport patch for timesyncd log spam
- nginx: fix permissions of nginx-keygen.conf
- empkg: error handling fixes
### Removed
- empkg: remove obsolete copy_defaults mechanism
- Remove old bundle build code, clean up em-image.bbclass
- Remove obsolete app packaging classes
- nativesdk-packagegroup-sdk-host: remove qemu from toolchain package
- em-image-core: remove Golang packages from toolchain package
- custom-licenses: remove BSD-1-Clause
- mosquitto: remove outdated recipe
- target-sdk-provides-dummy.bbappend: remove
- emos-upgrade: remove redundant log messages, avoid unnecessary `rauc status` calls
- emos-upgrade: remove useless states
- emos-upgrade: clean up finalize script, always reset status on exit
- systemd: remove obsolete path extension
- Remove recipes for packages that are new enough in Yocto Thud
- systemd: remove obsolete patches
- classes/em-image: remove nvram-wrapper
- local.conf.sample: remove unused GIT_SERVER variable
- paho-mqtt-c: remove obsolete recipe
- empkg: remove --no-lock option
- Remove /etc overlay mount
- local.conf.sample: disable unneeded debug tweaks
- nginx: remove obsolete locations

## [1.2.1] - 2019-02-18
### Changed
- Update RAUC compatible string to match master SDK images
- distro/emos.conf: use tar for IMAGE_FSTYPES only
### Fixed
- dropbear: fix ConditionPathExists for host key generation
- rauc: update development key pair for rauc bundle signature

## [1.2.0] - 2018-08-17
### Added
- empkg: add update_licenses
- Add classes for simple definition of additional images and bundles
- busybox: add unxz
### Changed
- empkg: create /cfglog/apps before switching to restrictive umask
- emos-upgrade: use update partition
- base-files: set new partitions cfglog and update
- classes/em-image: set IMAGE_ROOTFS_SIZE, set DROPBEAR_RSAKEY_DIR
- nginx.conf: make em-apps license infos accessible
- emos-image: copy licenses to image
- base-files: emos-init: create overlay directories if nonexistent
- base-files: emos-init: sync preinstalled apps into appfs on boot
- empkg: add tool for app management
- base-files: mount appfs
- emos-image: tell Yocto that our rootfs image is read-only
- systemd: move remaining machines.target and var-lib-machines.mount symlinks to systemd-container
- treewide: adjust to GIT_SERVER definition without git:// scheme
- paho-mqtt-c: fix SRCREV for v1.2.1

## [1.1.0] - 2018-07-04
### Added
- emos-image.bb: add status-led
- passwd/group: add avahi and avahi-autoipd entries
- busybox: add devmem
- Add overload protection backend to nginx config.
- em300:emos-image: add netdev-led
- emos-image: add haveged entropy gathering daemon
### Changed
- linux-em: enable CONFIG_NET_NS
- opkg: disable opkg-configure service by default
- linux-em: update (fix em300 mmc1 pinmux)
- linux-em: update to fix eMMC DSR setting
- em3x0:u-boot: set revision to 33860d3
- em3x0:u-boot: set revision to 1a0a82b
- linux-em: re-enable DS1307/1337 RTC support
- rauc, em-bundle: set "compatible" string to ${MACHINE}
- emos-image: set rootfs size for em300
- linux-em: em300: sync config with em310
- linux-em: em310: enable initramfs support
- busybox: enable telnetd
- u-boot: set common source for em300 and em310
- paho-mqtt-c: update from 1.2.0 to 1.2.1
- imx28-blupdate: update to v1.3
- linux-em: update to latest version
- linux-em: update, ignore RTC oscillator stop flag on em310
### Removed
- protobuf: remove obsolete .bbappend
- busybox: remove obsolete "command" config snippet
- intltool: remove obsolete .bbappend

## [1.0.2] - 2018-06-01
### Added
- add status-led service
### Changed
- network: set ClientIdentifier=mac for DHCP
- nginx: explicitly use IPv4 for reverse proxy

## [1.0.1] - 2018-05-29
