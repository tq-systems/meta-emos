# SPDX-License-Identifier: MIT
#
# Ensure that EM-owned source packages (identified by a TQSPSLA license)
# contain a LICENSE and an AUTHORS file in the root of the source archive
# produced by the Yocto archiver.  The archive filename is extended with the
# meta-emos git tag so that license-clearing tools can trace archives back to
# a specific layer release.
#
# The class is intended to be loaded globally via
#   INHERIT += "em-license-compliance"
# in the build configuration.  It activates automatically for every recipe
# whose LICENSE contains the string TQSPSLA.
#
# This class inherits the Yocto archiver class and is only effective when
# ARCHIVER_MODE[src] = "patched" is configured.
#
# Variables:
#   EM_LICENSE_COMPLIANCE_ENABLED  - Set to "1" to activate.  Defaults to "1"
#                                    for recipes whose LICENSE contains TQSPSLA,
#                                    empty otherwise.
#   EM_LICENSE_AUTHOR_PERSON       - Full name and e-mail of the natural person
#                                    who is the copyright holder of this package,
#                                    e.g. "Jane Doe <jane.doe@example.com>".
#                                    Required when the class is active and
#                                    ARCHIVER_MODE[src] = "patched"; the build
#                                    will fail if this variable is not set.

inherit archiver

def em_license_compliance_default_enabled(d):
    license_str = (d.getVar('LICENSE') or '').upper()
    if 'TQSPSLA' in license_str:
        return '1'
    return ''

EM_LICENSE_COMPLIANCE_ENABLED ?= "${@em_license_compliance_default_enabled(d)}"

def em_license_create_compliance_files(d, srcdir):
    import os
    import shutil
    import bb.fetch2

    author = (d.getVar('EM_LICENSE_AUTHOR_PERSON') or '').strip()

    # Derive the LICENSE file path from LIC_FILES_CHKSUM.
    lic_src = None
    for spec in (d.getVar('LIC_FILES_CHKSUM') or '').split():
        lic_uri = spec.split(';', 1)[0]
        scheme, _, path, _, _, _ = bb.fetch2.decodeurl(lic_uri)
        if scheme != 'file':
            continue
        file_dir = os.path.dirname(d.getVar('FILE') or '')
        real_path = os.path.realpath(os.path.join(file_dir, path))
        if os.path.exists(real_path):
            lic_src = real_path
            break

    if lic_src is None:
        bb.warn("em-license-compliance: no local license file found via LIC_FILES_CHKSUM for %s, LICENSE not written" % d.getVar('PN'))
    else:
        dest_license = os.path.join(srcdir, 'LICENSE')
        if not os.path.exists(dest_license):
            shutil.copy2(lic_src, dest_license)
            bb.debug(1, "em-license-compliance: wrote %s" % dest_license)

    dest_authors = os.path.join(srcdir, 'AUTHORS')
    if not os.path.exists(dest_authors):
        with open(dest_authors, 'w') as f:
            f.write(author + '\n')
        bb.debug(1, "em-license-compliance: wrote %s" % dest_authors)

python do_ar_patched:prepend () {
    import os
    import shutil

    if not bb.utils.to_boolean(d.getVar('EM_LICENSE_COMPLIANCE_ENABLED') or "0", False):
        return
    if d.getVarFlag('ARCHIVER_MODE', 'src') != 'patched':
        return
    archiver_workdir = d.getVar('ARCHIVER_WORKDIR')
    workdir = d.getVar('WORKDIR')
    s = d.getVar('S')
    if not archiver_workdir or not workdir or not s:
        return
    # Save ARCHIVER_OUTDIR now, before do_ar_patched changes WORKDIR.
    d.setVar('EM_LC_SAVED_AR_OUTDIR', d.getVar('ARCHIVER_OUTDIR'))
    # Derive the directory the archiver will pack: S relative to WORKDIR,
    # transposed onto ARCHIVER_WORKDIR.  Handles S=${WORKDIR} (rel='.')
    # as well as S=${WORKDIR}/subdir.
    rel = os.path.relpath(s, workdir)
    srcdir = os.path.normpath(os.path.join(archiver_workdir, rel))

    # For pure file://-recipes, S is typically an empty subdir of WORKDIR because
    # file:// sources are unpacked directly into WORKDIR, not into S.  Seed srcdir
    # with the actual source files from WORKDIR so the archive is not empty.
    src_uri = (d.getVar('SRC_URI') or '').split()
    if src_uri and all(bb.fetch2.decodeurl(u)[0].lower() == 'file' for u in src_uri):
        compliance_names = {'LICENSE', 'AUTHORS'}
        skip_names = {'.pc', 'patches', 'temp', 'archiver-sources', 'archiver-work'}
        skip_names.add(os.path.relpath(s, workdir).split(os.sep, 1)[0])
        build = d.getVar('B') or ''
        b_head = os.path.relpath(build, workdir).split(os.sep, 1)[0] if build else None
        if b_head and not b_head.startswith('..'):
            skip_names.add(b_head)
        has_real_content = any(
            e not in compliance_names and e not in skip_names and not e.startswith('.')
            for e in os.listdir(srcdir)
        ) if os.path.isdir(srcdir) else False
        if not has_real_content and os.path.isdir(archiver_workdir):
            bb.utils.mkdirhier(srcdir)
            for entry in sorted(os.listdir(workdir)):
                if entry in skip_names:
                    continue
                if entry.startswith('recipe-sysroot') or entry.startswith('deploy-'):
                    continue
                if entry in ('source-date-epoch', 'configure.sstate'):
                    continue
                src_path = os.path.join(workdir, entry)
                dst_path = os.path.join(srcdir, entry)
                if not os.path.exists(dst_path):
                    if os.path.isdir(src_path):
                        oe.path.copytree(src_path, dst_path)
                    else:
                        shutil.copy2(src_path, dst_path)
            bb.debug(1, "em-license-compliance: seeded %s with WORKDIR contents" % srcdir)

    em_license_create_compliance_files(d, srcdir)
}

python do_ar_patched:append () {
    import os
    import subprocess
    import glob as glob_mod

    if not bb.utils.to_boolean(d.getVar('EM_LICENSE_COMPLIANCE_ENABLED') or "0", False):
        return

    # Locate the meta-emos layer directory via BBPATH.
    layer_dir = None
    for p in (d.getVar('BBPATH') or '').split(':'):
        if os.path.exists(os.path.join(p, 'classes', 'em-license-compliance.bbclass')):
            layer_dir = p
            break
    if not layer_dir:
        bb.warn("em-license-compliance: could not locate meta-emos layer dir, archive not renamed")
        return

    try:
        result = subprocess.run(
            ['git', 'describe', '--tags', '--match', 'v*', '--always'],
            cwd=layer_dir, capture_output=True, text=True, timeout=10
        )
        version_tag = result.stdout.strip()
    except Exception as e:
        bb.warn("em-license-compliance: could not determine meta-emos version: %s" % e)
        return

    if not version_tag:
        bb.warn("em-license-compliance: git describe returned empty string, archive not renamed")
        return

    ar_outdir = d.getVar('EM_LC_SAVED_AR_OUTDIR')
    pf = d.getVar('PF')
    if not ar_outdir or not pf:
        return

    for old_path in glob_mod.glob(os.path.join(ar_outdir, pf + '-patched.tar.*')):
        suffix = old_path[len(os.path.join(ar_outdir, pf)):]
        new_path = os.path.join(ar_outdir, '%s-%s%s' % (pf, version_tag, suffix))
        os.rename(old_path, new_path)
        bb.note("em-license-compliance: renamed archive to %s" % os.path.basename(new_path))
}

python () {
    if not bb.utils.to_boolean(d.getVar('EM_LICENSE_COMPLIANCE_ENABLED') or "0", False):
        return
    if d.getVarFlag('ARCHIVER_MODE', 'src') != 'patched':
        return
    if not (d.getVar('EM_LICENSE_AUTHOR_PERSON') or '').strip():
        bb.fatal("%s: EM_LICENSE_AUTHOR_PERSON must be set for TQSPSLA-licensed packages" % (d.getVar('PN') or '?'))
}
