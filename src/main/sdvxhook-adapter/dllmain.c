#include <windows.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cconfig/cconfig-hook.h"

#include "hooklib/acp.h"
#include "hooklib/adapter.h"
#include "hooklib/app.h"
#include "hooklib/config-adapter.h"

#include "imports/avs.h"

#include "util/log.h"
#include "util/str.h"
#include "util/thread.h"

#define SDVXHOOK_ADAPTER_INFO_HEADER \
    "sdvxhook-adapter"         \
    ", build " __DATE__ " " __TIME__ ", gitrev " STRINGIFY(GITREV) "\n"
#define SDVXHOOK_ADAPTER_CMD_USAGE \
    "Usage: launcher.exe -K sdvxhook_adapter.dll <soundvoltex.dll> [options...]"

struct hooklib_config_adapter config_adapter;

static bool my_dll_entry_init(char *sidcode, struct property_node *param)
{
    struct cconfig *config;

    log_info("--- Begin sdvxhook-adapter dll_entry_init ---");

    config = cconfig_init();

    hooklib_config_adapter_init(config);

    if (!cconfig_hook_config_init(
            config,
            SDVXHOOK_ADAPTER_INFO_HEADER "\n" SDVXHOOK_ADAPTER_CMD_USAGE,
            CCONFIG_CMD_USAGE_OUT_STDOUT)) {
        cconfig_finit(config);
        exit(EXIT_FAILURE);
    }

    hooklib_config_adapter_get(&config_adapter, config);

    cconfig_finit(config);

    log_info(SDVXHOOK_ADAPTER_INFO_HEADER);
    log_info("Initializing sdvxhook-adapter...");

    adapter_hook_override(config_adapter.override_ip);

    log_info("--- End sdvxhook-adapter dll_entry_init ---");

    return app_hook_invoke_init(sidcode, param);
}

static bool my_dll_entry_main(void)
{
    bool result;

    result = app_hook_invoke_main();

    return result;
}

/**
 * Hook library SDVX5+
 */
BOOL WINAPI DllMain(HMODULE mod, DWORD reason, void *ctx)
{
    if (reason != DLL_PROCESS_ATTACH) {
        goto end;
    }

    log_to_external(
        log_body_misc, log_body_info, log_body_warning, log_body_fatal);

    app_hook_init(my_dll_entry_init, my_dll_entry_main);

    acp_hook_init();
    adapter_hook_init();

end:
    return TRUE;
}
