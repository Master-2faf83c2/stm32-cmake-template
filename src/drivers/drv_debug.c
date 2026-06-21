#include "drv_debug.h"
#include "usart.h"

#include <stdio.h>
#include <string.h>

#define DEBUG_TX_SIZE          (1*1024)
#define DEBUG_RX_SIZE          (1*1024)
#define DEBUG_RX_MAX           256

static uint8_t rx_buff[DEBUG_RX_SIZE];

RingBuf_t debug_tx_cb;

void DMA_Receive(void){
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, debug_tx_cb.DataIn->start, DEBUG_RX_SIZE - debug_tx_cb.Counter);
}

void RxHandle(uint8_t *data, uint16_t length){
    DebugPrintf("接收的数据: %s, 长度: %d\r\n", data, length);
}

void DebugInit(void){
    RingBufferInit(&debug_tx_cb, rx_buff, DEBUG_RX_SIZE);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_buff, DEBUG_RX_SIZE);
    __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
}

void DebugRxCirculation(){
    RingBufferRxHandle(&debug_tx_cb, RxHandle);
}

void DebugPrintf(const char *format, ...){
    va_list args;
    uint32_t length;
    uint8_t arg_buff[DEBUG_TX_SIZE];
    
    va_start(args, format);
    length = vsnprintf((char *)arg_buff, DEBUG_TX_SIZE, (char *)format, args);
    va_end(args);

    while(HAL_UART_Transmit(&huart2, arg_buff, length, HAL_MAX_DELAY) != HAL_OK);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Pos) {
    if (huart->Instance == USART1) {
        RingBufferRxCallback(&debug_tx_cb, Pos, rx_buff, DEBUG_RX_SIZE, DEBUG_RX_MAX, DMA_Receive);
    }
}
