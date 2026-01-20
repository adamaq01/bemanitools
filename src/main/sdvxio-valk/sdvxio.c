#include <windows.h>

#define LOG_MODULE "sdvxio-valk"

#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "bemanitools/glue.h"
#include "bemanitools/sdvxio.h"

#include "util/log.h"

#include "bi2x_sdvx.h"
#include "load_aio.h"

static atomic_bool running;
static atomic_bool processing_io;

static struct AIO_IOB2_BI2X_UFC__DEVSTATUS pin_cur;

static struct bi2x_ctx *bi2x_ctx;

struct sdvxio_lights {
    union {
        struct {
            rgb_t wingUpperLeft;
            rgb_t wingUpperRight;
            rgb_t wingLowerLeft;
            rgb_t wingLowerRight;
            rgb_t woofer;
            rgb_t controller;
            uint8_t generator_red;
            uint8_t generator_green;
            uint8_t pop;
            uint8_t titleL;
            uint8_t titleR;
        } lights;
        uint8_t raw[23];
    } pwm;

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
            uint8_t generator_blue : 1;
        } gpio;
#pragma pack(pop)
        uint8_t raw;
    } gpio;
};
struct sdvxio_lights state_staging;
_Static_assert(
    sizeof(state_staging.pwm.lights) == 23, "lights is the wrong size");
_Static_assert(sizeof(state_staging.gpio.gpio) == 1, "gpio is the wrong size");

void sdvxio_lights_to_bi2x_lights_state(
    struct sdvxio_lights *state, bi2x_output_t *bi2x_output)
{
    bi2x_output_init(bi2x_output);

    // Tape LEDs
    size_t count = 0;

    // Left wing
    rgb_t wingLeft = mix_rgb(
        state->pwm.lights.wingUpperLeft, state->pwm.lights.wingLowerLeft);
    rgb_t *left_wing =
        bi2x_output_lights(bi2x_output, SDVX_TAPE_LED_LEFT_WING, &count);
    fill_rgb_array(left_wing, count, wingLeft);
    count = 0;

    // Right wing
    rgb_t wingRight = mix_rgb(
        state->pwm.lights.wingUpperRight, state->pwm.lights.wingLowerRight);
    rgb_t *right_wing =
        bi2x_output_lights(bi2x_output, SDVX_TAPE_LED_RIGHT_WING, &count);
    fill_rgb_array(right_wing, count, wingRight);
    count = 0;

    // Woofer
    rgb_t *woofer =
        bi2x_output_lights(bi2x_output, SDVX_TAPE_LED_WOOFER, &count);
    fill_rgb_array(woofer, count, state->pwm.lights.woofer);
    count = 0;

    // Control Panel
    rgb_t *controller =
        bi2x_output_lights(bi2x_output, SDVX_TAPE_LED_CONTROL_PANEL, &count);
    fill_rgb_array(controller, count, state->pwm.lights.controller);
    count = 0;

    // Title
    rgb_t titleLeft = (rgb_t) {state->pwm.lights.titleL,
                               state->pwm.lights.titleL,
                               state->pwm.lights.titleL};
    rgb_t titleRight = (rgb_t) {state->pwm.lights.titleR,
                                state->pwm.lights.titleR,
                                state->pwm.lights.titleR};
    rgb_t *title = bi2x_output_lights(bi2x_output, SDVX_TAPE_LED_TITLE, &count);
    fill_rgb_array(title, count / 2, titleLeft);
    fill_rgb_array(title + count / 2, count / 2, titleRight);
    count = 0;

    // GPIO
    bi2x_output->gpio.raw = state->gpio.raw;

    // TODO: Figure out what to do with the rest of the lights
}

bi2x_output_t bi2x_output_ready;
static bi2x_output_t bi2x_output_old;
static bool _bio2_sdvx_io_poll(
    struct AIO_IOB2_BI2X_UFC__DEVSTATUS *pin, bi2x_output_t *bi2x_output)
{
    if (!running) {
        return false;
    }

    processing_io = true;

    // Inputs
    poll_bi2x(bi2x_ctx, pin);

    // Outputs
    // PWM
    for (size_t i = 0; i < SDVX_TAPE_LED_TOTAL; i++) {
        if (bi2x_output_compare_tape_led(&bi2x_output_old, bi2x_output, i) !=
            0) {
            rgb_t *leds = bi2x_output_lights(bi2x_output, i, NULL);
            set_tape_led_data(bi2x_ctx, i, (uint8_t *) leds);
        }
    }
    // GPIO
    for (size_t i = 0; i < SDVX_GPIO_TOTAL; i++) {
        uint8_t state = (bi2x_output->gpio.raw & (1 << i));
        if (state != (bi2x_output_old.gpio.raw & (1 << i))) {
            set_player_button_lamp(bi2x_ctx, i, state);
        }
    }
    memcpy(&bi2x_output_old, bi2x_output, sizeof(bi2x_output_t));

    processing_io = false;
    return true;
}

void sdvx_io_set_loggers(
    log_formatter_t misc,
    log_formatter_t info,
    log_formatter_t warning,
    log_formatter_t fatal)
{
    log_to_external(misc, info, warning, fatal);
}

bool sdvx_io_init(
    thread_create_t thread_create,
    thread_join_t thread_join,
    thread_destroy_t thread_destroy)
{
    bi2x_output_init(&bi2x_output_old);

    init_aio_handles();

    bi2x_ctx = setup_bi2x();
    if (!bi2x_ctx) {
        log_warning("Unable to start BI2X?");
        return false;
    }
    running = true;

    return running;
}

void sdvx_io_fini(void)
{
    running = false;

    while (processing_io) {
        // avoid banging
        Sleep(1);
    }

    close_bi2x(bi2x_ctx);

    bi2x_ctx = NULL;
}

void sdvx_io_set_gpio_lights(uint32_t gpio_lights)
{
    state_staging.gpio.gpio.start =
        (gpio_lights & (1 << SDVX_IO_OUT_GPIO_START)) > 0;
    state_staging.gpio.gpio.a = (gpio_lights & (1 << SDVX_IO_OUT_GPIO_A)) > 0;
    state_staging.gpio.gpio.b = (gpio_lights & (1 << SDVX_IO_OUT_GPIO_B)) > 0;
    state_staging.gpio.gpio.c = (gpio_lights & (1 << SDVX_IO_OUT_GPIO_C)) > 0;
    state_staging.gpio.gpio.d = (gpio_lights & (1 << SDVX_IO_OUT_GPIO_D)) > 0;
    state_staging.gpio.gpio.fxl =
        (gpio_lights & (1 << SDVX_IO_OUT_GPIO_FX_L)) > 0;
    state_staging.gpio.gpio.fxr =
        (gpio_lights & (1 << SDVX_IO_OUT_GPIO_FX_R)) > 0;
    state_staging.gpio.gpio.generator_blue =
        (gpio_lights & (1 << SDVX_IO_OUT_GPIO_GENERATOR_B)) > 0;
}

void sdvx_io_set_pwm_light(uint8_t light_no, uint8_t intensity)
{
    if (light_no < sizeof(state_staging.pwm.lights)) {
        state_staging.pwm.raw[light_no] = intensity;
    }
}

bool sdvx_io_write_output(void)
{
    sdvxio_lights_to_bi2x_lights_state(&state_staging, &bi2x_output_ready);

    return true;
}

bool sdvx_io_read_input(void)
{
    if (!_bio2_sdvx_io_poll(&pin_cur, &bi2x_output_ready)) {
        return false;
    }

    return true;
}

uint16_t sdvx_io_get_spinner_pos(uint8_t spinner_no)
{
    if (spinner_no == 0) {
        return pin_cur.analog_left >> 6;
    } else if (spinner_no == 1) {
        return pin_cur.analog_right >> 6;
    } else {
        return 0;
    }
}

static uint8_t shift_pin(uint16_t value, uint8_t pin)
{
    if (value) {
        return (1 << pin);
    }

    return 0;
}

uint8_t sdvx_io_get_input_gpio_sys(void)
{
    uint8_t state = 0;

    state |= shift_pin(pin_cur.test, SDVX_IO_IN_GPIO_SYS_TEST);
    state |= shift_pin(pin_cur.service, SDVX_IO_IN_GPIO_SYS_SERVICE);
    state |= shift_pin(pin_cur.coin, SDVX_IO_IN_GPIO_SYS_COIN);

    return state;
}

uint16_t sdvx_io_get_input_gpio(uint8_t gpio_bank)
{
    uint16_t state = 0;

    switch (gpio_bank) {
        case 0:
            state |= shift_pin(pin_cur.recorder, SDVX_IO_IN_GPIO_0_RECORDER);
            state |= shift_pin(pin_cur.headphone, SDVX_IO_IN_GPIO_0_HEADPHONE);
            state |=
                shift_pin(pin_cur.buttons & (1 << 0), SDVX_IO_IN_GPIO_0_START);
            state |= shift_pin(pin_cur.buttons & (1 << 1), SDVX_IO_IN_GPIO_0_A);
            state |= shift_pin(pin_cur.buttons & (1 << 2), SDVX_IO_IN_GPIO_0_B);
            state |= shift_pin(pin_cur.buttons & (1 << 3), SDVX_IO_IN_GPIO_0_C);
            break;
        case 1:
            state |= shift_pin(pin_cur.buttons & (1 << 4), SDVX_IO_IN_GPIO_1_D);
            state |=
                shift_pin(pin_cur.buttons & (1 << 5), SDVX_IO_IN_GPIO_1_FX_L);
            state |=
                shift_pin(pin_cur.buttons & (1 << 6), SDVX_IO_IN_GPIO_1_FX_R);
            break;
        default:
            break;
    }

    return state;
}

bool sdvx_io_set_amp_volume(
    uint8_t primary, uint8_t headphone, uint8_t subwoofer)
{
    return true;
}
