/**-------------------------------------------------------------------
 \date  24.06.2025
 *
 * Данный код позволяет проверить работу  всех кнопок и светодиодов , которые есть в стенде.
 *
 *    M031EC1AE
 *   ------------
 *  |            |
 *  |            |
 *  |            |
 *  |        PC.0| <---- Button 
 *  |        PB.0| <---- Button 
 *  |        PB.1| <---- Button 
 *  |        PB.2| <---- Button
 *  |            |
 *  |       PA.0 | ----> Led 
 *  |       PA.1 | ----> Led
 *  |       PA.2 | ----> Led
 *  |      PA.12 | ----> Led
 *  |      PA.13 | ----> Led
 *  |       PB.3 | ----> Led
 *  |       PC.1 | ----> Led
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

void GPABGH_IRQHandler(void){
  volatile u32 u32temp_1;
  if(GPIO_GET_INT_FLAG(PB, BIT2)) {
    GPIO_TOGGLE(PA12);
    GPIO_CLR_INT_FLAG(PB, BIT2);
   }
  else if(GPIO_GET_INT_FLAG(PB, BIT1)){
    GPIO_TOGGLE(PA0);
    GPIO_TOGGLE(PA1);
    GPIO_TOGGLE(PA2);
    GPIO_CLR_INT_FLAG(PB, BIT1);
   }
  else if(GPIO_GET_INT_FLAG(PB, BIT0)){
    GPIO_TOGGLE(PA13);
    GPIO_TOGGLE(PC1);
    GPIO_CLR_INT_FLAG(PB, BIT0);
   }
  else {
    // Un-expected interrupt. Just clear all PA interrupts 
    u32temp_1 = PB->INTSRC;
    PB->INTSRC =  u32temp_1;
    printf("Un-expected interrupts port_A.\n");
  }
}

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
    GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT1, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT2, GPIO_MODE_OUTPUT);
    PA0  = 0;
    PA1  = 0;
    PA2  = 0;
    GPIO_SetMode(PB, BIT3,  GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT12, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT13, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT1,  GPIO_MODE_OUTPUT);
    PB3  = 0;
    PA12 = 0;
    PA13 = 0;
    PC1  = 0;

    GPIO_SetMode(PC, BIT0, GPIO_MODE_INPUT);
    GPIO_SetMode(PB, BIT0, GPIO_MODE_INPUT);
    GPIO_SetMode(PB, BIT1, GPIO_MODE_INPUT);
    GPIO_SetMode(PB, BIT2, GPIO_MODE_INPUT);

    GPIO_EnableInt(PC, 0, GPIO_INT_FALLING);
    GPIO_EnableInt(PB, 0, GPIO_INT_FALLING);
    GPIO_EnableInt(PB, 1, GPIO_INT_FALLING);
    GPIO_EnableInt(PB, 2, GPIO_INT_FALLING);

    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC,  GPIO_DBCTL_DBCLKSEL_16);

    GPIO_ENABLE_DEBOUNCE(PC, BIT0);
    GPIO_ENABLE_DEBOUNCE(PB, BIT0);
    GPIO_ENABLE_DEBOUNCE(PB, BIT1);
    GPIO_ENABLE_DEBOUNCE(PB, BIT2);

    NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);
    NVIC_EnableIRQ(GPIO_PAPBPGPH_IRQn);
    while(1);
}
/*************************** End of file ****************************/
//**************************  RegBased  ******************************
#include "NuMicro.h"
#include <stdio.h>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

#define SET_BIT(REG, BIT) ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT) ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT) ((REG) & (BIT))
#define CLEAR_REG(REG) ((REG) = (0x0))
#define WRITE_REG(REG, VAL) ((REG) = (VAL))
#define READ_REG(REG) ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK) WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

void GPABGH_IRQHandler(void) {
  volatile u32 u32temp_1;
  //if (GPIO_GET_INT_FLAG(PB, BIT2)) {
   if (READ_BIT(PB->INTSRC, BIT2)) {
    //GPIO_TOGGLE(PA12);
    PA12 ^= 1;
    //GPIO_CLR_INT_FLAG(PB, BIT2);
    WRITE_REG(PB->INTSRC, BIT2);
 // } else if (GPIO_GET_INT_FLAG(PB, BIT1)) {
  } else if (READ_BIT(PB->INTSRC, BIT1))  {
    //GPIO_TOGGLE(PA0);
    PA0 ^= 1;
    //GPIO_TOGGLE(PA1);
    PA1 ^= 1;
    //GPIO_TOGGLE(PA2);
    PA2 ^= 1;
    //GPIO_CLR_INT_FLAG(PB, BIT1);
    WRITE_REG(PB->INTSRC, BIT1);
  //} else if (GPIO_GET_INT_FLAG(PB, BIT0)) {
  } else if (READ_BIT(PB->INTSRC, BIT0))  {
    //GPIO_TOGGLE(PA13);
    PA13 ^= 1;
    //GPIO_TOGGLE(PC1);
    PC1 ^= 1;
    //GPIO_CLR_INT_FLAG(PB, BIT0);
    WRITE_REG(PB->INTSRC, BIT0);
  } else {
    // Un-expected interrupt. Just clear all PA interrupts
    u32temp_1 = PB->INTSRC;
    PB->INTSRC = u32temp_1;
    printf("Un-expected interrupts port_A.\n");
  }
}

void GPCDEF_IRQHandler(void) {
  volatile u32 u32temp_2;
  //if (GPIO_GET_INT_FLAG(PC, BIT0)) {
  if (READ_BIT(PC->INTSRC, BIT0)) {
    //GPIO_TOGGLE(PB3);
    PB3 ^= 1;
    // printf("XL6009_out)\n");
    /* Clear interrupt flag */
    //GPIO_CLR_INT_FLAG(PC, BIT0);
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

  SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA0MFP_Msk)) | (SYS_GPA_MFPL_PA0MFP_GPIO);   // PA.0
  SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA1MFP_Msk)) | (SYS_GPA_MFPL_PA1MFP_GPIO);   // PA.1
  SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA2MFP_Msk)) | (SYS_GPA_MFPL_PA2MFP_GPIO);   // PA.2
  SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB3MFP_Msk)) | (SYS_GPB_MFPL_PB3MFP_GPIO);   // PB.3
  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC1MFP_Msk)) | (SYS_GPC_MFPL_PC1MFP_GPIO);   // PC.1
  SYS->GPA_MFPH = (SYS->GPA_MFPH & ~(SYS_GPA_MFPH_PA12MFP_Msk)) | (SYS_GPA_MFPH_PA12MFP_GPIO); // PA.12
  SYS->GPA_MFPH = (SYS->GPA_MFPH & ~(SYS_GPA_MFPH_PA13MFP_Msk)) | (SYS_GPA_MFPH_PA13MFP_GPIO); // PA.13

  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC0MFP_Msk)) | (SYS_GPC_MFPL_PC0MFP_GPIO);   // PC.0
  SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB0MFP_Msk)) | (SYS_GPB_MFPL_PB0MFP_GPIO);   // PB.0
  SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB1MFP_Msk)) | (SYS_GPB_MFPL_PB1MFP_GPIO);   // PB.1
  SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB2MFP_Msk)) | (SYS_GPB_MFPL_PB2MFP_GPIO);   // PB.2

  SystemCoreClockUpdate();
  SYS_LockReg();
}

int main(void) {
  SYS_Init();
  // GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT);
  MODIFY_REG(PA->MODE, GPIO_MODE_MODE0_Msk, GPIO_MODE_OUTPUT << GPIO_MODE_MODE0_Pos);
  // GPIO_SetMode(PA, BIT1, GPIO_MODE_OUTPUT);
  MODIFY_REG(PA->MODE, GPIO_MODE_MODE1_Msk, GPIO_MODE_OUTPUT << GPIO_MODE_MODE1_Pos);
  // GPIO_SetMode(PA, BIT2, GPIO_MODE_OUTPUT);
  MODIFY_REG(PA->MODE, GPIO_MODE_MODE2_Msk, GPIO_MODE_OUTPUT << GPIO_MODE_MODE2_Pos);

  // PA0  = 0;
  CLEAR_BIT(PA->DOUT, GPIO_DOUT_DOUT0_Msk);
  // PA1  = 0;
  CLEAR_BIT(PA->DOUT, GPIO_DOUT_DOUT1_Msk);
  // PA2  = 0;
  CLEAR_BIT(PA->DOUT, GPIO_DOUT_DOUT2_Msk);

  // GPIO_SetMode(PB, BIT3,  GPIO_MODE_OUTPUT);
  MODIFY_REG(PB->MODE, GPIO_MODE_MODE3_Msk, GPIO_MODE_OUTPUT << GPIO_MODE_MODE3_Pos);
  // GPIO_SetMode(PA, BIT12, GPIO_MODE_OUTPUT);
  MODIFY_REG(PA->MODE, GPIO_MODE_MODE12_Msk, GPIO_MODE_OUTPUT << GPIO_MODE_MODE12_Pos);
  // GPIO_SetMode(PA, BIT13, GPIO_MODE_OUTPUT);
  MODIFY_REG(PA->MODE, GPIO_MODE_MODE13_Msk, GPIO_MODE_OUTPUT << GPIO_MODE_MODE13_Pos);
  // GPIO_SetMode(PC, BIT1,  GPIO_MODE_OUTPUT);
  MODIFY_REG(PC->MODE, GPIO_MODE_MODE1_Msk, GPIO_MODE_OUTPUT << GPIO_MODE_MODE1_Pos);

  // PB3  = 0;
  CLEAR_BIT(PB->DOUT, GPIO_DOUT_DOUT3_Msk);
  // PA12 = 0;
  CLEAR_BIT(PA->DOUT, GPIO_DOUT_DOUT12_Msk);
  // PA13 = 0;
  CLEAR_BIT(PA->DOUT, GPIO_DOUT_DOUT13_Msk);
  // PC1  = 0;
  CLEAR_BIT(PC->DOUT, GPIO_DOUT_DOUT1_Msk);

  // GPIO_SetMode(PC, BIT0, GPIO_MODE_INPUT);
  MODIFY_REG(PC->MODE, GPIO_MODE_MODE0_Msk, GPIO_MODE_INPUT << GPIO_MODE_MODE0_Pos);
  // GPIO_SetMode(PB, BIT0, GPIO_MODE_INPUT);
  MODIFY_REG(PB->MODE, GPIO_MODE_MODE0_Msk, GPIO_MODE_INPUT << GPIO_MODE_MODE0_Pos);
  // GPIO_SetMode(PB, BIT1, GPIO_MODE_INPUT);
  MODIFY_REG(PB->MODE, GPIO_MODE_MODE1_Msk, GPIO_MODE_INPUT << GPIO_MODE_MODE1_Pos);
  // GPIO_SetMode(PB, BIT2, GPIO_MODE_INPUT);
  MODIFY_REG(PB->MODE, GPIO_MODE_MODE2_Msk, GPIO_MODE_INPUT << GPIO_MODE_MODE2_Pos);

  // GPIO_EnableInt(PC, 0, GPIO_INT_FALLING);
  CLEAR_BIT(PC->INTTYPE, BIT0);                                   // set PC0 to edge trigger interrupt
  //PC->INTEN = (PC->INTEN & ~(BIT16 << GPIO_INTEN_RHIEN0_Pos)) | // disable rising  edge trigger interrupt
  //            (BIT0);                                           // enable  falling edge trigger interrupt
  MODIFY_REG(PC->INTEN, BIT16 << GPIO_INTEN_RHIEN0_Pos, BIT0);

  //GPIO_EnableInt(PB, 0, GPIO_INT_FALLING);
  CLEAR_BIT(PB->INTTYPE, BIT0); 
  MODIFY_REG(PB->INTEN, BIT16 << GPIO_INTEN_RHIEN0_Pos, BIT0);

  //GPIO_EnableInt(PB, 1, GPIO_INT_FALLING);
  CLEAR_BIT(PB->INTTYPE, BIT1); 
  MODIFY_REG(PB->INTEN, BIT17 << GPIO_INTEN_RHIEN0_Pos, BIT1);

  //GPIO_EnableInt(PB, 2, GPIO_INT_FALLING);
  CLEAR_BIT(PB->INTTYPE, BIT2); 
  MODIFY_REG(PB->INTEN, BIT18 << GPIO_INTEN_RHIEN0_Pos, BIT2);

  //GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_16);
  GPIO->DBCTL = GPIO_DBCTL_ICLKON_Msk | GPIO_DBCTL_DBCLKSRC_LIRC | GPIO_DBCTL_DBCLKSEL_16;

  //GPIO_ENABLE_DEBOUNCE(PC, BIT0);
  //PC->DBEN |= BIT0;
  SET_BIT(PC->DBEN, BIT0);

  //GPIO_ENABLE_DEBOUNCE(PB, BIT0);
  SET_BIT(PB->DBEN, BIT0);

  //GPIO_ENABLE_DEBOUNCE(PB, BIT1);
  SET_BIT(PB->DBEN, BIT1);

  //GPIO_ENABLE_DEBOUNCE(PB, BIT2);
  SET_BIT(PB->DBEN, BIT2);

  NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);
  NVIC_EnableIRQ(GPIO_PAPBPGPH_IRQn);
  while (1) {
  }
}
/*************************** End of file ****************************/
