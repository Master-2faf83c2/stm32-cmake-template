#ifndef _APP_DEBUG_H
#define _APP_DEBUG_H

#include "global.h"

#include <stdarg.h>
#include <stdbool.h>

typedef struct {
    RingBuf_t   rx;
    RingBuf_t   tx;
    bool        tx_busy;
} DebugRingBuf_t;

void appDebugInit(void);
void appDebugPrintf(const char *format, ...);

#endif
