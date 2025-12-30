#ifndef __FILTER_H_
#define __FILTER_H_
#include <stdint.h>

// 建议将窗口扩大到 32 或 64 以应对 HX711 的底噪
#define WINDOW_SIZE 12              
#define STABLE_THRESHOLD 200         // 根据 DEFAULT_GAP_VALUE 调整，40 约等于 0.4g
#define STABLE_COUNT 6             // 连续 15 次稳定才触发 WF_Flag

typedef struct {
    int32_t buffer[WINDOW_SIZE];    
    int64_t sum;                    // 运行总和，用于 O(1) 效率计算
    uint16_t index;                 
    uint16_t count;                 
    int32_t last_output;            
    uint8_t stable_counter;         
    uint8_t initialized;            
} Filter;

void Filter_Init(Filter *filter);
int32_t Filter_Update(Filter *filter, int32_t new_sample);
uint8_t Is_Stable(Filter *filter);
void Filter_Reset(Filter *filter);

#endif
