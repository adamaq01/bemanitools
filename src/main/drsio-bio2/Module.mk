dlls += drsio-bio2

ldflags_drsio-bio2   := \
    -lsetupapi \

libs_drsio-bio2 := \
    aciodrv \
    bio2drv \
    cconfig \
    util \

src_drsio-bio2	:= \
    drsio.c \

