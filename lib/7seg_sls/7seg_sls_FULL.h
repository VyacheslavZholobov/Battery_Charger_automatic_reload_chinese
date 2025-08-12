// * Author: slaventiis (Vyacheslav Zholobov)
#ifdef USE_STDPERIPH_DRIVER
#include "stm8s.h"
#endif
#ifdef ARDUINO
#include <Arduino.h>
#endif

#if 1 //^ Settings
	#define COMMON_CATODE // Comment if indicator with common anode (+)
	#define IND_AMOUNT_NUM 3 // Number of digits in the indicator - without ticker line
	#define IND_AMOUNT_STR 14 // Number of digits in the ticker.
	// we want to see in the ticker: U14.2 A10.0 t25.0 = 14 (digits + letters + whites)

	#ifdef USE_STDPERIPH_DRIVER
		#define SEG_A_P		(GPIOD)
		#define SEG_A			(GPIO_PIN_4)	
		#define SEG_B_P		(GPIOD)
		#define SEG_B			(GPIO_PIN_6)
		#define SEG_C_P		(GPIOE)
		#define SEG_C			(GPIO_PIN_0)
		#define SEG_D_P		(GPIOD)
		#define SEG_D			(GPIO_PIN_1)
		#define SEG_E_P		(GPIOD)
		#define SEG_E			(GPIO_PIN_2)
		#define SEG_F_P		(GPIOD)
		#define SEG_F			(GPIO_PIN_5)
		#define SEG_G_P		(GPIOE)
		#define SEG_G			(GPIO_PIN_3)
		#define SEG_Dp_P	(GPIOD)  
		#define SEG_Dp		(GPIO_PIN_0)
		// Numbering of digits from right to left
		#define DIG_1_P		(GPIOC)
		#define DIG_1			(GPIO_PIN_7)
		#define DIG_2_P		(GPIOD)
		#define DIG_2			(GPIO_PIN_7)
		#define DIG_3_P		(GPIOD)
		#define DIG_3			(GPIO_PIN_3)
		#if (IND_AMOUNT_NUM > 3)
			#define DIG_4_P		(GPIOD)
			#define DIG_4			(GPIO_PIN_4)	// Dig4	
		#endif
		#if (IND_AMOUNT_NUM > 4)
			#define DIG_5_P		(GPIOD)
			#define DIG_5			(GPIO_PIN_6)	// Dig5
		#endif
		#if (IND_AMOUNT_NUM > 5)
			#define DIG_6_P		(GPIOD)
			#define DIG_6			(GPIO_PIN_5)	// Dig6
		#endif
	#endif

	#ifdef ARDUINO
		#define SEG_A			2
		#define SEG_B			3
		#define SEG_C			4
		#define SEG_D			5
		#define SEG_E			6
		#define SEG_F			7
		#define SEG_G			8
		#define SEG_Dp		9
		// Numbering of digits from right to left
		#define DIG_1			10
		#define DIG_2			11
		#define DIG_3			12
		#if (IND_AMOUNT_NUM > 3)
			#define DIG_4			
		#endif
		#if (IND_AMOUNT_NUM > 4)
			#define DIG_5			
		#endif
		#if (IND_AMOUNT_NUM > 5)
			#define DIG_6			
		#endif
	#endif
#endif // Settings

#if 1 //^ Variables
uint8_t ind_buf[IND_AMOUNT_STR]; // 7-segment indicator buffer
uint8_t count_digit_num = 0; // Count of active digit for indication
uint8_t shift = 0; // Shift for running line
uint16_t speed_shift = 0; // Speed for the ticker. Smaller value - faster ticker.
	/* Digit code in 7-segment display format */
	const uint8_t Dig_code[] = {
		//0bABCDEFGdP
			0b11111100, // 0
			0b01100000, // 1
			0b11011010, // 2
			0b11110010, // 3
			0b01100110, // 4
			0b10110110, // 5
			0b10111110, // 6
			0b11100000, // 7
			0b11111110, // 8
			0b11110110  // 9
	};

	#define S_A		0b10000000 // Код сегмента "A"
	#define S_B		0b01000000 // Код сегмента "B"
	#define S_C		0b00100000 // Код сегмента "C"
	#define S_D		0b00010000 // Код сегмента "D"
	#define S_E		0b00001000 // Код сегмента "E"
	#define S_F		0b00000100 // Код сегмента "F"
	#define S_G		0b00000010 // Код сегмента "G"
	#define S_Dp	0b00000001 // Код сегмента "."
	#define S_MINUS	S_G // Код сегмента "-"
	/*
	  A
	F   B
	  G
	E   C
	  D  Dp
	 */
								 //0bABCDEFG.
	#define symbol_0 0b11111100
	#define symbol_1 0b01100000
	#define symbol_2 0b11011010
	#define symbol_3 0b11110010
	#define symbol_4 0b01100110
	#define symbol_5 0b10110110
	#define symbol_6 0b10111110
	#define symbol_7 0b11100000
	#define symbol_8 0b11111110
	#define symbol_9 0b11110110
	#define symbol_A 0b11101110
	#define symbol_b 0b00111110
	#define symbol_c 0b00011010
	#define symbol_C 0b10011100
	#define symbol_d 0b01111010
	#define symbol_E 0b10011110
	#define symbol_F 0b10001110
	#define symbol_H 0b01101110
	#define symbol_i 0b00100000
	#define symbol_L 0b00011100
	#define symbol_n 0b00101010
	#define symbol_o 0b00111010
	#define symbol_O 0b11111100
	#define symbol_P 0b11001110	
	#define symbol_r 0b00001010
	#define symbol_S 0b10110110
	#define symbol_t 0b00011110
	#define symbol_u 0b00111000
	#define symbol_U 0b01111100
	#define symbol_UP_LINE 0b10000000
	#define symbol_MINUS 0b00000010
	#define symbol_DOWN_LINE 0b00010000
	#define symbol_EMPTY 0b00000000

	#ifdef COMMON_CATODE
		#ifdef USE_STDPERIPH_DRIVER
			#define SEG_A_ON	SEG_A_P->ODR |= (uint8_t)(SEG_A) // Set pin to 1
			#define SEG_B_ON	SEG_B_P->ODR |= (uint8_t)(SEG_B) // Set pin to 1
			#define SEG_C_ON	SEG_C_P->ODR |= (uint8_t)(SEG_C) // Set pin to 1
			#define SEG_D_ON	SEG_D_P->ODR |= (uint8_t)(SEG_D) // Set pin to 1
			#define SEG_E_ON	SEG_E_P->ODR |= (uint8_t)(SEG_E) // Set pin to 1
			#define SEG_F_ON	SEG_F_P->ODR |= (uint8_t)(SEG_F) // Set pin to 1
			#define SEG_G_ON	SEG_G_P->ODR |= (uint8_t)(SEG_G) // Set pin to 1
			#define SEG_Dp_ON	SEG_Dp_P->ODR |= (uint8_t)(SEG_Dp) // Set pin to 1

			#define DIG_1_ON	DIG_1_P->ODR &= (uint8_t)(~DIG_1) // Set pin to 0
			#define DIG_1_OFF	DIG_1_P->ODR |= (uint8_t)(DIG_1) // Set pin to 1
			#define DIG_2_ON	DIG_2_P->ODR &= (uint8_t)(~DIG_2) // Set pin to 0
			#define DIG_2_OFF	DIG_2_P->ODR |= (uint8_t)(DIG_2) // Set pin to 1
			#define DIG_3_ON	DIG_3_P->ODR &= (uint8_t)(~DIG_3) // Set pin to 0
			#define DIG_3_OFF	DIG_3_P->ODR |= (uint8_t)(DIG_3) // Set pin to 1
			
			#if (IND_AMOUNT_NUM > 3)
			#define DIG_4_ON	GPIO_WriteLow(DIG_4_P, DIG_4) // Set pin to 0
			#define DIG_4_OFF	GPIO_WriteHigh(DIG_4_P, DIG_4) // Set pin to 1
			#endif

			#if (IND_AMOUNT_NUM > 4)
			#define DIG_5_ON	GPIO_WriteLow(DIG_5_P, DIG_5) // Set pin to 0
			#define DIG_5_OFF	GPIO_WriteHigh(DIG_5_P, DIG_5) // Set pin to 1
			#endif

			#if (IND_AMOUNT_NUM > 5)
			#define DIG_6_ON	GPIO_WriteLow(DIG_6_P, DIG_6) // Set pin to 0
			#define DIG_6_OFF	GPIO_WriteHigh(DIG_6_P, DIG_6) // Set pin to 1
			#endif
		#endif
		#ifdef ARDUINO
			#define SEG_A_ON	digitalWrite(SEG_A, 1) // Set pin to 1
			#define SEG_B_ON	digitalWrite(SEG_B, 1) // Set pin to 1
			#define SEG_C_ON	digitalWrite(SEG_C, 1) // Set pin to 1
			#define SEG_D_ON	digitalWrite(SEG_D, 1) // Set pin to 1
			#define SEG_E_ON	digitalWrite(SEG_E, 1) // Set pin to 1
			#define SEG_F_ON	digitalWrite(SEG_F, 1) // Set pin to 1
			#define SEG_G_ON	digitalWrite(SEG_G, 1) // Set pin to 1
			#define SEG_Dp_ON	digitalWrite(SEG_Dp, 1) // Set pin to 1

			#define DIG_1_ON	digitalWrite(DIG_1, 0) // Set pin to 0
			#define DIG_1_OFF	digitalWrite(DIG_1, 1) // Set pin to 1
			#define DIG_2_ON	digitalWrite(DIG_2, 0) // Set pin to 0
			#define DIG_2_OFF	digitalWrite(DIG_2, 1) // Set pin to 1
			#define DIG_3_ON	digitalWrite(DIG_3, 0) // Set pin to 0
			#define DIG_3_OFF	digitalWrite(DIG_3, 1) // Set pin to 1
			
			#if (IND_AMOUNT_NUM > 3)
			#define DIG_4_ON	 // Set pin to 0
			#define DIG_4_OFF	 // Set pin to 1
			#endif

			#if (IND_AMOUNT_NUM > 4)
			#define DIG_5_ON	 // Set pin to 0
			#define DIG_5_OFF	 // Set pin to 1
			#endif
			#if (IND_AMOUNT_NUM > 5)
			#define DIG_6_ON	 // Set pin to 0
			#define DIG_6_OFF	 // Set pin to 1
			#endif
		#endif // ARDUINO

	#endif // COMMON_CATODE
	#ifndef COMMON_CATODE // COMMON_ANODE

	#endif // COMMON_ANODE
#endif // Variables

//! ***************************************************************************


void Ind_init(void)
{
	#ifdef USE_STDPERIPH_DRIVER
		//******* Сегменты *******/
		GPIO_Init(SEG_A_P, (GPIO_Pin_TypeDef)SEG_A, GPIO_MODE_OUT_PP_LOW_FAST);
		GPIO_Init(SEG_B_P, (GPIO_Pin_TypeDef)SEG_B, GPIO_MODE_OUT_PP_LOW_FAST);
		GPIO_Init(SEG_C_P, (GPIO_Pin_TypeDef)SEG_C, GPIO_MODE_OUT_PP_LOW_FAST);
		GPIO_Init(SEG_D_P, (GPIO_Pin_TypeDef)SEG_D, GPIO_MODE_OUT_PP_LOW_FAST);
		GPIO_Init(SEG_E_P, (GPIO_Pin_TypeDef)SEG_E, GPIO_MODE_OUT_PP_LOW_FAST);
		GPIO_Init(SEG_F_P, (GPIO_Pin_TypeDef)SEG_F, GPIO_MODE_OUT_PP_LOW_FAST);
		GPIO_Init(SEG_G_P, (GPIO_Pin_TypeDef)SEG_G, GPIO_MODE_OUT_PP_LOW_FAST);
		GPIO_Init(SEG_Dp_P, (GPIO_Pin_TypeDef)SEG_Dp, GPIO_MODE_OUT_PP_LOW_FAST);
			
		// SEG_A_P->CR2 &= (uint8_t)(~(SEG_A)); /* Reset corresponding bit to GPIO_Pin in CR2 register */
		// SEG_A_P->ODR &= (uint8_t)(~(SEG_A)); /* Low level */
		// SEG_A_P->DDR |= (uint8_t)SEG_A; /* Set Output mode */
		// SEG_A_P->CR1 |= (uint8_t)SEG_A; /* Pull-Up or Push-Pull */
		// SEG_A_P->CR2 &= (uint8_t)(~(SEG_A)); /* No external interrupt or No slope control */
		//******* Разряды *******/
		GPIO_Init(DIG_1_P, (GPIO_Pin_TypeDef)DIG_1, GPIO_MODE_OUT_PP_LOW_FAST);
		GPIO_Init(DIG_2_P, (GPIO_Pin_TypeDef)DIG_2, GPIO_MODE_OUT_PP_LOW_FAST);
		GPIO_Init(DIG_3_P, (GPIO_Pin_TypeDef)DIG_3, GPIO_MODE_OUT_PP_LOW_FAST);
		#if (IND_AMOUNT_NUM > 3)
		GPIO_Init(DIG_4_P, (GPIO_Pin_TypeDef)DIG_4, GPIO_MODE_OUT_PP_LOW_FAST);
		#endif
		#if (IND_AMOUNT_NUM > 4)
		GPIO_Init(DIG_5_P, (GPIO_Pin_TypeDef)DIG_5, GPIO_MODE_OUT_PP_LOW_FAST);
		#endif
		#if (IND_AMOUNT_NUM > 5)
		GPIO_Init(DIG_6_P, (GPIO_Pin_TypeDef)DIG_6, GPIO_MODE_OUT_PP_LOW_FAST);
		#endif
	#endif

	#ifdef ARDUINO
		pinMode(SEG_A, OUTPUT);
		pinMode(SEG_B, OUTPUT);
		pinMode(SEG_C, OUTPUT);
		pinMode(SEG_D, OUTPUT);
		pinMode(SEG_E, OUTPUT);
		pinMode(SEG_F, OUTPUT);
		pinMode(SEG_G, OUTPUT);
		pinMode(SEG_Dp, OUTPUT);

		pinMode(DIG_1, OUTPUT);
		pinMode(DIG_2, OUTPUT);
		pinMode(DIG_3, OUTPUT);
		#if (IND_AMOUNT_NUM > 3)
			pinMode(DIG_4, OUTPUT)
		#endif
		#if (IND_AMOUNT_NUM > 4)
			pinMode(DIG_5, OUTPUT)
		#endif
		#if (IND_AMOUNT_NUM > 5)
			pinMode(DIG_6, OUTPUT)
		#endif
	#endif
}

/******************************************************************************
** Converts a 16-bit number, writes it to the indicator buffer
** value - number to convert,
** comma - how many decimal places after the decimal point (1 for now)
** pos - from what position it outputs (from 0 to IND_AMOUNT_NUM) (counting from right to left)
** amount - how many digits (maximum) to output.
** Rounding - rounding of the number, if it does not fit on the display, but there is a point (0 - no, 1 - yes).
** Example: IND_Output(123, 1, 1, 1, 3, 0) - will output “12.3” without rounding.
** Example: IND_Output(1235, 1, 1, 1, 3, 0) - will output “---” without rounding - error.
** Example: IND_Output(1235, 1, 1, 1, 3, 1) - outputs “124.” with rounding.
******************************************************************************/
void IND_Output(int value, uint8_t comma, uint8_t pos, uint8_t amount, uint8_t rounding )
{
	disableInterrupts();
	#define MINUS	10
	#define MINUS_ONE	100
	uint8_t i = 0;
	uint8_t tmp = 0;
	#define Y	1
	#define N	0
	uint8_t minus = N;
	uint8_t digits = 0;
	uint8_t first_dig = 0; // The first (highest) digit of the number
	uint16_t value_temp = 0; 
	pos -= 1; // Shift the position of the number to start from 0
	uint16_t power10 [] = {1, 10, 100, 1000, 10000}; // Array for division by 10^n	
	uint8_t Error_value = N;
	comma = comma + pos; // Position for point

	//* Clear the indicator buffer
	uint8_t pos_max = pos + amount - 1;
	for(i = pos; i < pos_max+1; i++)
	{
		ind_buf[i] = 0;
	}
	//* If a negative number
	if(value < 0) 
	{
		minus = Y;
		value = value * -1;
	}	
	//* Let's determine the number of digits in the value
	value_temp = value;
	while(value_temp) 
	{
		digits++;
		first_dig = value_temp; // Let's remember the first digit.
		value_temp = value_temp / 10;
	}

	if(minus == Y)
	{
		//* The ability to stick a “-” into a senior “1”.
		if(first_dig == 1)
		{
			first_dig = MINUS_ONE;
		} 
		else 
		{
			first_dig = MINUS;
			if(digits > 1) digits += 1;
		}
	}
	//* The number doesn't fit, there's a dot and rounding is allowed.
	while (digits > amount && comma > 0 && rounding == 1)
	{ 
		if(value % 10 > 4)
		{ 
			value += 5;
			if ((first_dig == MINUS_ONE) && (value >= (power10[digits-comma] * 2)))
			{
				Error_value = Y;
				break;
			}
			if (value >= power10[digits])
			{	
				Error_value = Y;
				break;
			}
		}
		value /= 10; 
		comma -= 1;
		digits -= 1;
	}

	//* still doesn't fit after rounding or rounding is not allowed.
	if(digits > amount) 
	{	
		Error_value = Y;
	}

	//* Number of possessions
	if (Error_value == N)
	{ 
		while (digits <= comma && amount > comma +1) 
		{
			pos_max += 1;
			if(minus == Y) 
			{
				first_dig = MINUS;
				digits += 1;
			}
		}

		//* Let's decompose the number
		for (i = pos; i < pos_max+1; i++)
		{
			tmp = value % 10;
			ind_buf[i] = Dig_code[tmp];

			if (i == comma && comma > 0)
			{
				ind_buf[i] |= S_Dp;
			}

			if (minus == Y && i == pos_max)
			{
				if (first_dig == MINUS_ONE )
				{
					if(digits == amount) {ind_buf[i] |= S_MINUS;}
					else {ind_buf[i] = S_MINUS;}
				}
				if (first_dig == MINUS)
				{
					ind_buf[i] = S_MINUS;
				}
			}
			value /= 10;
		}
	}
	else
	{
		for (i = pos; i < pos_max+1; i++) //! --- ERROR ---
		{
			ind_buf[i] = S_MINUS;
		}
	}
	enableInterrupts();
} // void IND_Output -END-

/******************************************************************************
** Writes the symbol to the indicator buffer
** symbol - symbol code,
** pos - in which position to output (from 1 to IND_AMOUNT_NUM) (counting from right to left).
******************************************************************************/
void IND_Output_symbol(uint8_t symbol, uint8_t pos)
{
	pos -= 1;
	ind_buf[pos] = symbol;
}

/******************************************************************************
** Insert a point into the indicator buffer
** pos - in which position to embed (from 1 to IND_AMOUNT_NUM) (counting from right to left)
******************************************************************************/
void IND_insert_Dp(uint8_t pos)
{
	pos -= 1;
	ind_buf[pos] |= S_Dp;
}

/*** Let's put out all the segments ***/
void Seg_OFF (void)
{
	
	#ifdef COMMON_CATODE
	#ifdef USE_STDPERIPH_DRIVER
		SEG_A_P->ODR &= (uint8_t)(~SEG_A); // Set pin to 0
		SEG_B_P->ODR &= (uint8_t)(~SEG_B); // Set pin to 0
		SEG_C_P->ODR &= (uint8_t)(~SEG_C); // Set pin to 0
		SEG_D_P->ODR &= (uint8_t)(~SEG_D); // Set pin to 0
		SEG_E_P->ODR &= (uint8_t)(~SEG_E); // Set pin to 0
		SEG_F_P->ODR &= (uint8_t)(~SEG_F); // Set pin to 0
		SEG_G_P->ODR &= (uint8_t)(~SEG_G); // Set pin to 0
		SEG_Dp_P->ODR &= (uint8_t)(~SEG_Dp); // Set pin to 0
		#endif

		#ifdef ARDUINO
			digitalWrite(SEG_A, 0); 
			digitalWrite(SEG_B, 0);
			digitalWrite(SEG_C, 0);
			digitalWrite(SEG_D, 0);
			digitalWrite(SEG_E, 0);
			digitalWrite(SEG_F, 0);
			digitalWrite(SEG_G, 0);
			digitalWrite(SEG_Dp, 0);
		#endif
	#else

	#endif
}

 
/******************************************************************************
** Function name : IND_Update
** Outputs a number from the buffer to the indicator.
** delai_shift - in ? ms (if IND_Update is called every µs) shift the running line by 1 position. 0 - do not shift.
** This function must be called from a timer interrupt,
** with frequency minimum: 50Hz * indicator count (20ms / indicator count)
** Example: 3 ind: 20ms / 3 = 6.66(6)ms; 6 ind: 20ms / 6 = 3.33(3)ms; 8 ind: 20ms / 8 = 2.5(2)ms;
******************************************************************************/
void IND_Update(void)
{
	static uint16_t count_for_shift = 0;
	uint8_t number = 0;
	Seg_OFF();
	DIG_1_OFF;
	DIG_2_OFF;
	DIG_3_OFF;
	#if (IND_AMOUNT_NUM > 3)
	DIG_4_OFF;
	#endif
	#if (IND_AMOUNT_NUM > 4)
	DIG_5_OFF;
	#endif
	#if (IND_AMOUNT_NUM > 5)
	DIG_6_OFF;
	#endif	

	if(speed_shift) {count_for_shift++;shift = IND_AMOUNT_STR;}
	if(count_for_shift >= speed_shift)
	{
		count_for_shift = 0;
		shift++;
		if(shift > IND_AMOUNT_STR) shift = 0;
	}
	number = ind_buf[count_digit_num + IND_AMOUNT_STR-shift];
	if(count_digit_num + IND_AMOUNT_STR-shift == 15)
	{number = 0;}

	//* Зажигаем сегменты
	if((number & S_A) == S_A) SEG_A_ON; 
	if((number & S_B) == S_B) SEG_B_ON;
	if((number & S_C) == S_C) SEG_C_ON;
	if((number & S_D) == S_D) SEG_D_ON;
	if((number & S_E) == S_E) SEG_E_ON;
	if((number & S_F) == S_F) SEG_F_ON;
	if((number & S_G) == S_G) SEG_G_ON;
	if((number & S_Dp) == S_Dp) SEG_Dp_ON;
	
	switch (count_digit_num+1)
	{
	case 1:
		DIG_1_ON;
		break;
	case 2:
		DIG_2_ON;
		break;
	case 3:
		DIG_3_ON;
		break;
	#if (IND_AMOUNT_NUM > 3)
	case 4:
		DIG_4_ON;
		break;
	#endif
	#if (IND_AMOUNT_NUM > 4)
	case 5:
		DIG_5_ON;
		break;
	#endif
	#if (IND_AMOUNT_NUM > 5)
	case 6:
		DIG_6_ON;
		break;
	#endif

	default: count_digit_num = 1;
		break;
	}
  count_digit_num++;
  if (count_digit_num >= IND_AMOUNT_NUM) count_digit_num = 0;
	
	// /* Put this code in the interrupt */
	// /******* For 7seg indicator *******/
	// static uint8_t count_for_7seg = 0;
  // if (count_for_7seg > 5) // for 3 digits (for 4 digits > 4)
  // {
  //   IND_Update(); // must be called at a frequency of 50(60)Hz * number of digits
  //   count_for_7seg = 0;
  // }
  // else
  // {
  //   count_for_7seg++;
  // }
	
}

