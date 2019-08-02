/**
 * @file spbtle.h
 *
 * @brief BLE default service initialization
 *
 * @author Renato Freitas <renato.freitas@thunderatz.org>
 * @author Daniel Nery <daniel.nery@thunderatz.org>
 *
 * @date 04/2019
 */

#ifndef _USER_SERVICE_H_
#define _USER_SERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include "ble_types.h"

#include "hci_const.h"
#include "bluenrg_aci_const.h"
#include "bluenrg_gap.h"
#include "bluenrg_gap_aci.h"
#include "bluenrg_gatt_aci.h"
#include "bluenrg_hal_aci.h"
#include "hci.h"
#include "hci_le.h"
#include "hci_tl.h"
#include "bluenrg_utils.h"
#include "sm.h"
#include "string.h"

/*****************************************
 * Public Function Prototypes
 *****************************************/

/**
 * @brief   Initialize SPBTLE device.
 * @param   name            Device name.
 * @param   bd_addr         Device BD_ADDR.
 * @param   services        List of services to be added.
 * @param   service_count   Number of services to be added.
 * @retval  Value indicating success or error code.
 */
tBleStatus spbtle_init(char* name, uint8_t bd_addr, ble_service_t* services[], uint8_t service_count);

/**
 * @brief   Add GATT service
 * @param   service     Packed service structure containing characteristics
 * @retval  Value indicating success or error code.
 */
tBleStatus ble_add_service(ble_service_t* service);

/**
 * @brief   Update GATT characteristic value.
 * @param   service_handle  GATT service handle.
 * @param   char_handle     GATT characteristic handle.
 * @param   value           Value to be written.
 * @retval  Value indicating success or error code.
 */
tBleStatus ble_update_char_value(uint16_t service_handle, uint16_t char_handle, const void* value);

/**
 * @brief   Add GATT characteristic descriptors.
 * @note    Descriptors UUID will be sequential, starting with 0x0000
 * @param   service_handle  GATT service handle.
 * @param   char_handle     GATT characteristic handle.
 * @param   count           Number of descriptors to be added.
 * @param   values          Descriptor values array.
 * @retval  Value indicating success or error code.
 */
tBleStatus ble_add_char_descriptors(uint16_t service_handle, uint16_t char_handle, uint8_t count, char* values[]);

/**
 * @brief   Sets LED parameters to indicate connection status
 *          ON if connected, blinks otherwise.
 * @param   on      Turn on LED.
 * @param   toggle  Toggle LED.
 * @retval  None
 */
void ble_leds(led_function_t on, led_function_t toggle);

/**
 * @brief   BlueNRG-MS background task.
 *          Needs to be running constantly to manage HCI requests.
 * @param   None
 * @retval  None
 */
void ble_process(void);


#ifdef __cplusplus
}
#endif
#endif /* _USER_SERVICE_H_ */
