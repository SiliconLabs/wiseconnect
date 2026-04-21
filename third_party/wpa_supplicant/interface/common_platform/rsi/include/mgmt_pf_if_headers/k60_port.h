#ifndef _K60_PORTING_
#define _K60_PORTING_

#if K60_PORTING
/**
 * enum wps_event - WPS event types
 */
enum wps_event {
  /**
	 * WPS_EV_M2D - M2D received (Registrar did not know us)
	 */
  WPS_EV_M2D,

  /**
	 * WPS_EV_FAIL - Registration failed
	 */
  WPS_EV_FAIL,

  /**
	 * WPS_EV_SUCCESS - Registration succeeded
	 */
  WPS_EV_SUCCESS,

  /**
	 * WPS_EV_PWD_AUTH_FAIL - Password authentication failed
	 */
  WPS_EV_PWD_AUTH_FAIL,

  /**
	 * WPS_EV_PBC_OVERLAP - PBC session overlap detected
	 */
  WPS_EV_PBC_OVERLAP,

  /**
	 * WPS_EV_PBC_TIMEOUT - PBC walktime expired before protocol run start
	 */
  WPS_EV_PBC_TIMEOUT,
  /**
	 * WPS_EV_PBC_ACTIVE - PBC mode was activated
	 */
  WPS_EV_PBC_ACTIVE,

  /**
	 * WPS_EV_PBC_DISABLE - PBC mode was disabled
	 */
  WPS_EV_PBC_DISABLE,

  /**
	 * WPS_EV_ER_AP_ADD - ER: AP added
	 */
  WPS_EV_ER_AP_ADD,

  /**
	 * WPS_EV_ER_AP_REMOVE - ER: AP removed
	 */
  WPS_EV_ER_AP_REMOVE,

  /**
	 * WPS_EV_ER_ENROLLEE_ADD - ER: Enrollee added
	 */
  WPS_EV_ER_ENROLLEE_ADD,

  /**
	 * WPS_EV_ER_ENROLLEE_REMOVE - ER: Enrollee removed
	 */
  WPS_EV_ER_ENROLLEE_REMOVE,

  /**
	 * WPS_EV_ER_AP_SETTINGS - ER: AP Settings learned
	 */
  WPS_EV_ER_AP_SETTINGS,

  /**
	 * WPS_EV_ER_SET_SELECTED_REGISTRAR - ER: SetSelectedRegistrar event
	 */
  WPS_EV_ER_SET_SELECTED_REGISTRAR,
  /**
	 * WPS_EV_AP_PIN_SUCCESS - External Registrar used correct AP PIN
	 */
  WPS_EV_AP_PIN_SUCCESS
};

enum wpa_driver_if_type {
  /**
	 * WPA_IF_STATION - Station mode interface
	 */
  WPA_IF_STATION,

  /**
	 * WPA_IF_AP_VLAN - AP mode VLAN interface
	 *
	 * This interface shares its address and Beacon frame with the main
	 * BSS.
	 */
  WPA_IF_AP_VLAN,

  /**
	 * WPA_IF_AP_BSS - AP mode BSS interface
	 *
	 * This interface has its own address and Beacon frame.
	 */
  WPA_IF_AP_BSS,

  /**
	 * WPA_IF_P2P_GO - P2P Group Owner
	 */
  WPA_IF_P2P_GO,

  /**
	 * WPA_IF_P2P_CLIENT - P2P Client
	 */
  WPA_IF_P2P_CLIENT,

  /**
	 * WPA_IF_P2P_GROUP - P2P Group interface (will become either
	 * WPA_IF_P2P_GO or WPA_IF_P2P_CLIENT, but the role is not yet known)
	 */
  WPA_IF_P2P_GROUP,

  /**
	 * WPA_IF_P2P_DEVICE - P2P Device interface is used to indentify the
	 * abstracted P2P Device function in the driver
	 */
  WPA_IF_P2P_DEVICE

};

enum p2p_send_action_result {
  P2P_SEND_ACTION_SUCCESS /* Frame was send and acknowledged */,
  P2P_SEND_ACTION_NO_ACK /* Frame was sent, but not acknowledged */,
  P2P_SEND_ACTION_FAILED /* Frame was not sent due to a failure */
};

enum p2p_wps_method {
  WPS_NOT_READY,
  WPS_PIN_LABEL,
  WPS_PIN_DISPLAY,
  WPS_PIN_KEYPAD,
  WPS_PBC,
  WPS_NFC,
  WPS_P2PS

};

enum p2p_discovery_type { P2P_FIND_START_WITH_FULL, P2P_FIND_ONLY_SOCIAL, P2P_FIND_PROGRESSIVE };

#endif

#endif

/**
*$Log: k60_port.h,v $
*Revision 1.1.1.1  2012/02/28 07:14:12  kiran.gunda
*WiSe Connet initial import
*
**/
