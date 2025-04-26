#define LOG_MODULE "iidxio-lap"

#include <windows.h>

#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <stdio.h>

#include "bemanitools/iidxio.h"

#include "util/log.h"
#include "util/thread.h"
#include "util/time.h"

#include "iidxio-lap/iidxio_bio2.h"
#include "iidxio-lap/iidxio_con.h"

void iidx_io_set_loggers(
    log_formatter_t misc,
    log_formatter_t info,
    log_formatter_t warning,
    log_formatter_t fatal)
{
    bio2_iidx_io_set_loggers(misc, info, warning, fatal);
    con_iidx_io_set_loggers(misc, info, warning, fatal);
}

bool iidx_io_init(
    thread_create_t thread_create,
    thread_join_t thread_join,
    thread_destroy_t thread_destroy)
{
    log_info("Initializing iidxio-lap.dll...");

    if (!bio2_iidx_io_init(thread_create, thread_join, thread_destroy)) {
        log_fatal("Failed to initialize iidxio-lap.dll");
        return false;
    }
    log_info("iidxio-bio2 initialized");
    if (!con_iidx_io_init(thread_create, thread_join, thread_destroy)) {
        log_fatal("Failed to initialize iidxio-lap.dll");
        return false;
    }
    log_info("iidxio-con initialized");

    return true;
}

void iidx_io_fini(void)
{
    log_info("Shutting down iidxio-lap.dll...");

    bio2_iidx_io_fini();
    con_iidx_io_fini();

    log_info("iidxio-lap.dll shut down");
}

void iidx_io_ep1_set_deck_lights(uint16_t deck_lights)
{
    bio2_iidx_io_ep1_set_deck_lights(deck_lights);
    con_iidx_io_ep1_set_deck_lights(deck_lights);
}

void iidx_io_ep1_set_panel_lights(uint8_t panel_lights)
{
    bio2_iidx_io_ep1_set_panel_lights(panel_lights);
    con_iidx_io_ep1_set_panel_lights(panel_lights);
}

void iidx_io_ep1_set_top_lamps(uint8_t top_lamps)
{
    bio2_iidx_io_ep1_set_top_lamps(top_lamps);
    con_iidx_io_ep1_set_top_lamps(top_lamps);
}

void iidx_io_ep1_set_top_neons(bool top_neons)
{
    bio2_iidx_io_ep1_set_top_neons(top_neons);
    con_iidx_io_ep1_set_top_neons(top_neons);
}

bool iidx_io_ep1_send(void)
{
    if (!bio2_iidx_io_ep1_send()) {
        return false;
    }
    if (!con_iidx_io_ep1_send()) {
        return false;
    }
    return true;
}

bool iidx_io_ep2_recv(void)
{
    // TODO: Async?
    if (!bio2_iidx_io_ep2_recv()) {
        return false;
    }
    if (!con_iidx_io_ep2_recv()) {
        return false;
    }
    return true;
}

uint8_t iidx_io_ep2_get_turntable(uint8_t player_no)
{
    switch (player_no) {
        case 0:
            return con_iidx_io_ep2_get_turntable(0);
        case 1:
            return bio2_iidx_io_ep2_get_turntable(1);
        default:
            return 0;
    }
}

uint8_t iidx_io_ep2_get_slider(uint8_t slider_no)
{
    return bio2_iidx_io_ep2_get_slider(slider_no);
}

uint8_t iidx_io_ep2_get_sys(void)
{
    return bio2_iidx_io_ep2_get_sys() | con_iidx_io_ep2_get_sys();
}

uint8_t iidx_io_ep2_get_panel(void)
{
    return bio2_iidx_io_ep2_get_panel() | con_iidx_io_ep2_get_panel();
}

uint16_t iidx_io_ep2_get_keys(void)
{
    return bio2_iidx_io_ep2_get_keys() | con_iidx_io_ep2_get_keys();
}

bool iidx_io_ep3_write_16seg(const char *text)
{
    return bio2_iidx_io_ep3_write_16seg(text);
}