#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "main.h"

//缓冲区个数
#define RING_BUF_NUM        10

//内存地址
#define RAM_D2              __attribute__((section(".RAM_D2")))

typedef struct{
    uint8_t     *start;
    uint8_t     *end;
} UcbBuffPtr_t;

typedef struct {
    UcbBuffPtr_t    DataPtr[RING_BUF_NUM];          //UART 接收缓冲数组
    UcbBuffPtr_t    *DataIn;                        //写指针
    UcbBuffPtr_t    *DataOut;                       //读指针
    UcbBuffPtr_t    *DataEnd;                       //缓冲区末尾
    uint16_t        Counter;                        //数据计数
} RingBuf_t;

#endif
