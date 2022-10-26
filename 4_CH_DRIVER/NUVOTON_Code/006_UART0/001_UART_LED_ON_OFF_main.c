#include <stdio.h>
#include "NuMicro.h"
#include "stdbool.h"
#include "string.h"

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

#define RX_BUF_SIZE 80
bool  RX_FLAG_END_LINE = 0;
volatile u8 RXi;
volatile u8 RXc;
u8 RX_BUF[RX_BUF_SIZE] = { '\0' };

void UART_TEST_HANDLE();
void clear_RXBuffer();
void USARTSend(char *pucBuffer);

void UART02_IRQHandler(void){
  UART_TEST_HANDLE();
}

void SYS_Init(void) {
  SYS_UnlockReg();
  CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
  CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
  CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
  CLK_EnableModuleClock(UART0_MODULE);
  CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
  SystemCoreClockUpdate();
  /* Set PB multi-function pins for UART0 RXD=PA.15 and TXD=PA.14 */
  SYS->GPA_MFPH = (SYS->GPA_MFPH & ~(SYS_GPA_MFPH_PA14MFP_Msk | SYS_GPA_MFPH_PA15MFP_Msk)) | (SYS_GPA_MFPH_PA15MFP_UART0_RXD | SYS_GPA_MFPH_PA14MFP_UART0_TXD);
  SYS_LockReg();
}

int main() {
  SYS_Init();

  GPIO_SetMode(PA, BIT12 | BIT13, GPIO_MODE_OUTPUT);  //PA.12 -> LED 500kHz, PA.13 -> LED PWM
  PA13 = 0;
  PA12 = 0;

  SYS_ResetModule(UART0_RST);
  UART_Open(UART0, 115200);
  printf("Test Board\n");
  NVIC_EnableIRQ(UART02_IRQn);
  UART_EnableInt(UART0, (UART_INTEN_RDAIEN_Msk | UART_INTEN_THREIEN_Msk));
  USARTSend(" Hello.\r\nUSART0 is ready.\r\n");

  while (1) {
   if (RX_FLAG_END_LINE == 1) {
   // Reset END_LINE Flag
   RX_FLAG_END_LINE = 0;
   USARTSend("\r\nI has received a line:\r\n");
   USARTSend(RX_BUF);
   USARTSend("\r\n");
   if (strncmp(strupr(RX_BUF), "ON\r", 3) == 0) {
    USARTSend("\r\nThis is command \"ON\"!\r\n");
    PA13 = true;
    PA12 = true;
   }
   if (strncmp(strupr(RX_BUF), "OFF\r", 4) == 0) {
    USARTSend("\r\nThis is command \"OFF\"!\r\n");
    PA13 = false;
    PA12 = false;
   }
   clear_RXBuffer();
   }
  }
}

void UART_TEST_HANDLE() {
 if (UART_GET_INT_FLAG(UART0, UART_INTSTS_RDAINT_Msk)) {
  while (UART_IS_RX_READY(UART0)) {
  /* Get the character from UART Buffer */
  RXc = UART_READ(UART0);
  RX_BUF[RXi++] = RXc;
   if (RXc != 13) {
    if (RXi > RX_BUF_SIZE - 1) {
      clear_RXBuffer();
      printf("\nClear_buffer");
   }
   } else {
     RX_FLAG_END_LINE = 1;
   }
   }
    //Echo!!!
  UART_WRITE(UART0, RXc);
  }
  if (UART0->FIFOSTS & (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk)) {
    UART_ClearIntFlag(UART0, (UART_INTSTS_RLSINT_Msk | UART_INTSTS_BUFERRINT_Msk));
  }
}

void clear_RXBuffer() {
 for (RXi = 0; RXi < RX_BUF_SIZE; RXi++) {
  RX_BUF[RXi] = 0u;
 }
 RXi = 0;
}

void USARTSend(char *pucBuffer) {
 while (*pucBuffer) {
  while (UART_IS_TX_FULL(UART0));
  UART_WRITE(UART0, *pucBuffer++);
 }
}
