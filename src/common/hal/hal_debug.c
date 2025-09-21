#include "hal_debug.h"
#include <stdio.h>
#include <string.h>

#define USART_CLK_CMD           RCC_APB2PeriphClockCmd
#define USART_CLK               RCC_APB2Periph_USART1
#define USART                   USART1     
#define USART_IRQ               USART1_IRQn
#define USART_IRQHandler        USART1_IRQHandler
#define USART_DMA_CLK_CMD       RCC_AHB1PeriphClockCmd
#define USART_DMA_CLK           RCC_AHB1Periph_DMA2
#define USART_DMA_CHANNEL       DMA_Channel_4
#define USART_DMA_TX            DMA2_Stream7
#define USART_DMA_TX_TC_FLAG    DMA_FLAG_TCIF7
#define USART_DMA_RX            DMA2_Stream5
#define USART_DMA_RX_TC_FLAG    DMA_FLAG_TCIF5

#define GPIO_CLK_CMD            RCC_AHB1PeriphClockCmd
#define GPIO_CLK                RCC_AHB1Periph_GPIOA
#define GPIO_PORT               GPIOA
#define GPIO_AF                 GPIO_AF_USART1
#define GPIO_TX_PIN             GPIO_Pin_9
#define GPIO_TX_AF_PIN          GPIO_PinSource9
#define GPIO_RX_PIN             GPIO_Pin_10
#define GPIO_RX_AF_PIN          GPIO_PinSource10

#define U1_TX_SIZE          (512)
#define U1_RX_SIZE          (1*1024)
#define U1_RX_MAX           256

uint8_t tx_buff[U1_TX_SIZE];
uint8_t rx_buff[U1_RX_SIZE];
RingBuf_t debug_cb;

/**
 * @brief 串口 配置
 * 
 * @param bound 波特率
 */
void debugInitMode(uint32_t bound){

    GPIO_CLK_CMD(GPIO_CLK, ENABLE);  // 使能GPIOA时钟
    USART_CLK_CMD(USART_CLK, ENABLE); // 使能USART1时钟

    // USART 端口配置
    GPIO_PinAFConfig(GPIO_PORT,GPIO_TX_AF_PIN,GPIO_AF);
    GPIO_PinAFConfig(GPIO_PORT,GPIO_RX_AF_PIN,GPIO_AF);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_TX_PIN | GPIO_RX_PIN;    // GPIOA9与GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                // 复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;          // 速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              // 推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                // 上拉
    GPIO_Init(GPIO_PORT, &GPIO_InitStructure);              

    // USART 初始化设置
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = bound;                                     // 波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // 一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // 无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 收发模式
    USART_Init(USART, &USART_InitStructure);                                        // 初始化串口

    USART_ITConfig(USART, USART_IT_IDLE, ENABLE);

    //使能串口
    USART_Cmd(USART, ENABLE); 
}

/**
 * @brief 串口DMA 配置
 * 
 */
void debugInitDMA(void){

    DMA_InitTypeDef  DMA_InitStructure;
	
	USART_DMA_CLK_CMD(USART_DMA_CLK,ENABLE);//DMA2时钟使能 
        
    /* 配置 DMA Stream */
    DMA_InitStructure.DMA_Channel = USART_DMA_CHANNEL;  //通道选择
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART->DR;//DMA外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)tx_buff;//DMA 存储器0地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//存储器到外设模式
    DMA_InitStructure.DMA_BufferSize = 1;//数据传输量 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
    DMA_Init(USART_DMA_TX, &DMA_InitStructure);

    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)rx_buff;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = U1_RX_MAX+1;
    DMA_Init(USART_DMA_RX, &DMA_InitStructure);

    DMA_Cmd(USART_DMA_TX, DISABLE);  
    DMA_Cmd(USART_DMA_RX, ENABLE);     
}

void debugPtrInit(void){
    debug_cb.DataIn  = &debug_cb.DataPtr[0];
    debug_cb.DataOut = &debug_cb.DataPtr[0];
    debug_cb.DataEnd = &debug_cb.DataPtr[RING_BUF_NUM-1];
    debug_cb.DataIn->start = rx_buff;
    debug_cb.Counter = 0;
}

/**
 * @brief 串口初始化
 * 
 * @param bound 波特率
 */
void debugInit(uint32_t bound){

    debugInitMode(bound);
    debugInitDMA();
    debugPtrInit();
    USART_DMACmd(USART, USART_DMAReq_Tx, ENABLE);
    USART_DMACmd(USART, USART_DMAReq_Rx, ENABLE);
}

/**
 * @brief 返回中断通道
 * 
 * @return uint8_t 中断通道
 */
uint8_t debugIrqChannel(void){

    return USART_IRQ;
}

/**
 * @brief Debug 打印函数与printf用法一样
 * 
 * @param format    指定要显示的格式化字符串，范围：ASCII码可见字符组成的字符串
 * @param ...       格式化字符串参数列表
 */
void debugPrintf(char *format, ...){

	va_list arg;							            
	va_start(arg, format);					            
	uint16_t datalen = vsprintf((char *)tx_buff, format, arg);			
	va_end(arg);							           

	DMA_Cmd(USART_DMA_TX, DISABLE);                       
	DMA_SetCurrDataCounter(USART_DMA_TX, datalen);          
	DMA_Cmd(USART_DMA_TX, ENABLE);                         
    while(DMA_GetFlagStatus(USART_DMA_TX, USART_DMA_TX_TC_FLAG) != SET);
    DMA_ClearFlag(USART_DMA_TX, USART_DMA_TX_TC_FLAG);
}

/**
 * @brief 串口中断
 * 
 */
void USART_IRQHandler(void){

    if (USART_GetITStatus(USART, USART_IT_IDLE) != RESET){
        USART_GetFlagStatus(USART, USART_FLAG_IDLE);
        USART_ReceiveData(USART);
        debug_cb.Counter += ((U1_RX_MAX+1) - DMA_GetCurrDataCounter(USART_DMA_RX));
        debug_cb.DataIn->end = &rx_buff[debug_cb.Counter-1];
        debug_cb.DataIn ++;
        if (debug_cb.DataIn == debug_cb.DataEnd){
            debug_cb.DataIn  = &debug_cb.DataPtr[0];
        }
        if ((U1_RX_SIZE-debug_cb.Counter) >= U1_RX_MAX){
            debug_cb.DataIn->start = &rx_buff[debug_cb.Counter];
        }else {
            debug_cb.DataIn->start = rx_buff;
            debug_cb.Counter = 0;
        }
        DMA_Cmd(USART_DMA_RX, DISABLE);
        DMA_SetCurrDataCounter(USART_DMA_RX, U1_RX_MAX+1);
        USART_DMA_RX->M0AR = (uint32_t)debug_cb.DataIn->start;
        DMA_Cmd(USART_DMA_RX, ENABLE);
    }
}
