#include "sc32_conf.h"
#include "HX711.h"
#include "Weight.h"
#include "OLED.h"
#include "LED.h"
#include "Key.h"
#include "Utils.h"
#include "Delay.h"
#include "Voice.h"
#include <math.h>
#include <stdbool.h>

SystemMode_t Cur_SysMode = SYS_OFF;
SystemMode_t Prev_SysMode;
static Scale_Attribute myScale;
static Scale_Attribute *Scale;
extern volatile uint8_t OV_Flag;
volatile uint8_t WA_Flag;
volatile uint8_t WF_Flag;

volatile uint16_t TimeTick_Voice = 0;
volatile uint16_t TimeTick_Weight = 0;
volatile uint16_t TimeTick_Move = 0;

/**
 * @brief Convert the weight in the Scale base on the unit in the Scale 
 * @param  
 */
void Scale_ConvertUnit(void) {
    uint16_t alpha1;
    uint16_t alpha2;
    switch (Scale->weight_unit) {
        case UNIT_G: alpha1 = 1; break;
        case UNIT_KG: alpha1 = 1000; break;
        case UNIT_JIN: alpha1 = 500; break;
    }
    switch (Scale->tare_unit) {
        case UNIT_G: alpha2 = 1; break;
        case UNIT_KG: alpha2 = 1000; break;
        case UNIT_JIN: alpha2 = 500; break;
    }

    Scale->weight = Scale->weight / (float)alpha1; 
    Scale->tare_weight = Scale->tare_weight / (float)alpha2; 

}

/**
 * @brief Update the unit in the OLED
 * @param unit_idx 
 * @return [weight_int, weight_dec, tare_int, tare_dec]
 */
void UI_ShowWeight(Scale_Attribute *scale) {
    uint16_t alpha1;
    uint16_t alpha2;
    switch (Scale->weight_unit) {
        case UNIT_G: alpha1 = 1; break;
        case UNIT_KG: alpha1 = 1000; break;
        case UNIT_JIN: alpha1 = 500; break;
    }
    switch (Scale->tare_unit) {
        case UNIT_G: alpha2 = 1; break;
        case UNIT_KG: alpha2 = 1000; break;
        case UNIT_JIN: alpha2 = 500; break;
    }


    
    // 右对齐显示数值（字段宽度6），例如“  113.3”或“1989.3”
    // ASCII列从7开始，长度6（共48像素宽）
    OLED_ShowFloat(1, 7, scale->tare_weight / (float)alpha1, 6, scale->tare_dec);
    OLED_ShowFloat(2, 7, scale->weight / (float)alpha2, 6, scale->weight_dec);

    // 显示单位（“克”、“千克”或“斤”），调用OLED_ShowChinese2会自动清除旧字符

}

/**
 * @brief Switch the OLED_Display Mode 
 * @param Display_Mode 
 */
void UI_Display(SystemMode_t Display_Mode) {
    switch (Display_Mode) {
        case SYS_OFF: {
            if (Prev_SysMode != SYS_OFF) {
            _OLED_Clear();
            }
            Prev_SysMode = SYS_OFF;
            break;
        }
        case SYS_INIT: {
            _OLED_Clear();
            OLED_ShowChinese(2, 2, "正在初始化");
            Prev_SysMode = SYS_INIT;
            break;
        }
        case SYS_NORMAL: {
            if (Prev_SysMode != SYS_NORMAL) {
                _OLED_Clear();
                OLED_ShowChinese(1,1,"皮重：");
                OLED_ShowChinese(2,1,"重量：");
            }
            if (Scale->tare_unit == UNIT_G) {           // 克
                OLED_ShowChinese2(1, 7, 2, "克");
            } else if (Scale->tare_unit == UNIT_KG) {    // 千克
                OLED_ShowChinese2(1, 7, 2, "千克");
            } else if (Scale->tare_unit == UNIT_JIN) {    // 斤
                OLED_ShowChinese2(1, 7, 2, "斤");
            }
            if (Scale->weight_unit == UNIT_G) {         // 克
                OLED_ShowChinese2(2, 7, 2, "克");
            } else if (Scale->weight_unit == UNIT_KG) {  // 千克
                OLED_ShowChinese2(2, 7, 2, "千克");
            } else if (Scale->weight_unit == UNIT_JIN) {  // 斤
                OLED_ShowChinese2(2, 7, 2, "斤");
            }
            
            if (WF_Flag) {
                WF_Flag = 0;
                UI_ShowWeight(Scale);
                // printf("%f\n", Scale->weight);
            }

            Prev_SysMode = SYS_NORMAL;
            // TODO 按键功能
            break;
        }
        case SYS_CALIBRATION: {
            if (!Scale->is_calibrated) {
                if (Prev_SysMode != SYS_CALIBRATION){
                    _OLED_Clear();
                    OLED_ShowChinese(1, 3, "校准模式");
                    OLED_ShowChinese2(2, 1, 2, "重量：");
                    OLED_ShowChinese2(2, 7, 2, "克");
                    OLED_ShowChinese2(3, 4, 2, "步长");
                }
            } else {
                _OLED_Clear();
                OLED_ShowChinese(2, 2, "校准完成");
                Delay_ms(500);
                LEDX_Off(3);
                LEDX_Off(4);
                LEDX_Off(5);
                Cur_SysMode = SYS_NORMAL;
                Scale->is_calibrated = 0;
            }
            OLED_ShowInt(2, 7, Scale->calibrate_weight, 6);
            OLED_ShowInt(4, 8, Scale->calibrate_foot, 3);
            Prev_SysMode = SYS_CALIBRATION;
            // TODO 按键功能
            break;
        }
    }
}

/**
 * @brief Switch the function of the weight when kick different key or voice command
 * @param fun function flag
 */
void Switch_Function(uint8_t fun) {
    WeightUnit_t weight_unit;
    switch (fun) {
        // POWER ON
        case FUN_POWER_ON: {
            Cur_SysMode = SYS_INIT;
            LEDX_On(2);
            break;
        }

        // POWER OFF
        case FUN_POWER_OFF: {
            Cur_SysMode = SYS_OFF;
            LEDX_Off(2);
            LEDX_Off(3);
            LEDX_Off(4);
            LEDX_Off(5);
            break;
        } 

        // TARE
        case FUN_TARE: { 
            LEDX_On(3);
            Weight_Tare();
            Scale->is_tared = 1;
            break;
        }

        // TARE_C
        case FUN_TAREC: {
            LEDX_Off(3);
            Weight_TareC();
            Scale->is_tared = 0;
            break;
        }

        // ZERO
        case FUN_ZERO: {
            LEDX_Shine(4);
            Weight_SetZero();
            break;
        }

        // Switch Unit
        case FUN_UNIT: {
            LEDX_Shine(5);
            switch (Scale->weight_unit) {
                case UNIT_G: 
                    Scale->weight_unit = UNIT_KG;
                    Scale->tare_unit = UNIT_KG;
                    break;
                case UNIT_KG: 
                    Scale->weight_unit = UNIT_JIN; 
                    Scale->tare_unit = UNIT_JIN;
                    break;
                case UNIT_JIN: 
                    Scale->weight_unit = UNIT_G; 
                    Scale->tare_unit = UNIT_G;
                    break;
            }
            break;
        }

        // Switch Unit G
        case FUN_UNIT_G: {
            LEDX_Shine(5);
            Scale->weight_unit = UNIT_G;
            Scale->tare_unit = UNIT_G;
            break;
        }

        // Switch Unit KG
        case FUN_UNIT_KG: {
            LEDX_Shine(5);
            Scale->weight_unit = UNIT_KG; 
            Scale->tare_unit = UNIT_KG;
            break;
        }

        // Switch Unit Jin
        case FUN_UNIT_JIN: {
            LEDX_Shine(5);
            Scale->weight_unit = UNIT_JIN; 
            Scale->tare_unit = UNIT_JIN;
            break;
        }

        // Calibration
        case FUN_CALIBRATE: {
            LEDX_On(3);
            LEDX_On(4);
            LEDX_On(5);
            break;
        }
    }
}


/**
 * @brief Initialize the Scale_Attribute 
 * @param  
 */
void Scale_Init(void) {
    myScale.weight = 0;
    myScale.tare_weight = 0;
    myScale.weight_dec = 1;
    myScale.tare_dec = 1;
    myScale.weight_unit = UNIT_G;
    myScale.tare_unit = UNIT_G;
    myScale.is_overweight = 0;
    myScale.calibrate_weight = 0;
    myScale.is_calibrated = 0;
    myScale.calibrate_foot = 1;
    Scale = &myScale; 
}

/**
 * @brief OFF mode cmd 
 * @param  
 */
void System_OFF(void) {
    uint8_t voice_cmd = Voice_ReceiveCMD();
    // key cmd and voice cmd
    if (Key_Check(1, KEY_SINGLE) || voice_cmd == FUN_POWER_ON) {
        Switch_Function(FUN_POWER_ON);
        Cur_SysMode = SYS_INIT;
    }

}

/**
 * @brief Initialize the program 
 * @param  
 */
void System_Init(void) {
    // Init the using hardware
    HX711_Init();

    // Carry out the weight init program
    Weight_Init();

    // Initialize the Scale_Attribute
    Scale_Init();

    Cur_SysMode = SYS_NORMAL;
    TimeTick_Move = 0;
    TimeTick_Voice = 0;
    TimeTick_Weight = 0;
}

/**
 * @brief Normal mode cmd 
 * @param  
 */
void System_Normal(void) {
    uint8_t voice_cmd = Voice_ReceiveCMD();
    // key and voice cmd
    if (Key_Check(1, KEY_SINGLE) || voice_cmd == FUN_POWER_OFF) {
        Switch_Function(FUN_POWER_OFF);
        TimeTick_Move = 0;
    }
    if (Key_Check(2, KEY_SINGLE) || voice_cmd == FUN_TARE) {
        if (!Scale->is_tared) {
            Switch_Function(FUN_TARE);
        } else {
            Switch_Function(FUN_TAREC);
        }
        TimeTick_Move = 0;
    } 
    if (voice_cmd == FUN_TAREC) {
        Switch_Function(FUN_TAREC);
        TimeTick_Move = 0;
    }
    if (Key_Check(3, KEY_SINGLE) || voice_cmd == FUN_ZERO) {
        Switch_Function(FUN_ZERO);
        TimeTick_Move = 0;
    }
    if (Key_Check(4, KEY_SINGLE)) {
        Switch_Function(FUN_UNIT);
        TimeTick_Move = 0;
    }
    if (Key_Check(2, KEY_LONG)) {
        Switch_Function(FUN_CALIBRATE);
        Cur_SysMode = SYS_CALIBRATION; 
        TimeTick_Move = 0;
    }
    if (voice_cmd == FUN_UNIT_G) {
        Switch_Function(FUN_UNIT_G);
        TimeTick_Move = 0;
    } 
    if (voice_cmd == FUN_UNIT_KG) {
        Switch_Function(FUN_UNIT_KG);
        TimeTick_Move = 0;
    }
    if (voice_cmd == FUN_UNIT_JIN) {
        Switch_Function(FUN_UNIT_JIN);
        TimeTick_Move = 0;
    }
    if (voice_cmd == WEIGHT_ANNOUNCE) {
        Voice_SendWA(Scale->weight, Scale->tare_weight, Scale->weight_unit);
        TimeTick_Move = 0;
    }
}


/**
 * @brief Carry out the calibration program 
 * @param  
 */
void System_Calibration(void) {
    // 1. Listen to the key event
    if (Key_Check(KEY_1, KEY_SINGLE)) {
        Switch_Function(FUN_POWER_OFF);
        TimeTick_Move = 0;
    }
    if (Key_Check(KEY_4, KEY_SINGLE)) {
        // Comfire the weight
        Weight_Calibrate(Scale->calibrate_weight);
        Scale->is_calibrated = 1; 
        Scale->calibrate_foot = 1;
        TimeTick_Move = 0;
    }
    if (Key_Check(KEY_2, KEY_SINGLE)) {
        // Change the foot 
        uint8_t temp;
        switch (Scale->calibrate_foot) {
            case 1: temp = 5; break;
            case 5: temp = 10; break;
            case 10: temp = 100; break; 
        }
        Scale->calibrate_foot = temp;
        TimeTick_Move = 0;
    }
    if (Key_Check(KEY_2, KEY_LONG)) {
        // Clear the weight
        Scale->calibrate_weight = 0;
        TimeTick_Move = 0;
    }
    if (Key_Check(KEY_3, KEY_SINGLE)) {
        // Add the weight
        Scale->calibrate_weight += Scale->calibrate_foot;
        TimeTick_Move = 0;
    }
}

void System_Announce(void) {
    if (WA_Flag) {
        if (OV_Flag) {
            Voice_SendOV();
        } 
        WA_Flag = 0;
    }
}

/**
 * @brief Main loop called in the main function 
 * @param  
 */
void Main_Loop(void) {
    UI_Display(Cur_SysMode);
    switch(Cur_SysMode) {
        case SYS_OFF: System_OFF(); break;
        case SYS_INIT: System_Init(); break;
        case SYS_NORMAL: {
            Weight_Update();
            Scale->weight = Weight_GetWeight(); 
            Scale->tare_weight = Weight_GetTare();
            if (OV_Flag) {
                Scale->weight = MAX_WEIGHT;
            }
			
            System_Normal(); 
            System_Announce();

            break;
        }
        case SYS_CALIBRATION: 
            Weight_Update();
            System_Calibration();
            break;

    }
}

void Voice_It(void) {
   if (TIM_GetFlagStatus(TIM0, TIM_Flag_TI) == SET) {
		TIM_ClearFlag(TIM0, TIM_Flag_TI);
        if (Cur_SysMode != SYS_OFF) {
            
            TimeTick_Weight++;
            if (TimeTick_Weight >= 1000) {
                WF_Flag = 1;
                TimeTick_Weight = 0;
            }

            
            TimeTick_Voice++;
            if (TimeTick_Voice >= 2000) {
                WA_Flag = 1;
                TimeTick_Voice = 0;
            }

            TimeTick_Move++;
            if (TimeTick_Move >= 30000) {
                Switch_Function(FUN_POWER_OFF);
                TimeTick_Move = 0;
            }

        }
	}
}


