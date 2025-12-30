#include "Filter.h"
#include <stdlib.h>

void Filter_Init(Filter *filter) {
    for (int i = 0; i < WINDOW_SIZE; i++) {
        filter->buffer[i] = 0;
    }
    filter->sum = 0;
    filter->index = 0;
    filter->count = 0;
    filter->last_output = 0;
    filter->stable_counter = 0;
    filter->initialized = 0;
}

int32_t Filter_Update(Filter *filter, int32_t new_sample) {
    // 1. 更新滑动窗口总和 (减去旧值，加上新值)
    if (filter->count >= WINDOW_SIZE) {
        filter->sum -= filter->buffer[filter->index];
    } else {
        filter->count++;
    }
    
    filter->buffer[filter->index] = new_sample;
    filter->sum += new_sample;
    filter->index = (filter->index + 1) % WINDOW_SIZE;

    // 2. 计算当前平均值
    int32_t current_output = (int32_t)(filter->sum / filter->count);

    // 3. 稳定性检测 (必须在更新 last_output 之前对比！)
    if (filter->initialized) {
        if (abs(current_output - filter->last_output) < STABLE_THRESHOLD) {
            if (filter->stable_counter < 255) filter->stable_counter++;
        } else {
            filter->stable_counter = 0;
        }
    }

    // 4. 更新状态
    filter->last_output = current_output;
    filter->initialized = 1;

    return current_output;
}

uint8_t Is_Stable(Filter *filter) {
    return (filter->stable_counter >= STABLE_COUNT);
}

void Filter_Reset(Filter *filter) {
    Filter_Init(filter);
}
