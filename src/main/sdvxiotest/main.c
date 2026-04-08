#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <windows.h>

#include "bemanitools/sdvxio.h"

#include "util/log.h"
#include "util/thread.h"

/**
 * Tool to test your implementations of sdvxio.
 */
int main(int argc, char **argv)
{
    log_to_writer(log_writer_stdout, NULL);

    sdvx_io_set_loggers(
        log_impl_misc, log_impl_info, log_impl_warning, log_impl_fatal);

    if (!sdvx_io_init(crt_thread_create, crt_thread_join, crt_thread_destroy)) {
        printf("Initializing sdvxio failed\n");
        return -1;
    }

    printf(">>> Initializing sdvxio successful, press enter to continue <<<\n");

    if (getchar() != '\n') {
        return 0;
    }

    /* inputs */
    uint8_t input_sys = 0;
    uint16_t input_gpio_0 = 0;
    uint16_t input_gpio_1 = 0;
    uint16_t spinner[2] = {0, 0};

    /* outputs */
    uint32_t gpio_lights = 0;
    // TODO: Add pwm lights

    bool loop = true;
    uint8_t cnt = 0;
    bool all_on = false;
    while (loop) {
        if (!sdvx_io_read_input()) {
            printf("ERROR: Reading input failed\n");
            return -2;
        }

        /* get inputs */
        input_sys = sdvx_io_get_input_gpio_sys();
        input_gpio_0 = sdvx_io_get_input_gpio(0);
        input_gpio_1 = sdvx_io_get_input_gpio(1);
        spinner[0] = sdvx_io_get_spinner_pos(0);
        spinner[1] = sdvx_io_get_spinner_pos(1);

        system("cls");
        printf(
            ">>> SDVX IO %d:\n"
            "BTN A B C D: %d %d %d %d\n"
            "FX-L R: %d %d\n"
            "VOL L: %d\n"
            "VOL R: %d\n"
            "START COIN TEST SERV REC HP: %d %d %d %d %d %d\n",
            cnt,
            input_gpio_0 & (1 << SDVX_IO_IN_GPIO_0_A),
            input_gpio_0 & (1 << SDVX_IO_IN_GPIO_0_B),
            input_gpio_0 & (1 << SDVX_IO_IN_GPIO_0_C),
            input_gpio_1 & (1 << SDVX_IO_IN_GPIO_1_D),
            input_gpio_1 & (1 << SDVX_IO_IN_GPIO_1_FX_L),
            input_gpio_1 & (1 << SDVX_IO_IN_GPIO_1_FX_R),
            spinner[0],
            spinner[1],
            input_sys & (1 << SDVX_IO_IN_GPIO_SYS_COIN),
            input_sys & (1 << SDVX_IO_IN_GPIO_SYS_TEST),
            input_sys & (1 << SDVX_IO_IN_GPIO_SYS_SERVICE),
            input_gpio_0 & (1 << SDVX_IO_IN_GPIO_0_RECORDER),
            input_gpio_0 & (1 << SDVX_IO_IN_GPIO_0_HEADPHONE));

        /* set outputs */
        if (all_on) {
            gpio_lights = 0xFFFFFFFF;
        }

        sdvx_io_set_gpio_lights(gpio_lights);

        /* light up keys when pressed */
        if (!all_on) {
            if (input_gpio_0 & (1 << SDVX_IO_IN_GPIO_0_START)) {
                gpio_lights |= (1 << SDVX_IO_OUT_GPIO_START);
            }
            if (input_gpio_0 & (1 << SDVX_IO_IN_GPIO_0_A)) {
                gpio_lights |= (1 << SDVX_IO_OUT_GPIO_A);
            }
            if (input_gpio_0 & (1 << SDVX_IO_IN_GPIO_0_B)) {
                gpio_lights |= (1 << SDVX_IO_OUT_GPIO_B);
            }
            if (input_gpio_0 & (1 << SDVX_IO_IN_GPIO_0_C)) {
                gpio_lights |= (1 << SDVX_IO_OUT_GPIO_C);
            }
            if (input_gpio_1 & (1 << SDVX_IO_IN_GPIO_1_D)) {
                gpio_lights |= (1 << SDVX_IO_OUT_GPIO_D);
            }
            if (input_gpio_1 & (1 << SDVX_IO_IN_GPIO_1_FX_L)) {
                gpio_lights |= (1 << SDVX_IO_OUT_GPIO_FX_L);
            }
            if (input_gpio_1 & (1 << SDVX_IO_IN_GPIO_1_FX_R)) {
                gpio_lights |= (1 << SDVX_IO_OUT_GPIO_FX_R);
            }
        } else {
            /* disable all on when a single button is pressed */
            if (input_gpio_0 || input_gpio_1 || input_sys) {
                all_on = false;
                gpio_lights = 0;
            }
        }

        if (!sdvx_io_write_output()) {
            printf("ERROR: Writing output failed\n");
            return -4;
        }

        /* avoid CPU banging */
        Sleep(5);
        ++cnt;

        /* process menu */
        if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0) {
            system("cls");
            Sleep(5);
            printf(
                "Menu options:\n"
                "  0: Exit menu and continue loop\n"
                "  1: Exit\n"
                "  2: Set pwm state\n"
                "  3: Set all outputs on (cleared by pressing any SDVX "
                "button)\n"
                "  4: Clear all outputs\n"
                "Waiting for input: ");
            char c = getchar();

            switch (c) {
                case '1': {
                    loop = false;
                    break;
                }

                case '2': {
                    int light_no;
                    int intensity;
                    printf("Enter light number (0-17): ");
                    int n = scanf("%d", &light_no);
                    printf("Enter intensity (0-255): ");
                    n = scanf("%d", &intensity);

                    if (n > 0) {
                        sdvx_io_set_pwm_light(light_no, intensity);
                        sdvx_io_write_output();
                        sdvx_io_read_input();
                    }

                    break;
                }

                case '3': {
                    all_on = true;
                    break;
                }

                case '4': {
                    all_on = false;
                    break;
                }

                case '0':
                default:
                    break;
            }
        }
    }

    system("cls");


    sdvx_io_set_gpio_lights(0);
    sdvx_io_set_pwm_light(0, 0);
    sdvx_io_write_output();
    sdvx_io_read_input();

    sdvx_io_fini();

    return 0;
}
