#include "stm8s.h"
/*
! Not using “stm8s_clk.h” (+1901 bytes Flash +0 bytes RAM)
! With these clock settings (+57 bytes Flash +0 bytes RAM) */

/**
  * @brief  Configures the HSI and CPU clock dividers.
  * @param  CLK_Prescaler Specifies the HSI or CPU clock divider to apply.
  * @retval None
  */
void CLK_SYSCLKConfig(uint8_t CLK_Prescaler)
{
  /* check the parameters */
  // assert_param(IS_CLK_PRESCALER_OK(CLK_Prescaler));
  
  if (((uint8_t)CLK_Prescaler & (uint8_t)0x80) == 0x00) /* Bit7 = 0 means HSI divider */
  {
    CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_HSIDIV);
    CLK->CKDIVR |= (uint8_t)((uint8_t)CLK_Prescaler & (uint8_t)CLK_CKDIVR_HSIDIV);
  }
  else /* Bit7 = 1 means CPU divider */
  {
    CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_CPUDIV);
    CLK->CKDIVR |= (uint8_t)((uint8_t)CLK_Prescaler & (uint8_t)CLK_CKDIVR_CPUDIV);
  }
}


void CLK_config(void)
{
  CLK_SYSCLKConfig((uint8_t)0x80);  // Processor and peripheral clock frequency divider
  CLK_SYSCLKConfig((uint8_t)0x00);  // set 16 MHz for CPU (uint8_t)0x00

  // Variants:
  // CLK_PRESCALER_HSIDIV1   = (uint8_t)0x00, /*!< High speed internal clock prescaler: 1 */
  // CLK_PRESCALER_HSIDIV2   = (uint8_t)0x08, /*!< High speed internal clock prescaler: 2 */
  // CLK_PRESCALER_HSIDIV4   = (uint8_t)0x10, /*!< High speed internal clock prescaler: 4 */
  // CLK_PRESCALER_HSIDIV8   = (uint8_t)0x18, /*!< High speed internal clock prescaler: 8 */
  // CLK_PRESCALER_CPUDIV1   = (uint8_t)0x80, /*!< CPU clock division factors 1 */
  // CLK_PRESCALER_CPUDIV2   = (uint8_t)0x81, /*!< CPU clock division factors 2 */
  // CLK_PRESCALER_CPUDIV4   = (uint8_t)0x82, /*!< CPU clock division factors 4 */
  // CLK_PRESCALER_CPUDIV8   = (uint8_t)0x83, /*!< CPU clock division factors 8 */
  // CLK_PRESCALER_CPUDIV16  = (uint8_t)0x84, /*!< CPU clock division factors 16 */
  // CLK_PRESCALER_CPUDIV32  = (uint8_t)0x85, /*!< CPU clock division factors 32 */
  // CLK_PRESCALER_CPUDIV64  = (uint8_t)0x86, /*!< CPU clock division factors 64 */
  // CLK_PRESCALER_CPUDIV128 = (uint8_t)0x87  /*!< CPU clock division factors 128 */
}