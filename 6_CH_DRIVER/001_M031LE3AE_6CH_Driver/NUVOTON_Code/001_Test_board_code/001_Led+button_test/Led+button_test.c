

#include "NuMicro.h"
#include "stdbool.h"
#include <stdio.h>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

#define Bit_SET 1
#define Bit_RESET 0

volatile bool i;
volatile u8 pressed = false;

void LED_GPIO_init_out(void);
void LED_GPIO_init_input(void);
void TIMER_init_out(void);

void GPABGH_IRQHandler(void) {
  volatile uint32_t temp;
  if (GPIO_GET_INT_FLAG(PB, BIT6)) {
    switch (pressed) {
    case 0:
      TIMER_Start(TIMER2);
      PB7 = 1;
      pressed = true;
      break;
    case 1:
      TIMER_Stop(TIMER2);
      PB7 = 0;
      pressed = false;
      break;
    }
    GPIO_CLR_INT_FLAG(PB, BIT6);
  } else {
    /* Un-expected interrupt. Just clear all PB interrupts */
    temp = PB->INTSRC;
    PB->INTSRC = temp;
  }
}
//***************************************************************//
void GPCDEF_IRQHandler(void) {
  volatile u32 u32temp_2;
  if (GPIO_GET_INT_FLAG(PC, BIT0)) {
    GPIO_TOGGLE(PB15); // PB.15 --> EN_XL6009_out
    /* Clear interrupt flag */
    GPIO_CLR_INT_FLAG(PC, BIT0);
  } else {
    /* Un-expected interrupt. Just clear all PC interrupts */
    u32temp_2 = PC->INTSRC;
    PC->INTSRC = u32temp_2;
  }
}
//***************************************************************//
void SYS_Init(void) {
  SYS_UnlockReg();
  //**** Set GPF multi-function pins for XT1_IN and XT1_OUT *******//
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
  CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_PCLK0, 0);
  //********* TMR2_MODULE  ****************************************//
  CLK_EnableModuleClock(TMR2_MODULE);
  CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_PCLK1, 0);
  SystemCoreClockUpdate();
  //********* Set timer toggle out pin PB.3 ***********************//
  SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB3MFP_Msk)) |
                  (SYS_GPB_MFPL_PB3MFP_TM2);
  //********* UART0 RXD=PA.15 and TXD=PA.14 ***********************//
  // SYS->GPA_MFPH = (SYS->GPA_MFPH & ~(SYS_GPA_MFPH_PA14MFP_Msk | SYS_GPA_MFPH_PA15MFP_Msk)) |
  //                (SYS_GPA_MFPH_PA15MFP_UART0_RXD | SYS_GPA_MFPH_PA14MFP_UART0_TXD);
  //*********************** GPIO PB.15 ****************************//
  SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB6MFP_Msk)) | (SYS_GPB_MFPL_PB6MFP_GPIO);   // PB.6  --> 500kHz_start_stop_buttton
  SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB7MFP_Msk)) | (SYS_GPB_MFPL_PB7MFP_GPIO);   // PB.7  --> LED_500kHz_out
  SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB15MFP_Msk)) | (SYS_GPB_MFPH_PB15MFP_GPIO); // PB.15 --> EN_XL6009_out
  //********* GPIO PC.1 PC.2 PC.3 PC.4 PC.5 ***********************//
  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC0MFP_Msk)) | (SYS_GPC_MFPL_PC0MFP_GPIO); // PC.0 --> EN_XL6009_buttonn
  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC1MFP_Msk)) | (SYS_GPC_MFPL_PC1MFP_GPIO); // PC.1 --> Error1_LED
  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC2MFP_Msk)) | (SYS_GPC_MFPL_PC2MFP_GPIO); // PC.2 --> Error2_LED
  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC3MFP_Msk)) | (SYS_GPC_MFPL_PC3MFP_GPIO); // PC.3 --> Error3_LED
  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC4MFP_Msk)) | (SYS_GPC_MFPL_PC4MFP_GPIO); // PC.4 --> SIN_LED
  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC5MFP_Msk)) | (SYS_GPC_MFPL_PC5MFP_GPIO); // PC.5 --> PWM_LED
  SYS_LockReg();
}

int main(void) {
  SYS_Init();
  LED_GPIO_init_out();
  LED_GPIO_init_input();
  TIMER_init_out();
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

//************************* output ******************************//
void LED_GPIO_init_out(void) {
  GPIO_SetMode(PA, BIT6, GPIO_MODE_OUTPUT);
  PA6 = 0;
  GPIO_SetMode(PB, BIT7 | BIT15, GPIO_MODE_OUTPUT);
  PB7 = 0;
  PB15 = 0;
  GPIO_SetMode(PC, BIT1 | BIT2 | BIT3 | BIT4 | BIT5, GPIO_MODE_OUTPUT);
  PC1 = 0;
  PC2 = 0;
  PC3 = 0;
  PC4 = 0;
  PC5 = 0;
}
//************************** input ******************************//
void LED_GPIO_init_input(void) {
  GPIO_SetMode(PB, BIT6, GPIO_MODE_INPUT);
  GPIO_EnableInt(PB, 6, GPIO_INT_FALLING);
  GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_8);
  GPIO_ENABLE_DEBOUNCE(PB, BIT6);
  NVIC_EnableIRQ(GPIO_PAPBPGPH_IRQn);
  GPIO_SetMode(PC, BIT0, GPIO_MODE_INPUT);
  GPIO_EnableInt(PC, 0, GPIO_INT_FALLING);
  GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_8);
  GPIO_ENABLE_DEBOUNCE(PC, BIT0);
  NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);
}
//************************** input ******************************//
void TIMER_init_out(void) {
  TIMER_Open(TIMER2, TIMER_TOGGLE_MODE, 1000000);
  TIMER_SELECT_TOUT_PIN(TIMER2, TIMER_TOUT_PIN_FROM_TX);
}
//************************** End of file ************************//
