
M031EC1AE --> 32 KB Flash,4 KB SRAM.  
  
|                  | button |          led          |  pin(управляет каким выводом + режим)     |
|:----------------:|:------:|:---------------------:|:-----------------------------------------:|
|PWM_500kHz        | PC.0   | PA.12 (Led 500kHz)    | Timer2 (TIMER_TOGGLE_MODE) -->   PWM PB.3 |
|PWM_start_stop    | PA.0   | PA.13 (Led PWM)       | AH->PB.5  BH->PA.3  AL->PB.4  BL->PA.2    |  
|Reset_error_button| PA.1   | PB.13->Err1 PB.12->Er |   in PF.2->Error1   in PF.3->Rrror2       |
|UART0             |        |                       |  RXD=PA.15     TXD=PA.14                  |
  