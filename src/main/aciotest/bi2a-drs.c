#include "aciotest/bi2a-drs.h"

#include "acio/acio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bio2drv/bi2a-drs.h"

bool aciotest_bi2a_drs_handler_init(
    struct aciodrv_device_ctx *device, uint8_t node_id, void **ctx)
{
    *ctx = malloc(sizeof(uint32_t));
    *((uint32_t *) *ctx) = 0;

    return bio2drv_bi2a_drs_init(device, node_id);
}

bool aciotest_bi2a_drs_handler_update(
    struct aciodrv_device_ctx *device, uint8_t node_id, void *ctx)
{
    struct bi2a_drs_state_in pin;
    struct bi2a_drs_state_out pout;

    memset(&pout, 0, sizeof(pout));

    if (!bio2drv_bi2a_drs_poll(device, node_id, &pout, &pin)) {
        return false;
    }

    printf(
        "|---------------------------------------|\n"
        "|   Test %d           Coin %d             |\n"
        "|   Service %d                           |\n"
        "|---------------------------------------|\n"
        "|                                       |\n"
        "|        %d                   %d          |\n"
        "|      %d   %d   %d           %d   %d   %d    |\n"
        "|        %d                   %d          |\n"
        "|                                       |\n"
        "|---------------------------------------|\n",

        pin.system.b_test,
        pin.system.b_coin,
        pin.system.b_service,

        pin.player_1.b_up,
        pin.player_2.b_up,

        pin.player_1.b_left,
        pin.player_1.b_right,
        pin.player_1.b_start,
        pin.player_2.b_left,
        pin.player_2.b_right,
        pin.player_2.b_start,

        pin.player_1.b_down,
        pin.player_2.b_down
    );

    return true;
}
