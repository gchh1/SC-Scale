#ifndef __LED_H_
#define __LED_H_

#include "sc32_conf.h"

void LED_SetOneDigit(uint8_t position, uint8_t number);
void LED_SetDot(uint8_t position);
void LED_SetNum(uint16_t num);
void LED_Clear(void);
void LEDX_Init(void);
void LEDX_On(uint8_t LEDX);
void LEDX_Off(uint8_t LEDX);
void LEDX_Shift(uint8_t LEDX);
BitAction LEDX_State(uint8_t LEDX);
void LEDX_Shine(uint8_t LEDX);

#endif
