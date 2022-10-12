/* M031EC1AE --> 32 KB Flash,4 KB SRAM.
 * Test board, test GPIO output, test buttons
 */

#include <stdio.h>
#include "NuMicro.h"

typedef uint32_t  u32;
typedef uint16_t  u16;
typedef uint8_t   u8;

void GPABGH_IRQHandler(void){
  volatile u32 u32temp_1;
  if(GPIO_GET_INT_FLAG(PA, BIT0)) {
    GPIO_TOGGLE(PA13);
    printf("press PWM button\n");
    /* Clear interrupt flag */
    GPIO_CLR_INT_FLAG(PA, BIT0);
	}
  else if(GPIO_GET_INT_FLAG(PA, BIT1)){
    GPIO_TOGGLE(PB13);
    GPIO_TOGGLE(PB12);
    printf("press Reset button\n");
    GPIO_CLR_INT_FLAG(PA, BIT1);
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
  if(GPIO_GET_INT_FLAG(PC, BIT0)) {
    GPIO_TOGGLE(PA12);
    printf("press 500kHz button\n");
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
    SystemCoreClockUpdate();
    /* Set PB multi-function pins for UART0 RXD=PA.15 and TXD=PA.14 */
    SYS->GPA_MFPH = (SYS->GPA_MFPH & ~(SYS_GPA_MFPH_PA14MFP_Msk | SYS_GPA_MFPH_PA15MFP_Msk))    |       \
                    (SYS_GPA_MFPH_PA15MFP_UART0_RXD | SYS_GPA_MFPH_PA14MFP_UART0_TXD);

    SYS_LockReg();
}

int main()
{
    SYS_Init();
    UART_Open(UART0, 115200);
    GPIO_SetMode(PA, BIT2|BIT3|BIT12|BIT13, GPIO_MODE_OUTPUT);//PA.2 -> BL, PA.3 -> BH, PA.12 -> LED 500kHz, PA.13 -> LED PWM
    PA2=0;
    PA3=0;
    PA13=0;
    PA12=0;
    GPIO_SetMode(PB, BIT4|BIT5|BIT12|BIT13, GPIO_MODE_OUTPUT);//PB.4 -> AL, PB.5 -> AH, PB.12  -> Error2 , PB.13 -> Error1
    PB4=0;
    PB5=0;
    PB13=0;
    PB12=0;
    GPIO_SetMode(PA, BIT1 | BIT0, GPIO_MODE_INPUT); //PA.0  -> PWM button, PA.1 -> Reset Error1 and Error2 button
    GPIO_SetMode(PC, BIT0, GPIO_MODE_INPUT);        //PC.0  -> 500kHz button
    GPIO_EnableInt(PA, 0, GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 1, GPIO_INT_FALLING);
    GPIO_EnableInt(PC, 0, GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPIO_PAPBPGPH_IRQn);
    NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);
    printf("Hello World\n");// Test Uart0
    while(1){
    GPIO_TOGGLE(PA2);
    CLK_SysTickDelay(174762);
    GPIO_TOGGLE(PA3);
    CLK_SysTickDelay(174762);
    GPIO_TOGGLE(PB4);
    CLK_SysTickDelay(174762);
    GPIO_TOGGLE(PB5);
    CLK_SysTickDelay(174762);
    }
}
