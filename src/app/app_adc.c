#include "app_adc.h"
#include "app_debug.h"
#include "tim.h"
#include "adc.h"

typedef struct{
    uint8_t acc;
    float voltage;
}AdcInfo_t;

AdcInfo_t adc_info;

void appAdcInit(void){
    HAL_StatusTypeDef res = HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);
    if (res != HAL_OK){
        appDebugPrintf("ADC 校准失败!\r\n");
    }
    res = HAL_ADC_Start_DMA(&hadc1,(uint32_t *)&adc_buf1,4096);
    if (res != HAL_OK){
        appDebugPrintf("ADC DMA 初始化失败!\r\n");
    }
    // HAL_ADC_Start_IT(&hadc1);
    HAL_TIM_Base_Start(&htim3);
    adc_info.acc = 0;
}

uint8_t appAdcGetAcc(void){
    if (adc_info.acc){
        adc_info.acc = 0;
        return 1;
    }
    return 0;
}

float appAdcGetVoltage(void){
    return adc_info.voltage;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc){
    if(hadc->Instance == ADC1){
        adc_info.acc = 1;
        HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_3);
    }
    // if (hadc->Instance == ADC1){
    //     uint16_t adc_value = HAL_ADC_GetValue(hadc);
    //     adc_info.voltage = adc_value * 3.3f / 65535.0f;  // 电压换算
    //     adc_info.acc = 1;

    //     HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_3);  // LED 翻转，可选
    //     HAL_ADC_Start_IT(hadc);  // 再次启动 ADC 中断
    // }
}

// void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc){
//     if(hadc->Instance == ADC1){
//         HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_3);
//     }
// }
