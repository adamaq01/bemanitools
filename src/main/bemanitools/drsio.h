#ifndef BEMANITOOLS_DRSIO_H
#define BEMANITOOLS_DRSIO_H

/* IO emulation provider for DANCERUSH STARDOM. */

#include <stdbool.h>
#include <stdint.h>

#include "bemanitools/glue.h"

/* Bit mapping for the "buttons" word */

enum drs_io_sys_bit {
    DRS_IO_SYS_TEST = 0x00,
    DRS_IO_SYS_SERVICE = 0x01,
    DRS_IO_SYS_COIN = 0x02,
};

enum drs_io_panel_bit {
    DRS_IO_PANEL_P1_LEFT = 0x03,
    DRS_IO_PANEL_P1_RIGHT = 0x04,
    DRS_IO_PANEL_P1_UP = 0x05,
    DRS_IO_PANEL_P1_DOWN = 0x06,
    DRS_IO_PANEL_P1_START = 0x07,
    DRS_IO_PANEL_P2_LEFT = 0x08,
    DRS_IO_PANEL_P2_RIGHT = 0x09,
    DRS_IO_PANEL_P2_UP = 0x0A,
    DRS_IO_PANEL_P2_DOWN = 0x0B,
    DRS_IO_PANEL_P2_START = 0x0C,
};

/* The first function that will be called on your DLL. You will be supplied
   with four function pointers that may be used to log messages to the game's
   log file. See comments in glue.h for further information. */

void drs_io_set_loggers(
    log_formatter_t misc,
    log_formatter_t info,
    log_formatter_t warning,
    log_formatter_t fatal);

/* Initialize your DANCERUSH STARDOM IO emulation DLL. Thread management functions are
   provided to you; you must use these functions to create your own threads if
   you want to make use of the logging functions that are provided to
   eam_io_set_loggers(). You will also need to pass these thread management
   functions on to geninput if you intend to make use of that library.

   See glue.h and geninput.h for further details. */

bool drs_io_init(
    thread_create_t thread_create,
    thread_join_t thread_join,
    thread_destroy_t thread_destroy);

/* Shut down your DANCERUSH STARDOM IO emulation DLL */

void drs_io_fini(void);

/* Get the state of the buttons. See enums above. */

uint32_t drs_io_get_buttons(void);

/* Set the button light state. See enum drs_io_light_switch_lamp_bit above. */

void drs_io_set_button_lights(uint32_t lights);

#endif
