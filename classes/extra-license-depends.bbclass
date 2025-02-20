# SPDX-License-Identifier: MIT
#
# Copyright (C) 2024 TQ-Systems GmbH <oss@ew.tq-group.com>, D-82229 Seefeld, Germany.
# Author: Matthias Schiffer
#
# Import additional license files from dependencies, where they would be missing
# otherwise (because the dependencies are linked statically or included
# verbatim, rather than becoming runtime dependencies).
# This does not handle the -lic packages created by LICENSE_CREATE_PACKAGE,
# only the files deployed to LICENSE_DIRECTORY separately.

# To make use of EXTRA_LICENSE_MCDEPENDS, the variables LICENSE_DIRECTORY and
# PREFERRED_PROVIDER definitions for all referenced virtual/* entries must be
# made available through MULTICONFIG_VARS.

EXTRA_LICENSE_DEPENDS ?= ""
EXTRA_LICENSE_MCDEPENDS ?= ""

def copy_license_depend(srcdir, destdir, dep):
    licenses = os.listdir(srcdir)
    for lic in licenses:
        # Generic license files are handled through the LICENSE variable
        if lic.startswith('generic_'):
            continue

        src_license = os.path.join(srcdir, lic)
        dest_license = os.path.join(destdir, f"{dep}_{lic}")
        oe.path.copyhardlink(src_license, dest_license)

def copy_extra_license_depends(d):
    destdir = os.path.join(d.getVar('LICSSTATEDIR'), d.getVar('LICENSE_DEPLOY_PATHCOMPONENT'), d.getVar('PN'))

    for dep in d.getVar('EXTRA_LICENSE_DEPENDS').split():
        # Limitation: virtual dependencies must have a matching PREFERRED_PROVIDER
        # definition, even when the provider is unambiguous
        if dep.startswith('virtual/'):
            dep = d.getVar('PREFERRED_PROVIDER_' + dep)

        licdir = d.getVar('LICENSE_DIRECTORY')
        srcdir = os.path.join(licdir, d.getVar('LICENSE_DEPLOY_PATHCOMPONENT'), dep)

        copy_license_depend(srcdir, destdir, dep)

    for mcdep in d.getVar('EXTRA_LICENSE_MCDEPENDS').split():
        mc, fromconfig, toconfig, dep = mcdep.split(':')
        if mc != 'mc':
            bb.fatal('Invalid entry {mcdep} in EXTRA_LICENSE_MCDEPS')
        if fromconfig == '':
            fromconfig = 'default'
        if fromconfig != d.getVar('BB_CURRENT_MC'):
            continue

        if toconfig == '':
            toconfig = 'default'

        # Limitation: virtual dependencies must have a matching PREFERRED_PROVIDER
        # definition, even when the provider is unambiguous
        if dep.startswith('virtual/'):
            dep = d.getVar(f'PREFERRED_PROVIDER_{dep}_MC_{toconfig}')

        licdir = d.getVar(f'LICENSE_DIRECTORY_MC_{toconfig}')
        srcdir = os.path.join(licdir, toconfig.replace('-', '_'), dep)

        copy_license_depend(srcdir, destdir, f'mc:{toconfig}:{dep}')

python do_populate_lic:append () {
    copy_extra_license_depends(d)
}
do_populate_lic[depends] += " \
    ${@' '.join([dep + ':do_populate_lic' for dep in d.getVar('EXTRA_LICENSE_DEPENDS').split()])} \
"
do_populate_lic[mcdepends] += " \
    ${@' '.join([dep + ':do_populate_lic' for dep in d.getVar('EXTRA_LICENSE_MCDEPENDS').split()])} \
"
