#ifndef _ENCODER_H
#define _ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

//初始化
void EncoderInit();
//获取数据
bool EncoderGet(uint16_t *raw);
// 原始值转弧度
float EncoderToRad(uint16_t raw);
// 原始值转0-360
float EncoderToAngleDeg(uint16_t raw);
// 原始值转0-180
float EncoderToAngleDeg180(uint16_t raw);

#ifdef __cplusplus
}
#endif

#endif
