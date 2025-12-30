#include "HX711.h"
#include "Delay.h"

/**
 * @brief Init the HX711
 * @param None 
 */
void HX711_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // Init the DATA Pin
    GPIO_InitStructure.GPIO_Pin = HX711_DATA_GPIO_PIN;
    GPIO_InitStructure.GPIO_DriveLevel = GPIO_DriveLevel_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
    GPIO_Init(HX711_DATA_GPIO_PORT, &GPIO_InitStructure);

    // Init the SCK Pin
    GPIO_InitStructure.GPIO_Pin = HX711_SCK_GPIO_PIN;
    GPIO_InitStructure.GPIO_DriveLevel = GPIO_DriveLevel_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
    GPIO_Init(HX711_SCK_GPIO_PORT, &GPIO_InitStructure);

    GPIO_SetBits(HX711_SCK_GPIO_PORT, HX711_SCK_GPIO_PIN);
}

/**
 * @brief Init the DATA Pin as output
 * @param 
 */
void HX711_DATA_Out(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // Init the DATA Pin
    GPIO_InitStructure.GPIO_Pin = HX711_DATA_GPIO_PIN;
    GPIO_InitStructure.GPIO_DriveLevel = GPIO_DriveLevel_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
    GPIO_Init(HX711_DATA_GPIO_PORT, &GPIO_InitStructure);
}

void HX711_DATA_In(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // Init the DATA Pin
    GPIO_InitStructure.GPIO_Pin = HX711_DATA_GPIO_PIN;
    GPIO_InitStructure.GPIO_DriveLevel = GPIO_DriveLevel_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
    GPIO_Init(HX711_DATA_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @brief Read the value of HX711
 * @param  None 
 * @return The value of HX711 
 */
int32_t HX711_Read(void) {
    int32_t value = 0;
    uint8_t i;
    
    //* Set SCK to 0 and DATA to 1 
    HX711_DATA_Out();
    HX711_DATA_WRITE(1);
    Delay_us(1);
    HX711_SCK_WRITE(0);
    HX711_DATA_In();

    //* Wait until DATA is low
    int32_t timeout = 0xFFFFF; // 增加超时机制，防止死等
    while (HX711_DATA_READ && timeout) { timeout--; } 
    if(timeout == 0) return 0; // 超时退出

    //! When DATA is low, receive the value bit by bit
    for (i = 0; i < 24; i++) {
        HX711_SCK_WRITE(1);
        Delay_us(1);
        HX711_SCK_WRITE(0);
        
        value = value << 1;
        // 【优化】：简化位读取逻辑
        if (HX711_DATA_READ) {
            value |= 0x01;
        }

        Delay_us(1);
    }

    //! Convert the value and set Channel A gain 128
    HX711_SCK_WRITE(1);
    value = value ^ 0x800000;
    Delay_us(1);
    HX711_SCK_WRITE(0);

    return value;
}
