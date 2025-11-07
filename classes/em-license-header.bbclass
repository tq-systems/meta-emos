# SPDX-License-Identifier: MIT
#
# Ensure that config files shipped from meta-emos contain a consistent licence
# header referencing the TQSPSLA. The class injects the header into every file
# that gets copied verbatim from SRC_URI after all patches have been applied.
#
# Variables:
#   EM_LICENSE_HEADER_ENABLED    - Set to "1" to activate (defaults to 1 for
#                                  recipes whose LICENSE contains TQSPSLA).
#   EM_LICENSE_HEADER_FILES      - Whitespace separated list of files (relative
#                                  to ${WORKDIR}) that should receive the
#                                  header. Defaults to the non-patch file://
#                                  entries from SRC_URI.
#   EM_LICENSE_HEADER_PROJECT    - Project/package label shown in the header.
#                                  Defaults to ${PN}.
#   EM_LICENSE_HEADER_YEARS      - Year range used in the copyright text.
#                                  Defaults to ${TQ_LICENSE_HEADER_YEARS}.
#   EM_LICENSE_HEADER_AUTHOR     - Author line shown in the header. Defaults to
#                                  ${TQ_LICENSE_HEADER_AUTHOR}.
#   EM_LICENSE_HEADER_PREFIX     - Comment prefix to use. Defaults to "# ".

TQ_LICENSE_HEADER_YEARS ?= "2020-2025"
TQ_LICENSE_HEADER_AUTHOR ?= "TQ-Systems GmbH <license@tq-group.com>"

def em_license_header_default_enabled(d):
    license_str = (d.getVar('LICENSE') or '').upper()
    if 'TQSPSLA' in license_str:
        return '1'
    return ''

def em_license_header_detect_files(d):
    import os
    import bb.fetch2

    src_uri = (d.getVar('SRC_URI') or '').split()
    files = []
    for uri in src_uri:
        uri = uri.strip()
        if not uri:
            continue
        scheme, _, path, _, _, params = bb.fetch2.decodeurl(uri)
        if scheme != 'file':
            continue
        if params.get('patch') == '1' or params.get('apply') == 'no':
            continue
        if params.get('unpack') == '0':
            continue
        if path.endswith(('.patch', '.diff')):
            continue
        relpath = path.lstrip('/')
        subdir = params.get('subdir') or params.get('destsuffix')
        if subdir:
            relpath = os.path.join(subdir, relpath)
        if not relpath:
            continue
        if relpath not in files:
            files.append(relpath)
    return " ".join(files)

EM_LICENSE_HEADER_ENABLED ?= "${@em_license_header_default_enabled(d)}"
EM_LICENSE_HEADER_FILES ?= "${@em_license_header_detect_files(d)}"
EM_LICENSE_HEADER_PROJECT ?= "${PN}"
EM_LICENSE_HEADER_YEARS ?= "${TQ_LICENSE_HEADER_YEARS}"
EM_LICENSE_HEADER_AUTHOR ?= "${TQ_LICENSE_HEADER_AUTHOR}"
EM_LICENSE_HEADER_PREFIX ?= "# "
EM_LICENSE_HEADER_SPDX ?= "LicenseRef-TQSPSLA-1.0.3"

def em_license_header_render(d):
    prefix = d.getVar('EM_LICENSE_HEADER_PREFIX') or "# "
    blank_prefix = d.getVar('EM_LICENSE_HEADER_BLANK_PREFIX')
    if blank_prefix is None:
        blank_prefix = prefix.strip() or "#"

    project = d.getVar('EM_LICENSE_HEADER_PROJECT') or d.getVar('PN') or ""
    years = d.getVar('EM_LICENSE_HEADER_YEARS') or ""
    author = d.getVar('EM_LICENSE_HEADER_AUTHOR') or ""
    spdx = d.getVar('EM_LICENSE_HEADER_SPDX') or "LicenseRef-TQSPSLA-1.0.3"

    lines = [
        f"SPDX-License-Identifier: {spdx}",
        "",
        "More license information can be found in the root folder.",
        f"This file is part of {project}.",
        "",
        f"Copyright (c) {years} TQ-Systems GmbH <license@tq-group.com>, D-82229 Seefeld, Germany. All rights reserved.",
        f"Author: {author}",
        "",
    ]

    rendered = []
    for line in lines:
        if line:
            rendered.append(f"{prefix}{line}".rstrip())
        else:
            rendered.append(blank_prefix)
    # Add a trailing blank line to keep distance to the payload.
    rendered.append(blank_prefix)
    return "\n".join(rendered) + "\n"

def em_license_header_inject(path, header_text, d):
    import os

    if not os.path.isfile(path):
        return False

    with open(path, 'rb') as f:
        content = f.read()

    marker = b'SPDX-License-Identifier: ' + (d.getVar('EM_LICENSE_HEADER_SPDX') or "LicenseRef-TQSPSLA-1.0.3").encode('utf-8')
    if marker in content:
        return False

    if b'\0' in content:
        bb.note(f"em-license-header: skipping binary file {path}")
        return False

    shebang = b""
    body = content
    if content.startswith(b"#!"):
        newline = content.find(b"\n")
        if newline != -1:
            shebang = content[:newline + 1]
            body = content[newline + 1:]
        else:
            shebang = content
            body = b""

    header_bytes = header_text.encode('utf-8')
    with open(path, 'wb') as f:
        f.write(shebang)
        f.write(header_bytes)
        f.write(body)
    return True

python do_em_apply_license_header () {
    import os

    enabled = d.getVar('EM_LICENSE_HEADER_ENABLED') or "0"
    if not bb.utils.to_boolean(enabled, False):
        return

    workdir = d.getVar('WORKDIR')
    if not workdir:
        bb.warn("em-license-header: WORKDIR is undefined, cannot apply headers")
        return

    files = (d.getVar('EM_LICENSE_HEADER_FILES') or '').split()
    if not files:
        bb.debug(1, f"em-license-header: no files listed for {d.getVar('PN')}")
        return

    header_text = em_license_header_render(d)
    updated = 0
    for relpath in files:
        relpath = relpath.strip()
        if not relpath:
            continue
        target = os.path.join(workdir, relpath)
        if not os.path.exists(target):
            bb.debug(1, f"em-license-header: skip missing {relpath}")
            continue
        if em_license_header_inject(target, header_text, d):
            updated += 1

    if updated:
        bb.note(f"em-license-header: added header to {updated} file(s) for {d.getVar('PN')}")
}

addtask em_apply_license_header after do_patch before do_configure
do_em_apply_license_header[dirs] = "${WORKDIR}"

python () {
    enabled = d.getVar('EM_LICENSE_HEADER_ENABLED') or "0"
    if not bb.utils.to_boolean(enabled, False):
        return

    if d.getVarFlag('do_ar_patched', 'task'):
        pn = d.getVar('PN')
        d.appendVarFlag('do_ar_patched', 'depends', f" {pn}:do_em_apply_license_header")
}
