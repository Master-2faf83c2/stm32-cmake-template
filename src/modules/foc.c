#include "foc.h"
#include "encoder.h"
#include "drv_debug.h"

#include "tim.h"

#define EN_PIN      GPIO_PIN_12
#define EN_PORT     GPIOB

#define PI          3.1415926f
#define TWO_PI      6.2831853f
#define SQRT3_2     0.8660254f

#define FOC_ALIGN_VOLTAGE   2.0f
#define FOC_ALIGN_ANGLE     0.0f

static float NormalizeAngle(float angle){
    while (angle >= TWO_PI) angle -= TWO_PI;
    while (angle < 0.0f)    angle += TWO_PI;
    return angle;
}

static float Limit(float v, float min, float max){
    if (v > max)
        return max;
    if (v < min)
        return min;
    return v;
}

static void FOC_SetPwm(uint16_t a, uint16_t b, uint16_t c){
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, a);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, b);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, c);
}

static void FOC_SetVoltageAngle(
    FOC_HandleTypeDef *foc,
    float theta_e,
    float Uq,
    float Ud
)
{
    float sin_t, cos_t;
    float Ualpha, Ubeta;
    float Ua, Ub, Uc;
    float duty_a, duty_b, duty_c;

    theta_e = NormalizeAngle(theta_e);

    sin_t = sinf(theta_e);
    cos_t = cosf(theta_e);

    // dq -> alpha beta
    Ualpha = Ud * cos_t - Uq * sin_t;
    Ubeta  = Ud * sin_t + Uq * cos_t;

    // alpha beta -> 三相
    Ua = Ualpha;
    Ub = -0.5f * Ualpha + SQRT3_2 * Ubeta;
    Uc = -0.5f * Ualpha - SQRT3_2 * Ubeta;

    // 三相电压 -> 占空比
    duty_a = Ua / foc->vbus + 0.5f;
    duty_b = Ub / foc->vbus + 0.5f;
    duty_c = Uc / foc->vbus + 0.5f;

    duty_a = Limit(duty_a, 0.0f, 1.0f);
    duty_b = Limit(duty_b, 0.0f, 1.0f);
    duty_c = Limit(duty_c, 0.0f, 1.0f);

    uint16_t pwm_a = (uint16_t)(duty_a * foc->pwm_period);
    uint16_t pwm_b = (uint16_t)(duty_b * foc->pwm_period);
    uint16_t pwm_c = (uint16_t)(duty_c * foc->pwm_period);

    FOC_SetPwm(pwm_a, pwm_b, pwm_c);
}

static float FOC_ReadAverageAngle(){
    float sin_sum = 0.0f;
    float cos_sum = 0.0f;

    for (int i = 0; i < 32;){
        uint16_t raw = 0;
        if (EncoderGet(&raw)){
            float rad = EncoderToRad(raw);
            sin_sum += sinf(rad);
            cos_sum += cosf(rad);
            i++;
        }
        HAL_Delay(2);
    }
    return NormalizeAngle(atan2f(sin_sum, cos_sum));
}

void FOC_Init(FOC_HandleTypeDef *foc, float pole_pairs, float vbus, float pwm_period){
    foc->pole_pairs = pole_pairs;
    foc->vbus = vbus;
    foc->pwm_period = pwm_period;
    foc->zero_offset = 2.779906f;
    EncoderInit();
    FOC_Disable();
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    FOC_Enable();
    // FOC_CalibrateZero(foc);
}

void FOC_Enable(){
    HAL_GPIO_WritePin(EN_PORT, EN_PIN, GPIO_PIN_SET);
}

void FOC_Disable(){
    HAL_GPIO_WritePin(EN_PORT, EN_PIN, GPIO_PIN_RESET);
}

void FOC_Compute(
    FOC_HandleTypeDef *foc, 
    float encoder_angle,
    float Uq,
    float Ud
)
{
    float theta_e;

    // 机械角度 -> 电角度
    theta_e = encoder_angle * foc->pole_pairs + foc->zero_offset;
    theta_e = NormalizeAngle(theta_e);

    FOC_SetVoltageAngle(foc, theta_e, Uq, Ud);
}

void FOC_CalibrateZero(FOC_HandleTypeDef *foc){
    float encoder_angle;
    float zero_offset;

    if (foc == NULL){
        return;
    }

    FOC_Enable();

    foc->zero_offset = 0.0f;

    //缓慢增加Ud电压，让电机吸到固定电角度0, 这个过程电机会轻微转动一下并锁住
    for (int i = 0; i <= 100; i++){
        float v = FOC_ALIGN_VOLTAGE * ((float)i / 100.0f);

        FOC_SetVoltageAngle(
            foc,
            FOC_ALIGN_ANGLE,
            0.0f,
            v
        );

        HAL_Delay(3);
    }

    //保持一段时间，等待转子稳定
    for (int i = 0; i < 300; i++){
        FOC_SetVoltageAngle(
            foc,
            FOC_ALIGN_ANGLE,
            0.0f,
            FOC_ALIGN_VOLTAGE
        );

        HAL_Delay(3);
    }

    encoder_angle = FOC_ReadAverageAngle();
    zero_offset = FOC_ALIGN_ANGLE - encoder_angle * foc->pole_pairs;
    zero_offset = NormalizeAngle(zero_offset);

    foc->zero_offset = zero_offset;

    FOC_SetVoltageAngle(foc, 0.0f, 0.0f, 0.0f);

    DebugPrintf("zero_offset: %f, 校准完成!\r\n", zero_offset);
}
