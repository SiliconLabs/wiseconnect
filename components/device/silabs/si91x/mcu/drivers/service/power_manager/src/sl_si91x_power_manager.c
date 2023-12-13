/*******************************************************************************
 * @file sl_si91x_power_manager.c
 * @brief Power Manager Service API implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "sli_si91x_power_manager.h"

/*******************************************************************************
 ***************************  DEFINES / MACROS   ********************************
 ******************************************************************************/
#define POWER_STATE_TABLE_SIZE 5 // Power State Requirement Table size
#define PS_MIN_COUNTER         0 // Minimum Power State count
#define PS_MAX_COUNTER         5 // Maximum Power State count

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static sl_power_state_t current_state                          = SL_SI91X_POWER_MANAGER_PS4;
static boolean_t is_initialized                                = false;
static sl_slist_node_t *power_manager_ps_transition_event_list = NULL;

// Table of power state counters. Each counter indicates the presence (not zero)
// or absence (zero) of requirements on a given power state.
static uint8_t requirement_ps_table[POWER_STATE_TABLE_SIZE] = {
  0, // SL_SI91X_POWER_MANAGER_PS4 requirement counter
  0, // SL_SI91X_POWER_MANAGER_PS3 requirement counter
  0, // SL_SI91X_POWER_MANAGER_PS2 requirement counter
  0, // SL_SI91X_POWER_MANAGER_PS1 requirement counter
  0, // SL_SI91X_POWER_MANAGER_PS0 requirement counter
};
/*******************************************************************************
 ***************************  Local Types  ********************************
 ******************************************************************************/

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
static sl_status_t update_ps_requirements(sl_power_state_t state, boolean_t add);
static void notify_power_state_transition(sl_power_state_t from, sl_power_state_t to);

/*******************************************************************************
***********************  Global function Definitions *************************
 ******************************************************************************/

/*******************************************************************************
 * Initializes the Power Manager Service.
 * If service is not already initialized, then initialize the linked list and
 * clears the requirement table.
 * It also initializes the hardware: Sets the system clock to 100 MHz and changes the 
 * state to PS4.
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_init(void)
{
  if (!is_initialized) {
    // If power manager is not initialized, resets the linked list
    // and requirement table.
    sl_slist_init(&power_manager_ps_transition_event_list);
    memset(requirement_ps_table, 0, sizeof(requirement_ps_table));
    // Configures the clock to 100 MHz
    sli_si91x_power_manager_init_hardware();
    is_initialized = true;
    return SL_STATUS_OK;
  }
  return SL_STATUS_ALREADY_INITIALIZED;
}

/*******************************************************************************
 * Adds requirement on power states and changes the power state.
 * Validates the transition and returns error code if invalid transition.
 * Calls the update_ps_requirement static function which updates the requirement
 * table, changes the power state and notify the power state transition.
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_add_ps_requirement(sl_power_state_t state)
{
  if (!is_initialized) {
    // Validate the status of power manager service, if not initialized
    // returns error code.
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (state >= LAST_ENUM_POWER_STATE) {
    // Validate the power state, if not in range returns error code.
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (!sli_si91x_power_manager_is_valid_transition(current_state, state)) {
    // Validates the transition, if incorrect returns error code.
    return SL_STATUS_INVALID_STATE;
  }
  // returns the error code of state transition, if conditions are fulfilled,
  // updated the current power state.
  return update_ps_requirements(state, true);
}

/*******************************************************************************
 * Removes requirement on power states.
 * Calls the update_ps_requirement static function which updates the requirement
 * table.
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_remove_ps_requirement(sl_power_state_t state)
{
  if (!is_initialized) {
    // Validate the status of power manager service, if not initialized
    // returns error code.
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (state >= LAST_ENUM_POWER_STATE) {
    // Validate the power state, if not in range returns error code.
    return SL_STATUS_INVALID_PARAMETER;
  }
  // returns the error code of state transition, if conditions are fulfilled,
  // updated the current power state.
  return update_ps_requirements(state, false);
}

/*******************************************************************************
 * Adds the peripheral requirement.
 * Calls the internal peripheral update function which validates the peripheral
 * and enables them.
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_add_peripheral_requirement(sl_power_peripheral_t *peripheral)
{
  if (!is_initialized) {
    // Validate the status of power manager service, if not initialized
    // returns error code.
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (peripheral == NULL) {
    // Validates the peripheral, if null pointer, returns error code.
    return SL_STATUS_NULL_POINTER;
  }
  // returns the error code for peripheral update, if conditions are fulfilled,
  // updated the peripheral requirements.
  return sli_power_manager_update_peripheral(peripheral, true);
}

/*******************************************************************************
 * Removes the peripheral requirement.
 * Calls the internal peripheral update function which validates the peripheral
 * and disables them.
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_remove_peripheral_requirement(sl_power_peripheral_t *peripheral)
{
  if (!is_initialized) {
    // Validate the status of power manager service, if not initialized
    // returns error code.
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (peripheral == NULL) {
    // Validates the peripheral, if null pointer, returns error code.
    return SL_STATUS_NULL_POINTER;
  }
  // returns the error code for peripheral update, if conditions are fulfilled,
  // updated the peripheral requirements.
  return sli_power_manager_update_peripheral(peripheral, false);
}

/*******************************************************************************
 * Registers a callback to be called on given Power state transition(s).
 * New node is added to the linked list for handling the state transition callbacks.
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_subscribe_ps_transition_event(
  sl_power_manager_ps_transition_event_handle_t *event_handle,
  const sl_power_manager_ps_transition_event_info_t *event_info)
{
  if ((event_handle) == NULL || (event_info == NULL)) {
    // Validates the null pointer, if incorrect returns the error code.
    return SL_STATUS_NULL_POINTER;
  }
  if (!is_initialized) {
    // Validate the status of power manager service, if not initialized
    // returns error code.
    return SL_STATUS_NOT_INITIALIZED;
  }
  event_handle->info = (sl_power_manager_ps_transition_event_info_t *)event_info;
  // Push the data into the linked list.
  sl_slist_push(&power_manager_ps_transition_event_list, &event_handle->node);
  // If it reaches here, then returns SL_STATUS_OK
  return SL_STATUS_OK;
}

/*******************************************************************************
 * Un-registers a callback to be called on given Power state transition(s).
 * Existing node is deleted from the linked list for handling the state transition callbacks.
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_unsubscribe_ps_transition_event(
  sl_power_manager_ps_transition_event_handle_t *event_handle,
  const sl_power_manager_ps_transition_event_info_t *event_info)
{
  if ((event_handle) == NULL || (event_info == NULL)) {
    // Validates the null pointer, if incorrect returns the error code.
    return SL_STATUS_NULL_POINTER;
  }
  if (!is_initialized) {
    // Validate the status of power manager service, if not initialized
    // returns error code.
    return SL_STATUS_NOT_INITIALIZED;
  }
  // Pops out the data from the linked list.
  sl_slist_remove(&power_manager_ps_transition_event_list, &event_handle->node);
  // If it reaches here, then returns SL_STATUS_OK
  return SL_STATUS_OK;
}

/*******************************************************************************
 * Transit to sleep mode and waits for the peripheral to configured a wakeup source
 * to wakeup the m4 soc.
 * If the sleep mode transition is not successful, then it returns error code.
 * Configuration of sleep parameters are handled in the internal function.
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_sleep(void)
{
  sl_status_t status;
  if (!sli_si91x_power_manager_is_valid_transition(current_state, SL_SI91X_POWER_MANAGER_SLEEP)) {
    // Validates the state transition for sleep, if invalid returns error code.
    return SL_STATUS_INVALID_STATE;
  }
  if (!is_initialized) {
    // Validate the status of power manager service, if not initialized
    // returns error code.
    return SL_STATUS_NOT_INITIALIZED;
  }
  // Internal function to change active mode to sleep mode is called.
  // It sets the required configurations and goes into sleep mode.
  status = sli_si91x_power_manager_set_sleep_configuration(current_state);
  if (status != SL_STATUS_OK) {
    return status;
  }
  // Notifies the state transition who has subscribed to it.
  notify_power_state_transition(SL_SI91X_POWER_MANAGER_SLEEP, current_state);

  // After wakeup, clock is set to the particular PS4/PS3/PS2 mode.
  status = sl_si91x_power_manager_set_clock_scaling(SL_SI91X_POWER_MANAGER_POWERSAVE);
  if (status != SL_STATUS_OK) {
    return status;
  }
  // If it reaches here, then returns SL_STATUS_OK
  return SL_STATUS_OK;
}

/*******************************************************************************
 * Transit to standby state and will wait for the interrupt.
 * WFI instruction is called to move the soc power mode to standby mode.
 ******************************************************************************/
void sl_si91x_power_manager_standby(void)
{
  if (sli_si91x_power_manager_is_valid_transition(current_state, SL_SI91X_POWER_MANAGER_STANDBY)) {
    // Validates the state transition for sleep, if valid it transits to standby mode.
    __WFI();
    notify_power_state_transition(SL_SI91X_POWER_MANAGER_STANDBY, current_state);
  }
}

/*******************************************************************************
 * Configures the wakeup sources.
 * Configuration of wakeup source is handled in the internal function.
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_set_wakeup_sources(uint32_t source, boolean_t add)
{
  if (!is_initialized) {
    // Validate the status of power manager service, if not initialized
    // returns error code.
    return SL_STATUS_NOT_INITIALIZED;
  }

  return sli_si91x_power_configure_wakeup_resource(source, add);
}

/*******************************************************************************
 * Retains the RAM in low power state either by using size or RAM bank as input parameter.
 * All the RAM retention configuration parameters are handled in the internal function
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_configure_ram_retention(sl_power_ram_retention_config_t *config)
{
  if (!is_initialized) {
    // Validate the status of power manager service, if not initialized
    // returns error code.
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (config == NULL) {
    // Validates the config, if null pointer, returns error code.
    return SL_STATUS_NULL_POINTER;
  }
  return sli_si91x_power_manager_set_ram_retention_configuration(config);
}

/*******************************************************************************
 * Configures the clock scaling.
 * According to the mode, clock scaling is configured.
 * Internal function handles the clock configuration as per the paramater passed
 * in it.
 ******************************************************************************/
sl_status_t sl_si91x_power_manager_set_clock_scaling(sl_clock_scaling_t mode)
{
  if (!is_initialized) {
    // Validate the status of power manager service, if not initialized
    // returns error code.
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (mode >= LAST_ENUM_CLOCK_SCALING) {
    // Validates the clock scaling enum, if not in range returns error code.
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (mode == SL_SI91X_POWER_MANAGER_POWERSAVE) {
    // For powersave current state with false flag is passed as parameter
    // to the internal function.
    return sli_si91x_power_manager_configure_clock(current_state, false);
  }
  if (mode == SL_SI91X_POWER_MANAGER_PERFORMANCE) {
    // For performance current state with true flag is passed as parameter
    // to the internal function.
    return sli_si91x_power_manager_configure_clock(current_state, true);
  }
  // If it reaches here, then the entered mode is invalid.
  return SL_STATUS_INVALID_PARAMETER;
}

/*******************************************************************************
 * Returns the current state of SoC which is stored in static variable.
 ******************************************************************************/
sl_power_state_t sl_si91x_power_manager_get_current_state(void)
{
  return current_state;
}

/*******************************************************************************
 * Returns the pointer to the array of requirement table.
 ******************************************************************************/
uint8_t *sl_si91x_power_manager_get_requirement_table(void)
{
  return requirement_ps_table;
}

/*******************************************************************************
 * De-initialize the power manager service.
 * It clears the requirement table and the is_initialized flag.
 ******************************************************************************/
void sl_si91x_power_manager_deinit(void)
{
  // Clears the requirement table and disable the initialization flag.
  memset(requirement_ps_table, 0, sizeof(requirement_ps_table));
  is_initialized = false;
}

/*******************************************************************************
 * Updates the power state requirement, requirement table and current state variable.
 * 
 * @param[in] state Power state requirement that 
 * @param[in] add  Flag indicating if requirement is added (true) or removed
 *                 (false).
 ******************************************************************************/
static sl_status_t update_ps_requirements(sl_power_state_t state, boolean_t add)
{
  if ((requirement_ps_table[state] == PS_MIN_COUNTER) && !add) {
    // If requirement is to remove when it 0, i.e., user tries to
    // make the requirement less than 0 (wrap around not allowed), returns error code.
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((requirement_ps_table[state] == UINT8_MAX) && add) {
    // If requirement is to add when it is 255, i.e., user tries to
    // make the requirement more than 255 (wrap around not allowed), returns error code.
    return SL_STATUS_INVALID_PARAMETER;
  }
  // Updates the requirement table.
  requirement_ps_table[state] += (uint8_t)((add) ? 1 : -1);
  if (add) {
    // Only add requirement effects the state transition.

    // It updates the power state using internal api.
    sli_si91x_power_manager_change_power_state(current_state, state);

    if (current_state == SL_SI91X_POWER_MANAGER_PS2 && state == SL_SI91X_POWER_MANAGER_PS1) {
      // If transition is to PS1 state, its requirement needs to clear after wakeup.
      // It goes to sleep with retention with valid ULP peripheral based wakeup source.
      // Notifies the state transition who has subscribed to it.
      notify_power_state_transition(SL_SI91X_POWER_MANAGER_PS1, SL_SI91X_POWER_MANAGER_PS2);
      // Clears the PS1 requirement
      requirement_ps_table[SL_SI91X_POWER_MANAGER_PS1] -= 1;
      return SL_STATUS_OK;
    }

    // Notifies the state transition who has subscribed to it.
    notify_power_state_transition(current_state, state);
    // Updates the current state variable.
    current_state = state;
    // If it reaches here, then returns SL_STATUS_OK
  }
  return SL_STATUS_OK;
}

/*******************************************************************************
 * Notifies the power state transition to the event subscribers.
 * It creates a uint32_t variable named transition which consists the ored value of
 * the states, "From" and "To" transitions.
 * It searches the linked list and finds the subscribers who has subscribed for that
 * event. 
 * It masks the transition variable with the subscribers event mask and if it matches,
 * the callback function is called.
 ******************************************************************************/
static void notify_power_state_transition(sl_power_state_t from, sl_power_state_t to)
{
  sl_power_manager_ps_transition_event_handle_t *handle;
  sl_power_manager_ps_transition_event_t transition = 0;

  switch (to) {
    // Updates the transition variable with "To" which state the transition happened.
    case SL_SI91X_POWER_MANAGER_PS4:
      // Updates the transition variable with entering PS4 state flag.
      transition = SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS4;
      break;
    case SL_SI91X_POWER_MANAGER_PS3:
      // Updates the transition variable with entering PS3 state flag.
      transition = SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS3;
      break;
    case SL_SI91X_POWER_MANAGER_PS2:
      // Updates the transition variable with entering PS2 state flag.
      transition = SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS2;
      break;
    default:
      break;
  }

  switch (from) {
    // Updates the transition variable with "From" which state the transition happened.
    case SL_SI91X_POWER_MANAGER_PS4:
      // Updates the transition variable with leaving PS4 state flag.
      transition |= SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS4;
      break;
    case SL_SI91X_POWER_MANAGER_PS3:
      // Updates the transition variable with leaving PS3 state flag.
      transition |= SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS3;
      break;
    case SL_SI91X_POWER_MANAGER_PS2:
      // Updates the transition variable with leaving PS2 state flag.
      transition |= SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS2;
      break;
    case SL_SI91X_POWER_MANAGER_PS1:
      // Updates the transition variable with leaving PS1 state flag.
      transition |= SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS1;
      break;
    case SL_SI91X_POWER_MANAGER_SLEEP:
      // Updates the transition variable with leaving Sleep state flag.
      transition |= SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_SLEEP;
      break;
    case SL_SI91X_POWER_MANAGER_STANDBY:
      // Updates the transition variable with leaving Standby state flag.
      transition |= SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_STANDBY;
      break;
    default:
      break;
  }
  // For loop to search each entry of linked list for the notification of transition
  SL_SLIST_FOR_EACH_ENTRY(power_manager_ps_transition_event_list,
                          handle,
                          sl_power_manager_ps_transition_event_handle_t,
                          node)
  {
    if ((handle->info->event_mask & transition) > 0) {
      // Verifies if the subscription and event are same, if same callback function is called.
      handle->info->on_event(from, to);
    }
  }
}
