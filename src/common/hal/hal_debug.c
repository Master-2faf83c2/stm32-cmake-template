#include "hal_debug.h"
#include <stdio.h>

#define USART_CLK_CMD           RCC_APB2PeriphClockCmd
#define USART_CLK               RCC_APB2Periph_USART1
#define USART                   USART1     

#define USART_DMA_CLK_CMD       RCC_AHBPeriphClockCmd
#define USART_DMA_CLK           RCC_AHBPeriph_DMA1

#define GPIO_CLK_CMD            RCC_AHBPeriphClockCmd
#define GPIO_CLK                RCC_APB2Periph_GPIOA
#define GPIO_PORT               GPIOA
#define GPIO_AF                 GPIO_AF_USART1

#define USART_DMA_TX            DMA1_Channel4
#define USART_DMA_TX_TC_FLAG    DMA1_FLAG_TC4
#define GPIO_TX_PIN             GPIO_Pin_9

#define USART_IRQ               USART1_IRQn
#define USART_IRQHandler        USART1_IRQHandler
#define USART_DMA_RX            DMA1_Channel5
#define USART_DMA_RX_TC_FLAG    DMA1_FLAG_TC5
#define GPIO_RX_PIN             GPIO_Pin_10

#define U1_TX_SIZE          (512)
#define U1_RX_SIZE          (1*1024)
#define U1_RX_MAX           256

uint8_t debug_tx_buff[U1_TX_SIZE];
uint8_t debug_rx_buff[U1_RX_SIZE];
RingBuf_t debug_cb;

void debugInitMode(uint32_t bound){
    RCC_APB2PeriphClockCmd(GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(USART_CLK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = GPIO_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIO_PORT, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART, &USART_InitStructure);
    USART_ITConfig(USART, USART_IT_IDLE, ENABLE);
    USART_Cmd(USART, ENABLE);
}

void debugInitDMA(void){
    USART_DMA_CLK_CMD(USART_DMA_CLK,ENABLE);

    DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART->DR;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;

    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)debug_tx_buff;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_Init(USART_DMA_TX, &DMA_InitStructure);
    DMA_Cmd(USART_DMA_TX, DISABLE);

    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)debug_rx_buff;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = U1_RX_MAX+1;
    DMA_Init(USART_DMA_RX, &DMA_InitStructure);
    DMA_Cmd(USART_DMA_RX, ENABLE);
}

void debugPtrInit(void){
    debug_cb.DataIn  = &debug_cb.DataPtr[0];
    debug_cb.DataOut = &debug_cb.DataPtr[0];
    debug_cb.DataEnd = &debug_cb.DataPtr[RING_BUF_NUM-1];
    debug_cb.DataIn->start = debug_rx_buff;
    debug_cb.Counter = 0;
}

void debugInit(uint32_t bound){
    debugPtrInit();
    debugInitMode(bound);
    debugInitDMA();
    USART_DMACmd(USART, USART_DMAReq_Rx, ENABLE);
    USART_DMACmd(USART, USART_DMAReq_Tx, ENABLE);
}

uint8_t debugIrqChannel(void){
    return USART_IRQ;
}

void debugPrintf(const char *format, ...){
    va_list arg;							            
	va_start(arg, format);					            
	uint16_t datalen = vsprintf((char *)debug_tx_buff, format, arg);			
	va_end(arg);							           

	DMA_Cmd(USART_DMA_TX, DISABLE);                       
	DMA_SetCurrDataCounter(USART_DMA_TX, datalen);          
	DMA_Cmd(USART_DMA_TX, ENABLE);                         
    while(DMA_GetFlagStatus(USART_DMA_TX_TC_FLAG) != SET);
    DMA_ClearFlag(USART_DMA_TX_TC_FLAG);
}

void USART_IRQHandler(void){
    if (USART_GetITStatus(USART, USART_IT_IDLE) != RESET){
        USART_GetFlagStatus(USART, USART_FLAG_IDLE);
        USART_ReceiveData(USART);
        debug_cb.Counter += ((U1_RX_MAX+1) - DMA_GetCurrDataCounter(USART_DMA_RX));
        debug_cb.DataIn->end = &debug_rx_buff[debug_cb.Counter-1];
        debug_cb.DataIn ++;
        if (debug_cb.DataIn == debug_cb.DataEnd){
            debug_cb.DataIn  = &debug_cb.DataPtr[0];
        }
        if ((U1_RX_SIZE-debug_cb.Counter) >= U1_RX_MAX){
            debug_cb.DataIn->start = &debug_rx_buff[debug_cb.Counter];
        }
        else {
            debug_cb.DataIn->start = debug_rx_buff;
            debug_cb.Counter = 0;
        }
        DMA_Cmd(USART_DMA_RX, DISABLE);
        DMA_SetCurrDataCounter(USART_DMA_RX, U1_RX_MAX+1);
        USART_DMA_RX->CMAR = (uint32_t)debug_cb.DataIn->start;
        DMA_Cmd(USART_DMA_RX, ENABLE);
    }
}
