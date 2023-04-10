#ifndef _Init_H
#define _Init_H

typedef uint32_t  u32;
typedef uint16_t  u16;
typedef uint8_t   u8;

void SYS_Init(void);
void LED_GPIO_init(void);
void XL6009_OUTpin_init(void);
void Button_ERRinputPIN_init(void);

#endif