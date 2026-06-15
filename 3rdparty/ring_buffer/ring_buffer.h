#ifndef _RING_BUFFER_H
#define _RING_BUFFER_H

#include <stdint.h>

//缓冲区个数
#define RING_BUF_NUM        20

typedef struct{
    uint8_t     *start;
    uint8_t     *end;
} UcbBuffPtr_t;

typedef struct {
    UcbBuffPtr_t    DataPtr[RING_BUF_NUM];          //接收缓冲数组
    UcbBuffPtr_t    *DataIn;                        //写指针
    UcbBuffPtr_t    *DataOut;                       //读指针
    UcbBuffPtr_t    *DataEnd;                       //缓冲区末尾
    uint16_t        Counter;                        //数据计数
} RingBuf_t;

void RingBufferInit(RingBuf_t *cfg, uint8_t *buffer, uint16_t length);
void RingBufferRxCallback(uint16_t pos, uint8_t *buffer);

#endif
