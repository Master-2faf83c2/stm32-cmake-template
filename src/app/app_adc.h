#ifndef _APP_ADC_H
#define _APP_ADC_H

#include "global.h"

void appAdcInit(void);

uint8_t appAdcGetAcc(void);
float appAdcGetVoltage(void);

#endif
