#include "stm8s.h"
// #include "stm8s_tim2.h"


// #define PWM_MAX 999 // Maximum value for PWM
// uint16_t PWM_TM2_CH3 = 500; // PWM value for channel 3
#define PWM_MAX 499 // Maximum value for PWM //* чем меньше, тем больше частота ШИМ
uint16_t TIM2_Period = PWM_MAX; // Period value
// uint16_t PWM_TM2_CH3 = 250; // PWM value for channel 3

/*
! Не использую "stm8s_tim2.h" (+2 924 bytes Flash +7 bytes RAM)
! С этими настройками (+83 bytes Flash +6 bytes RAM) */

#define TIM2_PRESCALER_1  ((uint8_t)0x00) // Prescaler value
// #define TIM2_PRESCALER_2  ((uint8_t)0x01) // Prescaler value
// #define TIM2_PRESCALER_4  ((uint8_t)0x02) // Prescaler value
// #define TIM2_PRESCALER_8  ((uint8_t)0x03) // Prescaler value
// #define TIM2_PRESCALER_16 ((uint8_t)0x04) // Prescaler value
// #define TIM2_PRESCALER_32 ((uint8_t)0x05) // Prescaler value
// #define TIM2_PRESCALER_64 ((uint8_t)0x06) // Prescaler value
// #define TIM2_PRESCALER_128 ((uint8_t)0x07) // Prescaler value
// #define TIM2_PRESCALER_256 ((uint8_t)0x08) // Prescaler value
// #define TIM2_PRESCALER_512 ((uint8_t)0x09) // Prescaler value

#define TIM2_OUTPUTSTATE_ENABLE   ((uint8_t)0x11) // Output state value
// #define TIM2_OUTPUTSTATE_DISABLE  ((uint8_t)0x00) // Output state value
#define TIM2_OCPOLARITY_HIGH  ((uint8_t)0x00) // Output polarity value
// #define TIM2_OCPOLARITY_LOW   ((uint8_t)0x22) // Output polarity value
// #define TIM2_OCMODE_TIMING      ((uint8_t)0x00) // Output compare mode value
// #define TIM2_OCMODE_ACTIVE      ((uint8_t)0x10) // Output compare mode value
// #define TIM2_OCMODE_INACTIVE    ((uint8_t)0x20) // Output compare mode value
// #define TIM2_OCMODE_TOGGLE      ((uint8_t)0x30) // Output compare mode value
#define TIM2_OCMODE_PWM1        ((uint8_t)0x60) // Output compare mode value
// #define TIM2_OCMODE_PWM2        ((uint8_t)0x70) // Output compare mode value



/**
  * @brief  Configure TIM2 peripheral in PWM mode
  * @param  None
  * @retval None
  */
 void TIM2_PWM_Config(void)
{
  /* Time base configuration */
  // TIM2_TimeBaseInit(TIM2_PRESCALER_1, 999); //^ Вместо этого - ниже
  /* Set the Prescaler value */
  TIM2->PSCR = (uint8_t)(TIM2_PRESCALER_1);
  /* Set the Autoreload value */
  TIM2->ARRH = (uint8_t)(TIM2_Period >> 8);
  TIM2->ARRL = (uint8_t)(TIM2_Period);
 
  //* PWM1 Mode configuration: Channel1
  //  TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,CCR1_Val, TIM2_OCPOLARITY_HIGH); //^ Вместо этого - ниже
  /* Disable the Channel 1: Reset the CCE Bit, Set the Output State , the Output Polarity */
  // TIM2->CCER1 &= (uint8_t)(~( TIM2_CCER1_CC1E | TIM2_CCER1_CC1P));
  // /* Set the Output State &  Set the Output Polarity  */
  // TIM2->CCER1 |= (uint8_t)((uint8_t)(TIM2_OUTPUTSTATE_ENABLE & TIM2_CCER1_CC1E ) | 
  //                           (uint8_t)(TIM2_OCPOLARITY_HIGH & TIM2_CCER1_CC1P));
  
  // /* Reset the Output Compare Bits  & Set the Ouput Compare Mode */
  // TIM2->CCMR1 = (uint8_t)((uint8_t)(TIM2->CCMR1 & (uint8_t)(~TIM2_CCMR_OCM)) |
  //                         (uint8_t)TIM2_OCMODE_PWM1);
  
  // /* Set the Pulse value */
  // TIM2->CCR1H = (uint8_t)(CCR1_Val >> 8);
  // TIM2->CCR1L = (uint8_t)(CCR1_Val);

  // //  TIM2_OC1PreloadConfig(ENABLE);
  // /* Set or Reset the OC1PE Bit */
  // if (ENABLE != DISABLE) {TIM2->CCMR1 |= (uint8_t)TIM2_CCMR_OCxPE;}
  // else {TIM2->CCMR1 &= (uint8_t)(~TIM2_CCMR_OCxPE);}
  //* PWM1 Mode configuration: Channel1 - END -
 
   //* PWM1 Mode configuration: Channel2 */ аналогично каналу 1
  //  TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,CCR2_Val, TIM2_OCPOLARITY_HIGH); //^ Вместо этого - ниже
  //  TIM2_OC2PreloadConfig(ENABLE); //^ Вместо этого - ниже
 
   //* PWM1 Mode configuration: Channel3 */         
  //  TIM2_OC3Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,PWM_TM2_CH3, TIM2_OCPOLARITY_HIGH); //^ Вместо этого - ниже
  /* Disable the Channel 1: Reset the CCE Bit, Set the Output State , the Output Polarity */
  TIM2->CCER2 &= (uint8_t)(~( TIM2_CCER1_CC1E | TIM2_CCER1_CC1P));
  /* Set the Output State &  Set the Output Polarity  */
  TIM2->CCER2 |= (uint8_t)((uint8_t)(TIM2_OUTPUTSTATE_ENABLE & TIM2_CCER1_CC1E ) | 
                            (uint8_t)(TIM2_OCPOLARITY_HIGH & TIM2_CCER1_CC1P));
  
  /* Reset the Output Compare Bits  & Set the Ouput Compare Mode */
  TIM2->CCMR3 = (uint8_t)((uint8_t)(TIM2->CCMR3 & (uint8_t)(~TIM2_CCMR_OCM)) |
                          (uint8_t)TIM2_OCMODE_PWM1);
  
  /* Set the Pulse value */
  TIM2->CCR3H = (uint8_t)(PWM_TM2_CH3 >> 8);
  TIM2->CCR3L = (uint8_t)(PWM_TM2_CH3);

  //  TIM2_OC1PreloadConfig(ENABLE);
  /* Set or Reset the OC1PE Bit */
  TIM2->CCMR1 |= (uint8_t)TIM2_CCMR_OCxPE; // ENABLE
  // TIM2->CCMR1 &= (uint8_t)(~TIM2_CCMR_OCxPE); // DISABLE

  // GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST); // TIM2_CH3 - PA3
  //* PWM1 Mode configuration: Channel3 - END -

  //  TIM2_OC3PreloadConfig(ENABLE); //^ Вместо этого - ниже
     /* Set or Reset the OC3PE Bit */
  TIM2->CCMR3 |= (uint8_t)TIM2_CCMR_OCxPE; // ENABLE
  // TIM2->CCMR3 &= (uint8_t)(~TIM2_CCMR_OCxPE); // DISABLE
 
  //  TIM2_ARRPreloadConfig(ENABLE); //^ Вместо этого - ниже
  /* Set or Reset the ARPE Bit */
  TIM2->CR1 |= (uint8_t)TIM2_CR1_ARPE; // ENABLE
  // TIM2->CR1 &= (uint8_t)(~TIM2_CR1_ARPE); // DISABLE

  /* TIM2 enable counter */
  //  TIM2_Cmd(ENABLE); //^ Вместо этого - ниже
  TIM2->CR1 |= (uint8_t)TIM2_CR1_CEN; // ENABLE
  // TIM2->CR1 &= (uint8_t)(~TIM2_CR1_CEN); // DISABLE
 }

 //*
 void PWM_set_TIM2_CH3(uint16_t new_PWM)
 {
  TIM2->CCR3H = (uint8_t)(new_PWM >> 8);
  TIM2->CCR3L = (uint8_t)(new_PWM);
 }