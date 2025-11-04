#include "global.h"
#include "app_debug.h"
#include "app_adc.h"

void appInit(void){    
    appDebugInit();
    appAdcInit();
    appDebugPrintf("初始化完成!\r\n");
}

void appLoop(void){
    appFFTLoop();
}

