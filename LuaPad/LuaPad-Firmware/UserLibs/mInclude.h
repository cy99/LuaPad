#ifndef __MINCLUDE_H
#define __MINCLUDE_H

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <math.h>

extern __IO uint32_t SysTime;
extern __IO uint8_t ErrorCode;

#ifdef USE_MICROS
void delay(uint32_t _delay);
uint32_t micros(void);
#endif
uint32_t millis(void);

#endif
