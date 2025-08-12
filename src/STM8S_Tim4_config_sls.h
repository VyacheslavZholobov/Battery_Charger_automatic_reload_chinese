#include "stm8s.h"
// #include "stm8s_tim4.h"

// Copilot подсказал
// if (TIM4->CR1 & TIM4_CR1_CEN) {TIM4->CR1 &= ~TIM4_CR1_CEN;} // Выключаем таймер 4
// else {TIM4->CR1 |= TIM4_CR1_CEN;} // Включаем таймер 4

/*
! Не использую "stm8s_tim4.h"
! С этими настройками (+79 bytes Flash +0 bytes RAM) */

/*
* @brief   Включает или отключает указанный периферийный CLK.
* @param   CLK_Peripheral : Этот параметр указывает периферийные часы для стробирования.
* Этот параметр может быть любым из CLK_Peripheral_TypeDef enumeration.
* @param   NewState : Новое состояние указанных периферийных часов.
* Этот параметр может быть любым из перечисление FunctionalState.
* @retval None
*/
void CLK_PeripheralClockConfig(uint8_t CLK_Peripheral, uint8_t NewState)
{
  /* Check the parameters */
  // assert_param(IS_FUNCTIONALSTATE_ OK(NewState));
  // assert_param(IS_CLK_PERIPHERAL_OK(CLK_Peripheral));
  
  if (((uint8_t)CLK_Peripheral & (uint8_t)0x10) == 0x00)
  {
    if (NewState != DISABLE)
    {
      /* Enable the peripheral Clock */
      CLK->PCKENR1 |= (uint8_t)((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F));
    }
    else
    {
      /* Disable the peripheral Clock */
      CLK->PCKENR1 &= (uint8_t)(~(uint8_t)(((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F))));
    }
  }
  else
  {
    if (NewState != DISABLE)
    {
      /* Enable the peripheral Clock */
      CLK->PCKENR2 |= (uint8_t)((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F));
    }
    else
    {
      /* Disable the peripheral Clock */
      CLK->PCKENR2 &= (uint8_t)(~(uint8_t)(((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F))));
    }
  }
}

/*
* Настройка таймера 4 и прерывания по переполнению,
* прерывание выполняется каждую миллисекунду (при Fcpu 16 mHz)
* В функц. main вызвать Tim4_config();
*/
void Tim4_config(void)
{
  // Timer 4 as system tick, f = 1 kHz
  CLK_PeripheralClockConfig(CLK_PCKENR1_TIM4, ENABLE); //Включаем тактирование таймера
  TIM4_DeInit(); // На всякий случай сбрасываем все настройки таймера
  // Частота прерыв.по совпадению таймера расчитывается по формуле Ftim4_ovf=FCPU/(TIM4_Prescaler*(1+TIM4_Period))
  // F= 16000000/(128*(1+124))=1000кГц=0.001ms
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, 124); // настраиваем пределитель таймера на 128 и ограничиваем счет при 124 (для прерывания каждую миллисекунду)
  // TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE); //Разрешаем Прерывание таймера по переполнению
  TIM4_Cmd(ENABLE); // Включаем счет
  enableInterrupts(); //Включаем глобальное разрешение прерывания
}

//^ Для удобства, глоб.переменные которые исп. только в прерывании.
// uint16_t Time_ind_upd = 0;
// uint16_t Time_LED = 0;

//* Прерывание по переполнению таймера 4.
void TIM4_UPD_OVF_IRQHandler(void) __interrupt(23)
{
  if(Time_ind_out) {Time_ind_out--;} //* Чтоб не часто менялись данные на дисплее
  if(Time_ind_upd) {Time_ind_upd--;} //* Чтоб не часто обновлялся индикатор
  else {IND_Update(); Time_ind_upd = 6;} //* Обновляем индикатор каждые 5ms
  Buttons_read(); //* Читаем состояние кнопок
  if(count_ms) count_ms--; //* Для delay_ms
  if(Count_Wait_ON > 0) Count_Wait_ON--; //* Время ожидания включения
  if(Count_End_charge) Count_End_charge++; //* Время окончания заряда
  Count_Blink++; //* Время мигания или импульсной зарядки
  Count_Change_display++; //* Счетчик изменения дисплея
  
  TIM4_ClearITPendingBit(TIM4_IT_UPDATE); // Сбрасываем флаг прерывания переполнения
}

