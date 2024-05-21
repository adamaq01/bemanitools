#ifndef VIGEM_DRSIO_CONFIG_H
#define VIGEM_DRSIO_CONFIG_H

#include <windows.h>

#include "cconfig/cconfig.h"

struct vigem_drsio_config {
};

bool get_vigem_drsio_config(struct vigem_drsio_config *config_out);

#endif