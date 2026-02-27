# SPDX-License-Identifier: MIT
#
# Wrapper around oe-core's archiver class to keep local deviations in the
# meta-emos layer.

require ${COREBASE}/meta/classes/archiver.bbclass

def em_archiver_srcdir_has_real_content(srcdir):
    import os

    if not srcdir or not os.path.isdir(srcdir):
        return False

    ignored = {'.', '..', '.pc', 'patches', 'temp'}
    for entry in os.listdir(srcdir):
        if entry in ignored:
            continue
        return True

    return False

def em_archiver_seed_srcdir_from_workdir(d):
    import os
    import shutil

    src_uri = (d.getVar('SRC_URI') or '').split()
    if not src_uri:
        return

    if not all(bb.fetch2.decodeurl(url)[0].lower() == 'file' for url in src_uri):
        return

    archiver_workdir = d.getVar('ARCHIVER_WORKDIR')
    if not archiver_workdir or not os.path.isdir(archiver_workdir):
        return

    srcdir = os.path.join(archiver_workdir, d.getVar('BP'))
    if em_archiver_srcdir_has_real_content(srcdir):
        return

    workdir = d.getVar('WORKDIR')

    bb.debug(1, 'archiver: %s has no dedicated source tree, copying unpacked WORKDIR contents into ${S}' % d.getVar('PN'))

    bb.utils.mkdirhier(srcdir)

    skip_names = {'.pc', 'patches', 'temp', 'archiver-sources', 'archiver-work'}

    def relpath_head(path):
        if not path:
            return None
        rel = os.path.relpath(path, workdir)
        if rel.startswith('..'):
            return None
        return rel.split(os.sep, 1)[0]

    src_head = relpath_head(srcdir)
    if src_head:
        skip_names.add(src_head)

    build_head = relpath_head(d.getVar('B'))
    if build_head:
        skip_names.add(build_head)

    def should_skip(entry):
        if entry in skip_names:
            return True
        if entry.startswith('recipe-sysroot'):
            return True
        if entry.startswith('deploy-'):
            return True
        return False

    copied = False
    for entry in sorted(os.listdir(workdir)):
        if should_skip(entry):
            continue
        src_path = os.path.join(workdir, entry)
        dst_path = os.path.join(srcdir, entry)
        if os.path.isdir(src_path):
            oe.path.copytree(src_path, dst_path)
        else:
            bb.utils.mkdirhier(os.path.dirname(dst_path))
            shutil.copy2(src_path, dst_path)
        copied = True

    if not copied:
        bb.debug(1, 'archiver: no local files found to include for %s' % d.getVar('PN'))

def em_archiver_copy_license_files(d, srcdir):
    import os
    import shutil

    licenses = (d.getVar('LIC_FILES_CHKSUM') or '').split()
    if not licenses:
        return

    archiver_workdir = d.getVar('ARCHIVER_WORKDIR')
    archiver_root = os.path.realpath(archiver_workdir) if archiver_workdir else None

    copied = False
    seen = set()
    for spec in licenses:
        lic_uri = spec.split(';', 1)[0]
        scheme, _, path, _, _, _ = bb.fetch2.decodeurl(lic_uri)
        if scheme != 'file':
            continue

        real_path = os.path.realpath(path)
        if archiver_root and real_path.startswith(archiver_root):
            continue

        if not os.path.exists(real_path):
            bb.warn('archiver: referenced license file "%s" for %s not found and cannot be copied into the archive' % (path, d.getVar('PN')))
            continue

        bb.utils.mkdirhier(srcdir)
        dest_dir = os.path.join(srcdir, 'licenses')
        bb.utils.mkdirhier(dest_dir)

        dest = os.path.join(dest_dir, os.path.basename(real_path))
        if dest in seen:
            continue

        shutil.copy2(real_path, dest)
        seen.add(dest)
        copied = True

    if not copied:
        bb.debug(1, 'archiver: no external license references copied for %s' % d.getVar('PN'))

python do_ar_patched:prepend () {
    if d.getVarFlag('ARCHIVER_MODE', 'src') != 'patched':
        return

    em_archiver_seed_srcdir_from_workdir(d)
    archiver_workdir = d.getVar('ARCHIVER_WORKDIR')
    if not archiver_workdir:
        return
    srcdir = os.path.join(archiver_workdir, d.getVar('BP'))
    em_archiver_copy_license_files(d, srcdir)
}
