/**
 * @file Weight.c
 * @author yhc 
 * @brief The interface of Weight module
 *          Public functions:
 *                      1. GetWeight, GetTare. Converting to the given unit
 *                      2. TARE, Clear TARE. 
 *                      3. ZERO
 *                      4. Switch unit.
 *                      5. Boot
 *          
 *          Private attributes:
 *                      1. current_weight_adc.
 *                      2.  
 * @version 0.1
 * @date 2025-12-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "Weight.h"
#include "HX711.h"
#include "Filter.h"

static Filter weight_filter;
static int32_t raw_offset = 0;      // 置零偏移
static int32_t tare_offset = 0;     // 去皮偏移
static float gap_value = DEFAULT_GAP_VALUE;
static int32_t current_raw = 0;
static uint8_t is_tared = 0;
static uint16_t unit_alpha = 1;

volatile uint8_t OV_Flag = 0; 

/**
 * @brief Initialize the weight 
 * @param  
 */
void Weight_Init(void) {
    Filter_Init(&weight_filter);
    // 初始读取几次以稳定
    for(int i=0; i<20; i++) {
        current_raw = Filter_Update(&weight_filter, HX711_Read());
    }
    raw_offset = current_raw;
    tare_offset = 0;
}

/**
 * @brief Update the current_raw  
 * @param  
 */
void Weight_Update(void) {
    current_raw = Filter_Update(&weight_filter, HX711_Read());

    float current_g = (float)(current_raw - raw_offset) / gap_value; 
    if (current_g > OVERWEIGHT_THRESHOLD_G) {
        OV_Flag = 1;
    } else {
        OV_Flag = 0;
    }
}

/**
 * @brief Return the current weight in unit 
 * @param None 
 * @return Current Weight 
 */
float Weight_GetWeight(void) {
    int32_t active_offset = is_tared ? (raw_offset + tare_offset) : raw_offset;
    int32_t diff = current_raw - active_offset;
    if (diff < 0) diff = 0; 
    return (float)diff / gap_value;
}

float Weight_GetTare(void) {
    return (float)tare_offset / gap_value; 
}

/**
 * @brief  
 * @param  
 */
void Weight_Tare(void) {
    if (!is_tared) {
        tare_offset = current_raw - raw_offset;
        is_tared = 1;
    }
}

void Weight_TareC(void) {
    if (is_tared) {
        tare_offset = 0;
        is_tared = 0;
    }
}

/**
 * @brief 
 * @param  
 */
void Weight_SetZero(void) {
    raw_offset = current_raw;
    is_tared = 0;
}

/**
 * @brief 
 * @param standard_weight_g 
 */
void Weight_Calibrate(float standard_weight_g) {
    if (standard_weight_g <= 0) return;
    gap_value = (float)(current_raw - raw_offset) / standard_weight_g;
}

/**
 * @brief 
 * @param  
 * @return 
 */
uint8_t Weight_IsStable(void) {
    return Is_Stable(&weight_filter);
}


