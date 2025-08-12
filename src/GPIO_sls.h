#include "stm8s.h"

#define ADC_U_DC_GPIO_PORT  (GPIOB)
#define ADC_U_DC_GPIO_PINS  (GPIO_PIN_7) // ADC1_CHANNEL_7
#define ADC_U_DC_CHANNEL		((uint8_t) 7) // ADC1_CHANNEL_7

#define ADC_U_OUT_GPIO_PORT  (GPIOB)
#define ADC_U_OUT_GPIO_PINS  (GPIO_PIN_5) // ADC1_CHANNEL_5
#define ADC_U_OUT_CHANNEL		((uint8_t) 5) // ADC1_CHANNEL_5

#define ADC_I_GPIO_PORT  (GPIOB) 
#define ADC_I_GPIO_PINS  (GPIO_PIN_3) // ADC1_CHANNEL_3
#define ADC_I_CHANNEL		((uint8_t) 3) // ADC1_CHANNEL_3

#define NTC_PORT	(GPIOB) 
#define NTC_PIN		(GPIO_PIN_4) // ADC1_CHANNEL_4
#define NTC_CHANNEL		((uint8_t) 4) // ADC1_CHANNEL_4

#define PWM_PORT	(GPIOA)
#define PWM_PIN		(GPIO_PIN_3) // TIM2_CH3 PWM

#define ON_OUT_PORT		(GPIOB) 
#define ON_OUT_PIN		(GPIO_PIN_1)

#define FAN_PORT		(GPIOE)
#define FAN_PIN		(GPIO_PIN_7)

#define LED_CAR_PORT	(GPIOG) // Led 1
#define LED_CAR_PIN 	(GPIO_PIN_1)// Led 1

#define LED_GEL_PORT	(GPIOG) // Led 2
#define LED_GEL_PIN 	(GPIO_PIN_0)// Led 2

#define LED_MOTO_PORT	(GPIOC)// Led 3
#define LED_MOTO_PIN 	(GPIO_PIN_6)// Led 3

#define LED_PULSE_PORT	(GPIOE)// Led 4
#define LED_PULSE_PIN 	(GPIO_PIN_5)// Led 4

void Pin_init (void)
{
	GPIO_Init(ADC_U_DC_GPIO_PORT, ADC_U_DC_GPIO_PINS, GPIO_MODE_IN_FL_NO_IT); // Input, floating, no interruption
	GPIO_Init(ADC_U_OUT_GPIO_PORT, ADC_U_OUT_GPIO_PINS, GPIO_MODE_IN_FL_NO_IT); // Input, floating, no interruption
	GPIO_Init(ADC_I_GPIO_PORT, ADC_I_GPIO_PINS, GPIO_MODE_IN_FL_NO_IT); // Input, floating, no interruption
	GPIO_Init(NTC_PORT, NTC_PIN, GPIO_MODE_IN_FL_NO_IT); // Input, floating, no interruption
	GPIO_Init(ON_OUT_PORT, ON_OUT_PIN, GPIO_MODE_OUT_PP_LOW_FAST);	// Output two-cycle, low level, 10 MHz
	GPIO_Init(FAN_PORT, FAN_PIN, GPIO_MODE_OUT_PP_LOW_FAST);	// Output two-cycle, low level, 10 MHz
	GPIO_Init(LED_CAR_PORT, LED_CAR_PIN, GPIO_MODE_OUT_PP_LOW_FAST);	// Output two-cycle, low level, 10 MHz
	GPIO_Init(LED_GEL_PORT, LED_GEL_PIN, GPIO_MODE_OUT_PP_LOW_FAST);	// Output two-cycle, low level, 10 MHz
	GPIO_Init(LED_MOTO_PORT, LED_MOTO_PIN, GPIO_MODE_OUT_PP_LOW_FAST);	// Output two-cycle, low level, 10 MHz
	GPIO_Init(LED_PULSE_PORT, LED_PULSE_PIN, GPIO_MODE_OUT_PP_LOW_FAST);	// Output two-cycle, low level, 10 MHz
}

