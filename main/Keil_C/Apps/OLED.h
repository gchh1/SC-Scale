#ifndef __OLED_H_
#define __OLED_H_

void _OLED_Init(void);
void _OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowInt(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowChinese(uint8_t Line, uint8_t Column, char *Chinese);
void OLED_ShowChinese2(uint8_t Line, uint8_t Column, uint8_t Length, char *Chinese);
void OLED_ShowFloat(uint8_t Line, uint8_t Column, float Number, uint8_t Length, uint8_t DecLength);

#endif
