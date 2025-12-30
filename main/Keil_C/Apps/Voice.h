#ifndef __VOICE_C_
#define __VOICE_C_
#include "sc32_conf.h"

#define WEIGHT_ANNOUNCE     9

void Voice_SendWA(float weight, float tare, uint8_t Unit);
void Voice_SetWeight(float weight);
void Voice_SendOV(void);
uint8_t Voice_ReceiveCMD(void);

#endif
