#ifndef __Weight_H_
#define __Weight_H_
#include "sc32_conf.h"
#include "sc32f1xxx_gpio.h"
#include "HX711.h"

// 配置常量
#define OVERWEIGHT_THRESHOLD_G  1000.0f  // 超重阈值 (1kg)
#define DEFAULT_GAP_VALUE       1065.0f   // 默认标定系数


void Weight_Init(void);
void Weight_Update(void);          // 在主循环非阻塞调用
float Weight_GetWeight(void);      // 获取当前重量(g)
float Weight_GetTare(void);        // 获取当前皮重 
void Weight_Tare(void);             // 去皮
void Weight_TareC(void);
void Weight_SetZero(void);         // 置零
void Weight_Calibrate(float standard_weight_g); // 校准
uint8_t Weight_IsStable(void);

#endif
