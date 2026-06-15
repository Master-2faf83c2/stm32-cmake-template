#include "global.h"
#include "debug.h"
#include "as5600.h"

AS5600_HandleTypeDef has5600;
float motor_angle = 0.0f;
uint16_t raw_angle = 0;

void appInit(void){    
    DebugInit();
    AS5600_Init(&has5600, &hi2c1);
    DebugPrintf("初始化完成!\r\n");
    if (AS5600_IsConnected(&has5600) == HAL_OK){
        DebugPrintf("AS5600 连接成功!\r\n");
    }
    else{
        DebugPrintf("AS5600 没有检测到!\r\n");
    }
}

void appLoop(void){
    DebugRxRingBufLoop();
    if (AS5600_ReadAngleDeg(&has5600, &motor_angle) == HAL_OK){
        DebugPrintf("motor_angle: %f\r\n", motor_angle);
        HAL_Delay(10);
    }
}
