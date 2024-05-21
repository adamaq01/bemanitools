exes                  += vigem-drsio

deplibs_vigem-drsio  := \
    ViGEmClient \

cppflags_vigem-drsio  := \
    -I src/imports \

ldflags_vigem-drsio  := \
    -lsetupapi \

libs_vigem-drsio     := \
    cconfig \
    drsio \
    util \
    vigemstub \

src_vigem-drsio      := \
    main.c \
    config-vigem-drsio.c \
