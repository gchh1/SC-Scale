#ifndef __KEY_H_
#define __KEY_H_
#include <stdint.h>

#define Key_Port GPIOA
#define Key1_Pin GPIO_Pin_10
#define Key2_Pin GPIO_Pin_11
#define Key3_Pin GPIO_Pin_12
#define Key4_Pin GPIO_Pin_13

#define KEY_NONE    0
#define KEY_1       1
#define KEY_2       2 
#define KEY_3       3 
#define KEY_4       4

/* 按键数量配置 */
#define KEY_COUNT				5			// 系统支持的按键总数
/* 按键状态标志位定义 */
#define KEY_HOLD				0x01		// 按键保持按下状态
#define KEY_DOWN				0x02		// 按键刚按下瞬间
#define KEY_UP					0x04		// 按键刚松开瞬间
#define KEY_SINGLE				0x08		// 按键单击
#define KEY_DOUBLE				0x10		// 按键双击
#define KEY_LONG				0x20		// 按键长按
#define KEY_REPEAT				0x40		// 按键连按（长按后的重复触发）
/* 按键物理状态定义 */
#define KEY_PRESSED				1		// 按键被按下的物理状态
#define KEY_UNPRESSED			0		// 按键未被按下的物理状态
/* 按键时间参数定义 (单位: ms) */
#define KEY_TIME_DOUBLE			200		// 双击间隔时间阈值
#define KEY_TIME_LONG			2000	// 长按时间阈值
#define KEY_TIME_REPEAT			100		// 连按重复间隔时间


void Key_Init(void);
uint8_t Key_Check(uint8_t key, uint8_t flag);
void Key_Tick(void);

#endif
