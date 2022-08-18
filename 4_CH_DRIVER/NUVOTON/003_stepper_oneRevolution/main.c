/*
 * Engine's type       : Hybrid stepping motor
 * Model number        : 42BYGH721
 * Number of phases    : 3
 * Step angle (degrees): 1.8
 * 360 / 1.8 = 200 steps
 * Rated voltage       : 10V
 * https://www.alldatasheet.net/datasheet-pdf/pdf/1253732/PROMOCO/42BYGH721.html
 */
#include <stdio.h>
#include <stdlib.h>
#include "stdbool.h"
#include <math.h>
#include "NuMicro.h"
#include "system_init.h"
//---------------------------------------------------------------//
const int stepsPerRevolution = 200;
 int number_of_steps;
int step_number;
unsigned long last_step_time;
unsigned long step_delay;
unsigned long now;
unsigned long last_step_time = 0;
//---------------------------------------------------------------//
void stepMotor(uint8_t thisStep);
void setSpeed(long whatSpeed);
void step(int steps_to_move);
void micros();

int main()
{
    SYS_Init_();
    UART0_Init();
    TIMER_Open(TIMER2, TIMER_TOGGLE_MODE, 1000000);
    TIMER_SELECT_TOUT_PIN(TIMER2, TIMER_TOUT_PIN_FROM_TX);
    GPIO_SetMode(PB, BIT3, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, (BIT0 | BIT1 | BIT2 | BIT3), GPIO_MODE_OUTPUT);
    TIMER_Start(TIMER2);
    setSpeed(500);//max1000 - min100
    while(1){
    step(200);
    CLK_SysTickDelay(174000);
    step(-200);
    CLK_SysTickDelay(174000);
  }
}

void stepMotor(uint8_t thisStep){
    switch (thisStep) {
      case 0:  //1000
      PA0=1;PA1=0;PA2=0;PA3=0;
      break;
      case 1:  //0100
      PA0=0;PA1=1;PA2=0;PA3=0;
      break;
      case 2:  //0010
      PA0=0;PA1=0;PA2=1;PA3=0;
      break;
      case 3:  //0001
      PA0=0;PA1=0;PA2=0;PA3=1;
      break;
    }
}
void setSpeed(long whatSpeed)
{
 number_of_steps=stepsPerRevolution;
 step_delay = 60L * 1000L * 1000L / number_of_steps / whatSpeed;
}


void step(int steps_to_move){
    int steps_left = abs(steps_to_move);  // how many steps to take
    bool direction;
    if (steps_to_move > 0) { direction = true; }
    if (steps_to_move < 0) { direction = false; }
    printf(" %u \n", direction);
    while (steps_left > 0)
    {
    micros();
    if (now - last_step_time >= step_delay)
    {
     // get the timeStamp of when you stepped:
     last_step_time = now;
     // increment or decrement the step number,
     // depending on direction:
     if (direction)
    {
      //step_number++;
      if (step_number == number_of_steps) {
      step_number = 0;
    }
      step_number++;
    }
     else
    {
     //step_number--;
     if (step_number == 0) {
     step_number = number_of_steps;
    }
    step_number--;
    }
     // decrement the steps left:
     steps_left--;
     // step the motor to step number 0, 1, ..., {3 or 10}
     stepMotor(step_number & 0b11);
     printf(" %d \n", step_number);
    }
}
}
void micros(){
  TIMER_Delay(TIMER0, 1000000);
  now ++;
}
