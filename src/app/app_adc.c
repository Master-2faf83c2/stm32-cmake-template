#include "app_adc.h"
#include "app_debug.h"
#include "global.h"
#include "stm32h7xx_hal_gpio.h"
#include "tim.h"
#include "adc.h"

#define FFT_SIZE            1024
#define ADC_DMA_SIZE        (FFT_SIZE*2)

__attribute__((section(".RAM_D2"))) volatile uint16_t adc_buf[ADC_DMA_SIZE];

typedef struct{
    volatile uint16_t    *start;
    volatile uint16_t    *end;
}DMA_BufferPtr16_t;

typedef struct{
    DMA_BufferPtr16_t ptr;
    uint8_t acc;
}AdcInfo_t;

AdcInfo_t adc_info;

static void appAdcInfoInit(void){
    adc_info.acc = 0;
    adc_info.ptr.start = &adc_buf[0];
    adc_info.ptr.end = &adc_buf[FFT_SIZE-1];
}

void appAdcInit(void){
    appAdcInfoInit();
    HAL_StatusTypeDef res = HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);
    if (res != HAL_OK){
        appDebugPrintf("ADC 校准失败!\r\n");
    }
    res = HAL_ADC_Start_DMA(&hadc1,(uint32_t *)&adc_buf, ADC_DMA_SIZE);
    if (res != HAL_OK){
        appDebugPrintf("ADC DMA 初始化失败!\r\n");
    }
    HAL_TIM_Base_Start_IT(&htim3);
}

void appAdcTest(void){
    if (!appAdcGetAcc())
        return;
    volatile uint16_t *p = adc_info.ptr.start;
    for (uint32_t i = 0; i < FFT_SIZE; i ++){
        appDebugPrintf("adc_buf[%d]: %d\r\n", i, p[i]);
    }
}

uint8_t appAdcGetAcc(void){
    if (adc_info.acc){
        adc_info.acc = 0;
        return 1;
    }
    return 0;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc){
    if(hadc->Instance == ADC1){
        adc_info.acc = 1;
        adc_info.ptr.start = &adc_buf[0];
        adc_info.ptr.end = &adc_buf[ADC_DMA_SIZE/2-1];
    }
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc){
    if(hadc->Instance == ADC1){
        adc_info.acc = 1;
        adc_info.ptr.start = &adc_buf[FFT_SIZE];
        adc_info.ptr.end = &adc_buf[ADC_DMA_SIZE-1];
    }
}
