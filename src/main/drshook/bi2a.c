#define LOG_MODULE "drshook-bi2a"

#include <windows.h> /* for _BitScanForward */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "acioemu/emu.h"
#include "bio2emu/emu.h"
#include "drshook/bi2a.h"

#include "bemanitools/drsio.h"

static void bio2_emu_bi2a_cmd_send_version(
    struct ac_io_emu *emu, const struct ac_io_message *req);
static void bio2_emu_bi2a_send_state(
    struct ac_io_emu *emu, const struct ac_io_message *req);
static void bio2_emu_bi2a_send_empty(
    struct ac_io_emu *emu, const struct ac_io_message *req);
static void bio2_emu_bi2a_send_status(
    struct ac_io_emu *emu, const struct ac_io_message *req, uint8_t status);

static bool poll_delay;

void bio2_emu_bi2a_init(
    struct bio2emu_port *bio2_emu,
    bool disable_poll_limiter)
{
    bio2emu_port_init(bio2_emu);

    poll_delay = !disable_poll_limiter;

    if (!poll_delay) {
        log_warning("bio2_emu_bi2a_init: poll_delay has been disabled");
    }
}

void bio2_emu_bi2a_dispatch_request(
    struct bio2emu_port *bio2port, const struct ac_io_message *req)
{
    uint16_t cmd_code;

    cmd_code = ac_io_u16(req->cmd.code);

    switch (cmd_code) {
        case AC_IO_CMD_GET_VERSION:
            log_misc("BIO2_CMD_GET_VERSION(%d)", req->addr);
            bio2_emu_bi2a_cmd_send_version(&bio2port->acio, req);
            break;

        case AC_IO_CMD_CLEAR:
            log_misc("BIO2_BI2A_CMD_CLEAR(%d)", req->addr);
            bio2_emu_bi2a_send_status(&bio2port->acio, req, 0x00);
            break;

        case AC_IO_CMD_START_UP:
            log_misc("BIO2_BI2A_CMD_START_UP(%d)", req->addr);
            bio2_emu_bi2a_send_status(&bio2port->acio, req, 0x00);
            break;

        case AC_IO_CMD_KEEPALIVE:
            log_misc("BIO2_BI2A_CMD_KEEPALIVE(%d)", req->addr);
            bio2_emu_bi2a_send_empty(&bio2port->acio, req);
            break;

        case AC_IO_CMD_GET_STATE:
            log_misc("BIO2_BI2A_CMD_GET_STATE(%d)", req->addr);
            bio2_emu_bi2a_send_state(&bio2port->acio, req);
            break;

        case AC_IO_CMD_TAPELED_INIT:
            log_misc("BIO2_BI2A_CMD_TAPELED_INIT(%d)", req->addr);
            bio2_emu_bi2a_send_empty(&bio2port->acio, req);
            break;

        case AC_IO_CMD_TAPELED_SEND:
            log_misc("BIO2_BI2A_CMD_TAPELED_SEND(%d)", req->addr);
            bio2_emu_bi2a_send_empty(&bio2port->acio, req);
            break;

        default:
            log_warning(
                "Unknown BIO2 message %04x on BI2A node, addr=%d",
                cmd_code,
                req->addr);
            bio2_emu_bi2a_send_empty(&bio2port->acio, req);
            break;
    }
}

static void bio2_emu_bi2a_cmd_send_version(
    struct ac_io_emu *emu, const struct ac_io_message *req)
{
    struct ac_io_message resp;

    resp.addr = req->addr | AC_IO_RESPONSE_FLAG;
    resp.cmd.code = req->cmd.code;
    resp.cmd.seq_no = req->cmd.seq_no;
    resp.cmd.nbytes = sizeof(resp.cmd.version);
    resp.cmd.version.type = ac_io_u32(AC_IO_NODE_TYPE_BI2A);
    resp.cmd.version.flag = 0x00;
    resp.cmd.version.major = 0x01;
    resp.cmd.version.minor = 0x02;
    resp.cmd.version.revision = 0x09;
    memcpy(
        resp.cmd.version.product_code,
        "BI2A",
        sizeof(resp.cmd.version.product_code));
    strncpy(resp.cmd.version.date, __DATE__, sizeof(resp.cmd.version.date));
    strncpy(resp.cmd.version.time, __TIME__, sizeof(resp.cmd.version.time));

    ac_io_emu_response_push(emu, &resp, 0);
}

static void
bio2_emu_bi2a_send_empty(struct ac_io_emu *emu, const struct ac_io_message *req)
{
    struct ac_io_message resp;

    resp.addr = req->addr | AC_IO_RESPONSE_FLAG;
    resp.cmd.code = req->cmd.code;
    resp.cmd.seq_no = req->cmd.seq_no;
    resp.cmd.nbytes = 0;

    ac_io_emu_response_push(emu, &resp, 0);
}

static void bio2_emu_bi2a_send_status(
    struct ac_io_emu *emu, const struct ac_io_message *req, uint8_t status)
{
    struct ac_io_message resp;

    resp.addr = req->addr | AC_IO_RESPONSE_FLAG;
    resp.cmd.code = req->cmd.code;
    resp.cmd.seq_no = req->cmd.seq_no;
    resp.cmd.nbytes = sizeof(resp.cmd.status);
    resp.cmd.status = status;

    ac_io_emu_response_push(emu, &resp, 0);
}

static uint8_t check_pin(uint16_t value, uint8_t pin)
{
    return (value >> pin) & 1;
}

static void
bio2_emu_bi2a_send_state(struct ac_io_emu *emu, const struct ac_io_message *req)
{
    struct ac_io_message resp;
    struct bi2a_drs_state_in *pin;
    struct bi2a_drs_state_out *pout;

    resp.addr = req->addr | AC_IO_RESPONSE_FLAG;
    resp.cmd.code = req->cmd.code;
    resp.cmd.seq_no = req->cmd.seq_no;
    resp.cmd.nbytes = sizeof(*pin);

    pout = (struct bi2a_drs_state_out *) &req->cmd.raw;
    pin = (struct bi2a_drs_state_in *) &resp.cmd.raw;
    memset(pin, 0, sizeof(*pin));

    uint32_t buttons = drs_io_get_buttons();

    pin->system.b_coin = check_pin(buttons, DRS_IO_SYS_COIN);
    pin->system.b_service = check_pin(buttons, DRS_IO_SYS_SERVICE);
    pin->system.b_test = check_pin(buttons, DRS_IO_SYS_TEST);

    pin->player_1.b_start = check_pin(buttons, DRS_IO_PANEL_P1_START);
    pin->player_1.b_up = check_pin(buttons, DRS_IO_PANEL_P1_UP);
    pin->player_1.b_down = check_pin(buttons, DRS_IO_PANEL_P1_DOWN);
    pin->player_1.b_left = check_pin(buttons, DRS_IO_PANEL_P1_LEFT);
    pin->player_1.b_right = check_pin(buttons, DRS_IO_PANEL_P1_RIGHT);

    pin->player_2.b_start = check_pin(buttons, DRS_IO_PANEL_P2_START);
    pin->player_2.b_up = check_pin(buttons, DRS_IO_PANEL_P2_UP);
    pin->player_2.b_down = check_pin(buttons, DRS_IO_PANEL_P2_DOWN);
    pin->player_2.b_left = check_pin(buttons, DRS_IO_PANEL_P2_LEFT);
    pin->player_2.b_right = check_pin(buttons, DRS_IO_PANEL_P2_RIGHT);

    ac_io_emu_response_push(emu, &resp, 0);
}
