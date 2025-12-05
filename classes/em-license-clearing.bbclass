# SPDX-License-Identifier: MIT
#
# Helper class to generate a combined licence output that aggregates the
# package manifests and licence files from multiple build directories. Enable
# the helper by adding `INHERIT += "em-license-clearing"` to the image recipe
# (or globally via local.conf) and configure which TMPDIRs should be merged.
#
# Configuration variables:
#   EM_LICENSE_LIST_TAG
#       Name of the combined licence artefact (required). The output is written
#       to ${TMPDIR}/deploy/licenses/${EM_LICENSE_LIST_TAG}.
#   EM_LICENSE_LIST_EXTRA_BUILDS
#       Whitespace separated list of LABEL=TMPDIR assignments referencing other
#       build directories that should be merged in addition to the current one.
#       LABELs do not need to be unique; duplicates receive an automatically
#       generated suffix to keep their outputs separate.
#   EM_LICENSE_LIST_MERGE_SCRIPT
#       Path to the helper script that performs the actual merge. The default
#       points to meta-emos/scripts/merge-license-lists.py.

def em_license_default_extra_builds(d):
    machines = (d.getVar("MACHINES") or "").split()
    current_machine = d.getVar("MACHINE")
    tmpdir = d.getVar("TMPDIR") or d.expand("${TOPDIR}/tmp")
    if not tmpdir:
        return ""

    entries = []
    seen = set()
    for machine in machines:
        machine = machine.strip()
        if not machine or machine == current_machine:
            continue
        if machine in seen:
            continue
        entries.append(f"{machine}={tmpdir}")
        seen.add(machine)

    return " ".join(entries)

EM_LICENSE_LIST_TAG ??= "combined-licenses"
EM_LICENSE_LIST_EXTRA_BUILDS ?= "${@em_license_default_extra_builds(d)}"
EM_LICENSE_LIST_MERGE_SCRIPT ?= "${@ bb.utils.which(d.getVar('BBPATH'), 'scripts/merge-license-lists.py') or '' }"
EM_LICENSE_LIST_OUTPUT_DIR ?= "${TMPDIR}/deploy/licenses/${EM_LICENSE_LIST_TAG}"

do_em_license_merge[vardepsexclude] += "EM_LICENSE_LIST_OUTPUT_DIR"
do_em_license_merge[vardeps] += "EM_LICENSE_LIST_EXTRA_BUILDS EM_LICENSE_LIST_TAG EM_LICENSE_LIST_MERGE_SCRIPT"


python do_em_license_merge() {
    import os
    import subprocess

    tag = d.getVar("EM_LICENSE_LIST_TAG")
    if not tag:
        bb.debug(1, "em-license-clearing: EM_LICENSE_LIST_TAG not set, skipping merge task")
        return

    script = d.getVar("EM_LICENSE_LIST_MERGE_SCRIPT")
    if not script or not os.path.exists(script):
        bb.fatal(f"em-license-clearing: merge script not found at {script}")

    output_dir = d.getVar("EM_LICENSE_LIST_OUTPUT_DIR")
    if not output_dir:
        bb.fatal("em-license-clearing: EM_LICENSE_LIST_OUTPUT_DIR resolved to an empty path")

    builds = []
    current_machine = d.getVar("MACHINE")
    current_tmpdir = d.getVar("TMPDIR")
    if current_machine and current_tmpdir:
        builds.append(f"{current_machine}={current_tmpdir}")

    extras = d.getVar("EM_LICENSE_LIST_EXTRA_BUILDS")
    if extras:
        for raw_entry in extras.split():
            entry = raw_entry.strip()
            if not entry:
                continue
            if "=" not in entry:
                bb.fatal(f"em-license-clearing: invalid EM_LICENSE_LIST_EXTRA_BUILDS entry '{entry}'")
            name, path = entry.split("=", 1)
            expanded_path = bb.data.expand(path.strip(), d)
            builds.append(f"{name.strip()}={expanded_path}")

    if not builds:
        bb.warn("em-license-clearing: nothing to merge, no build directories configured")
        return

    python_exe = d.getVar("PYTHON3")
    if not python_exe:
        python_exe = bb.utils.which(d.getVar("PATH"), "python3")
    if not python_exe:
        bb.fatal("em-license-clearing: unable to locate python3 executable (set PYTHON3 or ensure python3 is in PATH)")

    cmd = [python_exe, script, "--output-dir", output_dir, "--tag", tag]
    for build in builds:
        cmd.extend(["--input", build])

    bb.note(f"em-license-clearing: generating combined licence output at {output_dir}")

    subprocess.check_call(cmd, cwd=d.getVar("TMPDIR"))
}

addtask em_license_merge after do_rootfs before do_build
