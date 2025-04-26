dlls                += iidxio-lap

ldflags_iidxio-lap      := \
    -lsetupapi \
    -lwinmm \

libs_iidxio-lap         := \
    aciodrv \
    bio2drv \
    cconfig \
    util \
    geninput \
    vefxio \

src_iidxio-lap          := \
    iidxio_bio2.c \
    iidxio_con.c \
    iidxio.c \
