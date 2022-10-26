
#include <stdio.h>
#include "NuMicro.h"
#include "stdbool.h"

typedef uint32_t  u32;
typedef uint16_t  u16;
typedef uint8_t   u8;

volatile bool pressed_500kHZ = true;
volatile bool pressed_PWM = true;

void GPABGH_IRQHandler(void){
 volatile u32 u32temp_1;
 pressed_PWM=!pressed_PWM;
  if(GPIO_GET_INT_FLAG(PA, BIT0)) {
    switch (pressed_PWM){
     case 0:
      PWM_ENABLE_COMPLEMENTARY_MODE(PWM0);
      PWM_EnableOutput(PWM0, 0xF);
      PA13=1;
      printf("PWM Start\n");
     break;
     case 1:
      PWM_DISABLE_COMPLEMENTARY_MODE(PWM0);
      PWM_DisableOutput(PWM0, 0xF);
      PA13=0;
      printf("PWM STOP\n");
     break;}
    /* Clear interrupt flag */
    GPIO_CLR_INT_FLAG(PA, BIT0);
   }
  else {
    /* Un-expected interrupt. Just clear all PA interrupts */
    u32temp_1 = PA->INTSRC;
    PA->INTSRC =  u32temp_1;
    printf("Un-expected interrupts port_A.\n");
  }
}

void GPCDEF_IRQHandler(void){
  volatile u32 u32temp_2;
  pressed_500kHZ =!pressed_500kHZ;
  if(GPIO_GET_INT_FLAG(PC, BIT0)) {
  switch (pressed_500kHZ){
     case 0:
      TIMER_Start(TIMER2);
       PA12=1;
       printf("PWM_500kHz Start\n");
      break;
      case 1:
       TIMER_Stop(TIMER2);
       PA12=0;
       printf("PWM_500kHz STOP\n");
      break;}
    /* Clear interrupt flag */
    GPIO_CLR_INT_FLAG(PC, BIT0);
  }
  else {
    /* Un-expected interrupt. Just clear all PC interrupts */
    u32temp_2 = PC->INTSRC;
    PC->INTSRC =  u32temp_2;
    printf("Un-expected interrupts port_C.\n");
  }
}


void SYS_Init(void)
{
    SYS_UnlockReg();
    /* Enable HIRC clock (Internal RC 48MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    /* Wait for HIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    /* Select HCLK clock source as HIRC and HCLK source divider as 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
    /* Enable UART0 clock */
    CLK_EnableModuleClock(UART0_MODULE);
    /* Switch UART0 clock source to HIRC */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
    /* Update System Core Clock */
    /* Enable PWM0 module clock */
    CLK_EnableModuleClock(TMR2_MODULE);
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_HIRC,  0);
    CLK_EnableModuleClock(PWM0_MODULE);
    CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL2_PWM0SEL_PCLK0,(uint32_t) NULL);
    SYS_ResetModule(PWM0_RST);
    SystemCoreClockUpdate();
    /* Set PB multi-function pins for UART0 RXD=PA.15 and TXD=PA.14 */
    SYS->GPA_MFPH = (SYS->GPA_MFPH & ~(SYS_GPA_MFPH_PA14MFP_Msk | SYS_GPA_MFPH_PA15MFP_Msk))    |       \
                    (SYS_GPA_MFPH_PA15MFP_UART0_RXD | SYS_GPA_MFPH_PA14MFP_UART0_TXD);
    // Set timer toggle out pin PB.3
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB3MFP_Msk))|(SYS_GPB_MFPL_PB3MFP_TM2);
    /* Set PB multi-function pins for PWM0 Channel 0 ~ 3 */
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB5MFP_Msk | SYS_GPB_MFPL_PB4MFP_Msk )) |
                    (SYS_GPB_MFPL_PB5MFP_PWM0_CH0 | SYS_GPB_MFPL_PB4MFP_PWM0_CH1);
    SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA3MFP_Msk |SYS_GPA_MFPL_PA2MFP_Msk))|
                                      (SYS_GPA_MFPL_PA3MFP_PWM0_CH2 |SYS_GPA_MFPL_PA2MFP_PWM0_CH3);
    SYS_LockReg();
}

int main()
{
    SYS_Init();

    GPIO_SetMode(PA, BIT12|BIT13, GPIO_MODE_OUTPUT);//PA.12 -> LED 500kHz, PA.13 -> LED PWM
    PA13=0;
    PA12=0;

    GPIO_SetMode(PA, BIT0, GPIO_MODE_INPUT); //PA.0  -> PWM button
    GPIO_SetMode(PC, BIT0, GPIO_MODE_INPUT); //PC.0  -> 500kHz button
    GPIO_EnableInt(PA, 0, GPIO_INT_FALLING);
    GPIO_EnableInt(PC, 0, GPIO_INT_FALLING);
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC,  GPIO_DBCTL_DBCLKSEL_8);
    GPIO_ENABLE_DEBOUNCE(PC, BIT0);//PC.0
    GPIO_ENABLE_DEBOUNCE(PA, BIT0);//PA.0
    NVIC_EnableIRQ(GPIO_PAPBPGPH_IRQn);
    NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);

    TIMER_Open(TIMER2, TIMER_TOGGLE_MODE, 1000000);
    TIMER_SELECT_TOUT_PIN(TIMER2, TIMER_TOUT_PIN_FROM_TX);



    PWM_ConfigOutputChannel(PWM0, 0, 503, 50);//500 kHz
    PWM_ConfigOutputChannel(PWM0, 2, 503, 50);//500 kHz

    SYS_UnlockReg();
    PWM_EnableDeadZone(PWM0, 0, 3000);
    PWM_EnableDeadZone(PWM0, 1, 3000);
    PWM_EnableDeadZone(PWM0, 2, 3000);
    PWM_EnableDeadZone(PWM0, 3, 3000);
    SYS_LockReg();
    PWM_Start(PWM0, 0xF);

    UART_Open(UART0, 115200);
    printf("Test Board\n");

    while(1);
}

