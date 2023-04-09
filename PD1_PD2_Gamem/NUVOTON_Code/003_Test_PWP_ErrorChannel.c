#include "NuMicro.h"
#include "stdbool.h"
#include <stdio.h>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

volatile bool pressed_PWM_PD1 = true;
volatile bool pressed_PWM_PD2 = true;
volatile u8 error_counter_PD1 = 0;
volatile u8 error_counter_PD2_01 = 10;
volatile u8 error_counter_PD2_02 = 20;

void UART_Open(UART_T *uart, uint32_t u32baudrate);
void PWM_Init(void);

void GPABGH_IRQHandler(void) {
  volatile u32 u32temp_1;
  if (GPIO_GET_INT_FLAG(PB, BIT1)) {
    pressed_PWM_PD2 = !pressed_PWM_PD2;
    u8 PWM_PD2 = pressed_PWM_PD2;
    switch (PWM_PD2) {
    case 0:
      PWM_ENABLE_COMPLEMENTARY_MODE(PWM0);
      PWM_EnableOutput(PWM0, 0xF);
      PA13 = 1;
      PC1 = 1;
      printf("PWM_PD2 Start\n");
      break;
    case 1:
      PWM_DISABLE_COMPLEMENTARY_MODE(PWM0);
      PWM_DisableOutput(PWM0, 0xF);
      PA13 = 0;
      PC1 = 0;
      printf("PWM_PD2 STOP\n");
      break;
    }
    GPIO_CLR_INT_FLAG(PB, BIT1);
  } else if (GPIO_GET_INT_FLAG(PB, BIT2)) {
    pressed_PWM_PD1 = !pressed_PWM_PD1;
    u8 PWM_PD1 = pressed_PWM_PD1;
    switch (PWM_PD1) {
    case 0:
      PWM_EnableOutput(PWM0, 0xF);
      PA12 = 1;
      printf("PWM_PD1 Start\n");
      break;
    case 1:
      PWM_DisableOutput(PWM0, 0xF);
      PA12 = 0;
      printf("PWM_PD1 STOP\n");
      break;
    }
    GPIO_CLR_INT_FLAG(PB, BIT2);
  } else if (GPIO_GET_INT_FLAG(PB, BIT12)) {
    error_counter_PD2_02++;
    GPIO_CLR_INT_FLAG(PB, BIT12);
  } else if (GPIO_GET_INT_FLAG(PB, BIT13)) {
    error_counter_PD2_01++;
    GPIO_CLR_INT_FLAG(PB, BIT13);
  } else if (GPIO_GET_INT_FLAG(PB, BIT14)) {
    error_counter_PD1++;
    GPIO_CLR_INT_FLAG(PB, BIT14);
  } else {
    // Un-expected interrupt. Just clear all PA interrupts
    u32temp_1 = PB->INTSRC;
    PB->INTSRC = u32temp_1;
    printf("Un-expected interrupts port_A.\n");
  }
}

void GPCDEF_IRQHandler(void) {
  volatile u32 u32temp_2;
  if (GPIO_GET_INT_FLAG(PC, BIT0)) {
    GPIO_TOGGLE(PB3);
    printf("XL6009_out)\n");
    /* Clear interrupt flag */
    GPIO_CLR_INT_FLAG(PC, BIT0);
  } else {
    /* Un-expected interrupt. Just clear all PC interrupts */
    u32temp_2 = PC->INTSRC;
    PC->INTSRC = u32temp_2;
    printf("Un-expected interrupts port_C.\n");
  }
}

void SYS_Init(void) {
  SYS_UnlockReg();
  CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
  CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
  CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
  CLK_EnableModuleClock(TMR0_MODULE);
  CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, 0);
  CLK_EnableModuleClock(UART0_MODULE);
  CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
  CLK_EnableModuleClock(PWM0_MODULE); // Enable PWM0 module clock
  CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL2_PWM0SEL_PCLK0, (uint32_t)NULL);
  SYS_ResetModule(PWM0_RST); // reset PWM module

  SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB5MFP_Msk | SYS_GPB_MFPL_PB4MFP_Msk)) |
                  (SYS_GPB_MFPL_PB5MFP_PWM0_CH0 | SYS_GPB_MFPL_PB4MFP_PWM0_CH1);

  SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA3MFP_Msk)) | (SYS_GPA_MFPL_PA3MFP_PWM0_CH2);

  SystemCoreClockUpdate();

  /* Set PB multi-function pins for UART0 RXD=PA.15 and TXD=PA.14 */
  SYS->GPA_MFPH = (SYS->GPA_MFPH & ~(SYS_GPA_MFPH_PA14MFP_Msk | SYS_GPA_MFPH_PA15MFP_Msk)) |
                  (SYS_GPA_MFPH_PA15MFP_UART0_RXD | SYS_GPA_MFPH_PA14MFP_UART0_TXD);
  SYS_LockReg();
}

int main() {
  SYS_Init();
  UART_Open(UART0, 115200);
  PWM_Init();

  GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT);
  PA0 = 0;
  GPIO_SetMode(PA, BIT1, GPIO_MODE_OUTPUT);
  PA1 = 0;
  GPIO_SetMode(PA, BIT2, GPIO_MODE_OUTPUT);
  PA2 = 0;

  GPIO_SetMode(PA, BIT12, GPIO_MODE_OUTPUT); // Led_PWM_PD_1     PA.12
  PA12 = 0;
  GPIO_SetMode(PA, BIT13, GPIO_MODE_OUTPUT); // Led_PWM_PD2_01   PA.13
  PA13 = 0;
  GPIO_SetMode(PC, BIT1, GPIO_MODE_OUTPUT); // Led_PWM_PD2_02   PC.1
  PC1 = 0;

  GPIO_SetMode(PB, BIT3, GPIO_MODE_OUTPUT); // EN_XL6009_OUT
  PB3 = 0;

  GPIO_SetMode(PB, BIT2, GPIO_MODE_INPUT); // PWM_PD1_button
  GPIO_EnableInt(PB, 2, GPIO_INT_FALLING);
  GPIO_SetMode(PB, BIT1, GPIO_MODE_INPUT); // PWM_PD2_button_EN
  GPIO_EnableInt(PB, 1, GPIO_INT_FALLING);
  GPIO_SetMode(PC, BIT0, GPIO_MODE_INPUT); // EN_XL6009_IN_button
  GPIO_EnableInt(PC, 0, GPIO_INT_FALLING);
  GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_16);
  GPIO_ENABLE_DEBOUNCE(PB, BIT2);
  GPIO_ENABLE_DEBOUNCE(PB, BIT1);
  GPIO_ENABLE_DEBOUNCE(PC, BIT0);

  GPIO_SetMode(PB, BIT12, GPIO_MODE_INPUT);
  GPIO_EnableInt(PB, 12, GPIO_INT_FALLING);
  GPIO_SetMode(PB, BIT13, GPIO_MODE_INPUT);
  GPIO_EnableInt(PB, 13, GPIO_INT_FALLING);
  GPIO_SetMode(PB, BIT14, GPIO_MODE_INPUT);
  GPIO_EnableInt(PB, 14, GPIO_INT_FALLING);

  NVIC_EnableIRQ(GPIO_PAPBPGPH_IRQn);
  NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);

  while (1) {
    if (error_counter_PD1 >= 255 || error_counter_PD2_01 >= 255 || error_counter_PD2_02 >= 255) {
      PA0 = 1;
      PA1 = 1;
      PA2 = 1;
      TIMER_Delay(TIMER0, 100000);
      PA0 = 0;
      PA1 = 0;
      PA2 = 0;
      error_counter_PD1 = 0;
      error_counter_PD2_01 = 10;
      error_counter_PD2_02 = 20;
}
}
}

void PWM_Init(void) {
  PWM_ConfigOutputChannel(PWM0, 0, 504, 50); // 500 kHz
  PWM_ConfigOutputChannel(PWM0, 2, 504, 50); // 500 kHz
  PWM_Start(PWM0, 0xF);
  SYS_UnlockReg();
  PWM_EnableDeadZone(PWM0, 0, 30);
  PWM_EnableDeadZone(PWM0, 1, 30);
  SYS_LockReg();
}