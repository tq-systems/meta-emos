# SPDX-License-Identifier: MIT
#
# Copyright (C) 2024 TQ-Systems GmbH <oss@ew.tq-group.com>, D-82229 Seefeld, Germany.
# Author: Matthias Schiffer
#
# Bitbake class to exchange variables contents between multiconfigs.
#
# MULTICONFIG_VARS must be set in the main multiconfig (usually a machine or distro). All listed
# variables will be copied to all other multiconfigs, suffixed with `_MC_<name>` (where `<name>` is
# set to `default` for the main multiconfig).
#
# Example:
#
#     If MULTICONFIG_VARS contains "DEPLOY_DIR_IMAGE" and there is a multiconfig "second", the
#     "DEPLOY_DIR_IMAGE" value from the main multiconfig of will be available as
#     "DEPLOY_DIR_IMAGE_MC_default" in "second", and the value from the "second" multiconfig
#     will be available as "DEPLOY_DIR_IMAGE_MC_second" in the main multiconfig.
#
# Copying happens when the MultiConfigParsed event is emitted, which is after global (machine,
# distro, ...) config has been evaluated, but before recipe content is parsed. All variables are
# copied fully expanded, as unexpanded variable references would not be meaningful in other
# multiconfigs.

MULTICONFIG_VARS[doc] = "Space separated list of variables to copy between multiconfigs"
MULTICONFIG_VARS ?= ""

python multiconfig_var_handler() {
    vars = d.getVar('MULTICONFIG_VARS').split()
    if not vars:
        return

    for fromconfig, fromdata in e.mcdata.items():
        for var in vars:
            value = fromdata.getVar(var)
            if value is None:
                continue

            for toconfig, todata in e.mcdata.items():
                if fromconfig == toconfig:
                    continue

                todata.setVar(var + '_MC_' + (fromconfig or 'default'), value)
}
addhandler multiconfig_var_handler
multiconfig_var_handler[eventmask] = "bb.event.MultiConfigParsed"
