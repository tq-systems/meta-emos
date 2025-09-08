# meta-emos
## Description
The meta-emos project is a Yocto layer maintained by TQ, which contains
definitions for the Energy Manager's operating system. It is integrated
into the em-build build framework with other layers.

## Release
Typically, a release commit updates the distribution version and the
changelogs. The new version must be entered into the `DISTRO_VERSION` variable
in the `conf/distro/emos.conf` file.

## License information
The files in this project are licensed under different licenses (a mix of MIT and TQSPSLA-1.0.3).
See LICENSE.MIT and LICENSE.TQSPSLA-1.0.3 for further details of the individual licenses.

As is common with other Yocto layers, all metadata of the meta-emos project is licensed
under MIT, unless otherwise stated. This includes, for example, recipes (.bb files),
recipe attachments (.bbappend files), classes (.bbclass files) and other files.

    SPDX-License-Identifier: MIT

The projects referenced in the recipes have their own licenses, as do their artifacts.
A license is defined in each recipe.

The layer also contains source code licensed under TQSPSLA-1.0.3. All files in this project
which are licensed under TQSPSLA-1.0.3 are classified as product-specific software
and bound to the use with the TQ-Systems GmbH product: EM400.

    SPDX-License-Identifier: LicenseRef-TQSPSLA-1.0.3

This license is not yet registered in the official SPDX license list (https://spdx.org/licenses/).
However, specifying the license via the SPDX identifier facilitates a license clearing.
