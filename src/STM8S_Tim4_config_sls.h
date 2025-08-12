#include "stm8s.h"
// #include "stm8s_tim4.h"

// Copilot подсказал
// if (TIM4->CR1 & TIM4_CR1_CEN) {TIM4->CR1 &= ~TIM4_CR1_CEN;} // Выключаем таймер 4
// else {TIM4->CR1 |= TIM4_CR1_CEN;} // Включаем таймер 4

/*
! Not using “stm8s_tim4.h”
! With these settings (+79 bytes Flash +0 bytes RAM) */

/*
* @brief   Enables or disables the specified peripheral CLK.
* @param   CLK_Peripheral : This parameter specifies the peripheral clock for gating.
* This parameter can be any of the CLK_Peripheral_TypeDef enumeration parameters.
* @param   NewState : The new state of the specified peripheral clocks.
* This parameter can be any of the FunctionalState enumerations.
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
* Setting Timer 4 and overflow interrupts,
* interrupt is executed every millisecond (at Fcpu 16 mHz)
*/
void Tim4_config(void)
{
  // Timer 4 as system tick, f = 1 kHz
  CLK_PeripheralClockConfig(CLK_PCKENR1_TIM4, ENABLE); // Enable timer clocking
  TIM4_DeInit(); // Just in case, reset all timer settings
  // The timer coincidence interrupt frequency is calculated according to the formula Ftim4_ovf=FCPU/(TIM4_Prescaler*(1+TIM4_Period))
  // F= 16000000/(128*(1+124))=1000кГц=0.001ms
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, 124); // set the timer divider to 128 and limit the count at 124 (to interrupt every millisecond).
  // TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE); // Enable Timer overflow interrupt
  TIM4_Cmd(ENABLE); // Enabling the account
  enableInterrupts(); // Enable global interrupt enable
}

//* Timer overflow interrupt 4.
void TIM4_UPD_OVF_IRQHandler(void) __interrupt(23)
{
  if(Time_ind_out) {Time_ind_out--;} //* So that the data on the display does not change frequently
  if(Time_ind_upd) {Time_ind_upd--;} //* So that the indicator is not frequently updated
  else {IND_Update(); Time_ind_upd = 6;} //* Update the indicator every 5ms
  Buttons_read();
  if(count_ms) count_ms--; //* for delay_ms
  if(Count_Wait_ON > 0) Count_Wait_ON--;
  if(Count_End_charge) Count_End_charge++;
  Count_Blink++;
  Count_Change_display++;
  
  TIM4_ClearITPendingBit(TIM4_IT_UPDATE); //* Reset overflow interrupt flag
}

