#define PWM_GPIO   23
#define pushButton_pin   22
#define LED_pin   2
#define PWM1_Ch    1
#define PWM1_Res   4
#define PWM1_Freq  500000
 
int PWM1_DutyCycle = 8;
bool pressed = true;
bool condition = true;
/**********************************/
void IRAM_ATTR ISR_btn1() {
  condition = false;
  pressed = !pressed;
}
/**********************************/
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
  delay(1000);
  }
}
