// * Author: slaventiis (Vyacheslav Zholobov)
#include "stm8s.h"

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
* Function of “single” ADC reading, without using buffer.
* ADC_Channel_Number - ADC channel number
* amount - How many times in a row to read ADC value and return the total value (max. 64).
************************************************************************/
uint16_t ADC_read_x(uint8_t ADC_Channel_Number, uint8_t amount)
{
	// disableInterrupts();
	uint16_t val = 0;
	uint16_t result = 0;
	#define ADC1_BaseAddress        0x53E0
	#define ADC1 ((ADC1_TypeDef *) ADC1_BaseAddress)
	ADC1->CSR &= (uint8_t)(~ADC1_CSR_CH);// Clear the ADC1 channels
	ADC1->CSR |= (uint8_t)(ADC_Channel_Number); // Select the ADC1 channel
	ADC1->CR1 &= (uint8_t)(~ADC1_CR1_SPSEL); // Clear the SPSEL bits
	ADC1->CR1 |= (uint8_t)(ADC1_PRESSEL_FCPU_D18); // Select the prescaler division ratio according to the ADC1_PrescalerSelection values
	ADC1->CR2 |= (1<<3); // data is right-aligned
	ADC1->CR1 |= (1<<0); // ADC enable
	// ADC1->CR2|=ADC_CR2_CAL; // turning on calibration !!! study
	// while((ADC1->CR2&ADC_CR2_CAL)==ADC_CR2_CAL); // waiting for calibration to complete !!! study
	
	for (; amount > 0; amount--)
	{
		val = 0;
		ADC1->CR1 |= (1<<0); // conversion start
		while(((ADC1->CSR)&(1<<7))== 0); // waiting for conversion to complete
		val |= (unsigned int)ADC1->DRL;
		val |= (unsigned int)ADC1->DRH<<8;
		val &= 0x03ff; // result
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
	ADC1->CR1 &= ~(1<<0); // conversion stop
	// enableInterrupts();
	return result;
}

