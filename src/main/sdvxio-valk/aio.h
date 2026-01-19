#pragma once

#include <stdint.h>

struct AIO_NODE;
struct AIO_NMGR;
struct AIO_SCI;

struct AIO_IOB2_BI2X_UFC__DEVSTATUS {
    uint8_t unk_0[18];
    uint8_t test;
    uint8_t service;
    uint8_t coin;
    uint8_t recorder;
    uint8_t headphone;
    uint8_t unk_1[312 - 18 - 5];
    uint16_t analog_left;
    uint16_t analog_right;
    uint8_t buttons;
    uint8_t unk_2[414 - 312 - 5];
};

_Static_assert(
    sizeof(struct AIO_IOB2_BI2X_UFC__DEVSTATUS) == 414,
    "bi2x_device_status is the wrong size");

typedef void (*t_aioNodeCtl_UpdateDevicesStatus)(void);
typedef unsigned int (*t_aioNodeCtl_Destroy)(struct AIO_NODE *node);
typedef unsigned int (*t_aioNodeMgr_Destroy)(struct AIO_NMGR *mgr);
typedef unsigned int (*t_aioSci_Destroy)(struct AIO_SCI *sci);

typedef struct AIO_SCI *(*t_aioIob2Bi2x_OpenSciUsbCdc)(uint8_t unk);
typedef struct AIO_NMGR *(*t_aioNMgrIob2_Create)(
    struct AIO_SCI *port, unsigned int delay);

typedef struct AIO_NODE *(*t_aioIob2Bi2xUFC_Create)(
    struct AIO_NMGR *node_mgr, uint8_t param);
typedef void (*t_aioIob2Bi2xUFC_IoReset)(
    struct AIO_NODE *i_pNodeCtl, int value);
typedef void (*t_aioIob2Bi2xUFC_GetDeviceStatus)(
    struct AIO_NODE *i_pNodeCtl,
    struct AIO_IOB2_BI2X_UFC__DEVSTATUS *status,
    int size);
typedef void (*t_aioIob2Bi2xUFC_SetTapeLedData)(
    struct AIO_NODE *i_pNodeCtl, unsigned int index, uint8_t *data);

// 0 = ON, 1 = OFF
typedef void (*t_aioIob2Bi2xUFC_SetPlayerButtonLamp)(
    struct AIO_NODE *i_pNodeCtl, unsigned int index, uint8_t state);
