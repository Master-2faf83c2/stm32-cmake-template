// #include "drv_debug.h"
// #include "foc.h"
// #include "encoder.h"

// #define PI                      3.1415926f
// #define TWO_PI                  6.2831853f

// FOC_HandleTypeDef foc;

// uint16_t cnt = 0;
// uint8_t target_mode = 0;

// void ApplicationInit(void){
//     DebugInit();
//     FOC_Init(&foc, 7.0f, 12.0f, 7199.0f);
// }

// void ApplicationHandle(void){
//     DebugRxCirculation();
//     uint16_t raw = 0;

//     if (EncoderGet(&raw)){
//         cnt ++;
//         if (cnt >= 250){
//             target_mode ++;
//             target_mode %= 5;
//             cnt = 0;
//         }

//         float current_angle = EncoderToRad(raw);
//         float target_angle = 0.0f;

//         if (target_mode == 0){
//             target_angle = 0.0f;
//         }else if (target_mode == 1){
//             target_angle = 72.0f;
//         }else if (target_mode == 2){
//             target_angle = 144.0f;
//         }else if (target_mode == 3){
//             target_angle = 216.0f;
//         }else if (target_mode == 4){
//             target_angle = 288.0f;
//         }
//         float target = target_angle * PI / 180.0f;

//         float error = target - current_angle;

//         while (error > PI)  error -= TWO_PI;
//         while (error < -PI) error += TWO_PI;

//         float Uq = error * 1.0f;

//         if (Uq > 0.8f)  Uq = 0.8f;
//         if (Uq < -0.8f) Uq = -0.8f;

//         FOC_Compute(&foc, current_angle, Uq, 0.0f);

//         DebugPrintf("当前角度: %f\r\n", EncoderToAngleDeg(raw));
//     }

//     HAL_Delay(1);
// }


#include "drv_debug.h"
#include "foc.h"
#include "encoder.h"

#define PI                      3.1415926f
#define TWO_PI                  6.2831853f

FOC_HandleTypeDef foc;

static uint8_t target_mode = 0;

// 最终目标角度
static float target_final = 0.0f;

// 平滑后的目标角度
static float target_smooth = 0.0f;

// 上一次当前角度
static float last_angle = 0.0f;

// 上一次Uq
static float last_uq = 0.0f;

// 时间记录
static uint32_t last_ctrl_tick = 0;
static uint32_t last_change_tick = 0;
static uint32_t last_print_tick = 0;

// 第一次运行标志
static uint8_t motor_started = 0;

// 位置环参数
static float pos_kp = 1.5f;
static float pos_kd = 0.03f;

// 最大Uq电压
static float max_uq = 0.8f;

// 最大目标角速度，单位 rad/s
// 90度/s
static float max_target_speed = 90.0f * PI / 180.0f;

// Uq变化速度限制，单位 V/s
static float max_uq_slew = 4.0f;

static float Limit(float v, float min, float max)
{
    if (v > max) return max;
    if (v < min) return min;
    return v;
}

static float NormalizeAngle(float angle)
{
    while (angle >= TWO_PI) angle -= TWO_PI;
    while (angle < 0.0f)    angle += TWO_PI;
    return angle;
}

// 返回 -PI ~ +PI 的最短角度误差
static float AngleError(float target, float current)
{
    float error = target - current;

    while (error > PI)  error -= TWO_PI;
    while (error < -PI) error += TWO_PI;

    return error;
}

static float DegToRad(float deg)
{
    return deg * PI / 180.0f;
}

static float TargetModeToRad(uint8_t mode)
{
    if (mode == 0) return DegToRad(0.0f);
    if (mode == 1) return DegToRad(72.0f);
    if (mode == 2) return DegToRad(144.0f);
    if (mode == 3) return DegToRad(216.0f);
    if (mode == 4) return DegToRad(288.0f);

    return 0.0f;
}

void ApplicationInit(void)
{
    DebugInit();

    FOC_Init(&foc, 7.0f, 12.0f, 7199.0f);
}

void ApplicationHandle(void)
{
    DebugRxCirculation();

    uint16_t raw = 0;

    if (!EncoderGet(&raw)){
        return;
    }

    float current_angle = EncoderToRad(raw);

    uint32_t now = HAL_GetTick();

    float dt = 0.001f;

    if (last_ctrl_tick != 0){
        dt = (now - last_ctrl_tick) / 1000.0f;

        if (dt <= 0.0f || dt > 0.1f){
            dt = 0.001f;
        }
    }

    last_ctrl_tick = now;

    /*
     * 第一次进入时，把目标初始化成当前角度
     * 避免上电瞬间猛冲
     */
    if (!motor_started){
        target_final = current_angle;
        target_smooth = current_angle;
        last_angle = current_angle;
        last_uq = 0.0f;
        last_change_tick = now;
        motor_started = 1;
    }

    /*
     * 每1000ms切换一次目标角度
     * 不要用cnt计数，因为DebugPrintf和EncoderGet会影响循环周期
     */
    if (now - last_change_tick >= 1000){
        target_mode++;
        target_mode %= 5;

        target_final = TargetModeToRad(target_mode);

        last_change_tick = now;
    }

    /*
     * 让 target_smooth 缓慢靠近 target_final
     */
    float target_err = AngleError(target_final, target_smooth);
    float max_step = max_target_speed * dt;

    if (target_err > max_step){
        target_smooth += max_step;
    }else if (target_err < -max_step){
        target_smooth -= max_step;
    }else{
        target_smooth = target_final;
    }

    target_smooth = NormalizeAngle(target_smooth);

    /*
     * 计算当前位置误差
     */
    float pos_error = AngleError(target_smooth, current_angle);

    /*
     * 计算实际速度
     * 注意角度也要用最短角度差
     */
    float speed = AngleError(current_angle, last_angle) / dt;
    last_angle = current_angle;

    /*
     * PD位置控制
     *
     * P：负责往目标转
     * D：抑制速度，防止冲过头
     */
    float Uq = pos_kp * pos_error - pos_kd * speed;

    /*
     * 限制最大电压
     */
    Uq = Limit(Uq, -max_uq, max_uq);

    /*
     * 限制Uq变化速度，让电压输出更柔和
     */
    float max_uq_step = max_uq_slew * dt;

    if (Uq > last_uq + max_uq_step){
        Uq = last_uq + max_uq_step;
    }else if (Uq < last_uq - max_uq_step){
        Uq = last_uq - max_uq_step;
    }

    last_uq = Uq;

    FOC_Compute(&foc, current_angle, Uq, 0.0f);

    /*
     * 打印不要每1ms打印一次，会严重影响控制流畅度
     * 这里100ms打印一次
     */
    if (now - last_print_tick >= 100){
        last_print_tick = now;

        DebugPrintf("mode=%d, cur=%f, tar=%f, smooth=%f, uq=%f\r\n",
                    target_mode,
                    EncoderToAngleDeg(raw),
                    target_final * 180.0f / PI,
                    target_smooth * 180.0f / PI,
                    Uq);
    }

    HAL_Delay(1);
}