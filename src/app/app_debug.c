#include "app_debug.h"
#include "usart.h"
#include <stdio.h>

#define DEBUG_TX_SIZE          (512)
#define DEBUG_RX_SIZE          (1*1024)
#define DEBUG_RX_MAX           128

uint8_t debug_tx_buf[DEBUG_TX_SIZE];

void appDebugInit(void){
    
}

void appDebugPrintf(const char *format, ...){
    va_list args;
    uint32_t length;
    

    va_start(args, format);
    length = vsnprintf((char *)debug_tx_buf, DEBUG_TX_SIZE, (char *)format, args);
    va_end(args);

    while(HAL_UART_Transmit(&huart1, debug_tx_buf, length, HAL_MAX_DELAY) != HAL_OK);
}
