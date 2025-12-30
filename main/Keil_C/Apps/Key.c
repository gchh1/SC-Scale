#include "sc32_conf.h"
#include "Key.h"
#include "Delay.h"

/* 按键状态标志数组 */
uint8_t Key_Flag[KEY_COUNT];			// 存储各按键的状态标志位


/**
 * @brief initialize the key
 * @param none 
 */
void Key_Init(void) {
	// Init the GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
	GPIO_InitStructure.GPIO_Pin = Key1_Pin;
	GPIO_InitStructure.GPIO_DriveLevel = 0;
	GPIO_Init(Key_Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
	GPIO_InitStructure.GPIO_Pin = Key2_Pin;
	GPIO_InitStructure.GPIO_DriveLevel = 0;
	GPIO_Init(Key_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
	GPIO_InitStructure.GPIO_Pin = Key3_Pin;
	GPIO_InitStructure.GPIO_DriveLevel = 0;
	GPIO_Init(Key_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
	GPIO_InitStructure.GPIO_Pin = Key4_Pin;
	GPIO_InitStructure.GPIO_DriveLevel = 0;
	GPIO_Init(Key_Port, &GPIO_InitStructure);
}

/**
 * @brief Return the KEY_PRESSED if the key is pressed  
 * @param key 
 * @return  
 */
uint8_t Key_GetState(uint8_t key) {
	switch (key) {
		case KEY_1: 
			if (GPIO_ReadDataBit(Key_Port, Key1_Pin) == 0) {
				return KEY_PRESSED;
			}
			break;
		case KEY_2: 
			if (GPIO_ReadDataBit(Key_Port, Key2_Pin) == 0) {
				return KEY_PRESSED;
			}
			break;
		case KEY_3: 
			if (GPIO_ReadDataBit(Key_Port, Key3_Pin) == 0) {
				return KEY_PRESSED;
			}
			break;
		case KEY_4: 
			if (GPIO_ReadDataBit(Key_Port, Key4_Pin) == 0) {
				return KEY_PRESSED;
			}
			break;
	}
	return KEY_UNPRESSED;
}

/**
 * @brief Return 1 if the flag of the key is triggered 
 * @param key 
 * @param flag KEY_SINGLE, KEY_DOUBLE, KEY_LONG  
 * @return 
 */
uint8_t Key_Check(uint8_t key, uint8_t flag) {
	// 检查指定按键的指定标志位是否置位
	if (Key_Flag[key] & flag) {
		if (flag != KEY_HOLD) {
			Key_Flag[key] &= ~flag;
		}
		return 1;
	}
	return 0;
}

/**
 * @brief Update the state bit. Called in the interrupt 
 * @param  
 */
void Key_Tick(void) {
	static uint8_t Count, i;						// Count: 扫描计数器, i: 循环变量
	static uint8_t CurrState[KEY_COUNT], PrevState[KEY_COUNT];	// 当前状态和前一次状态
	static uint8_t S[KEY_COUNT];					// 状态机状态数组
	static uint16_t Time[KEY_COUNT];				// 定时器数组
	// 所有按键的定时器递减
	for (i = 0; i < KEY_COUNT; i++) {
		Time[i] --;
	}
	
	// 每20ms扫描一次按键状态（假设本函数每1ms调用一次）
	Count ++;
	if (Count >= 20) {
		Count = 0;
		// 遍历所有按键
		for (i = 0; i < KEY_COUNT; i ++) {
			// 更新按键状态
			PrevState[i] = CurrState[i];
			CurrState[i] = Key_GetState(i);

			// 处理KEY_HOLD状态
			if (CurrState[i] == KEY_PRESSED) {
				Key_Flag[i] |= KEY_HOLD;		// 按键按下时置位HOLD标志
			}
			else {
				Key_Flag[i] &= ~KEY_HOLD;		// 按键松开时清除HOLD标志
			}

			// 处理KEY_DOWN状态（上升沿检测）
			if (CurrState[i] == KEY_PRESSED && PrevState[i] == KEY_UNPRESSED) {
				Key_Flag[i] |= KEY_DOWN;
			}

			// 处理KEY_UP状态（下降沿检测）
			if (CurrState[i] == KEY_UNPRESSED && PrevState[i] == KEY_PRESSED) {
				Key_Flag[i] |= KEY_UP;
			}

			// 按键状态机处理
			if (S[i] == 0) {	//* 状态0: 空闲状态
				if (CurrState[i] == KEY_PRESSED) {
					Time[i] = KEY_TIME_LONG;	// 设置长按计时器
					S[i] = 1;					// 转到状态1
				}
			} else if (S[i] == 1) { 	//* 状态1: 第一次按下状态
				if (CurrState[i] == KEY_UNPRESSED) {
					Time[i] = KEY_TIME_DOUBLE;	// 设置双击检测计时器
					S[i] = 2;					// 转到状态2
				} else if (Time[i] == 0) {    // 长按时间到
					Time[i] = KEY_TIME_REPEAT;	// 设置重复触发计时器
					Key_Flag[i] |= KEY_LONG;	// 置位长按标志
					S[i] = 4;					// 转到状态4
				}
			} else if (S[i] == 2) {		//* 状态2: 第一次松开状态
				if (CurrState[i] == KEY_PRESSED) {
					Key_Flag[i] |= KEY_DOUBLE;	// 置位双击标志
					S[i] = 3;					// 转到状态3
				} else if (Time[i] == 0) {		// 双击检测时间到
					Key_Flag[i] |= KEY_SINGLE;	// 置位单击标志
					S[i] = 0;					// 回到状态0
				}
			} else if (S[i] == 3) { 	//* 状态3: 双击确认状态
				if (CurrState[i] == KEY_UNPRESSED) {
					S[i] = 0;					// 回到状态0
				}
			} else if (S[i] == 4) {		//* 状态4: 长按状态
				if (CurrState[i] == KEY_UNPRESSED) {
					S[i] = 0;					// 回到状态0
				} else if (Time[i] == 0) {		// 重复触发时间到
					Time[i] = KEY_TIME_REPEAT;	// 重新设置重复触发计时器
					Key_Flag[i] |= KEY_REPEAT;	// 置位重复触发标志
					S[i] = 4;					// 保持状态4
				}
			}
		}
	}
}