// #define DEBUG 1 //^ Отладка - включить/выключить
// #define RUNNING_STR 1 //^ Бегущая строка - включить/выключить
#include "stm8s.h"
#include "stm8s_conf.h"
// #include "stm8s_it.h"    /* SDCC patch: required by SDCC for interrupts */
#include "main.h"
#include "GPIO_sls.h"
#include "STM8S_CLK_config_sls.h"
#include "STM8S_TIM4_config_sls.h"
#ifdef RUNNING_STR 
#include "7seg_sls_running_str.h" // C бегущей строкой
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

void _delay_ms(uint16_t nCount); // Обьявление функции задержки в миллисекундах

void main(void) //! Основная функция программы
{
  CLK_config(); // Настройка тактирования
  Tim4_config(); // Настройка таймера 4 и прерывания по переполнению
  Ind_init(); // Инициализация индикатора
  Buttons_init(); // Инициализация кнопок
  TIM2_PWM_Config(); // Настройка ШИМ на таймере 2
  Pin_init(); // Инициализация портов ввода-вывода
  ADC_NTC = ADC_read_x(NTC_CHANNEL, 16); 
 


while (1) //! Основной цикл программы
{
  ADC_cur = ADC_read_x(ADC_U_DC_CHANNEL, 10); //^ Напряжение блока питания
  ADC_U_DC = ADC_cur / 10;
  U_DC = ADC_U_DC * COEF_U_DC; // Преобразование в напряжение

  ADC_cur = ADC_read_x(ADC_U_OUT_CHANNEL, 10); //^ Напряжение на аккумуляторе
  //! Плавно усреднять НЕЛЬЗЯ, т.к. в импульсном режиме важно видеть пульсации
  ADC_U_OUT = ADC_cur / 10;
  U_Out = ADC_U_OUT * COEF_U_OUT; // Преобразование в напряжение
  U_Out_disp = Average_int(U_Out, U_Out_disp, 4); // Усреднение АЦП
  // if(U_Out < 500 || U_Out > U_DC+50) {Count_Wait_ON = TIME_WAIT_ON;Mode = MODE_AUTO_SELECT;} //^ Блок питания отключен (< 5.0V) или не подключен аккумулятор
  // if( U_Out < 500) {Count_Wait_ON = TIME_WAIT_ON;Mode = MODE_AUTO_SELECT;} //^ Блок питания отключен (< 5.0V) или не подключен аккумулятор
  
  ADC_cur = ADC_read_x(ADC_I_CHANNEL, 100); //^ Ток заряда аккумулятора
  // ADC_I = Average_int(ADC_cur, ADC_I, 4); // Усреднение АЦП
  ADC_I = ADC_cur/10;
  I_out = ADC_I * COEF_I; // Преобразование в ток

  //************************** BUTTON **************************
  if (BT_check(BT_FNC, CLICK)) 
  {
    Mode++; // Переключаем режим работы 
    if (Mode >= MODE_RESET_MODE) {Mode = MODE_AUTOMOBILE_CHARGING;} // Если режим больше максимального, то переходим в первый
    Count_Wait_ON = TIME_WAIT_ON; // Сбрасываем счетчик времени ожидания включения
  }
  // Count_Wait_ON = TIME_WAIT_ON;//!!!
  #ifndef DEBUG
  if(Count_Wait_ON != 0)
  {
    PWM_TM2_CH3 = 0; // Выключаем ШИМ
    Flag_ON_Out = FALSE; // Выключаем выход
    Flag_Fan = FALSE; // Выключаем вентилятор
  }
  Flag_LED_GEL = Flag_LED_MOTO = Flag_LED_PULSE = Flag_LED_CAR = 0; // Выключаем все светодиоды
  //************************** Режим работы **************************
  switch (Mode)
  {
  case MODE_AUTO_SELECT: //! Автоопределение режима работы
    // if(U_Out > U_DC+50) {Count_Wait_ON = TIME_WAIT_ON;} //^ Блок питания отключен
    if(Count_Wait_ON == 0) //^ Заканчивается время ожидания начала работы
    {
      if(U_Out < 1050) {Mode = MODE_CHARGE_PULSE;} //^ Если напряжение на аккумуляторе меньше 10.5V, то включим режим восстановления.
      else {Mode = MODE_AUTOMOBILE_CHARGING;} //^ Если напряжение на аккумуляторе больше 10.5V, то включим режим автомобильной зарядки.
    }
    Led_blink(&Flag_LED_CAR, 250, 250); // Функция мигания светодиода
    Flag_LED_GEL = Flag_LED_MOTO = Flag_LED_PULSE = Flag_LED_CAR; // Мигаем все светодиоды
    break;  
  case MODE_AUTOMOBILE_CHARGING: //! Режим заряда как на автомобиле
    if(Count_Wait_ON != 0) // ^ Держим паузу 10сек перед включением зарядки, мигаем нужным светодиодом
    {
      PWM_TM2_CH3 = 0; // Выключаем ШИМ
      Led_blink(&Flag_LED_CAR, 250, 250); // Функция мигания светодиода
    }
    else  //^ Время ожидания включения истекло - Переходим в режим заряда
    {
      Battery_type = BATTERY_Pb; // Устанавливаем тип аккумулятора
      Ustavka_U = U_AUTOMOBILE_CHARGING; // Уставка напряжения: 1420 это 14.20V на дисплей выводить / 10
      Ustavka_I = I_MAX; // Уставка тока: MAX 620 это 6.2A на дисплей выводить / 10
      Flag_LED_CAR = 1; // Включаем 1 светодиод
      Flag_ON_Out = TRUE; // Включаем выход
      Charging_DC(Ustavka_U, Ustavka_I); //^ Зарядка аккумулятора постоянным током и напряжением
      I_out_disp = Average_int(I_out, I_out_disp, 4);
    }
    break;
  case MODE_CHARGE_AGM_GEL: //! Режим заряда AGM/GEL аккумулятора
    if(Count_Wait_ON != 0) // ^ Держим паузу 10сек перед включением зарядки, мигаем нужным светодиодом
    {
      PWM_TM2_CH3 = 0; // Выключаем ШИМ
      Led_blink(&Flag_LED_GEL, 250, 250); // Функция мигания светодиода
    }
    else  //^ Время ожидания включения истекло - Переходим в режим заряда
    {
      Battery_type = BATTERY_AGM_GEL; // Устанавливаем тип аккумулятора
      Ustavka_U = U_AGM_GEL; // Уставка напряжения: 1420 это 14.20V на дисплей выводить / 10
      Ustavka_I = I_MAX; // Уставка тока: MAX 620 это 6.2A на дисплей выводить / 10
      Flag_LED_GEL = 1; // Включаем 2 светодиод
      Flag_ON_Out = TRUE; // Включаем выход
      Charging_DC(Ustavka_U, Ustavka_I); //^ Зарядка аккумулятора постоянным током и напряжением
      I_out_disp = Average_int(I_out, I_out_disp, 4);
    }
    break;
  case MODE_CHARGE_MOTO: //! Этот режим буду использовать для "ПРОЖАРКИ" убитых аккумов, без перехода в режим хранения
    if(Count_Wait_ON != 0) // ^ Держим паузу 10сек перед включением зарядки, мигаем нужным светодиодом
    {
      PWM_TM2_CH3 = 0; // Выключаем ШИМ
      Led_blink(&Flag_LED_MOTO, 250, 250); // Функция мигания светодиода
    }
    else  //^ Время ожидания включения истекло - Переходим в режим заряда
    {
      Battery_type = BATTERY_MOTO; // Устанавливаем тип аккумулятора
      Ustavka_U = U_MOTO; // Уставка напряжения: 1420 это 14.20V на дисплей выводить / 10
      Ustavka_I = I_MAX; // Уставка тока: MAX 620 это 6.2A на дисплей выводить / 10
      Flag_LED_MOTO = 1; // Включаем 3 светодиод
      Flag_ON_Out = TRUE; // Включаем выход
      Charging_DC(Ustavka_U, Ustavka_I); //^ Зарядка аккумулятора постоянным током и напряжением
      I_out_disp = Average_int(I_out, I_out_disp, 4);
    }
    break;
  case MODE_CHARGE_PULSE: //! Режим заряда пульсацией
    if(Count_Wait_ON != 0) // ^ Держим паузу 10сек перед включением зарядки, мигаем нужным светодиодом
    {
      PWM_TM2_CH3 = 0; // Выключаем ШИМ
      Led_blink(&Flag_LED_PULSE, 250, 250); // Функция мигания светодиода
    }
    else  //^ Время ожидания включения истекло - Переходим в режим заряда
    {
      Battery_type = BATTERY_Pb; // Устанавливаем тип аккумулятора
      Ustavka_U = U_PULSE; // Уставка напряжения: 1420 это 14.20V на дисплей выводить / 10
      Ustavka_I = I_MAX; // Уставка тока: MAX 620 это 6.2A на дисплей выводить / 10
      Flag_LED_PULSE = 1; // Включаем 4 светодиод
      
      if(Count_Blink < 10) 
      {
        Flag_ON_Out = TRUE;
      }
      else if(Count_Blink < 500) 
      {
        Flag_ON_Out = TRUE;
        Charging_DC(Ustavka_U, Ustavka_I);
        I_out_disp = Average_int(I_out, I_out_disp, 4); // Усреднение тока
      } //^ Зарядка аккумулятора постоянным током и напряжением}
      else if(Count_Blink < 1000) {Flag_ON_Out = FALSE;} //^ Выключаем выход
      else {Count_Blink = 0;} // Сбрасываем счетчик мигания
      Mode = MODE_CHARGE_PULSE; //^ Чтоб автоматически не выходить с этого режима 
    }
    break;
  case MODE_SAFE_KEEPING: //! Режим хранения
    PWM_TM2_CH3 = 0; // Выключаем ШИМ
    Flag_ON_Out = FALSE; // Выключаем выход
    Flag_Fan = FALSE; // Выключаем вентилятор
    // Led_blink(&Flag_LED_CAR, 50, 5000); // Функция мигания светодиода
    // Flag_LED_GEL = Flag_LED_MOTO = Flag_LED_PULSE = Flag_LED_CAR; // Мигаем все светодиоды
    if(U_Out < U_RECHARGING) //^ Если напряжение на аккумуляторе меньше 12.20V
    {
      _delay_ms(1000); // Ждем 1 сек
      if(Battery_type == BATTERY_Pb) {Mode = MODE_AUTOMOBILE_CHARGING;} // Переходим в режим заряда свинцового аккумулятора
      else if(Battery_type == BATTERY_AGM_GEL) {Mode = MODE_CHARGE_AGM_GEL;} // Переходим в режим заряда AGM/GEL аккумулятора
      else {Mode = MODE_AUTOMOBILE_CHARGING;} // Переходим в режим заряда свинцового аккумулятора
    } 
    break;
  default: Mode = 0; // Если режим больше максимального, то переходим в первый
    break;
  }
  #else 
  // if(Count_Blink < 1000)
  // {
  //   Flag_ON_Out = 1; // Включаем выход
  //   Flag_LED_CAR = 1; // Включаем 1 светодиод
  //   Flag_LED_GEL = 1; // Включаем 2 светодиод
  //   Flag_LED_MOTO = 1; // Включаем 3 светодиод
  //   Flag_LED_PULSE = 1; // Включаем 4 светодиод
  //   Flag_Fan = 1; // Включаем вентилятор
  // }
  // else if(Count_Blink < 2000)
  // {
  //   Flag_ON_Out = 0; // Выключаем выход
  //   Flag_LED_CAR = 0; // Выключаем 1 светодиод
  //   Flag_LED_GEL = 0; // Выключаем 2 светодиод
  //   Flag_LED_MOTO = 0; // Выключаем 3 светодиод
  //   Flag_LED_PULSE = 0; // Выключаем 4 светодиод
  //   Flag_Fan = 0; // Выключаем вентилятор
  // }
  // else {Count_Blink = 0;} // Сбрасываем счетчик мигания
  Led_blink(&Flag_LED_CAR, 250, 250); // Функция мигания светодиода
  // if(Count_Blink < 250) {Flag_LED_CAR = 1;} // Включаем выход
  // else if(Count_Blink < 500) {Flag_LED_CAR = 0;} // Выключаем выход
  // else {Count_Blink = 0;} // Сбрасываем счетчик мигания
  PWM_TM2_CH3 = PWM_MAX / 2;
  #endif // DEBUG
  //************************** PWM на таймере 2 **************************
  PWM_set_TIM2_CH3(PWM_TM2_CH3); // Установка ШИМ на таймере 2

  //************************** Упр. пинами **************************
  if(Flag_ON_Out) {GPIO_WriteHigh(ON_OUT_PORT, ON_OUT_PIN);} // Включаем выход
  else {GPIO_WriteLow(ON_OUT_PORT, ON_OUT_PIN);} // Выключаем выход
  if(Flag_Fan) {GPIO_WriteHigh(FAN_PORT, FAN_PIN);} // Включаем вентилятор
  else {GPIO_WriteLow(FAN_PORT, FAN_PIN);} // Выключаем вентилятор
  if(Flag_LED_CAR) {GPIO_WriteHigh(LED_CAR_PORT, LED_CAR_PIN);} // Включаем 1 светодиод
  else {GPIO_WriteLow(LED_CAR_PORT, LED_CAR_PIN);} // Выключаем 1 светодиод
  if(Flag_LED_GEL) {GPIO_WriteHigh(LED_GEL_PORT, LED_GEL_PIN);} // Включаем 2 светодиод
  else {GPIO_WriteLow(LED_GEL_PORT, LED_GEL_PIN);} // Выключаем 2 светодиод
  if(Flag_LED_MOTO) {GPIO_WriteHigh(LED_MOTO_PORT, LED_MOTO_PIN);} // Включаем 3 светодиод
  else {GPIO_WriteLow(LED_MOTO_PORT, LED_MOTO_PIN);} // Выключаем 3 светодиод
  if(Flag_LED_PULSE) {GPIO_WriteHigh(LED_PULSE_PORT, LED_PULSE_PIN);} // Включаем 4 светодиод
  else {GPIO_WriteLow(LED_PULSE_PORT, LED_PULSE_PIN);} // Выключаем 4 светодиод
  //************************** Display **************************
  if(Time_ind_out == 0) 
  {
    //^ Температура NTC
    ADC_cur = ADC_read_x(NTC_CHANNEL, 16); 
    ADC_NTC = Average_int(ADC_cur, ADC_NTC, 6); // Усреднение АЦП
    Temp = calc_temperature(ADC_NTC, table_arduino_10k_3950_Ra10,sizeof(table_arduino_10k_3950_Ra10), -400, 1500, 50); // Преобразование в температуру

    Display(); //^ Выводим на индикатор
    Time_ind_out = T_IND_OUT;

    // WWDG->CR = WWDG_CR_WDGA; /* Activate WWDG, with clearing T6 */
  }
  _delay_ms(1); // Задержка 1ms
} //& -end- while (1) //! Основной цикл программы
} //& -end- void main(void) //! Основная функция программы

//* Функция задержки в миллисекундах
//* если считать count_ms в прерывании каждую миллисекунду
void _delay_ms(uint16_t nCount)
{
  // uint16_t count_ms = 0; // for _delay_ms() - обьявить глобально 
  count_ms = nCount;
  while (count_ms != 0)
  {
    // Это кинуть в прерывание: if(count_ms) count_ms--; // Для delay_ms
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
