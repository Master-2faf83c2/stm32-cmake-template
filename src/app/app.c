#include "global.h"
#include "app_debug.h"
#include "app_adc.h"

void appInit(void){    
    debugInit();
    adcInit();
    debugPrintf("初始化完成!\r\n");
}

void appLoop(void){
    debugRxRingBufLoop();
    fftLoop();
    // adcTest();
}

