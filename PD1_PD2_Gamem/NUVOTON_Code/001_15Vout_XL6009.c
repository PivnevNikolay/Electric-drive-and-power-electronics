#include <stdio.h>

#include "NuMicro.h"

typedef uint32_t  u32;
typedef uint16_t  u16;
typedef uint8_t   u8;

void UART_Open(UART_T *uart, uint32_t u32baudrate);

void GPCDEF_IRQHandler(void){
  volatile u32 u32temp_2;
  if(GPIO_GET_INT_FLAG(PC, BIT0)) {
    GPIO_TOGGLE(PB3);
    printf("XL6009_out)\n");
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
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
    SystemCoreClockUpdate();

    /* Set PB multi-function pins for UART0 RXD=PB.12 and TXD=PB.13 */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk))    |       \
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
    SYS_LockReg();
}

int main()
{
    SYS_Init();
    UART_Open(UART0, 115200);
    GPIO_SetMode(PB, BIT3, GPIO_MODE_OUTPUT);
    PB3 = 0;
    GPIO_SetMode(PC, BIT0, GPIO_MODE_INPUT);
    GPIO_EnableInt(PC, 0, GPIO_INT_FALLING);
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC,  GPIO_DBCTL_DBCLKSEL_8);
    GPIO_ENABLE_DEBOUNCE(PC, BIT0);
    NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);
    while(1);
}
