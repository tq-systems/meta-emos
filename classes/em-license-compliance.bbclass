# SPDX-License-Identifier: MIT
#
# Ensure that EM-owned source packages (identified by a TQSPSLA license)
# contain a LICENSE and a COPYING file in the root of the source archive
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

require ${@bb.utils.which(d.getVar('BBPATH'), 'classes/em-archiver-common.bbclass')}
inherit archiver

def em_license_compliance_default_enabled(d):
    license_str = (d.getVar('LICENSE') or '').upper()
    if 'TQSPSLA' in license_str:
        return '1'
    return ''

EM_LICENSE_COMPLIANCE_ENABLED ?= "${@em_license_compliance_default_enabled(d)}"

def em_license_get_copyright_years(srcdir, recipe_dir):
    """Return (first_year, last_year) from git log.

    Tries srcdir first (git-fetched sources with a .git directory), then
    falls back to recipe_dir (file://-based packages whose sources live
    inside the meta-emos layer).  Returns the current year for both values
    if git is unavailable or the repository has no commits.
    """
    import datetime
    import subprocess

    def years_from_git(path):
        try:
            result = subprocess.run(
                ['git', 'log', '--format=%ai', '--', '.'],
                cwd=path, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                timeout=30
            )
            if result.returncode != 0 or not result.stdout:
                return None
            years = sorted(set(
                line.decode()[:4]
                for line in result.stdout.splitlines()
                if len(line) >= 4
            ))
            if years:
                return years[0], years[-1]
        except Exception:
            pass
        return None

    current_year = str(datetime.date.today().year)
    for path in (srcdir, recipe_dir):
        if path and os.path.isdir(path):
            result = years_from_git(path)
            if result:
                return result
    return current_year, current_year


def em_license_create_compliance_files(d, srcdir):
    import os
    import shutil
    import bb.fetch2

    author = (d.getVar('EM_LICENSE_AUTHOR_PERSON') or '').strip()
    pn = d.getVar('PN') or 'unknown'
    recipe_dir = os.path.dirname(d.getVar('FILE') or '')

    # Derive the LICENSE file path from LIC_FILES_CHKSUM.
    lic_src = None
    for spec in (d.getVar('LIC_FILES_CHKSUM') or '').split():
        lic_uri = spec.split(';', 1)[0]
        scheme, _, path, _, _, _ = bb.fetch2.decodeurl(lic_uri)
        if scheme != 'file':
            continue
        real_path = os.path.realpath(os.path.join(recipe_dir, path))
        if os.path.exists(real_path):
            lic_src = real_path
            break

    if lic_src is None:
        bb.warn("em-license-compliance: no local license file found via LIC_FILES_CHKSUM for %s, LICENSE not written" % pn)
    else:
        dest_license = os.path.join(srcdir, 'LICENSE')
        if not os.path.exists(dest_license):
            shutil.copy2(lic_src, dest_license)
            bb.debug(1, "em-license-compliance: wrote %s" % dest_license)

    dest_copying = os.path.join(srcdir, 'COPYING')
    first_year, last_year = em_license_get_copyright_years(srcdir, recipe_dir)
    year_str = first_year if first_year == last_year else '%s-%s' % (first_year, last_year)
    # Build SPDX identifier from the LICENSE variable: find the TQSPSLA token
    # and prepend "LicenseRef-" (e.g. "TQSPSLA-1.0.3" -> "LicenseRef-TQSPSLA-1.0.3").
    license_var = d.getVar('LICENSE') or ''
    spdx_token = next(
        (tok.strip() for tok in license_var.replace('&', ' ').split() if 'TQSPSLA' in tok.upper()),
        license_var.strip()
    )
    spdx_id = 'LicenseRef-%s' % spdx_token
    copying_content = (
        'SPDX-License-Identifier: %s\n'
        '\n'
        'More license information can be found in the root folder.\n'
        'This file is part of %s.\n'
        '\n'
        'Copyright (c) %s TQ-Systems GmbH <license@tq-group.com>,'
        ' D-82229 Seefeld, Germany. All rights reserved.\n'
        'Author: %s\n'
    ) % (spdx_id, pn, year_str, author)
    with open(dest_copying, 'w') as f:
        f.write(copying_content)
    bb.debug(1, "em-license-compliance: wrote %s" % dest_copying)

python do_ar_patched:prepend () {
    import os
    import shutil

    # Use a positive condition instead of early returns: in Python prepend functions
    # bitbake concatenates prepend + original + append into a single function, so a
    # bare "return" would skip the original do_ar_patched (and its create_tarball call)
    # for all non-TQSPSLA packages.
    if bb.utils.to_boolean(d.getVar('EM_LICENSE_COMPLIANCE_ENABLED') or "0", False) \
            and d.getVarFlag('ARCHIVER_MODE', 'src') == 'patched':
        archiver_workdir = d.getVar('ARCHIVER_WORKDIR')
        workdir = d.getVar('WORKDIR')
        s = d.getVar('S')
        if archiver_workdir and workdir and s:
            # Save ARCHIVER_OUTDIR now, before do_ar_patched changes WORKDIR.
            d.setVar('EM_LC_SAVED_AR_OUTDIR', d.getVar('ARCHIVER_OUTDIR'))
            # Derive the directory the archiver will pack: S relative to WORKDIR,
            # transposed onto ARCHIVER_WORKDIR.  Handles S=${WORKDIR} (rel='.')
            # as well as S=${WORKDIR}/subdir.
            rel = os.path.relpath(s, workdir)
            srcdir = os.path.normpath(os.path.join(archiver_workdir, rel))

            # For pure file://-recipes seed srcdir from ARCHIVER_WORKDIR, which is
            # populated by do_unpack_and_patch and contains only source files.
            # Reading from the real WORKDIR would include build artifacts (package/,
            # spdx/, pseudo/, sstate-install-*, etc.) created by later tasks.
            src_uri = (d.getVar('SRC_URI') or '').split()
            if src_uri and all(bb.fetch2.decodeurl(u)[0].lower() == 'file' for u in src_uri):
                skip_names = {'.pc', 'patches', 'temp', 'archiver-sources', 'archiver-work'}
                skip_names.add(os.path.relpath(s, workdir).split(os.sep, 1)[0])
                build = d.getVar('B') or ''
                b_head = os.path.relpath(build, workdir).split(os.sep, 1)[0] if build else None
                if b_head and not b_head.startswith('..'):
                    skip_names.add(b_head)
                if os.path.isdir(archiver_workdir):
                    # Seed additional flat file:// entries into srcdir.  Do NOT
                    # delete srcdir first: when S=${WORKDIR}/subdir and a
                    # file://subdir;localdir=subdir entry exists in SRC_URI,
                    # do_unpack_and_patch already populated srcdir with the real
                    # source tree.  Removing it would destroy those sources because
                    # the subdir name is in skip_names (to prevent a circular copy).
                    # ARCHIVER_WORKDIR is always fresh (do_unpack_and_patch cleans
                    # it beforehand), so no stale build-artifact risk exists here.
                    bb.utils.mkdirhier(srcdir)
                    for entry in sorted(os.listdir(archiver_workdir)):
                        if entry in skip_names or entry.startswith('.'):
                            continue
                        src_path = os.path.join(archiver_workdir, entry)
                        dst_path = os.path.join(srcdir, entry)
                        if not os.path.exists(dst_path):
                            if os.path.isdir(src_path):
                                oe.path.copytree(src_path, dst_path)
                            else:
                                shutil.copy2(src_path, dst_path)
                    bb.debug(1, "em-license-compliance: seeded %s from ARCHIVER_WORKDIR" % srcdir)

            em_license_create_compliance_files(d, srcdir)
}

python do_ar_patched:append () {
    import os
    import subprocess
    import glob as glob_mod

    if bb.utils.to_boolean(d.getVar('EM_LICENSE_COMPLIANCE_ENABLED') or "0", False):
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
