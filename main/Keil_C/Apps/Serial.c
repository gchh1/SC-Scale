#include "Serial.h"

extern volatile FlagStatus UART5_Flag;
extern volatile uint16_t UART5_RxData;


FlagStatus Serial_GetRXFlag(void) {
	if (UART5_Flag == SET) {
		UART5_Flag = RESET;
		return SET;
	}
	return RESET;
}

uint16_t Serial_GetRXData(void) {
	return UART5_RxData;
}
