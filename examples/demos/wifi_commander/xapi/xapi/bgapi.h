#ifndef BGAPI_H
#define BGAPI_H
#include <stdbool.h>
#include "bg_types.h"
#include "sl_bgapi.h"

/**
 * @addtogroup sli_bgapi_internal_handling BGAPI internal handling
 * @brief Internal handling of BGAPI commands, responses, and events
 * @{
 *
 * From the point of view of an external caller, BGAPI commands are handled one at a time, so only
 * one command and one response structure is needed concurrently. Both use struct @ref
 * sl_bt_packet and the structures are allocated by bgapi_buffer_init() to @ref
 * sli_bgapi_cmd_msg_buf (the command), and @ref sli_bgapi_rsp_msg_buf (the response).
 *
 * BGAPI command processing may however be nested, as the implementation in one BGAPI command
 * handler may call another BGAPI command. The function that calls another BGAPI function must be
 * aware of the fact that there is only one command structure. The function must read all the inputs
 * it needs from the command structure before calling another BGAPI command, as the generated code
 * in that command funtion will overwrite the command structure.
 *
 * The functions that fill responses set the response message header, the result, and optionally any
 * variable-length data, directly to the response buffer pointed to by @ref sli_bgapi_rsp_msg_buf.
 * As with commands, a BGAPI command handler that calls another BGAPI command must be aware of there
 * being only one response structure. Such a command handler must make sure to set the response
 * after the nested call has returned and has set its own response. Typically all BGAPI command
 * handlers set the response as the last thing in the function.
 *
 * The functions that are used to process SoC commands, create responses, and queue events are
 * considered internal to the generated BGAPI functions and shall not be used directly anywhere
 * else.
 *
 * 1. Commands
 * -----------
 *
 * The following steps illustrate how the internal functions are used to process SoC commands in the
 * generated BGAPI functions:
 *
 *  1. The generated BGAPI function (of naming convention `sl_<device>_<class>_<command>`) will
 *     first start the command handling by calling @ref sli_bgapi_start_soc_command() or @ref
 *     sli_bgapi_start_soc_command_with_data() and passing a pointer to the command declaration of
 *     type struct @ref bgapi_soc_cmd. Using the declaration structure avoids the need for the
 *     generated code to load many arguments to registers, as one pointer is sufficient to describe
 *     the properties of the command. The internal function will obtain the BGAPI lock, remember
 *     which command is being serviced, and in case of sli_bgapi_start_soc_command_with_data() also
 *     copy the variable-length data to the command structure.
 *
 *  2. The generated BGAPI function fills the non-array parameters, if any, to the command structure
 *
 *  3. The generated BGAPI function calls @ref sli_bgapi_call_delegate_for_soc_command() to invoke
 *     the delegate and process the command. No arguments need to be passed, as only one command is
 *     processed at a time, and the start call in step 1 already recorded which command we're
 *     handling by storing the command declaration pointer. The function that processes the command
 *     will call one of the functions that are generated to create responses. See the section on
 *     responses below.
 *
 *     The function @ref sli_bgapi_call_delegate_for_soc_command() returns the `result` value from
 *     the BGAPI response so that the generated code doesn't have to access the response if it only
 *     needs the result value.
 *
 *  4. The generated BGAPI function reads the output parameters from the response, if any, and
 *     writes them to output parameters of the BGAPI function.
 *
 *  5. The generated BGAPI function calls @ref sli_bgapi_end_soc_command() to end the handling of
 *     the command. That function releases the BGAPI lock that was obtained in step 1.
 *
 * If a generated command has no output parameters other than the `result`, it does not need any
 * actions in step 4. As an optimization, the generated function can then combine steps 3 and 5 into
 * one by calling @ref sli_bgapi_call_delegate_and_end_soc_command(). This avoids an extra function
 * call in the generated BGAPI function.
 *
 * The following principles guide the approach and the design of the functions that are used to
 * create responses and events.
 *
 *  - Responses and events are somewhat different in nature, so the underlying implementation is
 *    different and tuned specifically for the type in question.
 *
 *  - Even though the implementation is somewhat different, the interface in gecko_bgapi.h provides
 *    a very similar API for both responses and events. This makes the API nicely symmetric.
 *
 *  - For both responses and events, the implementation aims to avoid any unnecessary copying of
 *    data. Any field values or data is set directly to its final location, and any bgbufs that can
 *    be used as-is are used as they are without a copy or re-allocation.
 *
 *  - To avoid increasing the size of the code section in the binary, common parts are handled by
 *    the functions that the macros call, rather than duplicating the code in the macro body. This
 *    includes setting fields that have common values for a particular message type, or fields that
 *    have values that are calculated the same way (such as the size of a message).
 *
 *
 * 2. Responses
 * ------------
 *
 * The following functions are provided by `gecko_<device>_bgapi.h` for creating responses:
 *
 * 2.1. Responses with no variable-length data:
 *  - `gecko_rsp_<class>_<command>(<non-array parameters>)`
 *
 * 2.2. Responses with variable-length data that the caller has in contiguous memory:
 *  - `gecko_rsp_<class>_<command>(<non-array parameters>, <data_len>, <data_ptr>)`
 *
 * A caller that needs to obtain the response data from some other API may use the following
 * function first to obtain a pointer to the response-specific data array in the singleton response
 * message:
 *
 * - `gecko_get_rsp_<class>_<command>_data_buf()`
 *
 * The caller may then pass the data pointer to an API that reads the data to avoid the need for a
 * temporary buffer. The available space in the data buffer depends on the size of the response and
 * is `BGAPI_PAYLOAD_SIZE - sizeof(struct gecko_msg_<class>_<command>_rsp_t)`. After filling the
 * data to the buffer directly, the caller must call the corresponding
 * `gecko_rsp_<class>_<command>()` function with the data buffer pointer returned by
 * `gecko_get_rsp_<class>_<command>_data_buf()` to get the other fields of the message set. The
 * function will not perform an unnecessary copy when the data pointer already points to the data
 * array in the message.
 *
 *
 * 3. Events
 * ---------
 *
 * BGAPI events are added to an event queue that is a chain of bgbufs, starting at the list head
 * `evt_queue` in the respective instance of struct @bgapi_device. The `packet_boundary` flag in the
 * bgbuf is used to identify the beginning of a new event in the chain of bgbufs.
 *
 * Similarly to commands, events are declared in the generated code using a struct @ref bgapi_event.
 * A pointer to the event declaration is passed to the functions that create and queue events.
 *
 * When an event has variable-length data, the caller may have it available in different formats.
 * Some have it in contiguous memory, some have a bgbuf chain that must be copied, and some have a
 * bgbuf chain that can be re-used as-is. The following functions are provided by
 * `gecko_<device>_bgapi.h` to support easily creating events in all cases. Note that these
 * functions may only be called from within the same thread/task that is dedicated for running the
 * stack for the specific BGAPI device and popping its events to the application:
 *
 * 3.1. Events with no variable-length data:
 *  - `gecko_evt_<class>_<command>(<non-array parameters>)` creates and immediately queues an event
 *
 *  - `sli_bgapi_create_event_<device>_<class>_<event>(<non-array parameters>)` creates an event
 *    but does not queue it yet. The event can be queued later with @ref sli_bgapi_queue_event().
 *
 * 3.2. Events with variable-length data that the caller has in contiguous memory:
 *  - `gecko_evt_<class>_<command>(<non-array parameters>, <data_len>, <void* data>)`
 *
 * 3.3. Events with variable-length data that the caller has in a chain of bgbufs that must be copied:
 *  - `gecko_evt_<class>_<command>_bgbuf_copy(<non-array parameters>, <bgbuf* data>)`
 *
 * 3.4. Events with variable-length data that the caller has in a chain of bgbufs that can be used as-is:
 *  - `gecko_evt_<class>_<command>_bgbuf_reuse(<non-array parameters>, <bgbuf** data>)`
 *
 * As mentioned above, the `gecko_evt_*` functions may only be called from within the thread that's
 * running the stack corresponding to the BGAPI device. When events need to be generated from an
 * unknown thread (or an interrupt), the code that generates the event must use the lower level
 * functions and the following sequence to guarantee thread safety:
 *
 *  1. Create the event by calling @ref sli_bgapi_create_event()
 *  2. Call @ref sli_bgapi_get_message() to obtain the message pointer
 *  3. Fill the event-specific fields in the message
 *  4. Queue the filled event by calling @ref sli_bgapi_queue_event()
 *
 * @} end addtogroup sli_bgapi_internal_handling
 */

// BG-10920: Stop reporting Bootloader version, but we need to keep API
// backwards compatibility for now. Hard code it to 0. BGAPI document will
// have the information that the support is stopped.
#define DUMMY_BOOTLOADER_VERSION 0

struct bgapi_cmd {
  sl_bgapi_handler handler;
  uint8_t class_id;
  uint8_t command_id;
  struct {
    uint8_t sensitive : 1;
  } metadata;
};

PACKSTRUCT(struct sl_bgapi_rsp_error_s { uint16_t result; });

PACKSTRUCT(struct sl_bgapi_packet {
  uint32_t header;
  union {
    uint8_t handle;
    struct sl_bgapi_rsp_error_s rsp_error;
    uint8_t payload[SL_BGAPI_MAX_PAYLOAD_SIZE];
  } data;
});

/**
 * Initialize BGAPI internal state
 */
void bgapi_init(void);

/**
 * This function is called by bgapi_init(). It must be implemented by the platform
 * to initialize any platform-specific systems required to run BGAPI.
 */
void bgapi_hal_init(void);

/**
 * @brief Function prototype for BGAPI class initialization
 *
 * When BGAPI classes are being initialized for a BGAPI device, the common BGAPI
 * implementation will call the initialization function of each registered BGAPI
 * class.
 *
 * If the initialization function of a class returns an error, the BGAPI
 * implementation will call the corresponding de-initialization function of the
 * failed class, and all the classes that were already initialized.
 *
 * @return SL_STATUS_OK if successful, otherwise an error code
 */
typedef sl_status_t (*bgapi_lib_init)(void);

/**
 * @brief Function prototype for BGAPI class stopping
 *
 * When a BGAPI device is being stopped gracefully, the common BGAPI
 * implementation will call the stop function of each registered BGAPI class.
 * Once the stop function has been called, the class no longer needs to service
 * normal BGAPI commands. If the class has on-going operations that need to be
 * stopped asynchronously, the class needs to initiate the stopping operations
 * when the stop function is called and call @ref sli_bgapi_lib_stop_ongoing for
 * every asynchronous operation it started. Once an asynchronous stopping
 * finishes, the library needs to call @ref sli_bgapi_lib_stop_finished.
 *
 * The de-init function @ref bgapi_lib_deinit will be called when it's time to
 * free all resources allocated by the class. This will happen when the last
 * asynchronous stop operation has finished or when the device is forced to stop
 * immediately by a command from the application.
 *
 * Note that the stop function is used only when a BGAPI device is being stopped
 * gracefully. If a BGAPI device is stopped forcefully (immediately), the
 * libraries will directly get the @ref bgapi_lib_deinit call.
 */
typedef void (*bgapi_lib_stop)(void);

/**
 * @brief Function prototype for BGAPI class de-initialization
 *
 * The de-initialization function should release all resources allocated by the
 * initialization function. If class initialization has failed, the
 * de-initialization function can get called after a partially successful
 * initialization. The implementation of the de-init function must correspond to
 * the init function so that the combination is robust in all cases.
 *
 * If a BGAPI device was stopped forcefully (either directly or during a
 * graceful stop), class implementations may still have some on-going operations
 * that need to be stopped at de-init time. Each class that has such operations
 * is responsible for immediately stopping them at the time of de-init.
 */
typedef void (*bgapi_lib_deinit)(void);

/*
 * The public API for `sl_device_init_classes()` specifies the classes using
 * pointers to a forward-declared structure type `struct sli_bgapi_class` that
 * is not defined anywhere. Internally the class declarations are instances of
 * `struct bgapi_lib`. We forward-declare the public type here too to avoid
 * needing to include the public initialization header from the internal
 * implementation.
 */
struct sli_bgapi_class;

/**
 * @brief Macro to construct a BGAPI message header
 *
 * This a macro is used to initialize constants in the generated BGAPI command
 * facade. It is the caller's responsibility to verify that the input values are
 * within valid range.
 *
 * @param[in] class_id The class ID
 * @param[in] command_id The command ID
 * @param[in] type Type field including message and device type
 * @param[in] payload_len Length of the full message payload
 *
 * @return The header as a `uint32_t` value
 */
#define SLI_BGAPI_MSG_HEADER(class_id, command_id, type, payload_len)                                              \
  (uint32_t)(((uint32_t)(command_id) << 24) | ((uint32_t)(class_id) << 16) | (((uint32_t)(payload_len)&0xFF) << 8) \
             | ((uint32_t)(payload_len) >> 8) | ((uint32_t)(type)))

/**
 * @brief Macro to extract the class ID of a BGAPI message header
 *
 * @param[in] BGAPI message header
 *
 * @return The class ID as a `uint8_t` value
 */
#define SLI_BGAPI_MSG_CLASS_ID(header) ((uint8_t)(((header) >> 16) & 0xFF))

/**
 * @brief Macro to extract the command ID of a BGAPI message header
 *
 * @param[in] BGAPI message header
 *
 * @return The command ID as a `uint8_t` value
 */
#define SLI_BGAPI_MSG_COMMAND_ID(header) ((uint8_t)(((header) >> 24) & 0xFF))

/**
 * @brief Macro to extract the message type of a BGAPI message header
 *
 * @param[in] BGAPI message header
 *
 * @return The message type as a `uint8_t` value
 */
#define SLI_BGAPI_MSG_MESSAGE_TYPE(header) ((uint8_t)((header)&0x80))

/**
 * @brief Structure to declare a BGAPI command in SoC mode
 *
 * The generated BGAPI functions that are used in SoC builds declare the command
 * using this structure. The generated functions pass a pointer to the
 * declaration structure when they call @ref sli_bgapi_start_soc_command() or
 * @ref sli_bgapi_start_soc_command_with_data() to start the processing of a SoC
 * command.
 */
struct bgapi_soc_cmd {
  uint32_t command_header;          /**< The BGAPI message header for this command. The length is
                                       set to the length of the fixed-size part of the message,
                                       excluding any variable-length data. */
  sl_bgapi_handler command_handler; /**< The function that implements this command */
};

/**
 * @brief Structure to declare a BGAPI event
 *
 * The generated functions that create a BGAPI event declare the event using
 * this structure. The functions pass a pointer to the declaration structure
 * when they call the shared functions that allocate and queue events.
 */
struct bgapi_event {
  uint32_t event_header; /**< The BGAPI message header for this event. The length is set to the
                             length of the fixed-size part of the message, excluding any
                             variable-length data. */
};

/**
 * Structure used to declare a BGAPI library
 */
struct bgapi_lib {
  uint8_t cls_name;                  /**< BGAPI Class ID */
  const struct bgapi_cmd *cmd_table; /**< Command table or NULL */
  bgapi_lib_init init_func;          /**< Library initialization function */
  bgapi_lib_stop stop_func;          /**< Library asynchronous stop function */
  bgapi_lib_deinit deinit_func;      /**< Library de-initialization function */
};

/**
 * @brief Flags used in @ref bgapi_device
 */
typedef enum {
  sli_bgapi_device_flag_registered       = 0x01, /**< The BGAPI device has been registered. */
  sli_bgapi_device_flag_started          = 0x02, /**< The BGAPI device is in the active started state. */
  sli_bgapi_device_flag_stopping_ongoing = 0x04, /**< Stopping the BGAPI device is on-going. */
  sli_bgapi_device_flag_stopping_ready =
    0x08, /**< Stopping the BGAPI device has finished but the device is not yet fully stopped. */
} sli_bgapi_flags_t;

/**
 * @brief Structure to collect the information of one BGAPI device.
 *
 * An instance of this structure is statically allocated in RAM by each BGAPI
 * device implementation. The structure is linked to the list of registered BGAPI
 * devices in @ref bgapi_register_device().
 */
struct bgapi_device {
  /** The BGAPI device type */
  uint8_t dev_type;

  /** Bluetooth start stop flags */
  uint8_t flags;

  /** The number of asynchronous stopping operations that are on-going */
  uint16_t num_async_lib_stops_ongoing;

  /**
   * NULL-terminated array of pointers to declarations of those libraries that
   * are available when the BGAPI device has been started.
   */
  const struct bgapi_lib *const *lib_table;

  /**
   * NULL-terminated array of pointers to declarations of those libraries that
   * are available when the BGAPI device has been stopped. Only libraries in the
   * @p lib_table array receive the @ref bgapi_lib_init, @ref bgapi_lib_stop,
   * and @ref bgapi_lib_deinit calls. The array @p lib_table_stopped is only
   * used to find classes that are specifically meaning to be available even
   * when the BGAPI device is stopped. Implementations of such classes must be
   * prepared to receive BGAPI command calls even when the library has not
   * received the @ref bgapi_lib_init call.
   */
  const struct bgapi_lib *const *lib_table_stopped;

  /** Next registered device */
  struct bgapi_device *next;
};

/**
 * @brief Internal helper macro to define a BGAPI class declaration.
 *
 * This macro is an internal helper used to implement @ref DEFINE_BGAPI_CLASS()
 * and @ref DEFINE_BGAPI_CLASS_WITH_STOP(). BGAPI class implementations are
 * expected to use those macros instead of using this helper macro directly.
 */
#define DEFINE_BGAPI_CLASS_STRUCT(_device_name,                                       \
                                  _class_name,                                        \
                                  _device_name_upper,                                 \
                                  _class_name_upper,                                  \
                                  _init_func,                                         \
                                  _stop_func,                                         \
                                  _deinit_func,                                       \
                                  _mode,                                              \
                                  _cmd_table)                                         \
  const struct bgapi_lib sli_bgapi_class_##_device_name##_##_class_name##_##_mode = { \
    .cls_name    = SLI_##_device_name_upper##_##_class_name_upper##_CLASS_ID,         \
    .cmd_table   = _cmd_table,                                                        \
    .init_func   = _init_func,                                                        \
    .stop_func   = _stop_func,                                                        \
    .deinit_func = _deinit_func,                                                      \
  }

/**
 * @brief Internal helper macro to construct command table names.
 */
#define BGAPI_CLASS_CMD_TABLE_NAME(device_name, class_name) sli_bgapi_cmd_table_##device_name##_##class_name

/**
 * @brief Macro to define BGAPI class declaration structures.
 *
 * This macro is intended to be used by BGAPI class implementations to define
 * the relevant BGAPI class declaration structures. This macro hides the "full"
 * and "optimized" versions of the declarations and defines both in one
 * invocation.
 */
#define DEFINE_BGAPI_CLASS(_device_name, _class_name, _device_name_upper, _class_name_upper, _init_func, _deinit_func) \
  DEFINE_BGAPI_CLASS_STRUCT(_device_name,                                                                              \
                            _class_name,                                                                               \
                            _device_name_upper,                                                                        \
                            _class_name_upper,                                                                         \
                            _init_func,                                                                                \
                            NULL,                                                                                      \
                            _deinit_func,                                                                              \
                            optimized,                                                                                 \
                            NULL);                                                                                     \
  extern const struct bgapi_cmd BGAPI_CLASS_CMD_TABLE_NAME(_device_name, _class_name)[];                               \
  DEFINE_BGAPI_CLASS_STRUCT(_device_name,                                                                              \
                            _class_name,                                                                               \
                            _device_name_upper,                                                                        \
                            _class_name_upper,                                                                         \
                            _init_func,                                                                                \
                            NULL,                                                                                      \
                            _deinit_func,                                                                              \
                            full,                                                                                      \
                            BGAPI_CLASS_CMD_TABLE_NAME(_device_name, _class_name))

/**
 * @brief Macro to define BGAPI class declaration structures with stop function.
 *
 * This macro is intended to be used by BGAPI class implementations to define
 * the relevant BGAPI class declaration structures when the BGAPI class needs
 * the indication given by the call to @ref bgapi_lib_stop. This macro hides the
 * "full" and "optimized" versions of the declarations and defines both in one
 * invocation.
 */
#define DEFINE_BGAPI_CLASS_WITH_STOP(_device_name, _class_name, _init_func, _stop_func, _deinit_func)          \
  DEFINE_BGAPI_CLASS_STRUCT(_device_name, _class_name, _init_func, _stop_func, _deinit_func, optimized, NULL); \
  extern const struct bgapi_cmd BGAPI_CLASS_CMD_TABLE_NAME(_device_name, _class_name)[];                       \
  DEFINE_BGAPI_CLASS_STRUCT(_device_name,                                                                      \
                            _class_name,                                                                       \
                            _init_func,                                                                        \
                            _stop_func,                                                                        \
                            _deinit_func,                                                                      \
                            full,                                                                              \
                            BGAPI_CLASS_CMD_TABLE_NAME(_device_name, _class_name))

/**
 * Register a BGAPI device.
 *
 * @param[in,out] bgapi_device Pointer to the device declaration to register
 */
void bgapi_register_device(struct bgapi_device *bgapi_device);

/**
 * Initialize a BGAPI device.
 *
 * This function calls the @ref bgapi_lib_init function for all libraries
 * associated with the device and sets the device to the @ef
 * sli_bgapi_device_flag_started state if successful. If the BGAPI device has
 * not already been registered with @ref bgapi_register_device, the device is
 * automatically registered before the inits.
 *
 * @param[in,out] bgapi_device Pointer to the device declaration to init
 */
sl_status_t bgapi_init_device(struct bgapi_device *bgapi_device);

/**
 * Gracefully stop a BGAPI device.
 *
 * This function calls the @ref bgapi_lib_stop function for all libraries
 * associated with the device and sets the device to the @ref
 * sli_bgapi_device_flag_stopping_ongoing state. The stop calls are made in the
 * opposite order of initialization, i.e. the last library that was initialized
 * is the first to stop.
 *
 * When all libraries have finished stopping gracefully, the state changes to
 * @ref sli_bgapi_device_flag_stopping_ready. Device-specific code should follow
 * the state with @ref sli_bgapi_device_has_finished_stopping and make the final
 * call to @ref @ref bgapi_deinit_device when stopping is ready or when the
 * application requests stopping immediately.
 *
 * @param[in,out] bgapi_device The BGAPI device to stop
 */
void bgapi_stop_device(struct bgapi_device *bgapi_device);

/**
 * @brief Increase the count of asynchronous stopping operations
 *
 * This function is called by BGAPI class implementations that cannot finish the
 * stopping immediately when the @ref bgapi_lib_stop function of the BGAPI class
 * is called. For every call to this function, the BGAPI class is required to
 * call @ref sli_bgapi_lib_stop_finished when the asynchronous stopping has
 * finished.
 */
void sli_bgapi_lib_stop_ongoing(struct bgapi_device *bgapi_device);

/**
 * @brief Decrease the count of asynchronous stopping operations
 *
 * This function is called by BGAPI class implementations when an asynchronous
 * stopping indicated with a call to @ref sli_bgapi_lib_stop_ongoing has been
 * finished. When the last asynchronous stopping operation is finished, the
 * BGAPI device is set to the @ref sli_bgapi_device_flag_stopping_ready and the
 * device-specific code will de-init the device.
 */
void sli_bgapi_lib_stop_finished(struct bgapi_device *bgapi_device);

/**
 * De-initialize a BGAPI device.
 *
 * This function calls the @ref bgapi_lib_deinit function for all libraries
 * associated with the device and sets the device to the @ef
 * sli_bgapi_device_flag_stopped state. The de-init calls are made in the
 * opposite order of initialization, i.e. the last library that was initialized
 * is the first to de-initialize.
 *
 * Note that the BGAPI device is not
 * removed from the list of registered devices and the structure @p bgapi_device
 * must remain valid even after this call.
 *
 * @param[in,out] bgapi_device Pointer to the device declaration to de-init
 */
void bgapi_deinit_device(struct bgapi_device *bgapi_device);

/**
 * @brief Return true if the specified BGAPI device is started
 *
 * A BGAPI device is considered started when it has been started and is not
 * already stopping or stopped. The command implementations of normal classes
 * are called only when the device is in the started state, so normal class
 * implementations do not need to check. This function is provided for those
 * special BGAPI classes that have their command table registered also when the
 * BGAPI device is stopped. Implementations in those classes can use this
 * function to check if the device is currently started or not.
 *
 * @return True if the BGAPI device is started. False otherwise.
 */
bool sli_bgapi_device_is_started(const struct bgapi_device *bgapi_device);

/**
 * @brief Return true if the specified BGAPI device has finished stopping
 *
 * @return True if the specified BGAPI device has reached the @ref
 *   sli_bgapi_device_flag_stopping_ready state after a call to @ref
 *   bgapi_stop_device. False otherwise.
 */
bool sli_bgapi_device_has_finished_stopping(const struct bgapi_device *bgapi_device);

/**
 * @brief Return true if the specified BGAPI device is stopped
 *
 * @return True if the specified BGAPI device is fully stopped, i.e. is not
 *   started or still in the process of stopping. False otherwise.
 */
bool sli_bgapi_device_is_stopped(const struct bgapi_device *bgapi_device);

/** Declation of the command buffer used by BGAPI command handlers */
extern struct sl_bgapi_packet *sli_bgapi_cmd_msg_buf;

/** Declation of the response buffer used by BGAPI command handlers */
extern struct sl_bgapi_packet *sli_bgapi_rsp_msg_buf;

/**
 * @brief Start the handling of BGAPI command with no variable-length data.
 *
 * This function is called to start the handling of a BGAPI command in a
 * generated BGAPI SoC function. If the function is successful, the caller has
 * obtained exclusive access to BGAPI command and response buffers. When the
 * caller is done with the access, it must call @ref
 * sli_bgapi_end_command_handling() to release the buffers.
 *
 * @param[in] cmd_decl The declaration structure of the command we're starting
 *
 * @return SL_STATUS_OK if access was successfully acquired and command handling
 *   started successfully. Otherwise an error code.
 */
sl_status_t sli_bgapi_start_soc_command(const struct bgapi_soc_cmd *cmd_decl);

/**
 * @brief Start the handling of BGAPI command with variable-length data.
 *
 * This function is called to start the handling of a BGAPI command in a
 * generated BGAPI SoC function. If the function is successful, the caller has
 * obtained exclusive access to BGAPI command and response buffers. When the
 * caller is done with the access, it must call @ref
 * sli_bgapi_end_command_handling() to release the buffers.
 *
 * This function also verifies that the variable-length data can fit the command
 * structure and copies the data and sets its length to the correct place in the
 * command.
 *
 * @param[in] cmd_decl The declaration structure of the command we're starting
 * @param[in] length_field_size The size, in bytes, of the length field.
 *   Parameters with a 1 byte (8-bit) length field use type `uint8array`.
 *   Parameters with a 2 byte (16-bit) length field use type `byte_array`.
 * @param[in] data_len Length of the variable-length data
 * @param[in] data_buf Buffer holding the variable-length data
 *
 * @return SL_STATUS_OK if access was successfully acquired, data was
 *   successfully copied, and command handling started successfully. Otherwise
 *   an error code.
 */
sl_status_t sli_bgapi_start_soc_command_with_data(const struct bgapi_soc_cmd *cmd_decl,
                                                  size_t length_field_size,
                                                  size_t data_len,
                                                  const void *data);

/**
 * @brief Call the command handler delegate for the current BGAPI command.
 *
 * This function calls the BGAPI command handler delegate for the command that
 * is currently being handled. The command handling must have been started by
 * calling @ref sli_bgapi_start_soc_command(), and the command packet @ref
 * sli_bgapi_cmd_msg_buf must have been filled with the correct inputs. When
 * this function returns, the response packet @ref sli_bgapi_rsp_msg_buf has
 * been filled with the response.
 *
 * @return The response that was set by the command. For commands that do not
 *   set a response in the response message, the returned status defaults to
 *   SL_STATUS_OK.
 */
sl_status_t sli_bgapi_call_delegate_for_soc_command();

/**
 * @brief End the handling of a BGAPI command and release the exclusive lock.
 */
void sli_bgapi_end_soc_command();

/**
 * @brief Call the command handler delegate and end processing the current BGAPI command.
 *
 * This is a code size optimization helper to combine the calling of the
 * delegate and ending the processing of SoC command to one function. This is
 * used by the generated BGAPI command handler when no response parameters need
 * to be set between calling the delegate and ending the command handling.
 * Combining these steps to one call reduces the number of instructions in the
 * generated functions.
 */
sl_status_t sli_bgapi_call_delegate_and_end_soc_command();

/**
 * BGAPI Listener
 */
int8_t bgapi_ubt_listener(void *task, void *object, uint32_t msg, void *data);

void *sli_bt_get_command_response();

uint32_t bgapi_message_size();

void sl_bt_handle_command(uint32_t header, void *p);

/**
 * Returns whether or not there are events in any of the queues for the user.
 * NOTE: will not provide info about any pending msg/evt that could result in events.
 */
uint8_t gecko_check_evt();

/**
 * Returns whether or not there are events in the specified device type queue
 */
uint8_t gecko_check_evt_for_device(uint8_t dev_type);

/**
 * Returns the length of next event in the specified queue containing events for the user.
 */
uint32_t gecko_check_evt_len(uint8_t dev_type);

/**
 * Gets the length of next event from the queue.
 */
uint32_t gecko_next_evt_len(uint8_t dev_type);

/**
 * Create a BGAPI response with no variable-length data.
 *
 * @param rsp_header The 32-bit BGAPI response header
 *
 * @return Pointer to the header of the created response
 */
void *gecko_create_bgapi_response(uint32_t rsp_header);

/**
 * Create a BGAPI response with variable-length data in contiguous memory.
 *
 * @param rsp_header The 32-bit BGAPI response header with the size field set to
 *   the size of the event-specific structure without variable-length data
 * @param data_len Length of the variable-length data, if any
 * @param data_buf Buffer holding the variable-length data, if any
 *
 * @return Pointer to the header of the created response
 */
struct sl_bgapi_packet *gecko_create_bgapi_response_data(uint32_t rsp_header, size_t data_len, const void *data);

/* Function for creating a generic error response */
static inline void gecko_rsp_error(uint8_t device_type, uint8_t class_id, uint8_t command_id, uint16_t result)
{
  struct sl_bgapi_packet *bgapi_msg = (struct sl_bgapi_packet *)gecko_create_bgapi_response(
    SLI_BGAPI_MSG_HEADER(class_id,
                         command_id,
                         (uint8_t)sl_bgapi_msg_type_rsp | device_type,
                         sizeof(struct sl_bgapi_rsp_error_s)));
  bgapi_msg->data.rsp_error.result = result;
}

/* Needed for RTOS operation. Signal the stack that it needs to update its state, because
 * an event or a response is waiting to be processed.
 */
void gecko_stack_update_callback(void);

/**
 * Get short BG version hash.
 * @return the first 4 bytes of the hash value
 */
uint32_t bgapi_get_version_hash();

#endif
