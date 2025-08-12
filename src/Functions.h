// #include "stm8s.h"

//* Charging the battery with constant current and voltage.
void Charging_DC (int ustavra_u, int ustavka_i)
{
	if(Temp < 500) //! Temperature's normal
	{
		if (I_out < Ustavka_I)
		{
			if (U_Out < Ustavka_U) {if (PWM_TM2_CH3 < PWM_MAX) {PWM_TM2_CH3++;}} //* Increase the voltage by increasing the PWM duty cycle
		}
		if (U_Out > Ustavka_U || I_out > Ustavka_I) {if (PWM_TM2_CH3 > 0) {PWM_TM2_CH3--;}} //* Decrease the voltage by decreasing the PWM duty cycle
		if(I_out < I_MIN) //^ If the current is less than 0.5A
		{
			if(Count_End_charge == 0) {Count_End_charge++;} //* Start the countdown of the end of charge time
		}
		else {Count_End_charge = 0;} //* Resetting the end-of-charge time counter

		if(Count_End_charge > TIME_END_CHARGE) // Charge end time has elapsed
		{
			Count_End_charge = 0; // Resetting the end-of-charge time counter
			Mode = MODE_SAFE_KEEPING; // Switching to storage mode
		}		
	}
	else	{PWM_TM2_CH3 = 0;} //! Turning off PWM on overheating
	Flag_Fan = TRUE; // Turning on the fan
} 

//** LED flashing function
//** flag_led - pointer to the LED status variable (0 - off, 1 - on)
//** t_on - LED ON time in milliseconds
//** t_off - LED off time in milliseconds
//** Example: Led_blink(&Flag.LED_CAR, 500, 500); // blinking LED 0.5 sec on, 0.5 sec off
void Led_blink (uint8_t *flag_led, uint16_t t_on, uint16_t t_off)
{
	if(Count_Blink < t_on) {*flag_led = 1;} // Flashing time has not expired // Turn on the LED
	else if(Count_Blink < t_on + t_off) {*flag_led = 0;} // Blinking time has expired // Turn off the LED
	else {Count_Blink = 0;} // Reset the flashing counter
}
