// <<< Use Configuration Wizard in Context Menu >>>

/*=======================================================================*/
//!  Configure MAC address for BT/BLE, all 0s are ignored
/*=======================================================================*/
#define CONFIG_MAC_ADDRESS "00:00:00:00:00:00"

/******************************************************
 * *                      Macros
 * ******************************************************/

//! WLAN + BT Classic coex mode
#define RSI_OPERMODE_WLAN_BT_CLASSIC 5

/*=======================================================================*/
//! Discovery command parameters
/*=======================================================================*/

//! Discovery mode
//! Start the discoverable mode
#define START_DISCOVERY 0x01
//! Start the limited discoverable mode
#define START_LIMITED_DISCOVERY 0x02
//! Stop the discoverable mode
#define STOP_DISCOVERY 0x00

/*=======================================================================*/
//! Connectability command parameters
/*=======================================================================*/

//! Connectable mode
//! Start the connectable mode
#define CONNECTABLE 0x01
//! Stop the connectable mode
#define NON_CONNECTABLE 0x00

/*=======================================================================*/
//!  Authentication command parameters
/*=======================================================================*/

//! Pair mode
//! Enable authentication
//#define RSI_ENABLE                                  0x01
//! Enable authentication
//#define RSI_DISABLE                                 0x00

/*=======================================================================*/
//!  SSP Confirmation command parameters
/*=======================================================================*/

//! Confimation type
//! Accept confirmation
#define ACCEPT 0x01
//! Reject confirmation
#define REJECT 0x00

/*=======================================================================*/
//!  BT classic profile enable BITS
/*=======================================================================*/
#define RSI_SPP_PROFILE_BIT   BIT(0)
#define RSI_A2DP_PROFILE_BIT  BIT(1)
#define RSI_AVRCP_PROFILE_BIT BIT(2)
#define RSI_HFP_PROFILE_BIT   BIT(3)
#define RSI_PBAP_PROFILE_BIT  BIT(4)

#define BIN_FILE 1
//#define ARRAY       2
#define SD_BIN_FILE 3

#define PCM_AUDIO 1
#define SBC_AUDIO 2
#define MP3_AUDIO 3

//#define RSI_AUDIO_DATA_SRC ARRAY
#define RSI_AUDIO_DATA_TYPE PCM_AUDIO

/*=======================================================================*/
//!  SSP Confirmation command parameters
/*=======================================================================*/

//! A2DP Profile Enable
//#define A2DP_PROFILE_ENABLE
//! A2DP profile enable -> set the 23 bit in bt_feature_bit_map

/*=======================================================================*/

#define RSI_BT_MAX_PAYLOAD_SIZE 310 /* Max supported is 200, but 190 gives optimum Tx throughput */
