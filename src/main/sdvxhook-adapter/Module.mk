avsdlls             += sdvxhook-adapter

ldflags_sdvxhook-adapter   := \
    -liphlpapi \
    -lsetupapi \
    -lcfgmgr32 \
    -lmf \
    -lmfplat \
    -lole32 \

deplibs_sdvxhook-adapter   := \
    avs \

libs_sdvxhook-adapter      := \
    hook \
    hooklib \
    cconfig \
    util \

src_sdvxhook-adapter       := \
    dllmain.c \
