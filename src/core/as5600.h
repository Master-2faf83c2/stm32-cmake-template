#ifndef __AS5600_H
#define __AS5600_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "i2c.h"
#include <stdint.h>

#define AS5600_I2C_ADDR        (0x36 << 1)   // HAL库需要左移1位
#define AS5600_RAW_ANGLE_REG   0x0C
#define AS5600_ANGLE_REG       0x0E
#define AS5600_STATUS_REG      0x0B

typedef struct
{
    I2C_HandleTypeDef *hi2c;
    uint16_t raw_angle;
    float angle_deg;
} AS5600_HandleTypeDef;

/**
 * @brief 初始化 AS5600 句柄
 */
void AS5600_Init(AS5600_HandleTypeDef *has5600, I2C_HandleTypeDef *hi2c);

/**
 * @brief 检测 AS5600 是否在线
 * @return HAL_OK 表示在线
 */
HAL_StatusTypeDef AS5600_IsConnected(AS5600_HandleTypeDef *has5600);

/**
 * @brief 读取 AS5600 原始角度值
 * @return 0~4095
 */
HAL_StatusTypeDef AS5600_ReadRawAngle(AS5600_HandleTypeDef *has5600, uint16_t *raw_angle);

/**
 * @brief 读取 AS5600 角度，单位：度
 * @return 0~360.0
 */
HAL_StatusTypeDef AS5600_ReadAngleDeg(AS5600_HandleTypeDef *has5600, float *angle_deg);

/**
 * @brief 读取 AS5600 角度，并转换为 -180~180 度
 */
HAL_StatusTypeDef AS5600_ReadAngleDeg180(AS5600_HandleTypeDef *has5600, float *angle_deg);

/**
 * @brief 读取状态寄存器
 */
HAL_StatusTypeDef AS5600_ReadStatus(AS5600_HandleTypeDef *has5600, uint8_t *status);

#ifdef __cplusplus
}
#endif

#endif