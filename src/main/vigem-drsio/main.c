#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <windows.h>

#include "ViGEm/Client.h"

#include "bemanitools/drsio.h"
#include "util/log.h"
#include "util/math.h"
#include "util/thread.h"
#include "vigemstub/helper.h"

#include "vigem-drsio/config-vigem-drsio.h"

bool check_key(uint16_t input, size_t idx_in)
{
    if ((input >> idx_in) & 1) {
        return true;
    }

    return false;
}

uint16_t check_assign_key(uint16_t input, size_t idx_in, size_t bit_out)
{
    if (check_key(input, idx_in)) {
        return bit_out;
    }

    return 0;
}

uint16_t check_assign_gpio(uint16_t input, size_t idx_in, size_t gpio_out)
{
    if (check_key(input, idx_in)) {
        return 1 << gpio_out;
    }

    return 0;
}

int main(int argc, char **argv)
{
    log_to_writer(log_writer_stdout, NULL);

    struct vigem_drsio_config config;
    if (!get_vigem_drsio_config(&config)) {
        exit(EXIT_FAILURE);
    }

    drs_io_set_loggers(
        log_impl_misc, log_impl_info, log_impl_warning, log_impl_fatal);

    if (!drs_io_init(crt_thread_create, crt_thread_join, crt_thread_destroy)) {
        log_warning("Initializing drsio failed");
        return -1;
    }

    PVIGEM_CLIENT client = vigem_helper_setup();

    if (!client) {
        log_warning("client failed to connect failed");
        return -1;
    }

    PVIGEM_TARGET pad = vigem_helper_add_pad(client);

    if (!pad) {
        log_warning("vigem_alloc pad 1 failed");
        return -1;
    }

    bool loop = true;

    uint32_t buttons;

    XUSB_REPORT state;

    log_info("vigem init succeeded, beginning poll loop");

    while (loop) {
        buttons = drs_io_get_buttons();

        memset(&state, 0, sizeof(state));

        state.wButtons |=
            check_assign_key(buttons, DRS_IO_SYS_SERVICE, XUSB_GAMEPAD_START);
        state.wButtons |=
            check_assign_key(buttons, DRS_IO_SYS_TEST, XUSB_GAMEPAD_BACK);
        state.wButtons |=
            check_assign_key(buttons, DRS_IO_SYS_COIN, XUSB_GAMEPAD_GUIDE);

        state.wButtons |=
            check_assign_key(buttons, DRS_IO_PANEL_P1_RIGHT, XUSB_GAMEPAD_DPAD_RIGHT);
        state.wButtons |=
            check_assign_key(buttons, DRS_IO_PANEL_P1_DOWN, XUSB_GAMEPAD_DPAD_DOWN);
        state.wButtons |=
            check_assign_key(buttons, DRS_IO_PANEL_P1_UP, XUSB_GAMEPAD_DPAD_UP);
        state.wButtons |=
            check_assign_key(buttons, DRS_IO_PANEL_P1_LEFT, XUSB_GAMEPAD_DPAD_LEFT);
        state.wButtons |= check_assign_key(
            buttons, DRS_IO_PANEL_P1_START, XUSB_GAMEPAD_LEFT_SHOULDER);

        state.wButtons |=
            check_assign_key(buttons, DRS_IO_PANEL_P2_RIGHT, XUSB_GAMEPAD_A);
        state.wButtons |=
            check_assign_key(buttons, DRS_IO_PANEL_P2_DOWN, XUSB_GAMEPAD_B);
        state.wButtons |=
            check_assign_key(buttons, DRS_IO_PANEL_P2_UP, XUSB_GAMEPAD_X);
        state.wButtons |=
            check_assign_key(buttons, DRS_IO_PANEL_P2_LEFT, XUSB_GAMEPAD_Y);
        state.wButtons |= check_assign_key(
            buttons, DRS_IO_PANEL_P2_START, XUSB_GAMEPAD_RIGHT_SHOULDER);

        vigem_target_x360_update(client, pad, state);

        if (check_key(buttons, DRS_IO_SYS_TEST) &&
            check_key(buttons, DRS_IO_SYS_SERVICE)) {
            loop = false;
        }

        // avoid banging
        Sleep(1);
    }

    vigem_target_remove(client, pad);
    vigem_target_free(pad);

    vigem_free(client);

    Sleep(1000);

    drs_io_fini();

    return 0;
}
