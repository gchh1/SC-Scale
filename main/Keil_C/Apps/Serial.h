#ifndef __SERIAL_H_
#define __SERIAL_H_
#include "sc32_conf.h"

void Serial_ReadRxData(void);

FlagStatus Serial_GetRXFlag(void);

uint16_t Serial_GetRXData(void);
#endif
