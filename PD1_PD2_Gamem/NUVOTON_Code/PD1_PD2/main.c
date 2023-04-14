/*
 *ПД-1 Затвор ХР-2
 *
 */
#include "NuMicro.h"
#include "init.h"
#include "stdbool.h"
#include <stdio.h>

volatile bool pressed_15V_EN = true;
volatile bool pressed_PWM_PD1 = true;
volatile bool pressed_PWM_PD2 = true;
volatile u8 error_counter_PD1 = 0;
volatile u8 error_counter_PD2_01 = 5;
volatile u8 error_counter_PD2_02 = 10;

void UART_Open(UART_T *uart, uint32_t u32baudrate);

void GPABGH_IRQHandler(void) {
  volatile u32 u32temp_1;
  //********************** PD-1  PB.2 ******************************//
  if (GPIO_GET_INT_FLAG(PB, BIT2)) { // button PWM_PD1 PB.2
    pressed_PWM_PD1 = !pressed_PWM_PD1;
    u8 PWM_PD1 = pressed_PWM_PD1;
    switch (PWM_PD1) {
    case 0:
      PWM_Init_PD1();                           // Init_PWM
      PWM_EnableOutput(PWM0, 0x4);              // Start_PWM
      GPIO_EnableInt(PB, 14, GPIO_INT_FALLING); // start input Err_PD1
      GPIO_DisableInt(PB, 1);                   // PWM_PD2_button
      PA12 = 1;                                 // LED_PD1
      printf("PWM_PD1 Start\n");
      break;
    case 1:
      GPIO_DisableInt(PB, 14);                 // stop input Err_PD1
      PWM_DisableOutput(PWM0, 0x4);            // Stop_PWM
      PA12 = 0;                                // LED_PD1
      GPIO_EnableInt(PB, 1, GPIO_INT_FALLING); // PWM_PD2_button
      printf("PWM_PD1 STOP\n");
      break;
    }
    GPIO_CLR_INT_FLAG(PB, BIT2);
  }
  //****************** error_counter_PD1 ***************************//
  else if (GPIO_GET_INT_FLAG(PB, BIT14)) {
    error_counter_PD1++;
    GPIO_CLR_INT_FLAG(PB, BIT14);
  }
  //********************** PD-2 PB.1 *******************************//
  else if (GPIO_GET_INT_FLAG(PB, BIT1)) {
    pressed_PWM_PD2 = !pressed_PWM_PD2;
    u8 PWM_PD2 = pressed_PWM_PD2;
    switch (PWM_PD2) { // button PWM_PD2 PB.1
    case 0:
      PWM_Init_PD2_01_02(); // Init_PWM
      PWM_ENABLE_COMPLEMENTARY_MODE(PWM0);
      PWM_EnableOutput(PWM0, 0x3);              // Start_PWM
      GPIO_EnableInt(PB, 13, GPIO_INT_FALLING); // start input Err_PD2_1
      GPIO_EnableInt(PB, 12, GPIO_INT_FALLING); // start input Err_PD2_2
      GPIO_DisableInt(PB, 2);                   // PWM_PD2_button
      PA13 = 1;                                 // LED_PD2_1
      PC1 = 1;                                  // LED_PD2_2
      printf("PWM_PD2 Start\n");
      break;
    case 1:
      GPIO_DisableInt(PB, 13);                 // stop input Err_PD2_1
      GPIO_DisableInt(PB, 12);                 // stop input Err_PD2_2
      PWM_DisableOutput(PWM0, 0x3);            // Stop_PWM
      PA13 = 0;                                // LED_PD2_1
      PC1 = 0;                                 // LED_PD2_2
      GPIO_EnableInt(PB, 2, GPIO_INT_FALLING); // PWM_PD1_button
      printf("PWM_PD2 STOP\n");
      break;
    }
    GPIO_CLR_INT_FLAG(PB, BIT1);
  }
  //****************** error_counter_PD2_02 ***************************//
  else if (GPIO_GET_INT_FLAG(PB, BIT12)) {
    error_counter_PD2_02++;
    GPIO_CLR_INT_FLAG(PB, BIT12);
  }
  //****************** error_counter_PD2_01 ***************************//
  else if (GPIO_GET_INT_FLAG(PB, BIT13)) {
    error_counter_PD2_01++;
    GPIO_CLR_INT_FLAG(PB, BIT13);
  }
  //*******************************************************************//
  else {
    // Un-expected interrupt. Just clear all PA interrupts
    u32temp_1 = PB->INTSRC;
    PB->INTSRC = u32temp_1;
    printf("Un-expected interrupts port_A.\n");
  }
}

void GPCDEF_IRQHandler(void) {
  volatile u32 u32temp_2;
  if (GPIO_GET_INT_FLAG(PC, BIT0)) {
    pressed_15V_EN = !pressed_15V_EN;
    u8 EN_15V = pressed_15V_EN;
    switch (EN_15V) {
    case 0:
      PB3 = 1; // XL6009_out_enable
      // PWM_PD1_button
      GPIO_EnableInt(PB, 2, GPIO_INT_FALLING);
      // PWM_PD2_button
      GPIO_EnableInt(PB, 1, GPIO_INT_FALLING);
      printf("15V Start\n");
      break;
    case 1:
      PWM_DisableOutput(PWM0, 0xF);
      GPIO_DisableInt(PB, 14); // stop input Err_PD1
      GPIO_DisableInt(PB, 13); // stop input Err_PD2_1
      GPIO_DisableInt(PB, 12); // stop input Err_PD2_2
      PA12 = 0;                // LED_PD1
      PA13 = 0;                // LED_PD2_1
      PC1 = 0;                 // LED_PD2_2
      pressed_PWM_PD1 = true;
      pressed_PWM_PD2 = true;
      PB3 = 0; // XL6009_out_disabled
      // PWM_PD1_button
      GPIO_DisableInt(PB, 2);
      // PWM_PD2_button
      GPIO_DisableInt(PB, 1);
      printf("15V STOP\n");
      break;
    }
    GPIO_CLR_INT_FLAG(PC, BIT0);
  } else {
    u32temp_2 = PC->INTSRC;
    PC->INTSRC = u32temp_2;
    printf("Un-expected interrupts port_C.\n");
  }
}

int main() {
  SYS_Init();
  UART_Open(UART0, 115200);
  LED_GPIO_init();
  XL6009_OUTpin_init();
  Button_ERRinputPIN_init();

  while (1) {
    if (error_counter_PD1 >= 255) {
      PA0 = 1;
      TIMER_Delay(TIMER0, 100000);
      PA0 = 0;
      error_counter_PD1 = 0;
    } else if (error_counter_PD2_01 >= 255) {
      PA1 = 1;
      TIMER_Delay(TIMER0, 100000);
      PA1 = 0;
      error_counter_PD2_01 = 5;
    } else if (error_counter_PD2_02 >= 255) {
      PA2 = 1;
      TIMER_Delay(TIMER0, 100000);
      PA2 = 0;
      error_counter_PD2_02 = 10;
    }
  }
}