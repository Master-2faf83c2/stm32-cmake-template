#include "app_adc.h"
#include "app_debug.h"
#include "tim.h"
#include "adc.h"
#include "arm_math.h"

#define N                   1024
#define Fs                  60000
#define FreqResolution      (Fs / N)
#define ADC_DMA_SIZE        (N*2)

typedef struct{
    volatile uint16_t    *start;
    volatile uint16_t    *end;
}DMA_BufferPtr16_t;

typedef struct{
    DMA_BufferPtr16_t ptr;
    uint8_t acc;
}AdcInfo_t;

RAM_D2 volatile uint16_t adc_buf[ADC_DMA_SIZE];
// RFFT 输出长度=N
RAM_D2 float fft_out[N];
// 幅度结果
RAM_D2 float fft_mag[N/2];

AdcInfo_t adc_info;
float fft_in[N];      

arm_rfft_fast_instance_f32 fft_inst;

static void appAdcInfoInit(void){
    adc_info.acc = 0;
    adc_info.ptr.start = &adc_buf[0];
    adc_info.ptr.end = &adc_buf[N-1];
}

void appAdcInit(void){
    appAdcInfoInit();
    arm_rfft_fast_init_f32(&fft_inst, N);
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

void appFFTLoop(void){
    if (!appAdcGetAcc())
        return;
    volatile uint16_t *p = adc_info.ptr.start;
    for (int i = 0; i < N; i++) {
        fft_in[i] = ((float)p[i]) * (3.3f / 65535.0f);  // 16bit ADC 转换到电压
    }

    arm_rfft_fast_f32(&fft_inst, fft_in, fft_out, 0);

    for (int i = 0; i < N/2; i++){
        float real = fft_out[2*i];
        float imag = fft_out[2*i+1];
        fft_mag[i] = sqrtf(real*real + imag*imag);
    }

    uint32_t peakIndex = 0;
    float peakValue = 0;

    for (int i = 1; i < N/2; i++) {
        if (fft_mag[i] > peakValue) {
            peakValue = fft_mag[i];
            peakIndex = i;
        }
    }

    float freqRes = (float)Fs / N;
    float peakFreq = peakIndex * freqRes;

    appDebugPrintf("Peak Freq = %.2f Hz (Index=%d, Mag=%.3f)\r\n",
        peakFreq, peakIndex, peakValue);
}

void appAdcTest(void){
    if (!appAdcGetAcc())
        return;
    volatile uint16_t *p = adc_info.ptr.start;
    for (uint32_t i = 0; i < N; i ++){
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
        adc_info.ptr.start = &adc_buf[0];
        adc_info.ptr.end = &adc_buf[ADC_DMA_SIZE/2-1];
        adc_info.acc = 1;
    }
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc){
    if(hadc->Instance == ADC1){
        adc_info.ptr.start = &adc_buf[N];
        adc_info.ptr.end = &adc_buf[ADC_DMA_SIZE-1];
        adc_info.acc = 1;
    }
}
