#ifndef _DRV_DEBUG_H
#define _DRV_DEBUG_H

#include "ring_buffer.h"

#include <stdarg.h>
#include <stdbool.h>

void DebugInit(void);
void DebugRxCirculation();
void DebugPrintf(const char *format, ...);

#endif
