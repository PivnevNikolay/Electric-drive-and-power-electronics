/* M031EC1AE --> 32 KB Flash,4 KB SRAM.*/
#include <stdio.h>
#include "NuMicro.h"
#include "stdbool.h"
#include "init.h"

volatile bool pressed_500kHZ = true;
volatile bool pressed_PWM = true;
volatile bool Error1, Error2;

//--------------------------------------------------------//
void GPCDEF_IRQHandler(void) {
  volatile u32 u32temp_2;
  pressed_500kHZ = !pressed_500kHZ;
  if (GPIO_GET_INT_FLAG(PC, BIT0)) {  //PWM_500kHz button
    switch (pressed_500kHZ) {
      case 0:
        TIMER_Start(TIMER2);
        PA12 = 1;
        printf("PWM_500kHz Start\n");
        GPIO_EnableInt(PA, 0, GPIO_INT_FALLING);
        break;
      case 1:
        if (pressed_PWM == false) {
          PWM_DISABLE_COMPLEMENTARY_MODE(PWM0);
          PWM_DisableOutput(PWM0, 0xF);
          PA13 = 0;
          printf("PWM STOP\n");
          pressed_PWM = true;
        }
        TIMER_Stop(TIMER2);
        PA12 = 0;
        printf("PWM_500kHz STOP\n");
        GPIO_DisableInt(PA, 0);
        break;
    }
    /* Clear interrupt flag */
    GPIO_CLR_INT_FLAG(PC, BIT0);
  } else {
    /* Un-expected interrupt. Just clear all PC interrupts */
    u32temp_2 = PC->INTSRC;
    PC->INTSRC = u32temp_2;
    printf("Un-expected interrupts port_C.\n");
  }
}
//--------------------------------------------------------//
void GPABGH_IRQHandler(void) {
  volatile u32 u32temp_1;
  if (GPIO_GET_INT_FLAG(PA, BIT0)) {
    pressed_PWM = !pressed_PWM;
    switch (pressed_PWM) {
      case 0:
        PWM_ENABLE_COMPLEMENTARY_MODE(PWM0);
        PWM_EnableOutput(PWM0, 0xF);
        PA13 = 1;
        printf("PWM Start\n");
        break;
      case 1:
        PWM_DISABLE_COMPLEMENTARY_MODE(PWM0);
        PWM_DisableOutput(PWM0, 0xF);
        PA13 = 0;
        printf("PWM STOP\n");
        break;
    }
    /* Clear interrupt flag */
    GPIO_CLR_INT_FLAG(PA, BIT0);
  } else if (GPIO_GET_INT_FLAG(PA, BIT1)) {
    PC1 = 1;
    printf("press Reset button\n");
    PC1 = 0;
    GPIO_CLR_INT_FLAG(PA, BIT1);
  } else {
    /* Un-expected interrupt. Just clear all PA interrupts */
    u32temp_1 = PA->INTSRC;
    PA->INTSRC = u32temp_1;
    printf("Un-expected interrupts port_A.\n");
  }
}
//--------------------------------------------------------//

int main() {
  SYS_Init();
  GPIO_Init();
  Timer_Init();
  PWM_Init();

  UART_Open(UART0, 115200);
  printf("Hello Elsiel\n");

  while (1) {
    Error1 = PF2;
    Error2 = PF3;
    PB13 = !Error1;
    PB12 = !Error2;
    TIMER_Delay(TIMER0, 1000000);
  }
}
