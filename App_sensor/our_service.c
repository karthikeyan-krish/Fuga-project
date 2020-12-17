

#include <stdint.h>
#include <string.h>
#include "nrf_gpio.h"
#include "our_service.h"
//#include "sdk_common.h"
#include "ble_srv_common.h"
#include "app_error.h"


/**
 * @brief Declaration of a function that will take care of some housekeeping of ble connections related to our service and characteristic
 */
void ble_our_service_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
  	ble_os_t * p_our_service =(ble_os_t *) p_context;  
		// Implement switch case handling BLE events related to our service. 
	switch (p_ble_evt->header.evt_id)
        {
            case BLE_GAP_EVT_CONNECTED:
                p_our_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
                break;
            case BLE_GAP_EVT_DISCONNECTED:
                p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;
                break;
            default:
                // No implementation needed.
                break;
        }
	
}

static uint32_t our_char_add(ble_os_t * p_our_service)
{
  // Add a custom characteristic UUID
   uint32_t            err_code;
   ble_uuid_t          char_uuid;
   ble_uuid128_t       base_uuid = BLE_UUID_OUR_BASE_UUID;
   char_uuid.uuid      = BLE_UUID_ACCEL_CHARACTERISTIC_UUID;
   err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
   APP_ERROR_CHECK(err_code);

  // Add read/write properties to our characteristic
   ble_gatts_char_md_t char_md;
   memset(&char_md, 0, sizeof(char_md));
   char_md.char_props.read = 1;
   char_md.char_props.write = 1;

   // Configuring Client Characteristic Configuration Descriptor metadata and add to char_md structure
   ble_gatts_attr_md_t cccd_md;
   memset(&cccd_md, 0, sizeof(cccd_md));
   BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
   BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
   cccd_md.vloc                = BLE_GATTS_VLOC_STACK;
   char_md.p_cccd_md           = &cccd_md;
   char_md.char_props.notify   = 1;

    // Configure the attribute metadata
   ble_gatts_attr_md_t attr_md;
   memset(&attr_md, 0, sizeof(attr_md));
   attr_md.vloc        = BLE_GATTS_VLOC_STACK;

     // Set read/write security levels to our characteristic
   BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
   BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

   // Configure the characteristic value attribute
   ble_gatts_attr_t    attr_char_value;
   memset(&attr_char_value, 0, sizeof(attr_char_value));
   attr_char_value.p_uuid      = &char_uuid;
   attr_char_value.p_attr_md   = &attr_md;

   // Set characteristic length in number of bytes
   attr_char_value.max_len     = 6;
   attr_char_value.init_len    = 6;
   uint8_t value[6]            = {0x00,0x00,0x00,0x00,0x00,0x00};
   attr_char_value.p_value     = value;

   // Add our new characteristic to the service
   err_code = sd_ble_gatts_characteristic_add(p_our_service->service_handle,
                                    &char_md,
                                    &attr_char_value,
                                    &p_our_service->char_accel_handles);
   APP_ERROR_CHECK(err_code);

   return NRF_SUCCESS;

}

void our_service_init(ble_os_t * p_our_service)
{
  uint32_t   err_code;

  // Declare 16-bit service and 128-bit base UUIDs and add them to the BLE stack
  ble_uuid_t        service_uuid;
  ble_uuid128_t     base_uuid = BLE_UUID_OUR_BASE_UUID;
  service_uuid.uuid = BLE_UUID_OUR_SERVICE_UUID;
  err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
  APP_ERROR_CHECK(err_code);

  // Set our service connection handle to default value i.e. an invalid handle since we are not yet in a connection.
  p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;

  // Add our service
  err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                      &service_uuid,
                                      &p_our_service->service_handle);

  APP_ERROR_CHECK(err_code);

  // Add characteristic to the service.
  our_char_add(p_our_service);
  
}

void characteristic_accel_update(ble_os_t *p_our_service, int16_t *value)
{
  if (p_our_service->conn_handle != BLE_CONN_HANDLE_INVALID)
  {
    uint16_t               len = 6;
    ble_gatts_hvx_params_t hvx_params;
    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = p_our_service->char_accel_handles.value_handle;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
    hvx_params.offset = 0;
    hvx_params.p_len  = &len;
    hvx_params.p_data = (uint8_t *)value;

    sd_ble_gatts_hvx(p_our_service->conn_handle, &hvx_params);
  }
}