#ifndef DRSHOOK_CONFIG_IO_H
#define DRSHOOK_CONFIG_IO_H

#include <windows.h>

#include "cconfig/cconfig.h"

struct drshook_config_io {
    bool disable_card_reader_emu;
    bool disable_bio2_emu;
    bool disable_poll_limiter;
    bool disable_file_hooks;
};

void drshook_config_io_init(struct cconfig *config);

void drshook_config_io_get(
    struct drshook_config_io *config_io, struct cconfig *config);

#endif