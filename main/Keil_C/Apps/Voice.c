#include "sc32_conf.h"
#include "Voice.h"

volatile uint8_t Voice_CMD;
volatile FlagStatus UART2_Flag;
volatile uint8_t UART2_RxData;

void Serial_SendByte(uint8_t Byte)
{
	UART_SendData(UART2, Byte);
    while (UART_GetFlagStatus(UART2, UART_Flag_TX) == RESET);
    UART_ClearFlag(UART2, UART_Flag_TX);
}

/**
 * @brief Send array of the given length through the UART2
 * @param Array  
 * @param Length 
 */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
        Serial_SendByte(Array[i]);
	}
}

/**
 * @brief Send the information for weight announcement
 * @param weight 
 * @param unit 
 */
void Voice_SendWA(float weight, float tare, uint8_t Unit) {
    uint8_t packet[21]; 

    // --- 1. 报文头部 (Index 0-1) ---
    packet[0] = 0xAA;
    packet[1] = 0x55;

    // --- 2. 单位 (Index 2) ---
    packet[2] = Unit;

    // --- 3. 提取并填充 Weight (Index 3-10) ---
    // 假设小数部分保留 3 位精度 (例如 12.345 -> 整数12, 小数345)
    int32_t w_int = (int32_t)weight;
    int32_t w_frac = (int32_t)((weight - (float)w_int) * 10.0f);
    if (w_frac < 0) w_frac = -w_frac; // 取绝对值

    // Weight 整数部分 (4字节，大端序)
    packet[6] = (uint8_t)(w_int >> 24);
    packet[5] = (uint8_t)(w_int >> 16);
    packet[4] = (uint8_t)(w_int >> 8);
    packet[3] = (uint8_t)(w_int);
    // Weight 小数部分 (4字节)
    packet[10] = (uint8_t)(w_frac >> 24);
    packet[9] = (uint8_t)(w_frac >> 16);
    packet[8] = (uint8_t)(w_frac >> 8);
    packet[7] = (uint8_t)(w_frac);

    // --- 4. 提取并填充 Tare (Index 11-18) ---
    int32_t t_int = (int32_t)tare;
    int32_t t_frac = (int32_t)((tare - (float)t_int) * 10.0f);
    if (t_frac < 0) t_frac = -t_frac;

    // Tare 整数部分 (4字节)
    packet[14] = (uint8_t)(t_int >> 24);
    packet[13] = (uint8_t)(t_int >> 16);
    packet[12] = (uint8_t)(t_int >> 8);
    packet[11] = (uint8_t)(t_int);
    // Tare 小数部分 (4字节)
    packet[18] = (uint8_t)(t_frac >> 24);
    packet[17] = (uint8_t)(t_frac >> 16);
    packet[16] = (uint8_t)(t_frac >> 8);
    packet[15] = (uint8_t)(t_frac);

    // --- 5. 报文尾部 (Index 19-20) ---
    packet[19] = 0x55;
    packet[20] = 0xAA;

    // --- 6. 调用发送函数 ---
    Serial_SendArray(packet, 21);
}

/**
 * @brief Set the weight in the voice 
 * @param weight 
 */
void Voice_SetWeight(float weight) {
    uint8_t packet[13];

    packet[0] = 0xAA;
    packet[1] = 0x55;

    packet[2] = 0x04;
    int32_t w_int = (int32_t)weight;
    int32_t w_frac = (int32_t)((weight - (float)w_int) * 1000.0f);
    if (w_frac < 0) w_frac = -w_frac; // 取绝对值

    // Weight 整数部分 (4字节，大端序)
    packet[6] = (uint8_t)(w_int >> 24);
    packet[5] = (uint8_t)(w_int >> 16);
    packet[4] = (uint8_t)(w_int >> 8);
    packet[3] = (uint8_t)(w_int);
    // Weight 小数部分 (4字节)
    packet[10] = (uint8_t)(w_frac >> 24);
    packet[9] = (uint8_t)(w_frac >> 16);
    packet[8] = (uint8_t)(w_frac >> 8);
    packet[7] = (uint8_t)(w_frac);

    packet[11] = 0x55;
    packet[12] = 0xAA;

    Serial_SendArray(packet, 13);
}

/**
 * @brief Send the information of overweight
 * @param  
 */
void Voice_SendOV(void) {
    uint8_t packet[5];
    
    packet[0] = 0xAA;
    packet[1] = 0x55;

    packet[2] = 0x03;

    packet[3] = 0x55;
    packet[4] = 0xAA;

    Serial_SendArray(packet, 5);
}

/**
 * @brief Receive the command from the UART
 * @param None 
 * @return CMD 
 */
uint8_t Voice_ReceiveCMD(void) {
    if (UART2_Flag == SET) {
        UART2_Flag = RESET;
        switch (Voice_CMD) {
            case 0x00: return 1;
            case 0x10: return 2;
            case 0x01: return 3;
            case 0x02: return 4;
            case 0x03: return 6;
            case 0x04: return 7;
            case 0x05: return 8; 
            case 0x06: return WEIGHT_ANNOUNCE;
            case 0x07: return 10;
        }
    }
	return 0;
}
