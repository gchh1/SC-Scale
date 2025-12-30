#include "sc32f12xx.h"                  // Device header
#include "LED.h"
#include "Delay.h"

// ==========================================
// 1. 标准共阴极数码管段码表 (0-9)
// bit0=a, bit1=b, bit2=c, bit3=d, bit4=e, bit5=f, bit6=g, bit7=dp
// ==========================================
const uint8_t LED_Digit_Code[10] = {
    0x3F, // 0: 0011 1111
    0x06, // 1: 0000 0110
    0x5B, // 2: 0101 1011
    0x4F, // 3: 0100 1111
    0x66, // 4: 0110 0110
    0x6D, // 5: 0110 1101
    0x7D, // 6: 0111 1101
    0x07, // 7: 0000 0111
    0x7F, // 8: 0111 1111
    0x6F  // 9: 0110 1111
};

// ==========================================
// 2. 硬件映射表 (关键!)
// 索引对应标准段(0=a, 1=b...), 值对应 SEG 寄存器的偏移量
// 基于你的原理图: 
// a->SEG8(+0), b->SEG10(+2), c->SEG12(+4), d->SEG14(+6)
// e->SEG15(+7), f->SEG9(+1), g->SEG11(+3), dp->SEG13(+5)
// ==========================================
const uint8_t Segment_Map[8] = {
    0, // Index 0 (a)  -> SEG8
    2, // Index 1 (b)  -> SEG10
    4, // Index 2 (c)  -> SEG12
    6, // Index 3 (d)  -> SEG14
    7, // Index 4 (e)  -> SEG15
    1, // Index 5 (f)  -> SEG9
    3, // Index 6 (g)  -> SEG11
    5  // Index 7 (dp) -> SEG13
};

// 显存缓存 (对应 SEG8 ~ SEG15)
uint8_t LED_RAM_Buffer[8] = {0}; 

void LED_Clear(void) {
	LED_Write(LED_RAMRegister_8, 0x00);
	LED_Write(LED_RAMRegister_9, 0x00);
	LED_Write(LED_RAMRegister_10, 0x00);
	LED_Write(LED_RAMRegister_11, 0x00);
	LED_Write(LED_RAMRegister_12, 0x00);
	LED_Write(LED_RAMRegister_13, 0x00);
	LED_Write(LED_RAMRegister_14, 0x00);
	LED_Write(LED_RAMRegister_15, 0x00);
}

/**
 * @brief 在指定位置显示指定数字
 * @param position: 1 (COM4), 2 (COM5), 3 (COM6), 4 (COM7)
 * @param number:   0-9
 */
void LED_SetOneDigit(uint8_t position, uint8_t number) {
    // 1. 安全检查
    if (position < 1 || position > 4) return;
    if (number > 9) return;

    // 2. 获取标准段码 (比如数字 '1' 是 0x06)
    uint8_t seg_code = LED_Digit_Code[number]; 
    
    // 3. 计算位掩码 (High Nibble Logic)
    // COM4(Pos1) -> bit4 (0x10)
    // COM5(Pos2) -> bit5 (0x20) ...
    uint8_t com_mask = 1 << ((position - 1) + 4); 

    // 4. 遍历标准段 a-dp (共8个)
    for (int i = 0; i < 8; i++) {
        // 查找当前段对应的硬件 SEG 寄存器偏移量
        uint8_t reg_offset = Segment_Map[i];
        
        // 检查数字的这一段是否需要亮
        if ((seg_code >> i) & 0x01) {
            // 需要亮 -> 设置缓存中对应的 COM 位
            LED_RAM_Buffer[reg_offset] |= com_mask;
        }
        else {
            // 需要灭 -> 清除缓存中对应的 COM 位
            LED_RAM_Buffer[reg_offset] &= ~com_mask;
        }

        // 5. 将更新后的数据写入硬件
        // 基础地址 LED_RAMRegister_8 + 偏移量
        LED_Write(LED_RAMRegister_8 + reg_offset, LED_RAM_Buffer[reg_offset]);
    }
}

void LED_SetDot(uint8_t position) {
	// 1. 安全检查
	if (position < 1 || position > 4) return;
	
	LED_Write(LED_RAMRegister_13, (1 << (position - 1)) << 4);
}

void LED_SetNum(uint16_t num) {
	if (num > 9999) return;
    
    // 1. 计算当前数字实际占用了哪些 COM 位 (从右往左)
    // 例如，如果 num=12，则使用了 COM7 和 COM6 (position 4 和 3)
    uint8_t current_num_length = 0;
    if (num == 0) { // 特殊处理 0
        current_num_length = 1; 
    } else {
        uint16_t temp_num = num;
        while (temp_num > 0) {
            temp_num /= 10;
            current_num_length++;
        }
    }
    
    // 2. 清除未使用的 COM 位，以消除前一次显示的影响
    // 未使用的 COM 位从 position 1 开始，到 (4 - current_num_length) 结束
    // 例如，显示 "12" (长度为2)，则 position 1, 2 未使用 (COM4, COM5)
    for (int pos_to_clear = 1; pos_to_clear <= (4 - current_num_length); pos_to_clear++) {
        uint8_t com_mask = 1 << ((pos_to_clear - 1) + 4);
        // 遍历所有 SEG 寄存器 (假设有 15 个，即 0-14 偏移量)
        // 假设 Segment_Map 的最大偏移量在 [0, 14] 之间
        // 这里需要知道 LED_RAM_Buffer 的实际大小或有效索引范围
        // 根据您的代码，LED_RAM_Buffer 只使用了 Segment_Map 索引的偏移量
        
        // 我们可以遍历所有段的偏移量 (0-7 对应 a-dp)
        for (int i = 0; i < 8; i++) {
            uint8_t reg_offset = Segment_Map[i];
            
            // 清除缓存中对应 COM 位
            LED_RAM_Buffer[reg_offset] &= ~com_mask;
            
            // 写入硬件
            LED_Write(LED_RAMRegister_8 + reg_offset, LED_RAM_Buffer[reg_offset]);
        }
        
        // 额外处理 DP 段，它通常位于不同的寄存器 (LED_RAMRegister_13)
        // 假设 DP 段也受 COM 位的控制，并且 position i 的 DP 对应 (1 << (i - 1)) << 4
        // 您需要知道 DP 寄存器在 LED_RAM_Buffer 中的索引 (如果它也在 buffer 中)
        // 如果 DP 不在 LED_RAM_Buffer 中，则需要单独处理 DP 寄存器的清除
        // 鉴于您 LED_SetDot 只写 LED_RAMRegister_13，我们假设它不在此清除循环内，
        // 并且 LED_SetDot 的设计是覆盖式写入，所以这里可能不需要处理 DP。
    }

    // 3. 设置当前数字
    int po = 4; // 从最右边一位开始
    uint16_t temp_num = num;
    
    // 如果数字为 0，也需要显示
    if (temp_num == 0) {
        LED_SetOneDigit(po, 0);
        return; // 显示完毕
    }

    while (temp_num > 0) {
        LED_SetOneDigit(po, temp_num % 10);
        temp_num /= 10;
        po --;
    }
}
	

void LEDX_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_DriveLevel = 0;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_10); 
	GPIO_SetBits(GPIOB, GPIO_Pin_13); 
	GPIO_SetBits(GPIOB, GPIO_Pin_14); 
	GPIO_SetBits(GPIOB, GPIO_Pin_15); 
}

void LEDX_On(uint8_t LEDX) {
	if (LEDX < 2 || LEDX > 5) return;
	
	switch(LEDX) {
		case 2: GPIO_ResetBits(GPIOB, GPIO_Pin_10); break;
		case 3: GPIO_ResetBits(GPIOB, GPIO_Pin_13); break;
		case 4: GPIO_ResetBits(GPIOB, GPIO_Pin_14); break;
		case 5: GPIO_ResetBits(GPIOB, GPIO_Pin_15); break;
	}
}

void LEDX_Off(uint8_t LEDX) {
	if (LEDX < 2 || LEDX > 5) return;
	
	switch(LEDX) {
		case 2: GPIO_SetBits(GPIOB, GPIO_Pin_10); break;
		case 3: GPIO_SetBits(GPIOB, GPIO_Pin_13); break;
		case 4: GPIO_SetBits(GPIOB, GPIO_Pin_14); break;
		case 5: GPIO_SetBits(GPIOB, GPIO_Pin_15); break;
	}
}

void LEDX_Shift(uint8_t LEDX) {
	if (LEDX < 2 || LEDX > 5) return;
	uint16_t pin;
	
	switch(LEDX) {
		case 2: pin = GPIO_Pin_10; break;
		case 3: pin = GPIO_Pin_13; break;
		case 4: pin = GPIO_Pin_14; break;
		case 5: pin = GPIO_Pin_15; break;
	}
	
	if (GPIO_ReadDataBit(GPIOB, pin) == RESET) {
		GPIO_SetBits(GPIOB, pin);
	} else {
		GPIO_ResetBits(GPIOB, pin);
	}
}

BitAction LEDX_State(uint8_t LEDX) {
	uint16_t pin;
	
	switch(LEDX) {
		case 2: pin = GPIO_Pin_10; break;
		case 3: pin = GPIO_Pin_13; break;
		case 4: pin = GPIO_Pin_14; break;
		case 5: pin = GPIO_Pin_15; break;
	}
	
	return GPIO_ReadDataBit(GPIOB, pin);
}

void LEDX_Shine(uint8_t LEDX) {
 	uint16_t pin;
	
	switch(LEDX) {
		case 2: pin = GPIO_Pin_10; break;
		case 3: pin = GPIO_Pin_13; break;
		case 4: pin = GPIO_Pin_14; break;
		case 5: pin = GPIO_Pin_15; break;
	}

    GPIO_ResetBits(GPIOB, pin);
    Delay_s(1);
    GPIO_SetBits(GPIOB, pin);
}
