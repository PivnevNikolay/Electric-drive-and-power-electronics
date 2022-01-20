#include <stdio.h>
#include "NuMicro.h"
//-----------------------------------------------------------------------------------//
void SYS_Init(void)
{  SYS_UnlockReg();
// Enable HIRC 
  CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
// Waiting for HIRC clock ready 
  CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
// Switch HCLK clock source to HIRC 
  CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
// Set both PCLK0 and PCLK1 as HCLK/2 
  CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2);
// Switch UART0 clock source to HIRC 
  CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
// Enable UART peripheral clock 
  CLK_EnableModuleClock(UART0_MODULE);
// Enable IP clock 
  CLK_EnableModuleClock(TMR0_MODULE);
  CLK_EnableModuleClock(TMR1_MODULE);
// Select IP clock source 
  CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_PCLK0, 0);//??
  CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HIRC,  0);//??CLK_CLKSEL1_TMR1SEL_HIRC
// Update System Core Clock 
// User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. 
    SystemCoreClockUpdate();
// Set GPB multi-function pins for UART0 RXD and TXD
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
// Set timer toggle out pin PB.5
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB5MFP_Msk)) |
                    (SYS_GPB_MFPL_PB5MFP_TM0);
  SYS_LockReg();
}
//-----------------------------------------------------------------------------------//
void UART0_Init(void)
{
  SYS_ResetModule(UART0_RST);
  UART_Open(UART0, 115200);
}
//-----------------------------------------------------------------------------------//
