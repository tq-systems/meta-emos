#include "empkg.h"
#include "empkg_appdb.h"
#include "empkg_fops.h"

int empkg_dbus(const char *id) {
	const char *dbusconf = appdb_get_path(P_DBUSAPP, id);
	const char *dbusdest = appdb_get_path(P_DBUSCONF, id);

	if (!access(dbusconf, F_OK) && appdb_is(ENABLED, id)) {
		/* Now combine:
		 * cp -f "$conf" "$APPTMP/dbus/$app.conf"
		 * mv -f "$APPTMP/dbus/$app.conf" "$DBUSDIR/$app.conf"
		 *
		 * to:
		 * cp -f "$conf" "$DBUSDIR/$app.conf"
		 * via fread(fconf) and fwrite(fdest)
		 * skipping $APPTMP/dbus
		 */

		empkg_fops_rm(dbusdest);
		empkg_fops_cp(dbusconf, dbusdest);
	} else {
		empkg_fops_rm(dbusdest);
	}
	sync();

	return 0;
}
