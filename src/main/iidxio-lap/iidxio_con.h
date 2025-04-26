#ifndef BEMANITOOLS_IIDXIO_CON_H
#define BEMANITOOLS_IIDXIO_CON_H

#include "bemanitools/iidxio.h"

void con_iidx_io_set_loggers(
    log_formatter_t misc,
    log_formatter_t info,
    log_formatter_t warning,
    log_formatter_t fatal);

bool con_iidx_io_init(
    thread_create_t thread_create,
    thread_join_t thread_join,
    thread_destroy_t thread_destroy);

void con_iidx_io_fini(void);

void con_iidx_io_ep1_set_deck_lights(uint16_t deck_lights);

void con_iidx_io_ep1_set_panel_lights(uint8_t panel_lights);

void con_iidx_io_ep1_set_top_lamps(uint8_t top_lamps);

void con_iidx_io_ep1_set_top_neons(bool top_neons);

bool con_iidx_io_ep1_send(void);

bool con_iidx_io_ep2_recv(void);

uint8_t con_iidx_io_ep2_get_turntable(uint8_t player_no);

uint8_t con_iidx_io_ep2_get_slider(uint8_t slider_no);

uint8_t con_iidx_io_ep2_get_sys(void);

uint8_t con_iidx_io_ep2_get_panel(void);

uint16_t con_iidx_io_ep2_get_keys(void);

bool con_iidx_io_ep3_write_16seg(const char *text);

#endif
