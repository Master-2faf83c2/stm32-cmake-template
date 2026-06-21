#ifndef _FOC_INF_H
#define _FOC_INF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#include <math.h>

typedef struct{
    float   pole_pairs;                             // 电机极对数
    float   zero_offset;                            // 零位偏移, 弧度
    float   vbus;                                   // 母线电压, 例如 12V
    float   pwm_period;                             // PWM周期, 例如 7199
} FOC_HandleTypeDef;

// 初始化
void FOC_Init(FOC_HandleTypeDef *foc, float pole_pairs, float vbus, float pwm_period);
// 使能
void FOC_Enable();
// 失能
void FOC_Disable();
//校准
void FOC_CalibrateZero(FOC_HandleTypeDef *foc);
//计算 foc
void FOC_Compute(
    FOC_HandleTypeDef *foc, 
    float encoder_angle,
    float Uq,
    float Ud
);

#ifdef __cplusplus
}
#endif

#endif
