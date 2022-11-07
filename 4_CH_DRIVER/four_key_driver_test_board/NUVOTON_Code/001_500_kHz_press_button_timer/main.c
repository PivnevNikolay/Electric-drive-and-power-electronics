
#include <stdio.h>
#include "NuMicro.h"
#include "stdbool.h"
#include "Init.h"
//-----------------------------------------------------------------------------------//
bool pressed = true;
bool condition = true;
//-----------------------------------------------------------------------------------//
void GPABGH_IRQHandler(void)
{
  volatile uint32_t temp;
/* To check if PB.2 interrupt occurred */
  if(GPIO_GET_INT_FLAG(PB, BIT2))
  {
  condition = false;
  pressed = !pressed; 
  GPIO_CLR_INT_FLAG(PB, BIT2);
  }
    else
  {
/* Un-expected interrupt. Just clear all PB interrupts */
   temp = PB->INTSRC;
   PB->INTSRC = temp;
  }
}
//-----------------------------------------------------------------------------------//
int main(void)
{    
  SYS_Init();   
  UART0_Init();
//  printf("\nThis sample code use timer 0 to generate 500Hz toggle output to PA.0\n");
  GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT);
/* To generate 500HZ toggle output, timer frequency must set to 1000Hz.
Because toggle output state change on every timer timeout event */
  TIMER_Open(TIMER0, TIMER_TOGGLE_MODE, 1000000);
  TIMER_SELECT_TOUT_PIN(TIMER0, TIMER_TOUT_PIN_FROM_TX);
//TIMER_Start(TIMER1);
// Configure PB.2 as Input mode and enable interrupt by rising edge trigger
  GPIO_SetMode(PB, BIT2, GPIO_MODE_INPUT);
  GPIO_EnableInt(PB, 2, GPIO_INT_FALLING);
  NVIC_EnableIRQ(GPIO_PAPBPGPH_IRQn);
//-----------------------------------------------------------------------------------//
    while(1){
switch (pressed){      
 case 0:
  TIMER_Start(TIMER0);
  condition = true;	
 break;
 case 1:
  TIMER_Stop(TIMER0);
  condition = true;
 break;
  }
  while(condition){
  GPIO_TOGGLE(PA0);
  TIMER_Delay(TIMER1, 100000);
  }
 }
}
//----------------------------------------END----------------------------------------//
