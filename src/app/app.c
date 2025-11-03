#include "global.h"
#include "app_debug.h"
#include "app_adc.h"

void appInit(void){    
    appDebugPrintf("定时器触发 ADC 启动\r\n");
    appAdcInit();
    appDebugPrintf("初始化完成!\r\n");
}

void appLoop(void){
    appAdcTest();
}

