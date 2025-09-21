#ifndef _DELAY_H
#define _DELAY_H

#include "globals.h"

/*----------------------函数定义----------------------*/
void delay_us(uint32_t xus);                    //微妙延时
void delay_ms(uint32_t xms);                    //毫秒延时
void delay_s(uint32_t xs);                      //秒延时

#endif
