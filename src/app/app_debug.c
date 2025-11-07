#include "app_debug.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

#define DEBUG_TX_SIZE          (2*1024)
#define DEBUG_RX_SIZE          (2*1024)
#define DEBUG_RX_MAX           256

static RAM_D2 uint8_t tx_buff[DEBUG_TX_SIZE];
static RAM_D2 uint8_t rx_buff[DEBUG_RX_SIZE];

DebugRingBuf_t debug_cb;

static void debugRingBufInit(void){
    debug_cb.rx.DataIn  = &debug_cb.rx.DataPtr[0];
    debug_cb.rx.DataOut = &debug_cb.rx.DataPtr[0];
    debug_cb.rx.DataEnd = &debug_cb.rx.DataPtr[RING_BUF_NUM-1];
    debug_cb.rx.DataIn->start = rx_buff;
    debug_cb.rx.Counter = 0;

    debug_cb.tx.DataIn  = &debug_cb.tx.DataPtr[0];
    debug_cb.tx.DataOut = &debug_cb.tx.DataPtr[0];
    debug_cb.tx.DataEnd = &debug_cb.tx.DataPtr[RING_BUF_NUM-1];
    debug_cb.tx.DataIn->start = tx_buff;
    debug_cb.tx.Counter = 0;
    debug_cb.tx_busy = false;
}

static void DebugTxCallback(void){
    if (debug_cb.tx.DataOut != debug_cb.tx.DataIn){
        debug_cb.tx_busy = true;
        HAL_UART_Transmit_DMA(&huart1,debug_cb.tx.DataOut->start, debug_cb.tx.DataOut->end - debug_cb.tx.DataOut->start + 1);
        debug_cb.tx.DataOut ++;
        if (debug_cb.tx.DataOut > debug_cb.tx.DataEnd){
            debug_cb.tx.DataOut = &debug_cb.tx.DataPtr[0];
        }
    }else {
        debug_cb.tx_busy = false;
    }
}

static void DebugRxCallback(uint16_t pos){
    debug_cb.rx.Counter += ((DEBUG_RX_MAX+1) - pos);
    debug_cb.rx.DataIn->end = &rx_buff[debug_cb.rx.Counter-1];
    debug_cb.rx.DataIn ++;
    if (debug_cb.rx.DataIn == debug_cb.rx.DataEnd){
        debug_cb.rx.DataIn  = &debug_cb.rx.DataPtr[0];
    }
    if ((DEBUG_RX_SIZE-debug_cb.rx.Counter) >= DEBUG_RX_MAX){
        debug_cb.rx.DataIn->start = &rx_buff[debug_cb.rx.Counter];
    }
    else {
        debug_cb.rx.DataIn->start = rx_buff;
        debug_cb.rx.Counter = 0;
    }
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, debug_cb.rx.DataIn->start, sizeof(debug_cb.rx.DataIn->start));
}

static void SendDataBuff(uint8_t *data, uint16_t len){
    if ((DEBUG_TX_SIZE-debug_cb.tx.Counter) >= len){
        debug_cb.tx.DataIn->start = &tx_buff[debug_cb.tx.Counter];
    }else {
        debug_cb.tx.Counter = 0;
        debug_cb.tx.DataIn->start = tx_buff;
    }
    memcpy(debug_cb.tx.DataIn->start, data, len);
    debug_cb.tx.Counter += len;
    debug_cb.tx.DataIn->end = &tx_buff[debug_cb.tx.Counter-1];

    debug_cb.tx.DataIn ++;
    if (debug_cb.tx.DataIn > debug_cb.tx.DataEnd){
        debug_cb.tx.DataIn = &debug_cb.tx.DataPtr[0];
    }

    if (debug_cb.tx_busy)
        return;
    DebugTxCallback();
}

void appDebugInit(void){
    debugRingBufInit();
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buff, DEBUG_RX_SIZE);
}

void appDebugPrintf(const char *format, ...){
    va_list args;
    uint32_t length;
    uint8_t arg_buff[DEBUG_TX_SIZE];
    
    va_start(args, format);
    length = vsnprintf((char *)arg_buff, DEBUG_TX_SIZE, (char *)format, args);
    va_end(args);

    SendDataBuff(arg_buff, length);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        DebugTxCallback();
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Pos) {
    if (huart->Instance == USART1) {
        DebugRxCallback(Pos);
    }
}
