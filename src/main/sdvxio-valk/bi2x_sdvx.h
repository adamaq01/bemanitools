#pragma once

#include <stddef.h>
#include <stdint.h>

enum sdvx_tape_led_index {
    SDVX_TAPE_LED_TITLE = 0,
    SDVX_TAPE_LED_UPPER_LEFT_SPEAKER = 1,
    SDVX_TAPE_LED_UPPER_RIGHT_SPEAKER = 2,
    SDVX_TAPE_LED_LEFT_WING = 3,
    SDVX_TAPE_LED_RIGHT_WING = 4,
    SDVX_TAPE_LED_CONTROL_PANEL = 5,
    SDVX_TAPE_LED_LOWER_LEFT_SPEAKER = 6,
    SDVX_TAPE_LED_LOWER_RIGHT_SPEAKER = 7,
    SDVX_TAPE_LED_WOOFER = 8,
    SDVX_TAPE_LED_V_UNIT = 9,
    SDVX_TAPE_LED_TOTAL = 10,
};

enum sdvx_gpio_index {
    SDVX_GPIO_START = 0,
    SDVX_GPIO_A = 1,
    SDVX_GPIO_B = 2,
    SDVX_GPIO_C = 3,
    SDVX_GPIO_D = 4,
    SDVX_GPIO_FXL = 5,
    SDVX_GPIO_FXR = 6,
    SDVX_GPIO_TOTAL = 7,
};

typedef struct lights_table_entry {
    uint8_t count;
    size_t offset;
} lights_table_entry_t;

static const lights_table_entry_t lights_table[SDVX_TAPE_LED_TOTAL] = {
    [SDVX_TAPE_LED_TITLE] = {74, 0},
    [SDVX_TAPE_LED_UPPER_LEFT_SPEAKER] = {12, 74},
    [SDVX_TAPE_LED_UPPER_RIGHT_SPEAKER] = {12, 86},
    [SDVX_TAPE_LED_LEFT_WING] = {56, 98},
    [SDVX_TAPE_LED_RIGHT_WING] = {56, 154},
    [SDVX_TAPE_LED_CONTROL_PANEL] = {94, 210},
    [SDVX_TAPE_LED_LOWER_LEFT_SPEAKER] = {12, 304},
    [SDVX_TAPE_LED_LOWER_RIGHT_SPEAKER] = {12, 316},
    [SDVX_TAPE_LED_WOOFER] = {14, 328},
    [SDVX_TAPE_LED_V_UNIT] = {86, 342},
};
#define TOTAL_LIGHTS_COUNT \
    (74 + 12 + 12 + 56 + 56 + 94 + 12 + 12 + 14 + 86) // 428

typedef struct rgb {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_t;

rgb_t mix_rgb(rgb_t a, rgb_t b);
void fill_rgb_array(rgb_t *array, size_t count, rgb_t color);

typedef struct bi2x_output {
    union {
        rgb_t rgb[TOTAL_LIGHTS_COUNT];
        uint8_t raw[sizeof(rgb_t) * TOTAL_LIGHTS_COUNT];
    } tape_led;
    union {
#pragma pack(push, 1)
        struct {
            uint8_t start : 1;
            uint8_t a : 1;
            uint8_t b : 1;
            uint8_t c : 1;
            uint8_t d : 1;
            uint8_t fxl : 1;
            uint8_t fxr : 1;
        } buttons;
#pragma pack(pop)
        uint8_t raw;
    } gpio;
} bi2x_output_t;

void bi2x_output_init(bi2x_output_t *state);
rgb_t *bi2x_output_lights(
    bi2x_output_t *state, enum sdvx_tape_led_index index, size_t *count);
int bi2x_output_compare_tape_led(
    bi2x_output_t *old, bi2x_output_t *current, enum sdvx_tape_led_index index);
