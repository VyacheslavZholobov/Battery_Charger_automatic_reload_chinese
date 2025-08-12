// * Author: slaventiis (Vyacheslav Zholobov)
//! latest changes: 23.04.2025
// #define USE_7SEG //^ uncomment if the buttons are connected to the pins of the seven-segment indicator
#ifdef USE_7SEG //^ and specify which indicator
	// #define COMMON_CATODE			// Comment if indicator with common anode(+)
#endif

#if 1 //^ BUTTONS variables

	#define BT_AMOUNT	1		// number of buttons
	#define BT_FNC		0 	// it is necessary that
	// #define BT_DOWN	1 // the numbers were consistent
	// #define BT_FNC	2 	// array location
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
	#define BT_AUTOCLICK		100		// *2ms if USE_7SEG (4 digits) ~ 240ms
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

//* Setting the button pins
//* Clearing the button status buffer
void Buttons_init(void)
{
	for (uint8_t i = 0; i < BT_AMOUNT; i++)
	{
		#ifndef USE_7SEG
		//  pinMode_INPUT_PULLUP(BT_port_pins [i] [0], BT_port_pins [i] [1]);
		 GPIO_Init((BT_port_pins [i] [0]), (GPIO_Pin_TypeDef)(BT_port_pins [i] [1]), GPIO_MODE_IN_PU_NO_IT); // Switching on the power pull-up
		#endif
		BT_state[i] = 0;
		count_BT_pressed[i] = 0;
	}
	
}

//* Reading the state of buttons and writing their states to the buffer
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
		if (!GPIO_ReadInputPin(BT_port_pins[i][0], BT_port_pins[i][1])) //^ When the button is pressed (0)
		
		{
			count_BT_pressed[i]++;
			count_BT_no_pressed = 0;
			if (count_BT_pressed[i] >= BT_DREB && BT_state[i] == UNPRESS) {BT_state[i] = PRESS;} // Pressing with anti-jarring
			if (count_BT_pressed[i] >= BT_HOLD && BT_state[i] == WAIT_HOLD) {BT_state[i] = HOLD; count_BT_pressed[i] = BT_HOLD;} // Holding
			if (count_BT_pressed[i] >= BT_HOLD && BT_state[i] == WAIT_AUTOCLICK) {BT_state[i] = PRESS; count_BT_pressed[i] = BT_HOLD - BT_AUTOCLICK;} // Autoclick after holding
		}
		else 
		{
			count_BT_pressed[i] = 0;
			count_BT_no_pressed++;
			if(BT_state[i] == PRESS || BT_state[i] == WAIT_HOLD) {BT_state[i] = CLICK;}
			//* Button released and handled or not pressed for more than 500ms
		 	if(BT_state[i] == PRESS_PR || BT_state[i] == HOLD_PR || BT_state[i] == WAIT_AUTOCLICK || count_BT_no_pressed > BT_HOLD/2) 
			{BT_state[i] = UNPRESS;} // Let's reset the state of the button
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

// Returns (1) if the expected action was performed on the button
// No Press: Returns (0);
// BT - button number (name)
// what_state - expected action:
// PRESS - Just waiting for a simple press (return 1;)
// CLICK - Waiting for pressing and releasing (return 1;)
// HOLD - Expecting just a holding pattern (return 1;)
// AUTOCLICK - Waiting for a press (return 1;) and autoclick after holding (return 1;)
// if(BT_check(BT_UP, CLICK)) //*** BT_UP
uint8_t BT_check (uint8_t BT, uint8_t what_state) //^ BT_check
{
	uint8_t state = 0;
	switch (what_state)
	{
	case PRESS:	//* Just waiting for a simple press
		if (BT_state[BT] == PRESS) {BT_state[BT] = PRESS_PR; state = 1;}
		break;
	case CLICK:	//* Waiting for pressing and releasing
		if (BT_state[BT] == CLICK) {BT_state[BT] = UNPRESS; state = 1;}
		break;
	case HOLD: //* Expecting just a holding pattern
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

// Examples:
// if(BT_check(BT_UP, CLICK)) //*** BT_UP
// {Val++;}
// if(BT_check(BT_UP, AUTOCLICK)) //*** BT_UP
// {Val++;}
// if(BT_check(BT_UP, HOLD)) //*** BT_UP
// {Val++;}
// if(BT_check(BT_UP, PRESS) && BT_check(BT_SET, PRESS))
// {Val++;}
// if(BT_check(BT_UP, AUTOCLICK) && BT_check(BT_SET, AUTOCLICK))
// {Val++;}