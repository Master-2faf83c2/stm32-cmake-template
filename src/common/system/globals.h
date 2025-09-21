#ifndef _GLOBALS_H
#define _GLOBALS_H

#include "hal_sys.h"

//缓冲区个数
#define RING_BUF_NUM        20

typedef struct{
    uint8_t     *start;
    uint8_t     *end;
}UcbBuffPtr;

typedef struct {
    UcbBuffPtr  DataPtr[RING_BUF_NUM];      // UART 接收缓冲数组
    UcbBuffPtr  *DataIn;                    // 写指针
    UcbBuffPtr  *DataOut;                   // 读指针
    UcbBuffPtr  *DataEnd;                   // 缓冲区末尾
    uint16_t    Counter;                    // 数据计数
} RingBuf_t;

#endif
