DESCRIPTION = "Teridian metering processor querying daemon - 500ms firmware"

require teridiand-fw.inc

RPROVIDES_${PN} = "teridiand-fw"
RCONFLICTS_${PN} = "teridiand-fw"
