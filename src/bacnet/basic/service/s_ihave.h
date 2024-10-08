/**
 * @file
 * @author Steve Karg
 * @date October 2019
 * @brief Header file for a basic I-Have service send
 *
 * @section LICENSE
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef SEND_I_HAVE_H
#define SEND_I_HAVE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>
/* BACnet Stack defines - first */
#include "bacnet/bacdef.h"
/* BACnet Stack API */
#include "bacnet/bacapp.h"
#include "bacnet/apdu.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

BACNET_STACK_EXPORT
void Send_I_Have(
    uint32_t device_id,
    BACNET_OBJECT_TYPE object_type,
    uint32_t object_instance,
    const BACNET_CHARACTER_STRING *object_name);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
