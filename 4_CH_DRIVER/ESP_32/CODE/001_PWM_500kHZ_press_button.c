/***************************************************************
 *                GND<--|----------|-->GND
 *               3.3V<--|----------|-->GPIO23 PWM_OUT
 *             ChipPU<--|----------|-->GPIO22 Button ON OFF
 *             GPIO36<--|----------|-->GPIO1
 *             GPIO39<--|----------|-->GPIO3
 *             GPIO34<--|----------|-->GPIO21
 *             GPIO35<--|----------|-->NC
 *             GPIO32<--|----------|-->GPIO19
 *             GPIO33<--|----------|-->GPIO18
 *             GPIO25<--|----------|-->GPIO5
 *             GPIO26<--|----------|-->GPIO17
 *             GPIO27<--|----------|-->GPIO16
 *             GPIO14<--|----------|-->GPIO4
 *             GPIO12<--|----------|-->0GPIO
 *   
 * Далее Pinout слева направо...
 * GND
 * GPIO13
 * GPIO9
 * GPIO10
 * GPIO11
 * GPIO6
 * GPIO7
 * GPIO8
 * GPIO15
 * GPIO2 Led pin
***************************************************************/
#define PWM_GPIO   23          //ESP32-WROOM
#define PWM1_Ch    1
#define PWM1_Res   4
#define PWM1_Freq  500000

#define pushButton_pin   22    //ESP32-WROOM
#define LED_pin   2            //ESP32-WROOM
 
int PWM1_DutyCycle = 8;
bool pressed = true;
bool condition = true;

void IRAM_ATTR ISR_btn1() {
  condition = false;
  pressed = !pressed;
}

/*void IRAM_ATTR ISR_btn2() {
   // Statements;
}****************************/
 
void setup()
{
  ledcSetup(PWM1_Ch, PWM1_Freq, PWM1_Res);
  pinMode(pushButton_pin, INPUT);
  pinMode(LED_pin, OUTPUT);
  attachInterrupt(pushButton_pin, ISR_btn1, RISING); 
} 
void loop()
{
switch (pressed){      
 case 0:
  ledcAttachPin(PWM_GPIO, PWM1_Ch);  
  ledcWrite(PWM1_Ch, PWM1_DutyCycle);
  condition = true;	
 break;
 case 1:
	ledcDetachPin(PWM_GPIO);
  condition = true;
 break;
  }
  while(condition){
  digitalWrite(LED_pin, !digitalRead(LED_pin));
  //delay(1000);
   vTaskDelay(1000);
  }
}
