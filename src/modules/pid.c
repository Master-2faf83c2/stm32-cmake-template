#include "pid.h"

float PID_Position(float kp, float target, float current){
    float error = target - current;

    return (kp * error);
}

