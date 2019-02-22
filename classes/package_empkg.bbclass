inherit package

PKGWRITEDIR_EMPKG = "${WORKDIR}/deploy-empkg"

python do_package_empkg () {
    import shutil
    import subprocess

    oldcwd = os.getcwd()

    workdir = d.getVar('WORKDIR')
    outdir = d.getVar('PKGWRITEDIR_EMPKG')
    if not workdir or not outdir:
        bb.error("Variables incorrectly set, unable to package")
        return

    app_name = d.getVar('APP_ID')
    if not app_name:
        bb.error("APP_ID not defined, unable to package")
        return

    app_ver = d.getVar('APP_VER')
    if not app_ver:
        bb.error("APP_VER not defined, unable to package")
        return

    app_install_root = d.getVar('APP_INSTALL_ROOT')
    if not app_install_root:
        bb.error("APP_INSTALL_ROOT not defined, unable to package")
        return

    pkgdest = d.getVar('PKGDEST')

    pn = d.getVar('PN')

    localdata = bb.data.createCopy(d)
    root = '%s/%s%s' % (pkgdest, pn, app_install_root)

    overrides = localdata.getVar('OVERRIDES', False)
    localdata.setVar('OVERRIDES', '%s:%s' % (overrides, pn))

    arch = localdata.getVar('PACKAGE_ARCH')
    pkgoutdir = '%s/%s' % (outdir, arch)
    bb.utils.mkdirhier(pkgoutdir)

    os.chdir(root)
    dlist = os.listdir(root)

    # Remove manifest.json from file list: it is packaged separately
    dlist.remove('manifest.json')

    datatar = '%s/data.tar.xz' % outdir
    ret = subprocess.call(['tar', '-cJf', datatar] + dlist)
    if ret != 0:
        bb.error("Creation of empkg %s data.tar.xz failed." % packagetar)

    manifest = '%s/manifest.json' % outdir
    shutil.copyfile('manifest.json', manifest)

    os.chdir(outdir)

    packagetar = '%s/%s' % (pkgoutdir, localdata.expand("${APP_ID}_${APP_VER}_${PACKAGE_ARCH}.empkg"))
    ret = subprocess.call(['tar', '-cf', packagetar, 'manifest.json', 'data.tar.xz'])
    if ret != 0:
        bb.error("Creation of empkg %s failed." % packagetar)

    os.unlink(datatar)
    os.unlink(manifest)
    os.chdir(oldcwd)
}


python do_package_write_empkg () {
    bb.build.exec_func("read_subpackage_metadata", d)
    bb.build.exec_func("do_package_empkg", d)
}
do_package_write_empkg[dirs] = "${PKGWRITEDIR_EMPKG}"
do_package_write_empkg[cleandirs] = "${PKGWRITEDIR_EMPKG}"
do_package_write_empkg[umask] = "022"
do_package_write_empkg[fakeroot] = "1"
do_package_write_empkg[depends] += "tar-native:do_populate_sysroot xz-native:do_populate_sysroot virtual/fakeroot-native:do_populate_sysroot"
do_package_write_empkg[depends] += "${@oe.utils.build_depends_string(d.getVar('PACKAGE_WRITE_DEPS'), 'do_populate_sysroot')}"

SSTATETASKS += "do_package_write_empkg"
do_package_write_empkg[sstate-inputdirs] = "${PKGWRITEDIR_EMPKG}"
do_package_write_empkg[sstate-outputdirs] = "${DEPLOY_DIR_EMPKG}"

addtask package_write_empkg before do_build after do_packagedata do_package
