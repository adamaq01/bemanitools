avsdlls             += drshook

ldflags_drshook   := \
    -liphlpapi \
    -lsetupapi \
    -lcfgmgr32 \
    -lmf \
    -lmfplat \
    -lole32 \
    -lws2_32 \

deplibs_drshook   := \
    avs \

libs_drshook      := \
    acioemu \
    bio2emu \
    camhook \
    d3d9exhook \
    drsio \
    hook \
    hooklib \
    cconfig \
    util \
    eamio \

src_drshook       := \
    acio.c \
    bi2a.c \
    bind.c \
    dllmain.c \
    config-io.c \
