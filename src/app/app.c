#include "global.h"
#include "app_debug.h"
#include "app_adc.h"

void appInit(void){    
    appDebugPrintf("定时器触发 ADC 启动\r\n");
    appAdcInit();
    appDebugPrintf("初始化完成!\r\n");
}

void appLoop(void){
    if (appAdcGetAcc()){
        // for (uint16_t i = 0; i < 4096; i ++){
        //     appDebugPrintf("adc_buf1[%d] = %d\r\n", i, adc_buf1[i]);
        // }
        appDebugPrintf("ADC 电压: %.3f V\r\n", appAdcGetVoltage());
    }
}

