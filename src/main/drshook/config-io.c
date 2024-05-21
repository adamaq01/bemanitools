#include "cconfig/cconfig-util.h"

#include "drshook/config-io.h"

#include "util/log.h"

#define DRSHOOK_CONFIG_IO_DISABLE_CARD_READER_EMU_KEY \
    "io.disable_card_reader_emu"
#define DRSHOOK_CONFIG_IO_DISABLE_BIO2_EMU_KEY "io.disable_bio2_emu"
#define DRSHOOK_CONFIG_IO_DISABLE_POLL_LIMITER_KEY "io.disable_poll_limiter"
#define DRSHOOK_CONFIG_IO_DISABLE_FILE_HOOKS_KEY "io.disable_file_hooks"

#define DRSHOOK_CONFIG_IO_DEFAULT_DISABLE_CARD_READER_EMU_VALUE false
#define DRSHOOK_CONFIG_IO_DEFAULT_DISABLE_BIO2_EMU_VALUE false
#define DRSHOOK_CONFIG_IO_DEFAULT_DISABLE_POLL_LIMITER_VALUE false
#define DRSHOOK_CONFIG_IO_DEFAULT_DISABLE_FILE_HOOKS_VALUE false

void drshook_config_io_init(struct cconfig *config)
{
    cconfig_util_set_bool(
        config,
        DRSHOOK_CONFIG_IO_DISABLE_CARD_READER_EMU_KEY,
        DRSHOOK_CONFIG_IO_DEFAULT_DISABLE_CARD_READER_EMU_VALUE,
        "Disable card reader emulation and enable usage of real card reader "
        "hardware on COM1");

    cconfig_util_set_bool(
        config,
        DRSHOOK_CONFIG_IO_DISABLE_BIO2_EMU_KEY,
        DRSHOOK_CONFIG_IO_DEFAULT_DISABLE_BIO2_EMU_VALUE,
        "Disable BIO2 emulation and enable usage of real BIO2 hardware");

    cconfig_util_set_bool(
        config,
        DRSHOOK_CONFIG_IO_DISABLE_POLL_LIMITER_KEY,
        DRSHOOK_CONFIG_IO_DEFAULT_DISABLE_POLL_LIMITER_VALUE,
        "Disables the poll limiter, warning very high CPU usage may arise");

    cconfig_util_set_bool(
        config,
        DRSHOOK_CONFIG_IO_DISABLE_FILE_HOOKS_KEY,
        DRSHOOK_CONFIG_IO_DEFAULT_DISABLE_FILE_HOOKS_VALUE,
        "Disables the built in file hooks, requiring manual file creation "
        "(/dev/raw/j.dest)");
}

void drshook_config_io_get(
    struct drshook_config_io *config_io, struct cconfig *config)
{
    if (!cconfig_util_get_bool(
            config,
            DRSHOOK_CONFIG_IO_DISABLE_CARD_READER_EMU_KEY,
            &config_io->disable_card_reader_emu,
            DRSHOOK_CONFIG_IO_DEFAULT_DISABLE_CARD_READER_EMU_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            DRSHOOK_CONFIG_IO_DISABLE_CARD_READER_EMU_KEY,
            DRSHOOK_CONFIG_IO_DEFAULT_DISABLE_CARD_READER_EMU_VALUE);
    }

    if (!cconfig_util_get_bool(
            config,
            DRSHOOK_CONFIG_IO_DISABLE_BIO2_EMU_KEY,
            &config_io->disable_bio2_emu,
            DRSHOOK_CONFIG_IO_DEFAULT_DISABLE_BIO2_EMU_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            DRSHOOK_CONFIG_IO_DISABLE_BIO2_EMU_KEY,
            DRSHOOK_CONFIG_IO_DEFAULT_DISABLE_BIO2_EMU_VALUE);
    }

    if (!cconfig_util_get_bool(
            config,
            DRSHOOK_CONFIG_IO_DISABLE_POLL_LIMITER_KEY,
            &config_io->disable_poll_limiter,
            DRSHOOK_CONFIG_IO_DEFAULT_DISABLE_POLL_LIMITER_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            DRSHOOK_CONFIG_IO_DISABLE_POLL_LIMITER_KEY,
            DRSHOOK_CONFIG_IO_DEFAULT_DISABLE_POLL_LIMITER_VALUE);
    }

    if (!cconfig_util_get_bool(
            config,
            DRSHOOK_CONFIG_IO_DISABLE_FILE_HOOKS_KEY,
            &config_io->disable_file_hooks,
            DRSHOOK_CONFIG_IO_DEFAULT_DISABLE_FILE_HOOKS_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            DRSHOOK_CONFIG_IO_DISABLE_FILE_HOOKS_KEY,
            DRSHOOK_CONFIG_IO_DEFAULT_DISABLE_FILE_HOOKS_VALUE);
    }
}
