#include "cconfig/cconfig-main.h"
#include "cconfig/cconfig-util.h"

#include "vigem-drsio/config-vigem-drsio.h"

#include "util/log.h"

static void vigem_drsio_config_init(struct cconfig *config)
{
}

static void vigem_drsio_config_get(
    struct vigem_drsio_config *vigem_config, struct cconfig *config)
{
}

bool get_vigem_drsio_config(struct vigem_drsio_config *config_out)
{
    struct cconfig *config;

    config = cconfig_init();

    vigem_drsio_config_init(config);

    if (!cconfig_main_config_init(
            config,
            "--config",
            "vigem-drsio.conf",
            "--help",
            "-h",
            "vigem-drsio",
            CCONFIG_CMD_USAGE_OUT_STDOUT)) {
        cconfig_finit(config);
        return false;
    }

    vigem_drsio_config_get(config_out, config);

    cconfig_finit(config);
    return true;
}
