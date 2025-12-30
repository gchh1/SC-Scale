#include "LED_Example.h"

void LED_Example(void)
{
  uint16_t i, j;

  while(1)
  {
    for(i = 0; i < 1000; i++)
    {
      for(j = 0; j < 2000; j++);
    }

    LED_Write(LED_RAMRegister_8, 0xF0);
	LED_Write(LED_RAMRegister_9, 0xF0);
    LED_Write(LED_RAMRegister_10, 0xF0);
    LED_Write(LED_RAMRegister_11, 0xF0);
    LED_Write(LED_RAMRegister_12, 0xF0);
	LED_Write(LED_RAMRegister_13, 0xF0);
    LED_Write(LED_RAMRegister_14, 0xF0);
    LED_Write(LED_RAMRegister_15, 0xF0);


    for(i = 0; i < 1000; i++)
    {
      for(j = 0; j < 2000; j++);
    }


    LED_Write(LED_RAMRegister_8, 0x00);
	LED_Write(LED_RAMRegister_9, 0x00);
    LED_Write(LED_RAMRegister_10, 0x00);
    LED_Write(LED_RAMRegister_11, 0x00);
    LED_Write(LED_RAMRegister_12, 0x00);
	LED_Write(LED_RAMRegister_13, 0x00);
    LED_Write(LED_RAMRegister_14, 0x00);
    LED_Write(LED_RAMRegister_15, 0x00);

  }
}


