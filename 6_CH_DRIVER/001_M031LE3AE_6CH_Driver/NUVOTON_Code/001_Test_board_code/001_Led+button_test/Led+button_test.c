#include "NuMicro.h"
#include "stdbool.h"
#include <stdio.h>

#define Bit_SET 1
#define Bit_RESET 0

volatile bool i;

void SYS_Init(void) {
  SYS_UnlockReg();
  /* Set GPF multi-function pins for XT1_IN and XT1_OUT */
  SYS->GPF_MFPL &= ~(SYS_GPF_MFPL_PF3MFP_Msk | SYS_GPF_MFPL_PF2MFP_Msk);
  SYS->GPF_MFPL |= (SYS_GPF_MFPL_PF3MFP_XT1_IN | SYS_GPF_MFPL_PF2MFP_XT1_OUT);
  /* Enable HXT clock */
  CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
  /* Wait for HXT clock ready */
  CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
  /* Set PCLK0/PCLK1 to HCLK/2 */
  CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV1 | CLK_PCLKDIV_APB1DIV_DIV1);
  //********* UART0_MODULE  ***************************************//
  // CLK_EnableModuleClock(UART0_MODULE);
  // CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
  //********* TMR0_MODULE  ****************************************//
  CLK_EnableModuleClock(TMR0_MODULE);
  CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, 0);
  SystemCoreClockUpdate();
  //********* UART0 RXD=PA.15 and TXD=PA.14 ***********************//
  // SYS->GPA_MFPH = (SYS->GPA_MFPH & ~(SYS_GPA_MFPH_PA14MFP_Msk | SYS_GPA_MFPH_PA15MFP_Msk)) |
  //                (SYS_GPA_MFPH_PA15MFP_UART0_RXD | SYS_GPA_MFPH_PA14MFP_UART0_TXD);
  //********* GPIO PA.0 PA.1 PA.2 PA.3 ****************************//
  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC1MFP_Msk)) | (SYS_GPC_MFPL_PC1MFP_GPIO); // PC.1
  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC2MFP_Msk)) | (SYS_GPC_MFPL_PC2MFP_GPIO); // PC.2
  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC3MFP_Msk)) | (SYS_GPC_MFPL_PC3MFP_GPIO); // PC.3
  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC4MFP_Msk)) | (SYS_GPC_MFPL_PC4MFP_GPIO); // PC.4
  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC5MFP_Msk)) | (SYS_GPC_MFPL_PC5MFP_GPIO); // PC.5
  SYS_LockReg();
}

void LED_GPIO_init(void) {
  GPIO_SetMode(PC, BIT1, GPIO_MODE_OUTPUT);
  GPIO_SetMode(PC, BIT2, GPIO_MODE_OUTPUT);
  GPIO_SetMode(PC, BIT3, GPIO_MODE_OUTPUT);
  GPIO_SetMode(PC, BIT4, GPIO_MODE_OUTPUT);
  GPIO_SetMode(PC, BIT5, GPIO_MODE_OUTPUT);
  PC1 = 0;
  PC2 = 0;
  PC3 = 0;
  PC4 = 0;
  PC5 = 0;
}

int main(void) {
  SYS_Init();
  LED_GPIO_init();
  // UART0_Init();
  while (1) {
    //(i == 0) ? (i = Bit_SET, printf("Led Set\n")) : (i = Bit_RESET, printf("Led ReSet\n"));
    (i == 0) ? (i = Bit_SET) : (i = Bit_RESET);
    PC1 = i;
    PC2 = i;
    PC3 = i;
    PC4 = i;
    PC5 = i;
    TIMER_Delay(TIMER0, 500000);
  }
}
/*************************** End of file ****************************/