// #define DEBUG 1 //^ Debugging - enable/disable
// #define RUNNING_STR 1 //^ Running line - enable/disable
#include "stm8s.h"
#include "stm8s_conf.h"
// #include "stm8s_it.h"    /* SDCC patch: required by SDCC for interrupts */
#include "main.h"
#include "GPIO_sls.h"
#include "STM8S_CLK_config_sls.h"
#include "STM8S_TIM4_config_sls.h"
#ifdef RUNNING_STR 
#include "7seg_sls_running_str.h" // with  Running line
#else
#include "7seg_sls.h" 
#endif
#include "Display.h"
#include "BUTTON_SLS.h"
#include "STM8S_Tim2_config_PWM_sls.h"
#include "ADC_STM8_sls.h"
#include "Average_sls.h"
#include "Functions.h"
#include "NTC_sls.h"

void _delay_ms(uint16_t nCount); // Declaring the delay function in milliseconds

void main(void) //! Основная функция программы
{
  CLK_config(); // Clock setting
  Tim4_config(); // Setting Timer 4 and overflow interrupts
  Ind_init(); // Indicator initialization
  Buttons_init(); // Button initialization
  TIM2_PWM_Config(); // PWM setting on timer 2
  Pin_init(); // Initializing I/O ports
  ADC_NTC = ADC_read_x(NTC_CHANNEL, 16); 
 


while (1) //! Main program cycle
{
  ADC_cur = ADC_read_x(ADC_U_DC_CHANNEL, 10); //^ Power supply voltage
  ADC_U_DC = ADC_cur / 10;
  U_DC = ADC_U_DC * COEF_U_DC;

  ADC_cur = ADC_read_x(ADC_U_OUT_CHANNEL, 10); //^ Battery voltage
  //! Smooth averaging DOES NOT work, because in pulse mode it is important to see the ripples
  ADC_U_OUT = ADC_cur / 10;
  U_Out = ADC_U_OUT * COEF_U_OUT;
  U_Out_disp = Average_int(U_Out, U_Out_disp, 4);
  
  ADC_cur = ADC_read_x(ADC_I_CHANNEL, 100); //^ Battery charge current
  // ADC_I = Average_int(ADC_cur, ADC_I, 4);
  ADC_I = ADC_cur/10;
  I_out = ADC_I * COEF_I;

  //************************** BUTTON **************************
  if (BT_check(BT_FNC, CLICK)) 
  {
    Mode++; // Switching the operating mode
    if (Mode >= MODE_RESET_MODE) {Mode = MODE_AUTOMOBILE_CHARGING;} // If the mode is greater than the maximum mode, we go to the first mode
    Count_Wait_ON = TIME_WAIT_ON; // Reset the switch-on timeout counter
  }
  #ifndef DEBUG
  if(Count_Wait_ON != 0)
  {
    PWM_TM2_CH3 = 0; // PWM off
    Flag_ON_Out = FALSE; // Shutting down the output
    Flag_Fan = FALSE; // Turning off the fan
  }
  Flag_LED_GEL = Flag_LED_MOTO = Flag_LED_PULSE = Flag_LED_CAR = 0; // Turning off all the LEDs
  //************************** operating mode **************************
  switch (Mode)
  {
  case MODE_AUTO_SELECT: //! Autodetection of the operating mode
    // if(U_Out > U_DC+50) {Count_Wait_ON = TIME_WAIT_ON;} //^ The power supply is disconnected
    if(Count_Wait_ON == 0) //^ The waiting time for the start of operation has expired
    {
      if(U_Out < 1050) {Mode = MODE_CHARGE_PULSE;} //^ If the battery voltage is less than 10.5V, we will enable the recovery mode.
      else {Mode = MODE_AUTOMOBILE_CHARGING;} //^ If the battery voltage is greater than 10.5V, we will enable the car charging mode.
    }
    Led_blink(&Flag_LED_CAR, 250, 250); // LED flashing function
    Flag_LED_GEL = Flag_LED_MOTO = Flag_LED_PULSE = Flag_LED_CAR; // All LEDs are flashing
    break;  
  case MODE_AUTOMOBILE_CHARGING: //! Charge mode as on a car
    if(Count_Wait_ON != 0) // ^ Pause for 10sec before switching on the charging, blink the desired LED
    {
      PWM_TM2_CH3 = 0; // PWM off
      Led_blink(&Flag_LED_CAR, 250, 250);
    }
    else  //^ The power-on standby time has elapsed - Switch to charge mode
    {
      Battery_type = BATTERY_Pb; // Set the battery type
      Ustavka_U = U_AUTOMOBILE_CHARGING; // Voltage setpoint: 1420 is 14.20V on display output / 10
      Ustavka_I = I_MAX; // Current setpoint: MAX 620 is 6.2A on display output / 10
      Flag_LED_CAR = 1; // Turn on 1 LED
      Flag_ON_Out = TRUE; // Turning on the output
      Charging_DC(Ustavka_U, Ustavka_I); //^ Charging the battery with direct current and voltage
      I_out_disp = Average_int(I_out, I_out_disp, 4);
    }
    break;
  case MODE_CHARGE_AGM_GEL: //! AGM/GEL battery charging mode
    if(Count_Wait_ON != 0) // ^ Pause for 10sec before switching on the charging, blink the desired LED
    {
      PWM_TM2_CH3 = 0; // PWM off
      Led_blink(&Flag_LED_GEL, 250, 250); // LED flashing function
    }
    else  //^ The power-on standby time has elapsed - Switch to charge mode
    {
      Battery_type = BATTERY_AGM_GEL; // Set the battery type
      Ustavka_U = U_AGM_GEL; // Voltage setpoint: 1380 is 13.80V on display output / 10
      Ustavka_I = I_MAX; // Current setpoint: MAX 620 is 6.2A on display output / 10
      Flag_LED_GEL = 1; // Turn on 2 LED
      Flag_ON_Out = TRUE; // Turning on the output
      Charging_DC(Ustavka_U, Ustavka_I); //^ Charging the battery with direct current and voltage
      I_out_disp = Average_int(I_out, I_out_disp, 4);
    }
    break;
  case MODE_CHARGE_MOTO: //! I will use this mode to “fry” dead batteries, without switching to storage mode
    if(Count_Wait_ON != 0) // ^ Pause for 10sec before switching on the charging, blink the desired LED
    {
      PWM_TM2_CH3 = 0; // PWM off
      Led_blink(&Flag_LED_MOTO, 250, 250); // LED flashing function
    }
    else  //^ The power-on standby time has elapsed - Switch to charge mode
    {
      Battery_type = BATTERY_MOTO; // Set the battery type
      Ustavka_U = U_MOTO; // Voltage setpoint: 1600 is 16.00V on display output / 10
      Ustavka_I = I_MAX; // Current setpoint: MAX 620 is 6.2A on display output / 10
      Flag_LED_MOTO = 1; // Turn on 3 LED
      Flag_ON_Out = TRUE; // Turning on the output
      Charging_DC(Ustavka_U, Ustavka_I); //^ Charging the battery with direct current and voltage
      I_out_disp = Average_int(I_out, I_out_disp, 4);
    }
    break;
  case MODE_CHARGE_PULSE: //! Pulse charge mode
    if(Count_Wait_ON != 0) // ^ Pause for 10sec before switching on the charging, blink the desired LED
    {
      PWM_TM2_CH3 = 0; // PWM off
      Led_blink(&Flag_LED_PULSE, 250, 250);
    }
    else  //^ The power-on standby time has elapsed - Switch to charge mode
    {
      Battery_type = BATTERY_Pb; // Set the battery type
      Ustavka_U = U_PULSE; // Voltage setpoint: 1550 is 15.50V on display output / 10
      Ustavka_I = I_MAX; // Current setpoint: MAX 620 is 6.2A on display output / 10
      Flag_LED_PULSE = 1; // Turn on 4 LED
      
      if(Count_Blink < 10) 
      {
        Flag_ON_Out = TRUE;
      }
      else if(Count_Blink < 500) 
      {
        Flag_ON_Out = TRUE;
        Charging_DC(Ustavka_U, Ustavka_I); //^ Charging the battery with direct current and voltage
        I_out_disp = Average_int(I_out, I_out_disp, 4);
      } 
      else if(Count_Blink < 1000) {Flag_ON_Out = FALSE;} //^ Shutting down the output
      else {Count_Blink = 0;} // Resetting the flashing counter
      Mode = MODE_CHARGE_PULSE; //^ So that you don't automatically exit this mode 
    }
    break;
  case MODE_SAFE_KEEPING: //! Storage mode
    PWM_TM2_CH3 = 0; // PWM off
    Flag_ON_Out = FALSE; // Shutting down the output
    Flag_Fan = FALSE; // Turning off the fan

    if(U_Out < U_RECHARGING) //^ If the battery voltage is less than 12.20V
    {
      _delay_ms(1000);
      if(Battery_type == BATTERY_Pb) {Mode = MODE_AUTOMOBILE_CHARGING;}
      else if(Battery_type == BATTERY_AGM_GEL) {Mode = MODE_CHARGE_AGM_GEL;}
      else {Mode = MODE_AUTOMOBILE_CHARGING;}
    } 
    break;
  default: Mode = 0; // If the mode is greater than the maximum mode, we go to the first mode
    break;
  }
  #endif // DEBUG
  //************************** PWM on timer 2 **************************
  PWM_set_TIM2_CH3(PWM_TM2_CH3); // Setting PWM on timer 2

  //************************** Pin control **************************
  if(Flag_ON_Out) {GPIO_WriteHigh(ON_OUT_PORT, ON_OUT_PIN);} // Turn on exit
  else {GPIO_WriteLow(ON_OUT_PORT, ON_OUT_PIN);} // Turn off exit
  if(Flag_Fan) {GPIO_WriteHigh(FAN_PORT, FAN_PIN);} // Turn on fan
  else {GPIO_WriteLow(FAN_PORT, FAN_PIN);} // Turn off fan
  if(Flag_LED_CAR) {GPIO_WriteHigh(LED_CAR_PORT, LED_CAR_PIN);} // Turn on 1 LEDs
  else {GPIO_WriteLow(LED_CAR_PORT, LED_CAR_PIN);} // Turn off 1 LEDs
  if(Flag_LED_GEL) {GPIO_WriteHigh(LED_GEL_PORT, LED_GEL_PIN);} // Turn on 2 LEDs
  else {GPIO_WriteLow(LED_GEL_PORT, LED_GEL_PIN);} // Turn off 2 LEDs
  if(Flag_LED_MOTO) {GPIO_WriteHigh(LED_MOTO_PORT, LED_MOTO_PIN);} // Turn on 3 LEDs
  else {GPIO_WriteLow(LED_MOTO_PORT, LED_MOTO_PIN);} // Turn off 3 LEDs
  if(Flag_LED_PULSE) {GPIO_WriteHigh(LED_PULSE_PORT, LED_PULSE_PIN);} // Turn on 4 LEDs
  else {GPIO_WriteLow(LED_PULSE_PORT, LED_PULSE_PIN);} // Turn off 4 LEDs
  //************************** Display **************************
  if(Time_ind_out == 0) 
  {
    //^ NTC temperature
    ADC_cur = ADC_read_x(NTC_CHANNEL, 16); 
    ADC_NTC = Average_int(ADC_cur, ADC_NTC, 6);
    Temp = calc_temperature(ADC_NTC, table_arduino_10k_3950_Ra10,sizeof(table_arduino_10k_3950_Ra10), -400, 1500, 50);

    Display(); //^ Displayed on the indicator
    Time_ind_out = T_IND_OUT;
  }
  _delay_ms(1);
} //& -end- while (1)
} //& -end- void main(void)

//* Delay function in milliseconds
//* if you count the count_ms in the interrupt every millisecond.
void _delay_ms(uint16_t nCount)
{
  count_ms = nCount;
  while (count_ms != 0)
  {
    // This is to throw into the interrupt: if(count_ms) count_ms--;
  }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line)*/

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
