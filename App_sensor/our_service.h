
#ifndef OUR_SERVICE_H__
#define OUR_SERVICE_H__

#include <stdint.h>
#include "ble.h"
//#include "sdk_common.h"
#include "ble_srv_common.h"


//#define BLE_NUS_MAX_DATA_LEN (GATT_MTU_SIZE_DEFAULT - 3) /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */

// Defining 128-bit base UUID
#define BLE_UUID_OUR_BASE_UUID                 {{0x23, 0xD1, 0x13, 0xEF, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}} // 128-bit base UUID

//Defining 16-bit service UUID
#define BLE_UUID_OUR_SERVICE_UUID                0xF00D // Unique Service UUID

// Defining 16-bit characteristic UUID
#define BLE_UUID_ACCEL_CHARACTERISTIC_UUID          0xBEEF  // Unique Charecteristic UUID


// This structure contains various status information for our service. 
// The name is based on the naming convention used in Nordics SDKs. 
// 'ble� indicates that it is a Bluetooth Low Energy relevant structure and 
// �os� is short for Our Service).
typedef struct
{
    uint16_t                    conn_handle;    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).*/
    uint16_t                    service_handle; /**< Handle of Our Service (as provided by the BLE stack). */
    // Add handles for the characteristic attributes to our struct
    ble_gatts_char_handles_t    char_accel_handles;
}ble_os_t;

/**@brief Function for handling BLE Stack events related to our service and characteristic.
 *
 * @details Handles all events from the BLE stack of interest to Our Service.
 *
 * @param[in]   p_our_service       Our Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_our_service_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

/**@brief Function for initializing our new service.
 *
 * @param[in]   p_our_service       Pointer to Our Service structure.
 */
void our_service_init(ble_os_t * p_our_service);

/**
 * @brief Function to update Accelerometer characterisstic.
 */
void characteristic_accel_update(ble_os_t *p_our_service, int16_t *value);

#endif