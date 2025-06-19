/**-------------------------------------------------------------------
 \date  19.06.2025
 *
 * Данный код позволяет проверить работу XL6009 DC-DC boost module,
 * кнопка запуска подключена к PC.0, сигнал EN формируется на выходе PB.3 
 * через цифровой буфер не инвертирующий 74LVC1G125 на вывод EN XL6009. 
 *
 *    M031EC1AE
 *   ------------
 *  |            |
 *  |            |
 *  |            |
 *  |        PC.0| <----  EN_XL6009_IN (Button_pin)
 *  |            |
 *  |       PB.3 | ---->  EN_XL6009 (out_pin) 74LVC1G125
 *  |            |
 *  |      +3.3V |
 *  |        GND |
 *
 *\ authors ScuratovaAnna, PivnevNikolay
 *\ сode debugging PivnevNikolay
 */
 //**************************  StdDriver  *****************************
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
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC,  GPIO_DBCTL_DBCLKSEL_16);
    GPIO_ENABLE_DEBOUNCE(PC, BIT0);
    NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);
    while(1);
}
/*************************** End of file ****************************/
//**************************  RegBased  ******************************

#include "NuMicro.h"
#include "stdbool.h"
#include <stdio.h>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

#define SET_BIT(REG, BIT) ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT) ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT) ((REG) & (BIT))
#define CLEAR_REG(REG) ((REG) = (0x0))
#define WRITE_REG(REG, VAL) ((REG) = (VAL))
#define READ_REG(REG) ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK) WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

void GPCDEF_IRQHandler(void) {
  volatile u32 u32temp_2;
  if (READ_BIT(PC->INTSRC, BIT0)) {
    PB3 ^= 1;
    //printf("XL6009_out)\n");
    /* Clear interrupt flag */
    WRITE_REG(PC->INTSRC, BIT0);
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
  SystemCoreClockUpdate();
  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC0MFP_Msk)) | (SYS_GPC_MFPL_PC0MFP_GPIO); // PC.0
  SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB3MFP_Msk)) | (SYS_GPB_MFPL_PB3MFP_GPIO); // PB.3
  SYS_LockReg();
}

int main(void) {
  SYS_Init();
  // GPIO_SetMode(PB, BIT3, GPIO_MODE_OUTPUT);
  // PB3 = 0;
  MODIFY_REG(PB->MODE, GPIO_MODE_MODE3_Msk, GPIO_MODE_OUTPUT << GPIO_MODE_MODE3_Pos);
  CLEAR_BIT(PB->DOUT, GPIO_DOUT_DOUT3_Msk);
  // GPIO_SetMode(PC, BIT0, GPIO_MODE_INPUT);
  MODIFY_REG(PC->MODE, GPIO_MODE_MODE0_Msk, GPIO_MODE_INPUT << GPIO_MODE_MODE0_Pos);

  // GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_16);
  // GPIO_ENABLE_DEBOUNCE(PC, BIT0);
  GPIO->DBCTL = GPIO_DBCTL_ICLKON_Msk | GPIO_DBCTL_DBCLKSRC_LIRC | GPIO_DBCTL_DBCLKSEL_16;
  PC->DBEN |= BIT0;

  // GPIO_EnableInt(PC, 0, GPIO_INT_FALLING);
  // NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);
  CLEAR_BIT(PC->INTTYPE, BIT0);                                 // set PC0 to edge trigger interrupt
  PC->INTEN = (PC->INTEN & ~(BIT16 << GPIO_INTEN_RHIEN0_Pos)) | // disable rising  edge trigger interrupt
              (BIT0);                                           // enable  falling edge trigger interrupt
  NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);
  while (1) {
  }
}
/*************************** End of file ****************************/
