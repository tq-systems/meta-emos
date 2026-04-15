# SPDX-License-Identifier: MIT
#
# Wrapper around oe-core's archiver class to keep local deviations in the
# meta-emos layer.

require ${@bb.utils.which(d.getVar('BBPATH'), 'classes/em-archiver-common.bbclass')}
require ${COREBASE}/meta/classes/archiver.bbclass

python do_ar_patched:prepend () {
    if d.getVarFlag('ARCHIVER_MODE', 'src') != 'patched':
        return

    srcdir = em_archiver_patched_srcdir(d)
    if em_archiver_is_pure_file_recipe(d):
        srcdir = em_archiver_seed_patched_srcdir(d)
        if srcdir:
            d.setVar('S', srcdir)
            d.setVar('EM_ARCHIVER_PATCHED_SRCDIR', srcdir)

    if not srcdir:
        return
    em_archiver_copy_license_files(d, srcdir)
}
