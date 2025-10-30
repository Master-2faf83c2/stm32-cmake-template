#include "main.h"
#include "hal_debug.h"

void appInit(void){
    usbPrintf("你好世界!\r\n");
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
}

void appLoop(void){
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
    HAL_Delay(1000);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_Delay(1000);
    usbPrintf("你好世界!\r\n");
}
