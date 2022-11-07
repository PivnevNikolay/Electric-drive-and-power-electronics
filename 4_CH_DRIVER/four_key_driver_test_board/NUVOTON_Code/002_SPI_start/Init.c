#include <stdio.h>
#include "NuMicro.h"
#define SPI_CLK_FREQ    2000000
//-----------------------------------------------------------------------------------//
void SYS_Init(void)
{
   SYS_UnlockReg();
   CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
   CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
   CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
   // Select HIRC as the clock source of UART0 
   //CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
   // Select PCLK1 as the clock source of SPI0 
   CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL2_SPI0SEL_PCLK1, MODULE_NoMsk);
   // Enable UART peripheral clock 
   //CLK_EnableModuleClock(UART0_MODULE);
   // Enable SPI0 peripheral clock 
   CLK_EnableModuleClock(SPI0_MODULE);
   // Init I/O Multi-function
   // Set PB multi-function pins for UART0 RXD=PB.12 and TXD=PB.13
   SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
    //Timer1
    CLK_EnableModuleClock(TMR1_MODULE);
   CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HIRC,  0);
   // Setup SPI0 multi-function pins 
   // --- PA.3 is SPI0_SS,   PA.2 is SPI0_CLK, PA.1 is SPI0_MISO, PA.0 is SPI0_MOSI ---//
   //SYS->GPA_MFPL = SYS_GPA_MFPL_PA3MFP_SPI0_SS | SYS_GPA_MFPL_PA2MFP_SPI0_CLK | SYS_GPA_MFPL_PA1MFP_SPI0_MISO | SYS_GPA_MFPL_PA0MFP_SPI0_MOSI;
    SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA3MFP_Msk |
                                      SYS_GPA_MFPL_PA2MFP_Msk |
                                       SYS_GPA_MFPL_PA1MFP_Msk |
                                      SYS_GPA_MFPL_PA0MFP_Msk)) |
                     (SYS_GPA_MFPL_PA3MFP_SPI0_SS |
                     SYS_GPA_MFPL_PA2MFP_SPI0_CLK |
                    SYS_GPA_MFPL_PA1MFP_SPI0_MISO |
                     SYS_GPA_MFPL_PA0MFP_SPI0_MOSI);
    // Update System Core Clock 
    // User can use SystemCoreClockUpdate() to calculate SystemCoreClock and CyclesPerUs automatically.
    SystemCoreClockUpdate();
    SYS_LockReg();
}

void SPI_Init(void)
{
    //---------------------------------------------------------------------------------------------------------
    // Init SPI
    //---------------------------------------------------------------------------------------------------------
    // Configure as a master, clock idle low, 8-bit transaction, drive output on falling clock edge and latch input on rising edge. 
    // Set IP clock divider. SPI clock rate = 2MHz
    SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 8, SPI_CLK_FREQ);

    // Enable the automatic hardware slave select function. Select the SS pin and configure as low-active. 
    SPI_EnableAutoSS(SPI0, SPI_SS, SPI_SS_ACTIVE_LOW);
}

