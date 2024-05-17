#ifndef BIO2DRV_BI2A_DRS_H
#define BIO2DRV_BI2A_DRS_H

#include "aciodrv/device.h"

#include "bio2/bi2a-drs.h"

/**
 * Initialize a BI2A node.
 *
 * @param device Context of opened device
 * @param node_id Id of the node to initialize (0 based).
 * @return True if successful, false on error.
 * @note This module is supposed to be used in combination with the common
 *       device driver foundation.
 * @see driver.h
 */
bool bio2drv_bi2a_drs_init(struct aciodrv_device_ctx *device, uint8_t node_id);

/**
 * Poll the BI2A board
 *
 * @param device Context of opened device
 * @param node_id Id of the node to query (0 based).
 * @param state Pointer to a state struct to return the current state to
 *        (optional, NULL for none).
 * @return True on success, false on error.
 * @note This module is supposed to be used in combination with the common
 *       device driver foundation.
 * @see driver.h
 */
bool bio2drv_bi2a_drs_poll(
    struct aciodrv_device_ctx *device,
    uint8_t node_id,
    const struct bi2a_drs_state_out *pout,
    struct bi2a_drs_state_in *pin);

#endif
