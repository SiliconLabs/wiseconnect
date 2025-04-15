#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "bg_bit.h"
#include "bgapi.h"
#include "bgapi_rtos_adaptation.h"
#include "bg_compat.h" // for WEAK
#include "bg_config.h"
#include "sl_bgapi.h"

#define BG_ASSERT(...)

// Initialize BGAPI tracing
// WEAK void sli_bgapi_trace_init(void)
// {
//   // This weak stub is overridden by the BGAPI trace component when it's
//   // included in the application build.
// }

// Output a BGAPI message to the trace channel
//WEAK void sli_bgapi_trace_output_message(sli_bgapi_trace_message_type_t type,
//                                         uint32_t header,
//                                         const void *data)
//{
//  // This weak stub is overridden by the BGAPI trace component when it's
//  // included in the application build.
//  (void) type;
//  (void) header;
//  (void) data;
//}

struct sl_bgapi_packet *sli_bgapi_cmd_msg_buf =
  NULL; // buffer for storing current command, used by host and native API.
struct sl_bgapi_packet *sli_bgapi_rsp_msg_buf =
  NULL; // buffer for storing current response, used for host and native API.

static struct bgapi_device *registered_bgapi_devices = NULL;

static inline struct bgapi_device *get_bgapi_device(enum sl_bgapi_dev_types dev_type)
{
  struct bgapi_device *d = registered_bgapi_devices;

  while (d && d->dev_type != dev_type) {
    d = d->next;
  }

  return d;
}

static const struct bgapi_lib *get_bgapi_library(uint32_t cmdheader)
{
  // Get the device
  uint8_t dev_type       = SL_BGAPI_MSG_DEVICE_TYPE(cmdheader);
  struct bgapi_device *d = get_bgapi_device((enum sl_bgapi_dev_types)dev_type);
  if (!d) {
    return NULL;
  }

  // Use the right library table depending on the state of the device
  const struct bgapi_lib *const *ptr = (d->flags & sli_bgapi_device_flag_started) ? d->lib_table : d->lib_table_stopped;

  // Get the library
  uint8_t cls_name = SLI_BGAPI_MSG_CLASS_ID(cmdheader);
  while (*ptr) {
    const struct bgapi_lib *lib = *ptr;
    if (lib->cls_name == cls_name) {
      return lib;
    }
    ptr++;
  }

  return NULL;
}

/**
 * Finds a BGAPI command message from given lookup tables.
 */
static sl_bgapi_handler find_command_impl(uint8_t command_id, const struct bgapi_lib *lib)
{
  const struct bgapi_cmd *msg = lib ? lib->cmd_table : 0;
  for (; msg && msg->handler; msg++) {
    if (msg->command_id == command_id) {
      return msg->handler;
    }
  }

  return NULL;
}

// Initialize command, response and event buffers for API handling
void bgapi_buffer_init(void *(*alloc)(size_t size))
{
  // Allocate buffers to fit the header and the maximum possible payload
  size_t bgapi_cmdbuf_len = SL_BGAPI_MSG_HEADER_LEN + SL_BGAPI_MAX_PAYLOAD_SIZE;
  sli_bgapi_cmd_msg_buf   = (struct sl_bgapi_packet *)alloc(bgapi_cmdbuf_len);
  sli_bgapi_rsp_msg_buf   = (struct sl_bgapi_packet *)alloc(bgapi_cmdbuf_len);
}

// Initialize BGAPI internal state
void bgapi_init(void)
{
  // sli_bgapi_trace_init();
  registered_bgapi_devices = NULL;
  bgapi_buffer_init(malloc);
}

// Initialize command, response and event buffers for API handling

/**
 * @brief Count the number of libraries in the library table
 *
 * @param[in] lib_table Library table to count the libraries in
 *
 * @return The number of libraries in the table
 */
static uint32_t count_bgapi_libraries(const struct bgapi_lib *const *lib_table)
{
  uint32_t num_libraries = 0;
  if (lib_table) {
    while (lib_table[num_libraries] != NULL) {
      num_libraries++;
    }
  }

  return num_libraries;
}

/**
 * @brief De-initialize the specified number of BGAPI libraries
 *
 * @param[in] lib_table Library table
 * @param[in] num_libraries Number of libraries to de-initialize
 */
static void deinit_bgapi_libraries(const struct bgapi_lib *const *lib_table, uint32_t num_libraries)
{
  // De-initialize in the opposite order of initialization, i.e. higher index
  // first
  while (num_libraries > 0) {
    num_libraries--;
    const struct bgapi_lib *lib = lib_table[num_libraries];

    // Call the de-init function if the lib has it
    if (lib->deinit_func) {
      lib->deinit_func();
    }
  }
}

// Register a BGAPI device
void bgapi_register_device(struct bgapi_device *bgapi_device)
{
  // Find the end of the list and check for existing registration
  struct bgapi_device **p_next = &registered_bgapi_devices;
  while (*p_next != NULL) {
    struct bgapi_device *device = *p_next;
    if (device->dev_type == bgapi_device->dev_type) {
      // Already registered
      return;
    }

    p_next = &device->next;
  }

  // Link the new device to the end of the list
  bgapi_device->flags |= (uint8_t)sli_bgapi_device_flag_registered;
  bgapi_device->next = NULL;
  *p_next            = bgapi_device;
}

// Initialize a BGAPI device
sl_status_t bgapi_init_device(struct bgapi_device *bgapi_device)
{
  // If the device has not yet registered, register it first
  if ((bgapi_device->flags & sli_bgapi_device_flag_registered) == 0) {
    bgapi_register_device(bgapi_device);
  }

  // We're now committed to starting, so the device enters the "started" state
  // before the calls to the init functions.
  uint8_t stop_flags = (uint8_t)sli_bgapi_device_flag_stopping_ongoing | (uint8_t)sli_bgapi_device_flag_stopping_ready;
  bgapi_device->flags &= (uint8_t)~stop_flags;
  bgapi_device->flags |= (uint8_t)sli_bgapi_device_flag_started;

  // Initialize the libraries
  uint32_t i                               = 0;
  const struct bgapi_lib *const *lib_table = bgapi_device->lib_table;
  while (lib_table[i] != NULL) {
    const struct bgapi_lib *lib = lib_table[i++];

    // Call the init function if the lib has it
    if (lib->init_func) {
      sl_status_t status = lib->init_func();
      if (status != SL_STATUS_OK) {
        // If failed, de-initialize this lib and any that we already
        // successfully initialized
        deinit_bgapi_libraries(lib_table, i);
        bgapi_device->flags &= (uint8_t)~sli_bgapi_device_flag_started;
        return status;
      }
    }
  }

  return SL_STATUS_OK;
}

// Gracefully stop a BGAPI device
void bgapi_stop_device(struct bgapi_device *bgapi_device)
{
  // Set the flags to indicate that stopping is on-going
  uint8_t flags_to_clear = (uint8_t)sli_bgapi_device_flag_started | (uint8_t)sli_bgapi_device_flag_stopping_ready;
  bgapi_device->flags &= (uint8_t)~flags_to_clear;
  bgapi_device->flags |= (uint8_t)sli_bgapi_device_flag_stopping_ongoing;

  // The count of asynchronous stop operations may toggle between zero and
  // non-zero during the de-init if some asynchronous operations finish already
  // during the de-init sequence. To avoid prematurely setting the
  // "stopping_ready" flag, we count one asynchronous stop over the duration of
  // the library stop loop.
  BG_ASSERT(bgapi_device->num_async_lib_stops_ongoing == 0);
  bgapi_device->num_async_lib_stops_ongoing = 1;

  // Stop in the opposite order of initialization, i.e. higher index first
  const struct bgapi_lib *const *lib_table = bgapi_device->lib_table;
  uint32_t num_libraries                   = count_bgapi_libraries(lib_table);
  while (num_libraries > 0) {
    num_libraries--;
    const struct bgapi_lib *lib = lib_table[num_libraries];
    if (lib->stop_func) {
      lib->stop_func();
    }
  }

  // The BGAPI stop sequence we started above is now ready, so mark it finished.
  // Individual BGAPI libraries may still have asynchronous stopping operations
  // on-going.
  sli_bgapi_lib_stop_finished(bgapi_device);
}

// Increase the count of asynchronous stopping operations
void sli_bgapi_lib_stop_ongoing(struct bgapi_device *bgapi_device)
{
  BG_ASSERT(bgapi_device->num_async_lib_stops_ongoing < UINT16_MAX);
  bgapi_device->num_async_lib_stops_ongoing++;
}

// Decrease the count of asynchronous stopping operations
void sli_bgapi_lib_stop_finished(struct bgapi_device *bgapi_device)
{
  // Stopping is ready when the last asynchronous stop has finished
  BG_ASSERT(bgapi_device->num_async_lib_stops_ongoing > 0);
  if (bgapi_device->num_async_lib_stops_ongoing > 0) {
    bgapi_device->num_async_lib_stops_ongoing--;
  }
  if (bgapi_device->num_async_lib_stops_ongoing == 0) {
    // Stopping is now finished
    bgapi_device->flags &= (uint8_t)~sli_bgapi_device_flag_stopping_ongoing;
    bgapi_device->flags |= (uint8_t)sli_bgapi_device_flag_stopping_ready;
  }
}

// De-initialize a BGAPI device
void bgapi_deinit_device(struct bgapi_device *bgapi_device)
{
  // De-init the libraries
  uint32_t num_libraries = count_bgapi_libraries(bgapi_device->lib_table);
  deinit_bgapi_libraries(bgapi_device->lib_table, num_libraries);

  // The device is now completely de-initialized. Clear all relevant state
  // flags.
  bgapi_device->num_async_lib_stops_ongoing = 0;
  uint8_t flags_to_clear = (uint8_t)sli_bgapi_device_flag_started | (uint8_t)sli_bgapi_device_flag_stopping_ongoing
                           | (uint8_t)sli_bgapi_device_flag_stopping_ready;
  bgapi_device->flags &= (uint8_t)~flags_to_clear;
}

// Return true if the specified BGAPI device is started
bool sli_bgapi_device_is_started(const struct bgapi_device *bgapi_device)
{
  return ((bgapi_device->flags & sli_bgapi_device_flag_started) != 0);
}

// Return true if the specified BGAPI device has finished stopping
bool sli_bgapi_device_has_finished_stopping(const struct bgapi_device *bgapi_device)
{
  return ((bgapi_device->flags & sli_bgapi_device_flag_stopping_ready) != 0);
}

// Return true if the specified BGAPI device is stopped
bool sli_bgapi_device_is_stopped(const struct bgapi_device *bgapi_device)
{
  uint8_t active_flags = (uint8_t)sli_bgapi_device_flag_started | (uint8_t)sli_bgapi_device_flag_stopping_ongoing
                         | (uint8_t)sli_bgapi_device_flag_stopping_ready;
  return ((bgapi_device->flags & active_flags) == 0);
}

void sli_bgapi_cmd_handler_delegate(uint32_t header, sl_bgapi_handler handler, const void *data)
{
  // Trace the command that was issued by the application
  // sli_bgapi_trace_output_message(sli_bgapi_trace_message_type_command, header, data);

  // Make sure the command buffer has the header
  sli_bgapi_cmd_msg_buf->header = header;

  // Clear the response header to allow us to detect cases when the handler
  // returns without having set a response.
  sli_bgapi_rsp_msg_buf->header = 0;

  sl_status_t status          = SL_STATUS_FAIL;
  const struct bgapi_lib *lib = get_bgapi_library(header);
  uint8_t command_id          = SLI_BGAPI_MSG_COMMAND_ID(header);
  if (lib && !handler) {
    handler = find_command_impl(command_id, lib);
  }
  if (!lib || !handler) {
    status = SL_STATUS_NOT_SUPPORTED;
  } else {
    handler(data);

    // If the handler failed to set a response, set a generic failure
    if (sli_bgapi_rsp_msg_buf->header == 0) {
      status = SL_STATUS_FAIL;
    } else {
      status = SL_STATUS_OK;
    }
  }

  // If we failed to execute the handler, set a generic error response
  if (status != SL_STATUS_OK) {
    gecko_rsp_error(SL_BGAPI_MSG_DEVICE_TYPE(header),
                    SLI_BGAPI_MSG_CLASS_ID(header),
                    SLI_BGAPI_MSG_COMMAND_ID(header),
                    status);
  }

  // Trace the response that we're giving to the application
  // sli_bgapi_trace_output_message(sli_bgapi_trace_message_type_response,
  //                                sli_bgapi_rsp_msg_buf->header,
  //                                sli_bgapi_rsp_msg_buf->data.payload);
}

void (*sli_bgapi_cmd_handler_delegate_function)(uint32_t,
                                                sl_bgapi_handler,
                                                const void *) = sli_bgapi_cmd_handler_delegate;

void sli_bgapi_set_cmd_handler_delegate(void (*cmd_handler_delegate)(uint32_t, sl_bgapi_handler, const void *))
{
  sli_bgapi_cmd_handler_delegate_function = cmd_handler_delegate;
}

const struct bgapi_soc_cmd *current_cmd_decl = NULL;

/* Start the handling of BGAPI command with no variable-length data */
sl_status_t sli_bgapi_start_soc_command(const struct bgapi_soc_cmd *cmd_decl)
{
  // Must have a declaration
  BG_ASSERT(cmd_decl != NULL);

  // Lock
  sl_status_t status = sl_bgapi_lock();
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Commit to processing this command
  current_cmd_decl              = cmd_decl;
  sli_bgapi_cmd_msg_buf->header = cmd_decl->command_header;

  return SL_STATUS_OK;
}

/* Start the handling of BGAPI command with variable-length data */
sl_status_t sli_bgapi_start_soc_command_with_data(const struct bgapi_soc_cmd *cmd_decl,
                                                  size_t length_field_size,
                                                  size_t data_len,
                                                  const void *data)
{
  // Must have a declaration
  BG_ASSERT(cmd_decl != NULL);

  // First get the length of the fixed-size part of the command and verify the data fits
  uint32_t bgapi_header   = cmd_decl->command_header;
  uint32_t cmd_header_len = SL_BGAPI_MSG_LEN(bgapi_header);
  uint32_t msg_len        = cmd_header_len + data_len;
  if (msg_len > SL_BGAPI_MAX_PAYLOAD_SIZE) {
    return SL_STATUS_WOULD_OVERFLOW;
  }

  // The data fits so we can start the handling. Lock and commit to processing this command.
  sl_status_t status = sl_bgapi_lock();
  if (status != SL_STATUS_OK) {
    return status;
  }
  current_cmd_decl = cmd_decl;

  // Set the final header with the full message length including the variable-length data
  uint8_t dev_type   = SL_BGAPI_MSG_DEVICE_TYPE(bgapi_header);
  uint8_t class_id   = SLI_BGAPI_MSG_CLASS_ID(bgapi_header);
  uint8_t command_id = SLI_BGAPI_MSG_COMMAND_ID(bgapi_header);
  sli_bgapi_cmd_msg_buf->header =
    SLI_BGAPI_MSG_HEADER(class_id, command_id, (uint8_t)sl_bgapi_msg_type_cmd | (uint8_t)dev_type, msg_len);

  // The data buffer for the variable-length data is always after the command header. Copy the data
  // first.
  uint8_t *data_buffer = ((uint8_t *)&sli_bgapi_cmd_msg_buf->data) + cmd_header_len;
  memcpy(data_buffer, data, data_len);

  // The length field is always just before the data buffer but its type and length depends on the
  // type of the array parameter.
  if (length_field_size == sizeof(uint8array)) {
    // The array is a `uint8array` with an 8-bit length field
    uint8array *array_8bit = (uint8array *)(data_buffer - sizeof(uint8array));
    if (data_len > UINT8_MAX) {
      return SL_STATUS_WOULD_OVERFLOW;
    }
    array_8bit->len = (uint8_t)data_len;
  } else if (length_field_size == sizeof(byte_array)) {
    // The array is a `byte_array` with a 16-bit length field. The length can never overflow the
    // 16-bit value, as the BGAPI message length limitation that we checked is always smaller.
    byte_array *array_16bit = (byte_array *)(data_buffer - sizeof(byte_array));
    BG_ASSERT(data_len <= UINT16_MAX);
    array_16bit->len = (uint16_t)data_len;
  } else {
    // An unknown size can only occur if the generated code is somehow broken
    BG_ASSERT(false);
  }

  return SL_STATUS_OK;
}

/* Call the command handler delegate for the current BGAPI command */
sl_status_t sli_bgapi_call_delegate_for_soc_command()
{
  // Processing must have been started already with sli_bgapi_start_soc_command()
  BG_ASSERT(current_cmd_decl != NULL);
  BG_ASSERT(current_cmd_decl->command_handler != NULL);

  // Call the delegate
  sli_bgapi_cmd_handler_delegate_function(sli_bgapi_cmd_msg_buf->header,
                                          current_cmd_decl->command_handler,
                                          &sli_bgapi_cmd_msg_buf->data);

  // Return the response from the response structure. The result code, if any, is always at a fixed
  // position and this is verified by the tools that generate the BGAPI files.
  return sli_bgapi_rsp_msg_buf->data.rsp_error.result;
}

/* End the handling of BGAPI command */
void sli_bgapi_end_soc_command()
{
  // If the implementation of a BGAPI command handler calls another command, we will return here and
  // clear the command declaration even while the outer command is still being processed. That's OK,
  // though. The command declaration is only needed momentarily between
  // sli_bgapi_start_soc_command() and sli_bgapi_call_delegate_and_end_soc_command(), and no other
  // calls will happen between those. We can always clear this here to make sure we'll detect any
  // attempt to call the SoC delegate without having start the command handling first.
  current_cmd_decl = NULL;
  sl_bgapi_unlock();
}

/* Call the command handler delegate and end processing the current BGAPI command */
sl_status_t sli_bgapi_call_delegate_and_end_soc_command()
{
  sl_status_t status = sli_bgapi_call_delegate_for_soc_command();
  sli_bgapi_end_soc_command();
  return status;
}

/* Handle a BGAPI command in binary format */
void sl_bgapi_handle_command(uint32_t header, const void *data)
{
  sli_bgapi_cmd_handler_delegate_function(header, NULL, data);
}

uint8_t sl_bt_is_sensitive_message(uint32_t message_id)
{
  const struct bgapi_lib *lib = get_bgapi_library(message_id);

  if (!lib) {
    return 0;
  }

  uint8_t command_id = SLI_BGAPI_MSG_COMMAND_ID(message_id);
  for (const struct bgapi_cmd *msg = lib->cmd_table; msg && msg->handler; msg++) {
    if (msg->command_id == command_id) {
      return msg->metadata.sensitive ? 1 : 0;
    }
  }

  return 0;
}

void *sl_bgapi_get_command_response(void)
{
  return sli_bgapi_rsp_msg_buf;
}

// Set a generic error response to the specified buffer.
void sl_bgapi_set_error_response(uint32_t command_hdr, uint16_t result, void *response, size_t response_buf_size)
{
  // We can't trust the command header, so we'll rebuild a new header from its
  // parts. Some of the parts may be zeros if the NCP implementation does not
  // have a full command, but that's OK. BGAPI commands are processed one
  // command at a time and the recipient will be able to handle the error
  // response even if some IDs are zeros.
  uint8_t device_type   = SL_BGAPI_MSG_DEVICE_TYPE(command_hdr);
  uint8_t class_id      = SLI_BGAPI_MSG_CLASS_ID(command_hdr);
  uint8_t command_id    = SLI_BGAPI_MSG_COMMAND_ID(command_hdr);
  uint32_t response_hdr = SLI_BGAPI_MSG_HEADER(class_id,
                                               command_id,
                                               (uint8_t)sl_bgapi_msg_type_rsp | device_type,
                                               sizeof(struct sl_bgapi_rsp_error_s));

  // Fill the output if there's space
  if (response_buf_size >= sizeof(response_hdr) + sizeof(struct sl_bgapi_rsp_error_s)) {
    struct sl_bgapi_packet *bgapi_msg = response;
    bgapi_msg->header                 = response_hdr;
    bgapi_msg->data.rsp_error.result  = result;
  }
}

uint32_t bgapi_message_size()
{
  return sizeof(struct sl_bgapi_packet);
}

// Returns whether or not there are events in any of the queues for the user.
// uint8_t gecko_check_evt()
// {
//   return get_first_device_with_events() ? 1 : 0;
// }

// Create a BGAPI response with no variable-length data.
void *gecko_create_bgapi_response(uint32_t rsp_header)
{
  // Set the BGAPI header
  sli_bgapi_rsp_msg_buf->header = rsp_header;

  // Signal the need to update
  // gecko_stack_update_callback();

  return (void *)sli_bgapi_rsp_msg_buf;
}

// Create a BGAPI response with variable-length data in contiguous memory.
struct sl_bgapi_packet *gecko_create_bgapi_response_data(uint32_t rsp_header, size_t data_len, const void *data)
{
  // Extract the types, IDs, and the length of the fixed-size part of the response
  uint8_t dev_type         = SL_BGAPI_MSG_DEVICE_TYPE(rsp_header);
  uint8_t class_id         = SLI_BGAPI_MSG_CLASS_ID(rsp_header);
  uint8_t command_id       = SLI_BGAPI_MSG_COMMAND_ID(rsp_header);
  uint32_t evt_header_size = SL_BGAPI_MSG_LEN(rsp_header);

  // The combined size may not exceed the available BGAPI payload size
  BG_ASSERT(evt_header_size + data_len <= BGAPI_PAYLOAD_SIZE);

  // Either the data length must be zero or we must have data
  BG_ASSERT((data_len == 0) || (data != NULL));

  // Get the uint8array that starts at the last byte of the response-specific structure and set the
  // size
  uint8array *data_array = (uint8array *)(((uint8_t *)&sli_bgapi_rsp_msg_buf->data) + evt_header_size - 1);
  data_array->len        = (uint8_t)data_len;

  // If the data is not already in the right place, copy it now. We allow callers to use the buffer
  // as temporary storage, so use "memmove" instead of "memcpy" just in case the data is already
  // somewhere in the middle of the buffer.
  if (data && (data != data_array->data)) {
    memmove(data_array->data, data, data_len);
  }

  // Construct the full header and let the no-data version fill the parts common to all responses
  uint32_t bgapi_size = evt_header_size + data_len;
  uint32_t full_header =
    SLI_BGAPI_MSG_HEADER(class_id, command_id, (uint8_t)sl_bgapi_msg_type_rsp | (uint8_t)dev_type, bgapi_size);
  return gecko_create_bgapi_response(full_header);
}
