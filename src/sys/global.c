#include "global.h"

// __attribute__((section(".RAM_D2"))) volatile uint16_t adc_buf1[4096];
uint16_t adc_buf1[4096];
__attribute__((section(".RAM_D2"))) volatile uint16_t adc_buf2[4096];
