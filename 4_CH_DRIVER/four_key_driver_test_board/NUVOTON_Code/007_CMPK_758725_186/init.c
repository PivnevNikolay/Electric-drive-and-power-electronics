#include <stdio.h>
#include "NuMicro.h"

void SYS_Init(void)
{
    SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);//Enable HIRC clock (Internal RC 48MHz)
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);//Wait for HIRC clock ready
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
    CLK_EnableModuleClock(UART0_MODULE);//Enable UART0 clock
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));//Switch UART0 clock source to HIRC
    CLK_EnableModuleClock(TMR2_MODULE);//Enable TMR2 module clock
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_HIRC,  0);
    CLK_EnableModuleClock(PWM0_MODULE);//Enable PWM0 module clock
    CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL2_PWM0SEL_PCLK0,(uint32_t) NULL);
    SYS_ResetModule(PWM0_RST);//reset PWM module
    SystemCoreClockUpdate();//Update System Core Clock
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
    //Set PA.12(Led 500 kHz)  PA.13(Led PWM Start_Stop) to GPIO
    SYS->GPA_MFPH = (SYS->GPA_MFPH & ~(SYS_GPA_MFPH_PA12MFP_Msk | SYS_GPA_MFPH_PA13MFP_Msk ))|(SYS_GPA_MFPH_PA12MFP_GPIO  | SYS_GPA_MFPH_PA13MFP_GPIO);
    //Set PB.12(Led Error 2)  PB.13(Led Error 1) to GPIO
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk ))|(SYS_GPB_MFPH_PB12MFP_GPIO  | SYS_GPB_MFPH_PB13MFP_GPIO);
    //Set PF.2(in Error 1)  PF.3(in Error 2) to GPIO
    SYS->GPF_MFPL = (SYS->GPF_MFPL & ~(SYS_GPF_MFPL_PF2MFP_Msk | SYS_GPF_MFPL_PF3MFP_Msk))|(SYS_GPF_MFPL_PF2MFP_GPIO | SYS_GPF_MFPL_PF3MFP_GPIO);
    //Set PC.0(in button 500 kHz)
    SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC0MFP_Msk))|(SYS_GPC_MFPL_PC0MFP_GPIO);
    //Set PA.0(in button PWM Start Stop) PA.1 (in Reset Error button)
    SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA0MFP_Msk | SYS_GPA_MFPL_PA1MFP_Msk))|(SYS_GPA_MFPL_PA0MFP_GPIO | SYS_GPA_MFPL_PA1MFP_GPIO);
    SYS_LockReg();
}

void GPIO_Init(void){
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
    GPIO_SetMode(PC, BIT1, GPIO_MODE_OUTPUT);
    PC1=0;
    GPIO_SetMode(PA, BIT1 | BIT0, GPIO_MODE_INPUT); //PA.0  -> PWM button, PA.1 -> Reset Error1 and Error2 button
    GPIO_SetMode(PC, BIT0, GPIO_MODE_INPUT);        //PC.0  -> 500kHz button
    GPIO_EnableInt(PA, 1, GPIO_INT_FALLING);
    GPIO_EnableInt(PC, 0, GPIO_INT_FALLING);
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC,  GPIO_DBCTL_DBCLKSEL_8);
    GPIO_ENABLE_DEBOUNCE(PC, BIT0);//PC.0
    GPIO_ENABLE_DEBOUNCE(PA, BIT0);//PA.0
    NVIC_EnableIRQ(GPIO_PAPBPGPH_IRQn);
    NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);
}

void Timer_Init(void){
    TIMER_Open(TIMER2, TIMER_TOGGLE_MODE, 1000000);
    TIMER_SELECT_TOUT_PIN(TIMER2, TIMER_TOUT_PIN_FROM_TX);
}

void PWM_Init(void){
	PWM_ConfigOutputChannel(PWM0, 0, 504, 50);//500 kHz
    PWM_ConfigOutputChannel(PWM0, 2, 504, 50);//500 kHz
    SYS_UnlockReg();
    PWM_EnableDeadZone(PWM0, 0, 30);
    PWM_EnableDeadZone(PWM0, 1, 30);
    PWM_EnableDeadZone(PWM0, 2, 30);
    PWM_EnableDeadZone(PWM0, 3, 30);
    SYS_LockReg();
    PWM_Start(PWM0, 0xF);
}
