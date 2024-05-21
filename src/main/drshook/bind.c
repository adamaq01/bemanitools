#include <iphlpapi.h>
#include <wincrypt.h> /* Required by mingw for some reason */
#include <winsock2.h>
#include <windows.h>

#include <stdbool.h>
#include <string.h>

#include "hook/table.h"

#include "hooklib/adapter.h"

#include "util/codepage.h"
#include "util/defs.h"
#include "util/log.h"

static int WINAPI
my_bind(SOCKET s, const struct sockaddr *name, int namelen);

static int(WINAPI *real_bind)(
    SOCKET s, const struct sockaddr *name, int namelen);

static const struct hook_symbol bind_hook_syms[] = {
    {.name = "bind", .ordinal = 2, .patch = my_bind, .link = (void **) &real_bind},
};

static int WINAPI my_bind(SOCKET s, const struct sockaddr *name, int namelen)
{
    // cast to sockaddr_in
    struct sockaddr_in *in_name = (struct sockaddr_in *) name;

    log_warning("%s: bind: %s:%d", __FUNCTION__, inet_ntoa(in_name->sin_addr), ntohs(in_name->sin_port));
    // override bind to allow all hosts
    in_name->sin_addr.s_addr = inet_addr("0.0.0.0");

    // call original
    int ret = real_bind(s, name, namelen);
    if (ret != 0) {
        log_warning("%s: bind failed: %d", __FUNCTION__, WSAGetLastError());
    }

    // return result
    return ret;
}

void bind_hook_init(void)
{
    hook_table_apply(NULL, "ws2_32.dll", bind_hook_syms, lengthof(bind_hook_syms));
    HMODULE ess = GetModuleHandle("ess.dll");
    if (ess)
        hook_table_apply(ess, "ws2_32.dll", bind_hook_syms, lengthof(bind_hook_syms));
}
