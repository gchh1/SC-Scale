/**
 *****************************************************************************************************
  * @copyright	(c)  Shenzhen Saiyuan Microelectronics Co., Ltd
  * @file	         main.c
  * @author	 
  * @version 	
  * @date	
  * @brief	         
  * @details         Contains the MCU initialization function and its H file
 *****************************************************************************************************
 * @attention
 *
 *****************************************************************************************************
 */
/*******************Includes************************************************************************/

#include "SC_Init.h"
#include "sc32f12xx.h"                  // Device header
#include "SC_it.h"
#include "..\Drivers\SCDriver_list.h"
#include "HeadFiles\SysFunVarDefine.h"
#include "math.h"
#include "sc32_conf.h"
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Key.h"
#include "stdio.h"
#include "Serial.h"
#include "HX711.h"
#include "Weight.h"
#include "Utils.h"
#include "Voice.h"


volatile FlagStatus UART5_Flag;
volatile uint16_t UART5_RxData;

/**
  * @brief This function implements main function.
  * @note 
  * @param
  */
int main(void)
{
    IcResourceInit();  
	  Key_Init();
    LEDX_Init();
	  _OLED_Init();
	  HX711_Init();
	  Weight_Init();

    while (1) {
		    Main_Loop();

    }
}
