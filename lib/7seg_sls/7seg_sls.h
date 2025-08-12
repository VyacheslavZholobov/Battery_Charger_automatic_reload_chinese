//! Для STM8S105:
//! #include "7seg_sls.h" 
//! Добавил в прерывание обновление дисплея, c таймером обновления
//! инициализировал, вывел в майне на дисплей цифру
//! (+1053 bytes Flash +4 bytes RAM) 
#ifdef USE_STDPERIPH_DRIVER
#include "stm8s.h"
#endif
#ifdef ARDUINO
#include <Arduino.h>
#endif




#if 1 //^ Настройки и переменные для 7-сегментного индикатора
	#define COMMON_CATODE			// Закоментировать если индикатор с общим анодом(+)
	#define IND_AMOUNT_NUM	3	// Колличество цифр в индикаторе
	//^ Variables
		uint8_t ind_buf[IND_AMOUNT_NUM]; // буфер 7-ми сегметного индикатора
	uint8_t count_digit_num = 0; // Счетчик активной цифры для индикации
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
		// Нумерация цифр с право на лево
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
		// Нумерация цифр с лева на право
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
#endif // Настройки и переменные для 7-сегментного индикатора

#if 1 //^ Предопределения кодов цифр и символов для 7-сегментного индикатора
	/* Код цифры в формате 7сегм.индикатора*/
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
			#define SEG_A_ON	SEG_A_P->ODR |= (uint8_t)(SEG_A) // Установить пин в 1
			#define SEG_B_ON	SEG_B_P->ODR |= (uint8_t)(SEG_B) // Установить пин в 1
			#define SEG_C_ON	SEG_C_P->ODR |= (uint8_t)(SEG_C) // Установить пин в 1
			#define SEG_D_ON	SEG_D_P->ODR |= (uint8_t)(SEG_D) // Установить пин в 1
			#define SEG_E_ON	SEG_E_P->ODR |= (uint8_t)(SEG_E) // Установить пин в 1
			#define SEG_F_ON	SEG_F_P->ODR |= (uint8_t)(SEG_F) // Установить пин в 1
			#define SEG_G_ON	SEG_G_P->ODR |= (uint8_t)(SEG_G) // Установить пин в 1
			#define SEG_Dp_ON	SEG_Dp_P->ODR |= (uint8_t)(SEG_Dp) // Установить пин в 1

			#define DIG_1_ON	DIG_1_P->ODR &= (uint8_t)(~DIG_1) // Установить пин в 0
			#define DIG_1_OFF	DIG_1_P->ODR |= (uint8_t)(DIG_1) // Установить пин в 1
			#define DIG_2_ON	DIG_2_P->ODR &= (uint8_t)(~DIG_2) // Установить пин в 0
			#define DIG_2_OFF	DIG_2_P->ODR |= (uint8_t)(DIG_2) // Установить пин в 1
			#define DIG_3_ON	DIG_3_P->ODR &= (uint8_t)(~DIG_3) // Установить пин в 0
			#define DIG_3_OFF	DIG_3_P->ODR |= (uint8_t)(DIG_3) // Установить пин в 1
			
			#if (IND_AMOUNT_NUM > 3)
			#define DIG_4_ON	GPIO_WriteLow(DIG_4_P, DIG_4) // Установить пин в 0
			#define DIG_4_OFF	GPIO_WriteHigh(DIG_4_P, DIG_4) // Установить пин в 1
			#endif

			#if (IND_AMOUNT_NUM > 4)
			#define DIG_5_ON	GPIO_WriteLow(DIG_5_P, DIG_5) // Установить пин в 0
			#define DIG_5_OFF	GPIO_WriteHigh(DIG_5_P, DIG_5) // Установить пин в 1
			#endif

			#if (IND_AMOUNT_NUM > 5)
			#define DIG_6_ON	GPIO_WriteLow(DIG_6_P, DIG_6) // Установить пин в 0
			#define DIG_6_OFF	GPIO_WriteHigh(DIG_6_P, DIG_6) // Установить пин в 1
			#endif
		#endif
		#ifdef ARDUINO
			#define SEG_A_ON	digitalWrite(SEG_A, 1) // Установить пин в 1
			#define SEG_B_ON	digitalWrite(SEG_B, 1) // Установить пин в 1
			#define SEG_C_ON	digitalWrite(SEG_C, 1) // Установить пин в 1
			#define SEG_D_ON	digitalWrite(SEG_D, 1) // Установить пин в 1
			#define SEG_E_ON	digitalWrite(SEG_E, 1) // Установить пин в 1
			#define SEG_F_ON	digitalWrite(SEG_F, 1) // Установить пин в 1
			#define SEG_G_ON	digitalWrite(SEG_G, 1) // Установить пин в 1
			#define SEG_Dp_ON	digitalWrite(SEG_Dp, 1) // Установить пин в 1

			#define DIG_1_ON	digitalWrite(DIG_1, 0) // Установить пин в 0
			#define DIG_1_OFF	digitalWrite(DIG_1, 1) // Установить пин в 1
			#define DIG_2_ON	digitalWrite(DIG_2, 0) // Установить пин в 0
			#define DIG_2_OFF	digitalWrite(DIG_2, 1) // Установить пин в 1
			#define DIG_3_ON	digitalWrite(DIG_3, 0) // Установить пин в 0
			#define DIG_3_OFF	digitalWrite(DIG_3, 1) // Установить пин в 1
			
			#if (IND_AMOUNT_NUM > 3)
			#define DIG_4_ON	 // Установить пин в 0
			#define DIG_4_OFF	 // Установить пин в 1
			#endif

			#if (IND_AMOUNT_NUM > 4)
			#define DIG_5_ON	 // Установить пин в 0
			#define DIG_5_OFF	 // Установить пин в 1
			#endif
			#if (IND_AMOUNT_NUM > 5)
			#define DIG_6_ON	 // Установить пин в 0
			#define DIG_6_OFF	 // Установить пин в 1
			#endif
		#endif // ARDUINO

	#endif // COMMON_CATODE
	#ifndef COMMON_CATODE // COMMON_ANODE

	#endif // COMMON_ANODE
#endif // Предопределения кодов цифр и символов для 7-сегментного индикатора

//! ***************************************************************************

/******************************************************************************
** Функция инициализации пинов микроконтроллера,
** к которым подключен индикатор
******************************************************************************/
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

//* Преобразует 16-ти разрядное число, записывает его в буфер индикатора       
//* value - число для преобразования, 
//* comma - сколько знаков после запятой (пока 1)
//* pos - с какой позиции выводит (от1 до IND_AMOUNT_NUM) (счет справа на лево)
//* amount - сколько цифр (максимум) выводить.
//* Rounding - округление числа, если не влазиет на дисплей,но есть точка (0 - нет, 1 - да)
//* Пример: IND_Output(123, 1, 1, 3, 0) - выведет "12.3" без округления
//* Пример: IND_Output(1234, 1, 1, 3, 0) - выведет "---" без округления - ошибка
//* Пример: IND_Output(1234, 1, 1, 3, 1) - выведет "124" с округлением
void IND_Output(int value, uint8_t comma, uint8_t pos, uint8_t amount, uint8_t rounding )
{
	disableInterrupts(); // Отключаем прерывания
	#define MINUS	10
	#define MINUS_ONE	100
	uint8_t i = 0;
	uint8_t tmp = 0;
	#define Y	1
	#define N	0
	uint8_t minus = N; // Флаг минуса
	uint8_t digits = 0; // Колличество разрядов в числе
	uint8_t first_dig = 0; // Первая(старшая) цифра числа
	uint16_t value_temp = 0; // Временная переменная для определения разрядов числа
	pos -= 1; // Сместим поз.числа чтоб начать с 0
	uint16_t power10 [] = {1, 10, 100, 1000, 10000}; // Массив для деления на 10^n	
	uint8_t Error_value = N; // Флаг ошибки, если число не влазит в индикатор

	//* Очистим буфер индикатора
	for(i = pos; i < amount; i++)
	{
		ind_buf[i] = 0;
	}
	//* Если отрицательное число
	if(value < 0) 
	{
		minus = Y; // Поднимем флаг минуса
		value = value * -1; // Сделаем число положительным
	}	
	//* Опредилим колличество разрядов в value
	value_temp = value;
	while(value_temp) 
	{
		digits++;
		first_dig = value_temp; // Запомним первую(старшую цифру)
		value_temp = value_temp / 10;//print("цифр=", digits, "first_dig=", first_dig)
	}

	if(minus == Y)
	{
		//* Возможность вклеить "-" в старшую "1".
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
	//* Число не влазиет, есть точка и округление разрешено
	while (digits > amount && comma > 0 && rounding == 1) // Учитывая знак минуса
	{ 
		if(value % 10 > 4)
		{ 
			value += 5; // Округлим число
			if ((first_dig == MINUS_ONE) && (value >= (power10[digits-comma] * 2))) // после округления левая единица стала двойкой
			{
				Error_value = Y;// print("левая единица стала двойкой, value=", value)
				break;
			}
			if (value >= power10[digits]) // после округления добавился еще один разряд
			{	
				Error_value = Y;// print("появился ещё один разряд, value=", value)
				break;
			}
		}
		value /= 10; // Уменьшим кол.цифр
		comma -= 1;
		digits -= 1;// print("округлили value=", value)
	}

	//* все равно не влазиет после округления или округление не разрешено
	if(digits > amount) 
	{	
		Error_value = Y;
	}

	//* Число влазиет
	if (Error_value == N)
	{ 
		while (digits <= comma && amount > comma +1) // добавил кол. цифр, для нуля перед точкой, если возможно
		{
			digits += 1;
			if(minus == Y) 
			{
				first_dig = MINUS;
				digits += 1;
			}
		}

		//!!! нули перед точкой добавить не возможно // подумать, нужно ли это!!!
		if (digits <= comma)
		{
			if(value % 10 > 4)
			{ 
				value += 5; // Округлим число
			}
			value /= 10;
			if(digits < amount && minus == Y) 
			{
				first_dig = MINUS;
				comma -= 1;
				digits += 1;
			}
		}

		//* Разложим число
		for (i = pos; i < digits; i++) 
		{
			tmp = value % 10; // Отделим правую цифру
			ind_buf[i] = Dig_code[tmp]; // Запишем в буфер текущую цифру в коде для 7сегм индикатора

			if (i == comma && comma > 0)
			{
				ind_buf[i] |= S_Dp; // Вклеим точку
			}

			if (minus == Y && i == digits-1) // Число отрицательное и Первая цифра (старший разряд)
			{
				if (first_dig == MINUS_ONE ) // если первая цифра "-1"
				{
					if(digits == amount) {ind_buf[i] |= S_MINUS;} // Самая левая цифра на индикаторе "1" то вклеим знак минуса
					else {ind_buf[i+1] = S_MINUS;} // иначе просто запишем знак минуса в следующую позицию
				}
				if (first_dig == MINUS) // запишем знак минуса
				// else
				{
					ind_buf[i] = S_MINUS;
				}
			}
			value /= 10;
		}
	}
	else
	{
		for (i = pos; i < amount; i++) //! --- ERROR ---
		{
			ind_buf[i] = S_MINUS; // Заполним буфер "---"
		}
	}
	enableInterrupts(); //Включаем глобальное разрешение прерывания
} // void IND_Output -END-

/******************************************************************************
** Записывает символ в буфер индикатора       
** symbol - код символа, 
** pos - в какую позицию выводить (от1 до IND_AMOUNT_NUM) (счет справа на лево)
******************************************************************************/
void IND_Output_symbol(uint8_t symbol, uint8_t pos)
{
	pos -= 1;
	ind_buf[pos] = symbol;
}

/******************************************************************************
** Внедрить точку в буфер индикатора       
** pos - в какую позицию внедрить (от1 до IND_AMOUNT_NUM) (счет справа на лево)
******************************************************************************/
void IND_insert_Dp(uint8_t pos)
{
	pos -= 1;
	ind_buf[pos] |= S_Dp;
}

/*** Потушим все сегменты ***/
void Seg_OFF (void)
{
	
	#ifdef COMMON_CATODE
	#ifdef USE_STDPERIPH_DRIVER
		SEG_A_P->ODR &= (uint8_t)(~SEG_A); // Установить пин в 0
		SEG_B_P->ODR &= (uint8_t)(~SEG_B); // Установить пин в 0
		SEG_C_P->ODR &= (uint8_t)(~SEG_C); // Установить пин в 0
		SEG_D_P->ODR &= (uint8_t)(~SEG_D); // Установить пин в 0
		SEG_E_P->ODR &= (uint8_t)(~SEG_E); // Установить пин в 0
		SEG_F_P->ODR &= (uint8_t)(~SEG_F); // Установить пин в 0
		SEG_G_P->ODR &= (uint8_t)(~SEG_G); // Установить пин в 0
		SEG_Dp_P->ODR &= (uint8_t)(~SEG_Dp); // Установить пин в 0
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
** Function name :   IND_Update
** Выводит на индикатор число из буфера.
** Эту функцию нужно вызывать из прерывания таймера,
** с частотой минимум: 50Гц * колл.индикаторов (20ms / колл.индикаторов)
** Пример: 3 ind: 20ms / 3 = 6.66(6)ms; 6 ind: 20ms / 6 = 3.33(3)ms; 8 ind: 20ms / 8 = 2.5(2)ms;
******************************************************************************/
void IND_Update(void)
{
	uint8_t number = 0;
	//* гасим все сегменты и все разряды
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

	number = ind_buf[count_digit_num];

	//* Зажигаем сегменты
	if((number & S_A) == S_A) SEG_A_ON; 
	if((number & S_B) == S_B) SEG_B_ON;
	if((number & S_C) == S_C) SEG_C_ON;
	if((number & S_D) == S_D) SEG_D_ON;
	if((number & S_E) == S_E) SEG_E_ON;
	if((number & S_F) == S_F) SEG_F_ON;
	if((number & S_G) == S_G) SEG_G_ON;
	if((number & S_Dp) == S_Dp) SEG_Dp_ON;
	
	// switch (IND_AMOUNT_NUM - count_digit_num)
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
	
	// /* Это код поместить в прерывание */
	// /******* Для 7seg индикатора *******/
	// static uint8_t count_for_7seg = 0;
  // if (count_for_7seg > 5) // для трех зн.индикатора (для 4рех.зн. > 4)
  // {
  //   IND_Update(); // должно вызываться с частотой 50(60)Гц * колличество цифр
  //   count_for_7seg = 0;
  // }
  // else
  // {
  //   count_for_7seg++;
  // }
	
}

