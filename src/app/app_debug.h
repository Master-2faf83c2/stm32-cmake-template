#ifndef _APP_DEBUG_H
#define _APP_DEBUG_H

#include "global.h"

#include <stdarg.h>

void appDebugInit(void);
void appDebugPrintf(const char *format, ...);

#endif
