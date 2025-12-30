#ifndef __UTILS_H_
#define __UTILS_H_

#define FUN_POWER_ON    1
#define FUN_POWER_OFF   2
#define FUN_TARE        3
#define FUN_ZERO        4
#define FUN_UNIT        5
#define FUN_UNIT_G      6
#define FUN_UNIT_KG     7
#define FUN_UNIT_JIN    8
#define FUN_CALIBRATE   9
#define FUN_TAREC       10 

#define MAX_WEIGHT 1000.0f
#define DIFF 1.0f

#define OVERWEIGHT_THRESHOLD 1.0f    // 超重阈值 1kg
#define STABLE_THRESHOLD      1.0f  // 稳定重量阈值 ±0.05kg
#define NOTIFY_INTERVAL_MS    2000   // 通知间隔 2000ms

typedef enum {
    UNIT_G = 0,
    UNIT_KG = 1,
    UNIT_JIN = 2,
    UNIT_MAX
} WeightUnit_t;

typedef enum {
    SYS_OFF,
    SYS_INIT,
    SYS_NORMAL,
    SYS_CALIBRATION
} SystemMode_t;

typedef struct {
    // Weight information
    float weight;
    float tare_weight;
    uint8_t weight_dec;
    uint8_t tare_dec;
    WeightUnit_t weight_unit;
    WeightUnit_t tare_unit;

    // Flags  
    uint8_t is_overweight;
    uint8_t is_stable;
    uint8_t is_tared;

    

    // Calibration attribute
    float calibrate_weight;
    uint8_t is_calibrated;
    uint8_t calibrate_foot;
} Scale_Attribute;

void Main_Loop(void);
void Voice_It(void);

#endif
