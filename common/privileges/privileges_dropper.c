#include "privileges_dropper.h"

int drop_privileges(logger_t *logger)
{
    gid_t gid;
    uid_t uid;

    // no need to "drop" the privileges that you don't have in the first place!
    if (getuid() != 0)
         return 0;
    
    // when your program is invoked with sudo, getuid() will return 0 and you
    // won't be able to drop your privileges
    if ((uid = getuid()) == 0) {
        const char *sudo_uid = secure_getenv("SUDO_UID");
        if (sudo_uid == NULL) {
            log_error(logger, "environment variable `SUDO_UID` not found");
            return -1;
        }
        errno = 0;
        uid = (uid_t) strtoll(sudo_uid, NULL, 10);
        if (errno != 0) {
            log_error(logger, "under-/over-flow in converting `SUDO_UID` to integer");
            return -1;
        }
    }

    // again, in case your program is invoked using sudo
    if ((gid = getgid()) == 0) {
        const char *sudo_gid = secure_getenv("SUDO_GID");
        if (sudo_gid == NULL) {
            log_error(logger, "environment variable `SUDO_GID` not found");
            return -1;
        }
        errno = 0;
        gid = (gid_t) strtoll(sudo_gid, NULL, 10);
        if (errno != 0) {
            log_error(logger, "under-/over-flow in converting `SUDO_GID` to integer");
            return -1;
        }
    }

    if (setgid(gid) != 0) {
        log_error(logger, "setgid error");
        return -1;
    }
    if (setuid(uid) != 0) {
        log_error(logger, "setgid error");
        return -1;    
    }

    // check if we successfully dropped the root privileges
    if (setuid(0) == 0 || seteuid(0) == 0) {
        log_error(logger, "could not drop root privileges!");
        return -1;
    }

    log_info(logger, "Privileges are dropped"); 
    return 0;
}