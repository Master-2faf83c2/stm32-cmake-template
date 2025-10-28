#include "hal_debug.h"
#include <stdio.h>

uint8_t tx_buf[256];

void usbPrintf(const char *format, ...){
    va_list args;
    uint32_t length;

    va_start(args, format);
    length = vsnprintf((char *)tx_buf, 256, (char *)format, args);
    va_end(args);

    CDC_Transmit_FS(tx_buf, length);
}
