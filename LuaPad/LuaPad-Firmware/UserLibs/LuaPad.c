#include "LuaPad.h"

uint16_t ADCConvertedValue[9];

void PAD_LED(uint8_t _index, LED_status _status)
{
    switch(_index)
    {
        case 1:
            switch(_status)
            {
                case ON:
                    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
                    break;
                case OFF:
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
                    break;
                case TURN:
                    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);
                    break;
            }
            break;
        case 2:
            switch(_status)
            {
                case ON:
                    PAD_PWM(5,999);
                    break;
                case OFF:
                    PAD_PWM(5,0);
                    break;
				case TURN:
                    break;
            }
            break;
        case 3:
            switch(_status)
            {
                case ON:
                    PAD_PWM(6,999);
                    break;
                case OFF:
                    PAD_PWM(6,0);
                    break;
                case TURN:
                    break;
            }
            break;
    }
}




GPIO_PinState PAD_Key(void)
{
    return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12);
}

uint16_t PAD_ADC(int _channel)
{
			return ADCConvertedValue[_channel];
}

void PAD_PWM(int _channel,int _value)
{
	if(_value>999)
		_value=999;
	switch(_channel)
	{
		case 0:
			TIM3->CCR2 = (uint32_t)_value;
			break;
//		case 1:
//			TIM4->CCR3 = (uint32_t)_value;
//			break;
//		case 2:
//			TIM4->CCR4 = (uint32_t)_value;
//			break;
		case 3:
			TIM5->CCR1 = (uint32_t)_value;
			break;
		case 4:
			TIM5->CCR2 = (uint32_t)_value;
			break;
		case 5:
			TIM12->CCR1 = (uint32_t)_value;
			break;
		case 6:
			TIM12->CCR2 = (uint32_t)_value;
			break;
	}
}
