#ifndef _APP_ADC_H
#define _APP_ADC_H

#include "global.h"

void adcInit(void);
void fftLoop(void);
void adcTest(void);

uint8_t adcGetAcc(void);

#endif
