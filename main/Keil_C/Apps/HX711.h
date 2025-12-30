#ifndef __HX711_H_
#define __HX711_H_

#include "sc32_conf.h"
#include "sc32f1xxx_gpio.h"


// Predefine the GPIO
#define HX711_DATA_GPIO_PORT		GPIOC
#define HX711_DATA_GPIO_PIN			GPIO_Pin_3
#define HX711_SCK_GPIO_PORT			GPIOC
#define HX711_SCK_GPIO_PIN			GPIO_Pin_4


#define HX711_DATA_WRITE(x)               GPIO_WriteBit(HX711_DATA_GPIO_PORT, HX711_DATA_GPIO_PIN, (BitAction)(x))
#define HX711_SCK_WRITE(x)                GPIO_WriteBit(HX711_SCK_GPIO_PORT, HX711_SCK_GPIO_PIN, (BitAction)(x))
#define HX711_DATA_READ                	  GPIO_ReadDataBit(HX711_DATA_GPIO_PORT, HX711_DATA_GPIO_PIN)
#define HX711_SCK_READ                    GPIO_ReadDataBit(HX711_SCK_GPIO_PORT, HX711_SCK_GPIO_PIN)

void HX711_Init(void);
int32_t HX711_Read(void);

#endif 
