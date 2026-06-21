#include "encoder.h"
#include "drv_debug.h"
#include "i2c.h"

#define AS5600_I2C_ADDR         (0x36 << 1)   // HAL库需要左移1位
#define AS5600_RAW_ANGLE_REG    0x0C
#define AS5600_ANGLE_REG        0x0E
#define AS5600_STATUS_REG       0x0B
#define AS5600_I2C_TIMEOUT      100

#define TWO_PI                  6.2831853f

static bool ReadRaw(uint16_t *raw){
    uint16_t raw_angle = 0;
    uint8_t data[2] = {0};

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
        &hi2c1,
        AS5600_I2C_ADDR,
        AS5600_RAW_ANGLE_REG,
        I2C_MEMADD_SIZE_8BIT,
        data,
        2,
        AS5600_I2C_TIMEOUT
    );

    if (status != HAL_OK){
        return false;
    }

    raw_angle = ((uint16_t)data[0] << 8) | data[1];
    raw_angle &= 0x0FFF;

    *raw = raw_angle;

    return true;
}

static bool IsConnected(){

    HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(
        &hi2c1,
        AS5600_I2C_ADDR,
        3,
        AS5600_I2C_TIMEOUT
    );

    return (status == HAL_OK);
}

void EncoderInit(){
    if (!IsConnected()){
        DebugPrintf("编码器初始化失败!");
    }
}

bool EncoderGet(uint16_t *raw){
    return ReadRaw(raw);    
}

float EncoderToRad(uint16_t raw){
    return ((float)raw * TWO_PI / 4096.0f);
}

float EncoderToAngleDeg(uint16_t raw){
    return ((float)raw * 360.0f / 4096.0f);
}

float EncoderToAngleDeg180(uint16_t raw){
    float deg = EncoderToAngleDeg(raw);

    if (deg > 180.0f){
        deg -= 360.0f;
    }

    return deg;
}
