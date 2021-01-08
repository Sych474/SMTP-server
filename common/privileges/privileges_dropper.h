#ifndef COMMON_PRIVILEGES_PRIVILEGES_DROPPER_H_
#define COMMON_PRIVILEGES_PRIVILEGES_DROPPER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../logger/logger.h"

int drop_privileges(logger_t *logger);

#endif  // COMMON_PRIVILEGES_PRIVILEGES_DROPPER_H_
