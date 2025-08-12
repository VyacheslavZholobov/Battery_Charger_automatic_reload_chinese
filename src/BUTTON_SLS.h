//! последние изменения: 23.04.2025
// #define USE_7SEG //^ раскоментить если кнопки подключены к пинам семисегментного индикатора
#ifdef USE_7SEG //^ и указать какой индикатор
	// #define COMMON_CATODE			// Закоментировать если индикатор с общим анодом(+)
#endif

#if 1 //^ BUTTONS variables

	#define BT_AMOUNT	1		// количество кнопок
	#define BT_FNC		0 	// нужно чтобы
	// #define BT_DOWN	1 // цифры соответствовали
	// #define BT_FNC	2 	// месту в массиве 
	// #define BT_SET	3 	// BT_port_pins [BT_AMOUNT] [2]

	#define PORT_BT_FNC		GPIOB_BaseAddress // GPIOD
	#define PIN_BT_FNC		GPIO_PIN_6

	uint8_t count_BT_no_pressed = 0;
	uint16_t BT_port_pins[BT_AMOUNT] [2]={
		{PORT_BT_FNC, PIN_BT_FNC}};
	uint16_t count_BT_pressed[BT_AMOUNT];
	uint8_t BT_state[BT_AMOUNT];

	#define BT_DREB					50		// *2ms if USE_7SEG (4 digits) ~ 72ms (3*6*4=72)
	#define BT_HOLD					1000	// *2ms if USE_7SEG (4 digits) ~ 984ms
	#define BT_AUTOCLICK		100	// *2ms if USE_7SEG (4 digits) ~ 240ms
	#define UNPRESS					0
	#define PRESS						1
	#define PRESS_PR				2
	#define CLICK						3	
	#define WAIT_AUTOCLICK	4
	#define AUTOCLICK				5
	#define WAIT_HOLD				6
	#define HOLD						7
	#define HOLD_PR					8
#endif // BUTTONS variables

//* Настройка пинов кнопок
//* Очистка буфера состояний кнопок
void Buttons_init(void)
{
	for (uint8_t i = 0; i < BT_AMOUNT; i++)
	{
		#ifndef USE_7SEG
		//  pinMode_INPUT_PULLUP(BT_port_pins [i] [0], BT_port_pins [i] [1]);
		 GPIO_Init((BT_port_pins [i] [0]), (GPIO_Pin_TypeDef)(BT_port_pins [i] [1]), GPIO_MODE_IN_PU_NO_IT); // Включаем подтяжку к питанию
		#endif
		BT_state[i] = 0;
		count_BT_pressed[i] = 0;
	}
	
}

//* Чтение состояния кнопок и запись в буфер их состояний
void Buttons_read (void)
{
	uint8_t state_pin;

	for (uint8_t i = 0; i < BT_AMOUNT; i++)
	{
		#ifdef USE_7SEG
			state_pin = READ_PIN(BT_port_pins[i][0], BT_port_pins[i][1]);
			pinMode_INPUT(BT_port_pins[i][0], BT_port_pins[i][1]);
		#endif
		// if(READ_PIN(BT_port_pins[i][0], BT_port_pins[i][1]) == 0)
		if (!GPIO_ReadInputPin(BT_port_pins[i][0], BT_port_pins[i][1])) //^ Если кнопка нажата (0)
		
		{
			count_BT_pressed[i]++;
			count_BT_no_pressed = 0;
			if (count_BT_pressed[i] >= BT_DREB && BT_state[i] == UNPRESS) {BT_state[i] = PRESS;} // Нажатие с антидребезгом
			if (count_BT_pressed[i] >= BT_HOLD && BT_state[i] == WAIT_HOLD) {BT_state[i] = HOLD; count_BT_pressed[i] = BT_HOLD;} // Удержание
			if (count_BT_pressed[i] >= BT_HOLD && BT_state[i] == WAIT_AUTOCLICK) {BT_state[i] = PRESS; count_BT_pressed[i] = BT_HOLD - BT_AUTOCLICK;} // Автоклик после удержания
		}
		else 
		{
			count_BT_pressed[i] = 0;
			count_BT_no_pressed++;
			if(BT_state[i] == PRESS || BT_state[i] == WAIT_HOLD) {BT_state[i] = CLICK;}
			//* Кнопка отпущена и обработана или не нажимались более 500ms
		 	if(BT_state[i] == PRESS_PR || BT_state[i] == HOLD_PR || BT_state[i] == WAIT_AUTOCLICK || count_BT_no_pressed > BT_HOLD/2) 
			{BT_state[i] = UNPRESS;} // Обнулим состояние кнопки
		}

		#ifdef USE_7SEG
			pinMode_OUTPUT(BT_port_pins[i][0], BT_port_pins[i][1]);
			#ifdef COMMON_CATODE
				if(state_pin == 0) {SET_PIN(BT_port_pins[i][0], BT_port_pins[i][1]);}
				else {CLEAR_PIN(BT_port_pins[i][0], BT_port_pins[i][1]);}
			#else
				if(state_pin == 0) {CLEAR_PIN(BT_port_pins[i][0], BT_port_pins[i][1]);}
				else {SET_PIN(BT_port_pins[i][0], BT_port_pins[i][1]);}
			#endif
		#endif
	}
}

// Возвращает (1) если с кнопкой совершали ожидаемое действие
// Не было нажатия: Возвращает (0);
// BT - номер (имя) кнопки
// what_state - ожидаемое действие:
// PRESS - Ожидаем просто нажатие (return 1;)
// CLICK - Ожидаем нажатие и отжатие (return 1;)
// HOLD - Ожидаем просто удержание (return 1;)
// AUTOCLICK - Ожидаем нажатие (return 1;) и автоклик после удержания (return 1;)
// if(BT_check(BT_UP, CLICK)) //*** BT_UP
uint8_t BT_check (uint8_t BT, uint8_t what_state) //^ BT_check
{
	uint8_t state = 0;
	switch (what_state)
	{
	case PRESS:	//* Ожидаем просто нажатие
		if (BT_state[BT] == PRESS) {BT_state[BT] = PRESS_PR; state = 1;}
		break;
	case CLICK:	//* Ожидаем нажатие и отжатие
		if (BT_state[BT] == CLICK) {BT_state[BT] = UNPRESS; state = 1;}
		break;
	case HOLD: //* Ожидаем просто удержание	
		if (BT_state[BT] == PRESS) {BT_state[BT] = WAIT_HOLD;}
		if (BT_state[BT] == HOLD)	{ state = 1;}//BT_state[BT] = HOLD_PR;
		break;
	case AUTOCLICK:	//* Ожидаем нажатие	и автоклик после удержания
		if (BT_state[BT] == PRESS) {BT_state[BT] = WAIT_AUTOCLICK; state = 1;}
		break;
	
	default:
		break;
	}
	return state;
}

// Примеры:
// if(BT_check(BT_UP, CLICK)) //*** BT_UP
// {Val++;} // изменяем переменную 1 раз после кратковр.нажатия и отпускания кнопки
// if(BT_check(BT_UP, AUTOCLICK)) //*** BT_UP
// {Val++;} // изменяем переменную с автокликом
// if(BT_check(BT_UP, HOLD)) //*** BT_UP
// {Val++;} // изменяем переменную 1 раз после удержания нажатой кнопки
// if(BT_check(BT_UP, PRESS) && BT_check(BT_SET, PRESS))
// {Val++;} // изменяем переменную 1 раз при нажатии ДВУХ кнопок
// if(BT_check(BT_UP, AUTOCLICK) && BT_check(BT_SET, AUTOCLICK))
// {Val++;} // изменяем переменную с автокликом при нажатии и удержании ДВУХ кнопок