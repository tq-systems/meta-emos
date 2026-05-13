# SPDX-License-Identifier: MIT

# Detect recipes whose sources come exclusively from file:// entries.
def em_archiver_is_pure_file_recipe(d):
    src_uri = (d.getVar('SRC_URI') or '').split()
    if not src_uri:
        return False

    return all(bb.fetch2.decodeurl(url)[0].lower() == 'file' for url in src_uri)

# Return the first path component of path relative to root when it stays inside root.
def em_archiver_relpath_head(path, root):
    import os

    if not path or not root:
        return None

    rel = os.path.relpath(path, root)
    if rel.startswith('..') or rel == '.':
        return None

    return rel.split(os.sep, 1)[0]

# Compute the directory that should be archived for the patched-source tarball.
def em_archiver_patched_srcdir(d):
    import os

    archiver_workdir = d.getVar('ARCHIVER_WORKDIR')
    if not archiver_workdir:
        return None

    if em_archiver_is_pure_file_recipe(d):
        return os.path.join(archiver_workdir, d.getVar('BP'))

    workdir = d.getVar('WORKDIR')
    srcdir = d.getVar('S')
    if not workdir or not srcdir:
        return None

    rel = os.path.relpath(srcdir, workdir)
    return os.path.normpath(os.path.join(archiver_workdir, rel))

# Copy a file or directory tree into the staged archive source tree without overwriting existing content.
def em_archiver_copy_entry(src_path, dst_path):
    import os
    import shutil

    if os.path.isdir(src_path):
        if not os.path.exists(dst_path):
            oe.path.copytree(src_path, dst_path)
            return

        for entry in sorted(os.listdir(src_path)):
            em_archiver_copy_entry(os.path.join(src_path, entry), os.path.join(dst_path, entry))
        return

    bb.utils.mkdirhier(os.path.dirname(dst_path))
    if not os.path.exists(dst_path):
        shutil.copy2(src_path, dst_path)

# Check whether a staged source directory already contains non-archiver content.
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

# Build the normalized patched-source staging tree for pure file:// recipes.
def em_archiver_seed_patched_srcdir(d):
    import os

    if not em_archiver_is_pure_file_recipe(d):
        return em_archiver_patched_srcdir(d)

    archiver_workdir = d.getVar('ARCHIVER_WORKDIR')
    if not archiver_workdir or not os.path.isdir(archiver_workdir):
        return None

    srcdir = em_archiver_patched_srcdir(d)
    if not srcdir:
        return None

    workdir = d.getVar('WORKDIR')
    original_srcdir = d.getVar('S')
    translated_srcdir = None
    if workdir and original_srcdir:
        rel = os.path.relpath(original_srcdir, workdir)
        translated_srcdir = os.path.normpath(os.path.join(archiver_workdir, rel))

    bb.debug(1, 'archiver: %s normalizing patched archive root to ${BP}' % d.getVar('PN'))

    bb.utils.mkdirhier(srcdir)

    skip_names = {'.pc', 'patches', 'temp', 'archiver-sources', 'archiver-work'}

    src_head = em_archiver_relpath_head(srcdir, archiver_workdir)
    if src_head:
        skip_names.add(src_head)

    translated_head = em_archiver_relpath_head(translated_srcdir, archiver_workdir)
    if translated_head:
        skip_names.add(translated_head)

    build_head = em_archiver_relpath_head(d.getVar('B'), workdir)
    if build_head:
        skip_names.add(build_head)

    translated_real = os.path.realpath(translated_srcdir) if translated_srcdir and os.path.isdir(translated_srcdir) else None
    archiver_real = os.path.realpath(archiver_workdir)
    srcdir_real = os.path.realpath(srcdir)
    copied = False
    if translated_real and translated_real not in (archiver_real, srcdir_real):
        for entry in sorted(os.listdir(translated_srcdir)):
            em_archiver_copy_entry(os.path.join(translated_srcdir, entry), os.path.join(srcdir, entry))
        copied = True

    def should_skip(entry):
        if entry in skip_names:
            return True
        if entry.startswith('recipe-sysroot'):
            return True
        if entry.startswith('deploy-'):
            return True
        return False

    for entry in sorted(os.listdir(archiver_workdir)):
        if entry.startswith('.'):
            continue
        if should_skip(entry):
            continue
        src_path = os.path.join(archiver_workdir, entry)
        dst_path = os.path.join(srcdir, entry)
        em_archiver_copy_entry(src_path, dst_path)
        copied = True

    if not copied:
        bb.debug(1, 'archiver: no local files found to include for %s' % d.getVar('PN'))

    return srcdir

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
