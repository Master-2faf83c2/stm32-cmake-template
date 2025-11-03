#ifndef _APP_ADC_H
#define _APP_ADC_H

#include "global.h"

void appAdcInit(void);
void appFFTLoop(void);
void appAdcTest(void);

uint8_t appAdcGetAcc(void);

#endif
