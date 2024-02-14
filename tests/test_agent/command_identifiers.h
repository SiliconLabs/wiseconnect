// enumerations for protocol_type
typedef enum protocol_type {
  WIFI = 1,
  BLE,
  AP,
  BT,
} protocol_type_t;

typedef enum sl_ble_event {
  ADVERTISE = 1,
  SCAN,
  BLE_CONNECT,
  DISCONNECT,
  GATT_PROFILES,
  GATT_PROFILE,
  GET_ATT_VALUE,
  GET_ATT_DESC,
} sl_ble_event_t;

typedef enum sl_async_wifi_event {
  SOCKET_SELELECT = 20,
} sl_wifi_async_event_t;

#define START_IDENTIFIER                   "<$"
#define END_IDENTIFIER                     "$>"
#define PRINT_EVENT_START(event, protocol) printf(START_IDENTIFIER " %08lx ", (uint32_t)(event | (protocol << 28)));
#define PRINT_EVENT_END()                  printf(END_IDENTIFIER)
