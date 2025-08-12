#include "stm8s.h"

#define ADC_U_DC_GPIO_PORT  (GPIOB) // Порт для измерения напряжения блока питания
#define ADC_U_DC_GPIO_PINS  (GPIO_PIN_7) // Пин для измерения напряжения блока питания (ADC1_CHANNEL_7)
#define ADC_U_DC_CHANNEL		((uint8_t) 7) // ADC1_CHANNEL_7

#define ADC_U_OUT_GPIO_PORT  (GPIOB) // Порт для измерения напряжения на аккумуляторе
#define ADC_U_OUT_GPIO_PINS  (GPIO_PIN_5) // Пин для измерения напряжения на аккумуляторе (ADC1_CHANNEL_5)
#define ADC_U_OUT_CHANNEL		((uint8_t) 5) // ADC1_CHANNEL_5

#define ADC_I_GPIO_PORT  (GPIOB) // Порт для измерения тока заряда аккумулятора
#define ADC_I_GPIO_PINS  (GPIO_PIN_3) // Пин для измерения тока заряда аккумулятора (ADC1_CHANNEL_3)
#define ADC_I_CHANNEL		((uint8_t) 3) // ADC1_CHANNEL_3

#define NTC_PORT	(GPIOB) // Порт для измерения температуры NTC
#define NTC_PIN		(GPIO_PIN_4) // Пин для измерения температуры NTC (ADC1_CHANNEL_4)
#define NTC_CHANNEL		((uint8_t) 4) // ADC1_CHANNEL_4

#define PWM_PORT	(GPIOA) // Порт для управления ШИМ
#define PWM_PIN		(GPIO_PIN_3) // Пин для управления ШИМ (TIM2_CH3)

#define ON_OUT_PORT		(GPIOB) // Порт для управления выходом
#define ON_OUT_PIN		(GPIO_PIN_1) // Пин для управления выходом

#define FAN_PORT		(GPIOE) // Порт для управления вентилятором
#define FAN_PIN		(GPIO_PIN_7) // Пин для управления вентилятором

#define LED_CAR_PORT	(GPIOG)//(GPIOE) // Порт для управления светодиодом 1
#define LED_CAR_PIN 	(GPIO_PIN_1)//(GPIO_PIN_5) // Пин для управления светодиодом 1

#define LED_GEL_PORT	(GPIOG)//(GPIOC) // Порт для управления светодиодом 2
#define LED_GEL_PIN 	(GPIO_PIN_0)//(GPIO_PIN_6) // Пин для управления светодиодом 2

#define LED_MOTO_PORT	(GPIOC)//(GPIOG) // Порт для управления светодиодом 3
#define LED_MOTO_PIN 	(GPIO_PIN_6)//(GPIO_PIN_0) // Пин для управления светодиодом 3

#define LED_PULSE_PORT	(GPIOE)//(GPIOG) // Порт для управления светодиодом 4
#define LED_PULSE_PIN 	(GPIO_PIN_5)//(GPIO_PIN_1) // Пин для управления светодиодом 4

void Pin_init (void)
{
	GPIO_Init(ADC_U_DC_GPIO_PORT, ADC_U_DC_GPIO_PINS, GPIO_MODE_IN_FL_NO_IT); // Входной, плавающий, без прерывания
	GPIO_Init(ADC_U_OUT_GPIO_PORT, ADC_U_OUT_GPIO_PINS, GPIO_MODE_IN_FL_NO_IT); // Входной, плавающий, без прерывания
	GPIO_Init(ADC_I_GPIO_PORT, ADC_I_GPIO_PINS, GPIO_MODE_IN_FL_NO_IT); // Входной, плавающий, без прерывания
	GPIO_Init(NTC_PORT, NTC_PIN, GPIO_MODE_IN_FL_NO_IT); // Входной, плавающий, без прерывания
	GPIO_Init(ON_OUT_PORT, ON_OUT_PIN, GPIO_MODE_OUT_PP_LOW_FAST);	// Выходной двухтактный, низкий уровень, 10 МГц
	GPIO_Init(FAN_PORT, FAN_PIN, GPIO_MODE_OUT_PP_LOW_FAST);	// Выходной двухтактный, низкий уровень, 10 МГц
	GPIO_Init(LED_CAR_PORT, LED_CAR_PIN, GPIO_MODE_OUT_PP_LOW_FAST);	// Выходной двухтактный, низкий уровень, 10 МГц
	GPIO_Init(LED_GEL_PORT, LED_GEL_PIN, GPIO_MODE_OUT_PP_LOW_FAST);	// Выходной двухтактный, низкий уровень, 10 МГц
	GPIO_Init(LED_MOTO_PORT, LED_MOTO_PIN, GPIO_MODE_OUT_PP_LOW_FAST);	// Выходной двухтактный, низкий уровень, 10 МГц
	GPIO_Init(LED_PULSE_PORT, LED_PULSE_PIN, GPIO_MODE_OUT_PP_LOW_FAST);	// Выходной двухтактный, низкий уровень, 10 МГц
}

