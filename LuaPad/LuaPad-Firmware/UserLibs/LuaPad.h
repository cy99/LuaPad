#ifndef __LUAPAD_H
#define __LUAPAD_H
#include "mInclude.h"

#define LED2 5
#define LED3 6
#define RES 8

typedef enum
{
	ON,
	OFF,
	TURN
}LED_status;

uint16_t PAD_ADC(int _channel);
void PAD_PWM(int _channel,int _value);
void PAD_LED(uint8_t _index,LED_status _status);
GPIO_PinState PAD_Key(void);

#endif
