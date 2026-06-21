#ifndef _PID_H
#define _PID_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#include <math.h>

float PID_Position(float kp, float target, float current);

#ifdef __cplusplus
}
#endif

#endif
