/**
 * @file spbtle.c
 *
 * @brief BLE default service initialization
 *
 * @author Renato Freitas <renato.freitas@thunderatz.org>
 * @author Daniel Nery <daniel.nery@thunderatz.org>
 *
 * @date 04/2019
 */
#include <stdbool.h>

#include "spbtle.h"

/*****************************************
 * Private Defines
 *****************************************/

/*
 * Bluetooth MAC Address: 00:80:E1:14:14:XX
 * 00:80:E1 identifies the manufacturer (STMicroelectronics)
 * 14:14 identifies ThundeRatz Robotics Team
 * XX assigned by user
 */
#define BLE_BDADDR(XX) \
    XX, 0x14, 0x14, 0xE1, 0x80, 0x00


/*****************************************
 * Private Variables
 *****************************************/

static char* ble_name;
static volatile uint32_t connected = false;
static volatile uint8_t set_connectable = 1;
static volatile uint16_t connection_handle = 0;
static volatile uint8_t notification_enabled = false;

static ble_service_t** _services;
static uint8_t _service_count;

/*****************************************
 * Private Function Prototypes
 *****************************************/

static void led_empty(void) {}
static led_function_t led_on = led_empty;
static led_function_t led_toggle = led_empty;

static void ble_set_connectable(void);
static void ble_connection_complete_cb(uint8_t addr[6], uint16_t handle);
static void ble_disconnection_complete_cb(void);
static void ble_event_notify(void* pData);


/*****************************************
 * Public Functions Bodies Definitions
 *****************************************/

tBleStatus spbtle_init(char* name, uint8_t bd_addr, ble_service_t* services[], uint8_t service_count) {
    uint8_t bdaddr[] = { BLE_BDADDR(bd_addr) };
    uint16_t service_handle, dev_name_char_handle, appearance_char_handle;

    uint8_t hwVersion;
    uint16_t fwVersion;
    tBleStatus ret;

    ble_name = name;
    _services = services;
    _service_count = service_count;

    hci_init(ble_event_notify, NULL);

    if ((ret = getBlueNRGVersion(&hwVersion, &fwVersion)) != BLE_STATUS_SUCCESS) {
        PRINTF("Get BNRG version failed \n");
        return ret;
    } else {
        PRINTF("HWver %d\nFWver %d\n", hwVersion, fwVersion);
    }

    /*
    * Reset BlueNRG again otherwise we won't
    * be able to change its MAC address.
    * aci_hal_write_config_data() must be the first
    * command after reset otherwise it will fail.
    */
    hci_reset();
    HAL_Delay(100);

    // Set BLE MAC address
    if ((ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN, bdaddr)) !=
        BLE_STATUS_SUCCESS) {
        PRINTF("Setting bdaddr failed.\n");
        return ret;
    } else {
        PRINTF("Setting bdaddr success.\n");
    }

    // GATT layer initialization
    if ((ret = aci_gatt_init()) != BLE_STATUS_SUCCESS) {
        PRINTF("GATT_Init failed.\n");
        return ret;
    } else {
        PRINTF("GATT_Init success.\n");
    }

    // Initializes BlueNRG device with default services
    if ((ret = aci_gap_init_IDB05A1(GAP_PERIPHERAL_ROLE_IDB05A1, 0, strlen(ble_name), &service_handle,
                                    &dev_name_char_handle, &appearance_char_handle)) != BLE_STATUS_SUCCESS) {
        PRINTF("GAP_Init failed.\n");
        return ret;
    } else {
        PRINTF("GAP_Init success.\n");
    }

    // Sets device name characteristic
    if ((ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0, strlen(ble_name), (uint8_t*) ble_name)) !=
        BLE_STATUS_SUCCESS) {
        PRINTF("aci_gatt_update_char_value failed.\n");
        return ret;
    } else {
        PRINTF("aci_gatt_update_char_value success.\n");
    }

    // Auth requirement for pairing, PIN = 123456
    if ((ret = aci_gap_set_auth_requirement(MITM_PROTECTION_REQUIRED, OOB_AUTH_DATA_ABSENT, NULL, 7, 16,
                                            USE_FIXED_PIN_FOR_PAIRING, 123456, BONDING)) == BLE_STATUS_SUCCESS) {
        PRINTF("BLE stack initialized!\n");
    } else {
        PRINTF("Error initializing :(\n");
        return ret;
    }

    // Add user defined services
    for (uint8_t i = 0; i < _service_count; i++) {
        ret = ble_add_service(_services[i]);
    }

    // Select transmiting power level
    ret = aci_hal_set_tx_power_level(1, 4);
    return ret;
}

void ble_leds(led_function_t on, led_function_t toggle) {
    led_on = on;
    led_toggle = toggle;
}

tBleStatus ble_add_service(ble_service_t* service) {
    tBleStatus ret;

    ret = aci_gatt_add_serv(UUID_TYPE_128, service->uuid, PRIMARY_SERVICE, 20, &(service->handle));
    if (ret != BLE_STATUS_SUCCESS) {
        PRINTF("Error adding new Service.\n");
        return ret;
    }

    for (uint8_t i = 0; i < service->characteristic_count; i++) {
        ret = aci_gatt_add_char(service->handle, UUID_TYPE_128, service->characteristics[i].uuid, 20,
                                service->characteristics[i].properties, ATTR_PERMISSION_NONE,
                                GATT_NOTIFY_ATTRIBUTE_WRITE, 16, 1, &(service->characteristics[i].handle));

        if (ret != BLE_STATUS_SUCCESS) {
            PRINTF("Error adding new Characteristic.\n");
            return ret;
        }
    }

    return ret;
}

tBleStatus ble_update_char_value(uint16_t service_handle, uint16_t char_handle, const void* value) {
    tBleStatus ret;

    ret = aci_gatt_update_char_value(service_handle, char_handle, 0, strlen(value), value);

    if (ret != BLE_STATUS_SUCCESS) {
        PRINTF("Failed to update characteristic value.\n");
        return ret;
    }

    return ret;
}

tBleStatus ble_add_char_descriptors(uint16_t service_handle, uint16_t char_handle, uint8_t count, char* values[]) {
    tBleStatus ret = BLE_STATUS_FAILED;

    uint8_t uuid[] = { 0x00, 0x00 };

    for (uint8_t i = 0; i < count; i++) {
        ret = aci_gatt_add_char_desc(service_handle, char_handle, UUID_TYPE_16, uuid, 20, strlen(values[i]),
                                     values[i], ATTR_PERMISSION_NONE, ATTR_ACCESS_READ_ONLY, 0, 16, 1, NULL);

        if (ret != BLE_STATUS_SUCCESS) {
            PRINTF("Failed to add characteristic descriptor\n");
            return ret;
        }
        uuid[0]++;
    }

    return ret;
}

void ble_process(void) {
    if (set_connectable) {
        ble_set_connectable();
        set_connectable = false;
    }

    if (connected) {
        led_on();
    } else {
        led_toggle();
    }

    hci_user_evt_proc();
}

/*****************************************
 * Private Functions Bodies Definitions
 *****************************************/

/**
 * @brief  Puts the device in connectable mode.
 *         If you want to specify a UUID list in the advertising data, those data can
 *         be specified as a parameter in aci_gap_set_discoverable().
 *         For manufacture data, aci_gap_update_adv_data must be called.
 * @param  None
 * @retval None
 */
static void ble_set_connectable(void) {
    tBleStatus ret;

    char local_name[256] = {AD_TYPE_COMPLETE_LOCAL_NAME};
    memcpy(local_name + 1, ble_name, strlen(ble_name));

    /* disable scan response */
    hci_le_set_scan_resp_data(0, NULL);
    PRINTF("General Discoverable Mode.\n");

    ret = aci_gap_set_discoverable(ADV_DATA_TYPE, ADV_INTERV_MIN, ADV_INTERV_MAX, PUBLIC_ADDR, NO_WHITE_LIST_USE,
                                   strlen(local_name), local_name, 0, NULL, 0, 0);
    if (ret != BLE_STATUS_SUCCESS) {
        PRINTF("Error while setting discoverable mode (%d)\n", ret);
    }
}

/**
 * @brief  This function is called when there is a LE Connection Complete event.
 * @param  uint8_t  Address of peer device
 * @param  uint16_t Connection handle
 * @retval None
 */
static void ble_connection_complete_cb(uint8_t addr[6], uint16_t handle) {
    connected = true;
    connection_handle = handle;

    PRINTF("Connected to device:");
    for (uint32_t i = 5; i > 0; i--) {
        PRINTF("%02X-", addr[i]);
    }
    PRINTF("%02X\n", addr[0]);
}

/**
 * @brief  This function is called when the peer device gets disconnected.
 * @param  None
 * @retval None
 */
static void ble_disconnection_complete_cb(void) {
    connected = false;
    PRINTF("Disconnected\n");
    /* Make the device connectable again. */
    set_connectable = true;
    notification_enabled = false;
}

/**
 * @brief  Callback processing the ACI events.
 * @note   Inside this function each event must be identified and correctly
 *         parsed.
 * @param  void* Pointer to the ACI packet
 * @retval None
 */
static void ble_event_notify(void* pData) {
    hci_uart_pckt* hci_pckt = pData;
    /* obtain event packet */
    hci_event_pckt* event_pckt = (hci_event_pckt*) hci_pckt->data;

    if (hci_pckt->type != HCI_EVENT_PKT)
        return;

    switch (event_pckt->evt) {
        case EVT_DISCONN_COMPLETE: {
            ble_disconnection_complete_cb();
        } break;

        case EVT_LE_META_EVENT: {
            evt_le_meta_event* evt = (void*) event_pckt->data;

            switch (evt->subevent) {
                case EVT_LE_CONN_COMPLETE: {
                    evt_le_connection_complete* cc = (void*) evt->data;
                    ble_connection_complete_cb(cc->peer_bdaddr, cc->handle);
                } break;
            }
        } break;

        case EVT_VENDOR: {
            evt_blue_aci* blue_evt = (void*) event_pckt->data;
            switch (blue_evt->ecode) {
                case EVT_BLUE_GATT_READ_PERMIT_REQ: {
                    // @TODO: ble_read_request_cb function
                    // evt_gatt_read_permit_req* pr = (void*) blue_evt->data;
                    // Read_Request_CB(pr->attr_handle);
                } break;

                case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED: {
                    evt_gatt_attr_modified_IDB05A1* evt = (evt_gatt_attr_modified_IDB05A1*) blue_evt->data;

                    for (uint8_t i = 0; i < _service_count; i++) {
                        for (uint8_t j = 0; j < _services[i]->characteristic_count; j++) {
                            if (evt->attr_handle == _services[i]->characteristics[j].handle + 1) {
                                if (_services[i]->characteristics[j].cb == NULL) {
                                    continue;
                                }
                                _services[i]->characteristics[j].cb(evt->data_length, evt->att_data);
                            }
                        }
                    }
                } break;
            }
        } break;
    }
}
