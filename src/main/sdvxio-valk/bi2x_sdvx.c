#include "bi2x_sdvx.h"

#include <stdlib.h>
#include <string.h>

rgb_t mix_rgb(rgb_t a, rgb_t b)
{
    return (rgb_t) {a.red / 2 + b.red / 2,
                    a.green / 2 + b.green / 2,
                    a.blue / 2 + b.blue / 2};
}

void fill_rgb_array(rgb_t *array, size_t count, rgb_t color)
{
    for (size_t i = 0; i < count; i++) {
        array[i] = color;
    }
}

void bi2x_output_init(bi2x_output_t *output)
{
    memset(output, 0, sizeof(bi2x_output_t));
}

rgb_t *bi2x_output_lights(
    bi2x_output_t *output, enum sdvx_tape_led_index index, size_t *count)
{
    lights_table_entry_t entry = lights_table[index];
    if (count) {
        *count = entry.count;
    }
    return output->tape_led.rgb + entry.offset;
}

int bi2x_output_compare_tape_led(
    bi2x_output_t *old, bi2x_output_t *current, enum sdvx_tape_led_index index)
{
    lights_table_entry_t entry = lights_table[index];
    return memcmp(
        old->tape_led.raw + entry.offset * sizeof(rgb_t),
        current->tape_led.raw + entry.offset * sizeof(rgb_t),
        entry.count * sizeof(rgb_t));
}
