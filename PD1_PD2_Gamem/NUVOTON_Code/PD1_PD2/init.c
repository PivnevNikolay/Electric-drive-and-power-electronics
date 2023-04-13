#include "NuMicro.h"
#include <stdio.h>

void SYS_Init(void) {
  SYS_UnlockReg();
  //****************  Enable HIRC clock (Internal RC 48MHz)  ***************************//
  CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk); // Enable HIRC clock (Internal RC 48MHz)
  CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
  CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
  //************************  TMR0_MODULE  *********************************************//
  CLK_EnableModuleClock(TMR0_MODULE);
  CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, 0);
  //***********************  UART0_MODULE  *********************************************//
  CLK_EnableModuleClock(UART0_MODULE);
  CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
  //***********************  PWM0_MODULE  **********************************************//
  CLK_EnableModuleClock(PWM0_MODULE);
  CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL2_PWM0SEL_PCLK0, (uint32_t)NULL);
  SYS_ResetModule(PWM0_RST);
  //************************************************************************************//
  SystemCoreClockUpdate();
  //******* PWM PD2-01 -> PB.5(PWM0_CH0) ********* PWM PD2-02 -> PB.4 (PWM0_CH1) *******//
  SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB5MFP_Msk | SYS_GPB_MFPL_PB4MFP_Msk)) |
                  (SYS_GPB_MFPL_PB5MFP_PWM0_CH0 | SYS_GPB_MFPL_PB4MFP_PWM0_CH1);
  //**********************  PWM PD1 -> PA.3 (PWM0_CH2)  ********************************//
  SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA3MFP_Msk)) | (SYS_GPA_MFPL_PA3MFP_PWM0_CH2);
  //***********************  UART0 RXD=PA.15 and TXD=PA.14 *****************************//
  SYS->GPA_MFPH = (SYS->GPA_MFPH & ~(SYS_GPA_MFPH_PA14MFP_Msk | SYS_GPA_MFPH_PA15MFP_Msk)) |
                  (SYS_GPA_MFPH_PA15MFP_UART0_RXD | SYS_GPA_MFPH_PA14MFP_UART0_TXD);
  //*********************  Button  PB.0  PB.1  PB.2  PC.0  *****************************//
  SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB0MFP_Msk)) | (SYS_GPB_MFPL_PB0MFP_GPIO); // PB.0
  SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB1MFP_Msk)) | (SYS_GPB_MFPL_PB1MFP_GPIO); // PB.1
  SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB2MFP_Msk)) | (SYS_GPB_MFPL_PB2MFP_GPIO); // PB.2
  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC0MFP_Msk)) | (SYS_GPC_MFPL_PC0MFP_GPIO); // PC.0
  //*******************  LED_Error  PA.0  PA.1  PA.2  **********************************//
  SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA0MFP_Msk)) | (SYS_GPA_MFPL_PA0MFP_GPIO); // PA.0
  SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA1MFP_Msk)) | (SYS_GPA_MFPL_PA1MFP_GPIO); // PA.1
  SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA2MFP_Msk)) | (SYS_GPA_MFPL_PA2MFP_GPIO); // PA.2
  //*******************  LED_PWM  PA.12  PA.13  PC.1  **********************************//
  SYS->GPA_MFPH = (SYS->GPA_MFPH & ~(SYS_GPA_MFPH_PA12MFP_Msk)) | (SYS_GPA_MFPH_PA12MFP_GPIO); // PA.12
  SYS->GPA_MFPH = (SYS->GPA_MFPH & ~(SYS_GPA_MFPH_PA13MFP_Msk)) | (SYS_GPA_MFPH_PA13MFP_GPIO); // PA.13
  SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC1MFP_Msk)) | (SYS_GPC_MFPL_PC1MFP_GPIO);   // PC.1
  //**********************  EN_XL6009_OUT  PB.3  ***************************************//
  SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB3MFP_Msk)) | (SYS_GPB_MFPL_PB3MFP_GPIO); // PB.3
  //********************  ERROR_PD1_Input_pin  PB.14  **********************************//
  SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB14MFP_Msk)) | (SYS_GPB_MFPH_PB14MFP_GPIO); // PB.14
  //******************  ERROR_PD2_Input_pin  PB.12  PB.13  *****************************//
  SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk)) | (SYS_GPB_MFPH_PB12MFP_GPIO); // PB.12
  SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB13MFP_Msk)) | (SYS_GPB_MFPH_PB13MFP_GPIO); // PB.13
  //************************************************************************************//
  SYS_LockReg();
}

void LED_GPIO_init(void) {
  GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT);
  PA0 = 0; // ERR_PD1_LED
  GPIO_SetMode(PA, BIT1, GPIO_MODE_OUTPUT);
  PA1 = 0; // ERR_PD2_01_LED
  GPIO_SetMode(PA, BIT2, GPIO_MODE_OUTPUT);
  PA2 = 0; // ERR_PD2_02_LED
  GPIO_SetMode(PA, BIT12, GPIO_MODE_OUTPUT);
  PA12 = 0; // Led_PWM_PD1
  GPIO_SetMode(PA, BIT13, GPIO_MODE_OUTPUT);
  PA13 = 0; // Led_PWM_PD2_01
  GPIO_SetMode(PC, BIT1, GPIO_MODE_OUTPUT);
  PC1 = 0; // Led_PWM_PD2_02
}

void XL6009_OUTpin_init(void) {
  GPIO_SetMode(PB, BIT3, GPIO_MODE_OUTPUT);
  PB3 = 0; // EN_XL6009_OUT
}

void Button_ERRinputPIN_init(void) {
  // EN_XL6009_IN_button
  GPIO_SetMode(PC, BIT0, GPIO_MODE_INPUT);
  GPIO_EnableInt(PC, 0, GPIO_INT_FALLING);
  // PWM_PD1_button
  GPIO_SetMode(PB, BIT2, GPIO_MODE_INPUT);
  // PWM_PD2_button
  GPIO_SetMode(PB, BIT1, GPIO_MODE_INPUT);
  // PWM_ENchannel_button
  GPIO_SetMode(PB, BIT0, GPIO_MODE_INPUT);
  // ERROR_Input_pin
  GPIO_SetMode(PB, BIT12, GPIO_MODE_INPUT);
  GPIO_SetMode(PB, BIT13, GPIO_MODE_INPUT);
  GPIO_SetMode(PB, BIT14, GPIO_MODE_INPUT);

  GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_16);
  GPIO_ENABLE_DEBOUNCE(PC, BIT0);
  GPIO_ENABLE_DEBOUNCE(PB, BIT0);
  GPIO_ENABLE_DEBOUNCE(PB, BIT1);
  GPIO_ENABLE_DEBOUNCE(PB, BIT2);

  NVIC_EnableIRQ(GPIO_PAPBPGPH_IRQn);
  NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);
}

void PWM_Init_PD1(void) {
  // PWM0_CH2 --> 500 kHz output PA.3
  PWM_ConfigOutputChannel(PWM0, 2, 504, 50);
  PWM_Start(PWM0, 0xF);
}

void PWM_Init_PD2_01_02(void) {
  // PWM0_CH0 output PB.5  --> 500 kHz (PD2_01)
  // PWM0_CH1 output PB.4  --> 500 kHz (COMPLEMENTARY_MODE) (PD2_02)
  PWM_ConfigOutputChannel(PWM0, 0, 504, 50);
  PWM_Start(PWM0, 0xF);
  SYS_UnlockReg();
  PWM_EnableDeadZone(PWM0, 0, 30);
  PWM_EnableDeadZone(PWM0, 1, 30);
  SYS_LockReg();
}

void PWM_Init_PD2_01(void) {
PWM_ConfigOutputChannel(PWM0, 0, 504, 50);
PWM_Start(PWM0, 0xF);
}

void PWM_Init_PD2_02(void) {
PWM_ConfigOutputChannel(PWM0, 1, 504, 50);
PWM_Start(PWM0, 0xF);
}