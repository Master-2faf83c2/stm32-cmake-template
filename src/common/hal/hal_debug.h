#ifndef _HAL_DEBUG_H
#define _HAL_DEBUG_H

# include "globals.h"
#include <stdarg.h>

extern RingBuf_t  debug_cb;

void        debugInit(uint32_t bound);
uint8_t     debugIrqChannel(void);
void        debugPrintf(char *format, ...);

#endif
