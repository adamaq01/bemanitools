#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "bemanitools/glue.h"
#include "bemanitools/drsio.h"

#include "cconfig/cconfig-main.h"

#include "aciodrv/device.h"
#include "bio2drv/bi2a-drs.h"
#include "bio2drv/config-bio2.h"
#include "bio2drv/detect.h"

#define LOG_MODULE "drsio-bio2"

#define log_misc(...) drs_io_log_misc(LOG_MODULE, __VA_ARGS__)
#define log_info(...) drs_io_log_info(LOG_MODULE, __VA_ARGS__)
#define log_warning(...) drs_io_log_warning(LOG_MODULE, __VA_ARGS__)
#define log_fatal(...) drs_io_log_fatal(LOG_MODULE, __VA_ARGS__)

static log_formatter_t drs_io_log_misc;
static log_formatter_t drs_io_log_info;
static log_formatter_t drs_io_log_warning;
static log_formatter_t drs_io_log_fatal;

static uint32_t drs_io_buttons;

static char autodetect_buffer[512];

static atomic_bool running;
static atomic_bool processing_io;
static int16_t bio2_node_id;

uint8_t wing_staging[12];
struct bi2a_drs_state_out pout_staging;
struct bi2a_drs_state_out pout_ready;

static struct aciodrv_device_ctx *bio2_device_ctx;

void drs_io_set_loggers(
    log_formatter_t misc,
    log_formatter_t info,
    log_formatter_t warning,
    log_formatter_t fatal)
{
    drs_io_log_misc = misc;
    drs_io_log_info = info;
    drs_io_log_warning = warning;
    drs_io_log_fatal = fatal;

    bio2drv_set_loggers(misc, info, warning, fatal);
}

bool drs_io_init(
    thread_create_t thread_create,
    thread_join_t thread_join,
    thread_destroy_t thread_destroy)
{

    struct cconfig *config;
    struct bio2drv_config_bio2 config_bio2;

    config = cconfig_init();

    bio2drv_config_bio2_init(config);

    if (!cconfig_main_config_init(
            config,
            "--bio2-config",
            "drsio-bio2.conf",
            "--help",
            "-h",
            "drsio-bio2",
            CCONFIG_CMD_USAGE_OUT_STDOUT)) {
        cconfig_finit(config);
        exit(EXIT_FAILURE);
    }

    bio2drv_config_bio2_get(&config_bio2, config);

    cconfig_finit(config);

    const char *selected_port = config_bio2.port;

    if (config_bio2.autodetect) {
        log_info("Attempting autodetect");

        if (bio2drv_detect(
                DETECT_DEVICEID,
                0,
                autodetect_buffer,
                sizeof(autodetect_buffer))) {
            selected_port = autodetect_buffer;
        } else {
            log_info("Autodetect failed, falling back to using specified port");
        }
    }

    // BIO2's cannot share a bus with anything else, so use device directly
    bio2_device_ctx = aciodrv_device_open_path(selected_port, config_bio2.baud);

    if (bio2_device_ctx == NULL) {
        log_info("Opening BIO2 device on [%s] failed", selected_port);
        return 0;
    }

    log_info("Opening BIO2 device on [%s] successful", selected_port);

    uint8_t node_count = aciodrv_device_get_node_count(bio2_device_ctx);
    log_info("Enumerated %d nodes", node_count);

    bio2_node_id = -1;

    for (uint8_t i = 0; i < node_count; i++) {
        char product[4];
        aciodrv_device_get_node_product_ident(bio2_device_ctx, i, product);
        log_info(
            "> %d: %c%c%c%c\n",
            i,
            product[0],
            product[1],
            product[2],
            product[3]);

        if (!memcmp(product, "BI2A", 4)) {
            if (bio2_node_id != -1) {
                log_warning("Multiple BI2A found! Using highest node id.");
            }
            bio2_node_id = i;
        }
    }

    if (bio2_node_id != -1) {
        log_warning("Using BI2A on node: %d", bio2_node_id);

        if (!bio2drv_bi2a_drs_init(bio2_device_ctx, bio2_node_id)) {
            log_warning("Unable to start BI2A on node: %d", bio2_node_id);
            return false;
        }

        running = true;
        log_warning("drsio-bio2 now running");
    } else {
        log_warning("No BI2A device found");
    }

    return running;
}

void drs_io_fini(void)
{
    running = false;
    while (processing_io) {
        // avoid banging
        Sleep(1);
    }
}

static uint8_t shift_pin(uint16_t value, uint8_t pin)
{
    if (value) {
        return (1 << pin);
    }

    return 0;
}

bool _bio2_drs_io_poll(
    const struct bi2a_drs_state_out *pout, struct bi2a_drs_state_in *pin)
{
    if (!running) {
        return false;
    }

    processing_io = true;

    if (!bio2drv_bi2a_drs_poll(bio2_device_ctx, bio2_node_id, pout, pin)) {
        processing_io = false;
        return false;
    }

    processing_io = false;
    return true;
}

uint32_t drs_io_get_buttons(void)
{
    struct bi2a_drs_state_in pin;

    if (!_bio2_drs_io_poll(&pout_ready, &pin)) {
        return false;
    }

    drs_io_buttons = 0;

    drs_io_buttons |= shift_pin(pin.system.b_coin, DRS_IO_SYS_COIN);
    drs_io_buttons |= shift_pin(pin.system.b_service, DRS_IO_SYS_SERVICE);
    drs_io_buttons |= shift_pin(pin.system.b_test, DRS_IO_SYS_TEST);

    drs_io_buttons |= shift_pin(pin.player_1.b_right, DRS_IO_PANEL_P1_RIGHT);
    drs_io_buttons |= shift_pin(pin.player_1.b_left, DRS_IO_PANEL_P1_LEFT);
    drs_io_buttons |= shift_pin(pin.player_1.b_down, DRS_IO_PANEL_P1_DOWN);
    drs_io_buttons |= shift_pin(pin.player_1.b_up, DRS_IO_PANEL_P1_UP);
    drs_io_buttons |= shift_pin(pin.player_1.b_start, DRS_IO_PANEL_P1_START);

    drs_io_buttons |= shift_pin(pin.player_2.b_right, DRS_IO_PANEL_P2_RIGHT);
    drs_io_buttons |= shift_pin(pin.player_2.b_left, DRS_IO_PANEL_P2_LEFT);
    drs_io_buttons |= shift_pin(pin.player_2.b_down, DRS_IO_PANEL_P2_DOWN);
    drs_io_buttons |= shift_pin(pin.player_2.b_up, DRS_IO_PANEL_P2_UP);
    drs_io_buttons |= shift_pin(pin.player_2.b_start, DRS_IO_PANEL_P2_START);

    return drs_io_buttons;
}

void drs_io_set_button_lights(uint32_t lights)
{
}
