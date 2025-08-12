#include "stm8s.h"
// #include "stm8s_adc1.h"

/***********************************************************************************
*	Функция чтения АЦП.
*	ADC_Channel_Number - Номер канала АЦП
*	amount - Сколько раз подряд считать значение АЦП и вернуть суммарное значение(не более 64).
************************************************************************************/
// uint16_t ADC_read(ADC1_Channel_TypeDef ADC_Channel_Number, uint8_t amount)
// {
// 	unsigned int result = 0;
// 	ADC1_DeInit();
// 	ADC1_DataBufferCmd(DISABLE);
// 	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS,
// 						 ADC_Channel_Number,
// 						 ADC1_PRESSEL_FCPU_D18,
// 						 ADC1_EXTTRIG_TIM, DISABLE,
// 						 ADC1_ALIGN_RIGHT,
// 						 ADC1_SCHMITTTRIG_ALL, DISABLE);
// 	ADC1_Cmd(ENABLE);

// 	for (; amount > 0; amount--)
// 	{
// 		ADC1_StartConversion();
// 		while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == FALSE);
// 		result += ADC1_GetConversionValue();
// 		ADC1_ClearFlag(ADC1_FLAG_EOC);
// 	}

// 	ADC1_DeInit();
// 	return result;
// /* Пример:
// uint8_t ADC_ntc_current = ADC_read(NTC_CHANNEL, 10);
// */
// }


  #define ADC1_PRESSEL_FCPU_D2  0x00	/**< Prescaler selection fADC1 = fcpu/2 */
  #define ADC1_PRESSEL_FCPU_D3  0x10	/**< Prescaler selection fADC1 = fcpu/3 */
  #define ADC1_PRESSEL_FCPU_D4  0x20	/**< Prescaler selection fADC1 = fcpu/4 */
  #define ADC1_PRESSEL_FCPU_D6  0x30	/**< Prescaler selection fADC1 = fcpu/6 */
  #define ADC1_PRESSEL_FCPU_D8  0x40	/**< Prescaler selection fADC1 = fcpu/8 */
  #define ADC1_PRESSEL_FCPU_D10 0x50	/**< Prescaler selection fADC1 = fcpu/10 */
  #define ADC1_PRESSEL_FCPU_D12 0x60	/**< Prescaler selection fADC1 = fcpu/12 */
  #define ADC1_PRESSEL_FCPU_D18 0x70	/**< Prescaler selection fADC1 = fcpu/18 */
	#define ADC1_FLAG_EOC (uint8_t)0x80	/**< EOC falg */

/************************************************************************
*	Функция "одиночного" чтения АЦП, без использования буфера.
*	ADC_Channel_Number - Номер канала АЦП
*	amount - Сколько раз подряд считать значение АЦП и вернуть суммарное значение(не более 64).
************************************************************************/
uint16_t ADC_read_x(uint8_t ADC_Channel_Number, uint8_t amount)
{
	// disableInterrupts(); //выключаем прерывания
	uint16_t val = 0;
	uint16_t result = 0;
	#define ADC1_BaseAddress        0x53E0
	#define ADC1 ((ADC1_TypeDef *) ADC1_BaseAddress)
	ADC1->CSR &= (uint8_t)(~ADC1_CSR_CH);// Clear the ADC1 channels
	ADC1->CSR |= (uint8_t)(ADC_Channel_Number); // Select the ADC1 channel
	ADC1->CR1 &= (uint8_t)(~ADC1_CR1_SPSEL); // Clear the SPSEL bits
	ADC1->CR1 |= (uint8_t)(ADC1_PRESSEL_FCPU_D18); // Выберите коэффициент деления прескалера в соответствии со значениями ADC1_PrescalerSelection
	ADC1->CR2 |= (1<<3); //данные выравниваются справа
	ADC1->CR1 |= (1<<0); //включение АЦП
	// ADC1->CR2|=ADC_CR2_CAL; //включение калибровки !!! изучить
	// while((ADC1->CR2&ADC_CR2_CAL)==ADC_CR2_CAL); //ожидание завершения калибровки !!! изучить
	
	for (; amount > 0; amount--)//считывание данных из АЦП
	{
		val = 0;
		ADC1->CR1 |= (1<<0); //запуск преобразования
		while(((ADC1->CSR)&(1<<7))== 0); //ожидание завершения преобразования
		val |= (unsigned int)ADC1->DRL;
		val |= (unsigned int)ADC1->DRH<<8;
		val &= 0x03ff; //результат
		result += val;
		
		#if(1) //* Clear the specified ADC1 Flag.
			uint8_t temp = 0;
			if ((ADC1_FLAG_EOC & 0x0F) == 0x01) {ADC1->CR3 &= (uint8_t)(~ADC1_CR3_OVR);} /* Clear OVR flag status */
			else if ((ADC1_FLAG_EOC & 0xF0) == 0x10)
			{
				temp = (uint8_t)(ADC1_FLAG_EOC & (uint8_t)0x0F);/* Clear analog watchdog channel status */
				if (temp < 8) {ADC1->AWSRL &= (uint8_t)~(uint8_t)((uint8_t)1 << temp);}
				else {ADC1->AWSRH &= (uint8_t)~(uint8_t)((uint8_t)1 << (temp - 8));}
			}
			else {ADC1->CSR &= (uint8_t) (~ADC1_FLAG_EOC);} /* Clear EOC | AWD flag status */
		#endif
	}
	ADC1->CR1 &= ~(1<<0); //остановка преобразования
	// enableInterrupts(); //включаем прерывания
	return result;

	/*
	ui
	Напряжение можно определить умножением считанного из АЦП значения на VCC/1023, где VCC - напряжение питания на шине 3.3V.
	512 * 3.3 / 1023 = 1.65V.
	*/
}

