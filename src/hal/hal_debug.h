#ifndef _HAL_DEBUG_H
#define _HAL_DEBUG_H

#include "usbd_cdc_if.h"

#include <stdarg.h>

void usbPrintf(const char *format, ...);

#endif
