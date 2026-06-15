#ifndef _DEBUG_H
#define _DEBUG_H

#include "global.h"
#include "ring_buffer.h"

#include <stdarg.h>
#include <stdbool.h>

typedef struct {
    RingBuf_t   rx;
    RingBuf_t   tx;
    bool        tx_busy;
} DebugRingBuf_t;

void DebugInit(void);
void DebugRxRingBufLoop(void);
void DebugPrintf(const char *format, ...);

#endif
