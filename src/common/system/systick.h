# ifndef _SYSTICK_H
# define _SYSTICK_H

# include "globals.h"

void        systickInit(void);
uint8_t     systickGetIrqChannel(void);
uint16_t    systickGet(void);
uint16_t    systickGetSub(uint16_t c);
void        systickLoading(uint16_t *loading_time, uint16_t time);

# endif
