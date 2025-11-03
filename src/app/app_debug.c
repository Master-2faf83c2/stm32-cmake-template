#include "app_debug.h"
#include "usart.h"
#include <stdio.h>

#define DEBUG_TX_SIZE          (512)
#define DEBUG_RX_SIZE          (1*1024)
#define DEBUG_RX_MAX           128

__attribute__((section(".RAM_D2"))) uint8_t tx_buff[DEBUG_TX_SIZE];

void appDebugInit(void){
    // HAL_UART_Transmit_DMA(&huart1, tx_buff, DEBUG_TX_SIZE);
}

void appDebugPrintf(const char *format, ...){
    va_list args;
    uint32_t length;
    uint8_t tx_buf[256];

    va_start(args, format);
    length = vsnprintf((char *)tx_buf, 256, (char *)format, args);
    va_end(args);

    HAL_UART_Transmit(&huart1, tx_buf, length, HAL_MAX_DELAY);
}
