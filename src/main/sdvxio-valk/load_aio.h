#pragma once

#include "aio.h"

void init_aio_handles(void);

struct bi2x_ctx;

struct bi2x_ctx *setup_bi2x(void);

void close_bi2x(struct bi2x_ctx *ctx);

void poll_bi2x(
    struct bi2x_ctx *ctx, struct AIO_IOB2_BI2X_UFC__DEVSTATUS *status);

void set_tape_led_data(struct bi2x_ctx *ctx, uint32_t index, uint8_t *data);

void set_player_button_lamp(
    struct bi2x_ctx *ctx, uint32_t index, uint8_t state);
