#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "bemanitools/input.h"
#include "bemanitools/drsio.h"

void drs_io_set_loggers(
    log_formatter_t misc,
    log_formatter_t info,
    log_formatter_t warning,
    log_formatter_t fatal)
{
    input_set_loggers(misc, info, warning, fatal);
}

bool drs_io_init(
    thread_create_t thread_create,
    thread_join_t thread_join,
    thread_destroy_t thread_destroy)
{
    input_init(thread_create, thread_join, thread_destroy);
    mapper_config_load("drs");

    return true;
}

void drs_io_fini(void)
{
    input_fini();
}

uint32_t drs_io_get_buttons(void)
{
    return (uint32_t) mapper_update();
}

void drs_io_set_button_lights(uint32_t lights)
{
}
