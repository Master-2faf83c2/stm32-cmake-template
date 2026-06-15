#include "as5600.h"

#define AS5600_I2C_TIMEOUT  100

void AS5600_Init(AS5600_HandleTypeDef *has5600, I2C_HandleTypeDef *hi2c)
{
    has5600->hi2c = hi2c;
    has5600->raw_angle = 0;
    has5600->angle_deg = 0.0f;
}

HAL_StatusTypeDef AS5600_IsConnected(AS5600_HandleTypeDef *has5600)
{
    return HAL_I2C_IsDeviceReady(
        has5600->hi2c,
        AS5600_I2C_ADDR,
        3,
        AS5600_I2C_TIMEOUT
    );
}

HAL_StatusTypeDef AS5600_ReadRawAngle(AS5600_HandleTypeDef *has5600, uint16_t *raw_angle)
{
    uint8_t data[2] = {0};

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
        has5600->hi2c,
        AS5600_I2C_ADDR,
        AS5600_RAW_ANGLE_REG,
        I2C_MEMADD_SIZE_8BIT,
        data,
        2,
        AS5600_I2C_TIMEOUT
    );

    if (status != HAL_OK)
    {
        return status;
    }

    /*
     * AS5600 原始角度是 12 位：
     * data[0] 高 8 位
     * data[1] 低 8 位
     * 有效范围 0~4095
     */
    *raw_angle = ((uint16_t)data[0] << 8) | data[1];
    *raw_angle &= 0x0FFF;

    has5600->raw_angle = *raw_angle;

    return HAL_OK;
}

HAL_StatusTypeDef AS5600_ReadAngleDeg(AS5600_HandleTypeDef *has5600, float *angle_deg)
{
    uint16_t raw = 0;

    HAL_StatusTypeDef status = AS5600_ReadRawAngle(has5600, &raw);

    if (status != HAL_OK)
    {
        return status;
    }

    *angle_deg = (float)raw * 360.0f / 4096.0f;

    has5600->angle_deg = *angle_deg;

    return HAL_OK;
}

HAL_StatusTypeDef AS5600_ReadAngleDeg180(AS5600_HandleTypeDef *has5600, float *angle_deg)
{
    float angle = 0.0f;

    HAL_StatusTypeDef status = AS5600_ReadAngleDeg(has5600, &angle);

    if (status != HAL_OK)
    {
        return status;
    }

    if (angle > 180.0f)
    {
        angle -= 360.0f;
    }

    *angle_deg = angle;

    return HAL_OK;
}

HAL_StatusTypeDef AS5600_ReadStatus(AS5600_HandleTypeDef *has5600, uint8_t *status_reg)
{
    return HAL_I2C_Mem_Read(
        has5600->hi2c,
        AS5600_I2C_ADDR,
        AS5600_STATUS_REG,
        I2C_MEMADD_SIZE_8BIT,
        status_reg,
        1,
        AS5600_I2C_TIMEOUT
    );
}