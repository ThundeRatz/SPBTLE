/**
 * @file ble_types.h
 *
 * @brief Custom BLE data types definitions
 *
 * @author Renato Freitas <renato.freitas@thunderatz.org>
 * @author Daniel Nery <daniel.nery@thunderatz.org>
 *
 * @date 04/2019
 */

#ifndef __BLE_TYPES_H__
#define __BLE_TYPES_H__

#include <stdint.h>

typedef struct ble_characteristic ble_characteristic_t;

typedef void (*characteristic_change_cb_t)(uint8_t data_len, uint8_t* data);

typedef void (*led_function_t)(void);

struct __attribute__((packed)) ble_characteristic {
    uint8_t uuid[16]; /**< BLE characteristic UUID */
    uint16_t handle;  /**< BLE characteristic handle */
    uint8_t properties; /**< BLE characteristic UUID */
    characteristic_change_cb_t cb; /**< BLE characteristic callback function */
};

typedef struct __attribute__((packed)) ble_service {
    uint8_t uuid[16]; /**< BLE service UUID */
    uint16_t handle; /**< BLE service UUID */
    uint8_t characteristic_count; /**< Number of service's characteristics  */
    ble_characteristic_t characteristics[]; /**< Service's characteristics */
} ble_service_t;

#endif  // __BLE_TYPES_H__
