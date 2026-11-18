/***************************************************************************/ /**
 * @file sl_si91x_logger_example.c
 * @brief Logger backend example
 *******************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "sl_si91x_button.h"
#include "sl_si91x_button_pin_config.h"
#include "sl_si91x_button_instances.h"
#include "FreeRTOS.h"
#include "sl_log_helper.h"
#include "cmsis_os2.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
/*******************************************************************************
   ***************************  Defines / Macros  ********************************/

#ifndef BUTTON_INSTANCE
#define BUTTON_INSTANCE button_btn0
#endif

/* Helper: convert a float to its IEEE-754 bit pattern (uint32_t) so it can
 * be passed through the log ABI for %f / %F. The formatted backend's argv
 * slots are uint32_t, so a raw float would otherwise be silently truncated
 * (the salvage path mitigates this but a bit-cast is the canonical form). */
#define SL_LOG_FLOAT_BITS(f_val) \
  (((union {                     \
     float _f;                   \
     uint32_t _u;                \
   }){ ._f = (f_val) })          \
     ._u)

/*******************************************************************************
   *************************** LOCAL VARIABLES   *********************************
   ******************************************************************************/
// Initial runtime log level. Tracks the compile-time default configured in the
// Universal Configurator (UC) so the example matches whatever the user picked
// in the logger backend component's settings. BTN0 cycles this at runtime.
sl_log_level_t log_level = SL_LOG_CONFIG_LEVEL_COMPILE_TIME;

const osThreadAttr_t thread_attributes = {
  .name       = "app",
  .stack_size = 3072,
  .priority   = osPriorityLow3,
};

// prototypes
static void application_start(void *argument);
#ifdef SL_CATALOG_SI91X_LOG_BACKEND_IOSTREAM_FORMATTED_PRESENT
static void sl_log_run_format_specifier_check(void);
#endif

void sl_si91x_button_isr(uint8_t pin, int8_t state)
{
  if (pin == BUTTON_INSTANCE.pin) {
    if (state == BUTTON_PRESSED) {
      log_level++;
      if (log_level > SL_LOG_CONFIG_LEVEL_ERROR) {
        log_level = SL_LOG_CONFIG_LEVEL_DEBUG;
      }
      sl_log_set_loglevel(log_level);
    }
  }
}
// FreeRTOS Idle Hook: flush log in idle context

void vApplicationIdleHook(void)
{
  sl_log_flush();
}

/*******************************************************************************
   **************************** Helper Functions *********************************
   ******************************************************************************/

/*******************************************************************************
   **************************** App (Orchestrator) *******************************
   ******************************************************************************/
// Main application thread that cycles through power/profile combinations
static void application_start(void *argument)
{
  SL_PRINT_STRING_DEBUG("ENTER: application_start\r\n");
  (void)argument;

#ifdef SL_CATALOG_SI91X_LOG_BACKEND_IOSTREAM_PRESENT
#ifdef SL_CATALOG_SI91X_LOG_BACKEND_IOSTREAM_COMPACT_PRESENT
#ifdef SL_CATALOG_SEGGER_RTT_PRESENT
  SL_PRINT_STRING_INFO("Log Backend: iostream compact logging with RTT\r\n");
#endif
#ifdef SL_CATALOG_IOSTREAM_SWO_SI91X_PRESENT
  SL_PRINT_STRING_INFO("Log Backend: iostream compact logging with SWO\r\n");
#else
  SL_PRINT_STRING_INFO("Log Backend: iostream compact logging with UART\r\n");
#endif
#endif
#ifdef SL_CATALOG_SI91X_LOG_BACKEND_IOSTREAM_FORMATTED_PRESENT
#ifdef SL_CATALOG_SEGGER_RTT_PRESENT
  SL_PRINT_STRING_INFO("Log Backend: iostream formatted logging with RTT\r\n");
#elif defined(SL_CATALOG_IOSTREAM_SWO_SI91X_PRESENT)
  SL_PRINT_STRING_INFO("Log Backend: iostream formatted logging with SWO\r\n");
#else
  SL_PRINT_STRING_INFO("Log Backend: iostream formatted logging with UART\r\n");
#endif
#endif
#endif
#ifdef SL_CATALOG_SI91X_LOG_BACKEND_SEGGER_SYSVIEW_PRESENT
  SL_PRINT_STRING_INFO("Log Backend: SystemView logging\r\n");
#endif

  // Sample values used across all backend demonstrations below.
  uint32_t test_number    = 67;
  uint32_t another_number = 69;
  uint32_t i_value        = 420;
  const char *test_str1   = "first_string";
  const char *test_str2   = "second_string";
  // Index 0 is unused; SL_LOG_CONFIG_LEVEL_DEBUG starts at 1 and NONE is 6.
  static const char *level_names[] = { "", "DEBUG", "INFO", "WARN", "ERROR", "CRASH", "NONE" };
  (void)test_str1;
  (void)test_str2;
  (void)level_names;

  while (1) {
    // Demonstrate logger print capabilities at each log level.

#ifdef SL_CATALOG_SI91X_LOG_BACKEND_SEGGER_SYSVIEW_PRESENT
    // SystemView backend: Allowed only ONE type of argument (all string or all number, not mix)
    // Demonstrate SystemView backend: log current log level and which log levels are visible
    const char *curr_level_str = (log_level <= SL_LOG_CONFIG_LEVEL_NONE) ? level_names[log_level] : "UNKNOWN";
    SL_PRINT_STRING_INFO("SystemView: Current log level: %d (%s)\r\n", (uint32_t)log_level, (uintptr_t)curr_level_str);

    // Print logs at different levels, using only supported format specifiers
    // (%d, %x, %p, %s, %%). Char args are not supported by the backend.

    switch (log_level) {
      case SL_LOG_CONFIG_LEVEL_DEBUG:
        SL_PRINT_STRING_INFO("All log prints will be visible: DEBUG, INFO, WARN, ERROR.\r\n");
        SL_PRINT_STRING_DEBUG("DEBUG log values: %d %d %d\r\n", test_number, another_number, i_value);
        SL_PRINT_STRING_INFO("INFO log values: %d %d\r\n", test_number + 1, i_value + 1);
        SL_PRINT_STRING_WARN("WARN log values: %d %d %d\r\n", 100, 200, 300);
        SL_PRINT_STRING_DEBUG("DEBUG log strings: %s %s\r\n", (uintptr_t)test_str1, (uintptr_t)test_str2);
        SL_PRINT_STRING_WARN("WARN log strings: %s\r\n", (uintptr_t)test_str2);
        break;
      case SL_LOG_CONFIG_LEVEL_INFO:
        SL_PRINT_STRING_INFO("Visible prints: INFO, WARN, ERROR. DEBUG will be filtered out.\r\n");
        SL_PRINT_STRING_INFO("INFO log values: %d %d\r\n", i_value + 2, i_value + 3);
        SL_PRINT_STRING_WARN("WARN log values: %d %d\r\n", another_number, test_number);
        SL_PRINT_STRING_WARN("WARN log strings: %s %s\r\n", (uintptr_t)test_str1, (uintptr_t)test_str2);
        break;
      case SL_LOG_CONFIG_LEVEL_WARN:
        SL_PRINT_STRING_WARN("Visible prints: WARN and ERROR. DEBUG/INFO will be filtered out.\r\n");
        SL_PRINT_STRING_WARN("WARN log values: %d %d\r\n", test_number, another_number);
        SL_PRINT_STRING_WARN("WARN log strings: %s\r\n", (uintptr_t)test_str2);
        break;
      case SL_LOG_CONFIG_LEVEL_ERROR:
        SL_PRINT_STRING_ERROR("Visible prints: ERROR only. DEBUG/INFO/WARN will be filtered out.\r\n");
        SL_PRINT_STRING_ERROR("ERROR log values: %d\r\n", i_value + 5);
        SL_PRINT_STRING_ERROR("ERROR log strings: %s\r\n", (uintptr_t)test_str1);
        break;
      default:
        SL_PRINT_STRING_INFO("Unknown log level. No log prints will be visible.\r\n");
        break;
    }

#elif defined(SL_CATALOG_SI91X_LOG_BACKEND_IOSTREAM_FORMATTED_PRESENT)
    // Formatted iostream backend: supports %d, %x, %p, %s, %%.
    // Unlike SystemView, this backend allows MIXING arg types in a single call.
    // Cast every non-pointer arg to (uint32_t) and every pointer/string arg
    // (for %p / %s) to (uintptr_t). Max args per call is SL_LOG_CONFIG_ARG
    // (up to 10).

    // One-shot self-test: exercise every supported format specifier
    // exactly once on the first pass through the formatted-backend branch.
    // The expected output for each line is documented inline in
    // sl_log_run_format_specifier_check().
    static bool ran_format_check = false;
    if (!ran_format_check) {
      ran_format_check = true;
      sl_log_run_format_specifier_check();
    }

    const char *curr_level_str = (log_level <= SL_LOG_CONFIG_LEVEL_NONE) ? level_names[log_level] : "UNKNOWN";
    SL_PRINT_STRING_INFO("Formatted: Current log level: %d (%s)\r\n", (uint32_t)log_level, (uintptr_t)curr_level_str);

    switch (log_level) {
      case SL_LOG_CONFIG_LEVEL_DEBUG:
        SL_PRINT_STRING_DEBUG("All log prints will be visible: DEBUG, INFO, WARN, ERROR.\r\n");
        SL_PRINT_STRING_DEBUG("DEBUG mix: int=%d, str=%s, int=%d\r\n",
                              test_number,
                              (uintptr_t)test_str1,
                              another_number);
        SL_PRINT_STRING_INFO("INFO : str=%s, int=%d\r\n", (uintptr_t)test_str2, i_value);
        SL_PRINT_STRING_WARN("WARN : int=%d, str=%s\r\n", 100, (uintptr_t)test_str1);
        SL_PRINT_STRING_ERROR("ERROR : str=%s, int=%d\r\n", (uintptr_t)test_str2, 999);
        break;
      case SL_LOG_CONFIG_LEVEL_INFO:
        SL_PRINT_STRING_INFO("Visible prints: INFO, WARN, ERROR. DEBUG will be filtered out.\r\n");
        SL_PRINT_STRING_INFO("INFO : int=%d, str=%s\r\n", i_value + 1, (uintptr_t)test_str1);
        SL_PRINT_STRING_WARN("WARN : str=%s, int=%d\r\n", (uintptr_t)test_str2, another_number);
        SL_PRINT_STRING_ERROR("ERROR : int=%d, str=%s\r\n", 500, (uintptr_t)test_str1);
        break;
      case SL_LOG_CONFIG_LEVEL_WARN:
        SL_PRINT_STRING_WARN("Visible prints: WARN and ERROR. DEBUG/INFO will be filtered out.\r\n");
        SL_PRINT_STRING_WARN("WARN : int=%d, str=%s, int=%d\r\n", test_number, (uintptr_t)test_str1, another_number);
        SL_PRINT_STRING_ERROR("ERROR : str=%s, int=%d\r\n", (uintptr_t)test_str2, i_value);
        break;
      case SL_LOG_CONFIG_LEVEL_ERROR:
        SL_PRINT_STRING_ERROR("Visible prints: ERROR only. DEBUG/INFO/WARN will be filtered out.\r\n");
        SL_PRINT_STRING_ERROR("ERROR : int=%d, str=%s, int=%d\r\n", test_number, (uintptr_t)test_str1, i_value);
        break;
      default:
        SL_PRINT_STRING_INFO("Unknown log level. No log prints will be visible.\r\n");
        break;
    }

#else
    // Compact iostream backend (default): only %lu is supported. Args are
    // emitted as raw uint32_t values; strings cannot be rendered here.
    // Multiple %lu args per call are fine (up to SL_LOG_CONFIG_ARG).
    SL_PRINT_STRING_INFO("Compact: Current log level: %lu\r\n", (uint32_t)log_level);

    switch (log_level) {
      case SL_LOG_CONFIG_LEVEL_DEBUG:
        SL_PRINT_STRING_INFO("All log prints will be visible: DEBUG, INFO, WARN, ERROR.\r\n");
        SL_PRINT_STRING_DEBUG("DEBUG : %lu %lu %lu\r\n", test_number, another_number, i_value);
        SL_PRINT_STRING_INFO("INFO : %lu %lu\r\n", (uint32_t)(test_number + 1), (uint32_t)(i_value + 1));
        SL_PRINT_STRING_WARN("WARN : %lu %lu %lu\r\n", (uint32_t)100, (uint32_t)200, (uint32_t)300);
        SL_PRINT_STRING_ERROR("ERROR : %lu\r\n", (uint32_t)(i_value + 5));
        break;
      case SL_LOG_CONFIG_LEVEL_INFO:
        SL_PRINT_STRING_INFO("Visible prints: INFO, WARN, ERROR. DEBUG will be filtered out.\r\n");
        SL_PRINT_STRING_INFO("INFO : %lu %lu\r\n", (uint32_t)(i_value + 2), (uint32_t)(i_value + 3));
        SL_PRINT_STRING_WARN("WARN : %lu %lu\r\n", another_number, test_number);
        SL_PRINT_STRING_ERROR("ERROR : %lu %lu\r\n", (uint32_t)500, i_value);
        break;
      case SL_LOG_CONFIG_LEVEL_WARN:
        SL_PRINT_STRING_WARN("Visible prints: WARN and ERROR. DEBUG/INFO will be filtered out.\r\n");
        SL_PRINT_STRING_WARN("WARN : %lu %lu\r\n", test_number, another_number);
        SL_PRINT_STRING_ERROR("ERROR : %lu\r\n", i_value);
        break;
      case SL_LOG_CONFIG_LEVEL_ERROR:
        SL_PRINT_STRING_ERROR("Visible prints: ERROR only. DEBUG/INFO/WARN will be filtered out.\r\n");
        SL_PRINT_STRING_ERROR("ERROR : %lu %lu %lu\r\n", test_number, another_number, i_value);
        break;
      default:
        break;
    }
#endif

    osDelay(1000);
  }
}

#ifdef SL_CATALOG_SI91X_LOG_BACKEND_IOSTREAM_FORMATTED_PRESENT
/*******************************************************************************
 * Self-test for the SiWx91x formatted iostream log backend.
 *
 * Each call below produces one log line that exercises one feature of the
 * backend; the expected output is given in the inline comment so the
 * captured console output can be eyeball-diff'd or grep'd against it.
 *
 * Conventions enforced by the formatted backend ABI (every arg is delivered
 * as uint32_t):
 *   - For "%s" and "%p", cast the argument to (uintptr_t).
 *   - For "%d" / "%i" / "%u" / "%x" / "%X" / "%o", cast to (uint32_t) /
 *     (int32_t).
 *   - For "%f" / "%F", pass the IEEE-754 bit pattern as uint32_t (use the
 *     SL_LOG_FLOAT_BITS() helper above). Passing a raw float still prints
 *     a sensible (integer-part) value via the salvage path.
 *
 * Each SL_PRINT_STRING_* call is wrapped in its own block, as required by
 * the .slcc documentation.
 ******************************************************************************/
static void sl_log_run_format_specifier_check(void)
{
  /* ---------------------------------------------------------------- *
   * 1. Log level smoke test (one line per level).                    *
   *    Expected header letters: D, I, W, E.                          *
   * ---------------------------------------------------------------- */
  {
    SL_PRINT_STRING_DEBUG("LEVEL DEBUG line\r\n"); /* [D|S|...] LEVEL DEBUG line */
  }
  {
    SL_PRINT_STRING_INFO("LEVEL INFO  line\r\n"); /* [I|S|...] LEVEL INFO  line */
  }
  {
    SL_PRINT_STRING_WARN("LEVEL WARN  line\r\n"); /* [W|S|...] LEVEL WARN  line */
  }
  {
    SL_PRINT_STRING_ERROR("LEVEL ERROR line\r\n"); /* [E|S|...] LEVEL ERROR line */
  }

  /* ---------------------------------------------------------------- *
   * 2. Signed decimal: %d, %i, with width and zero-pad.              *
   * ---------------------------------------------------------------- */
  {
    SL_PRINT_STRING_INFO("d  pos = %d\r\n", (uint32_t)42); /* d  pos = 42              */
  }
  {
    SL_PRINT_STRING_INFO("d  neg = %d\r\n", (uint32_t)-42); /* d  neg = -42             */
  }
  {
    SL_PRINT_STRING_INFO("i  alias = %i\r\n", (uint32_t)-1); /* i  alias = -1            */
  }
  {
    SL_PRINT_STRING_INFO("d  INT_MIN = %d\r\n", (uint32_t)0x80000000); /* d  INT_MIN = -2147483648 */
  }
  {
    SL_PRINT_STRING_INFO("d  width  [%6d]\r\n", (uint32_t)42); /* d  width  [    42]       */
  }
  {
    SL_PRINT_STRING_INFO("d  zero   [%06d]\r\n", (uint32_t)42); /* d  zero   [000042]       */
  }
  {
    SL_PRINT_STRING_INFO("d  neg-w  [%06d]\r\n", (uint32_t)-7); /* d  neg-w  [-00007]       */
  }

  /* ---------------------------------------------------------------- *
   * 3. Unsigned decimal: %u, with width and zero-pad.                *
   * ---------------------------------------------------------------- */
  {
    SL_PRINT_STRING_INFO("u  small = %u\r\n", (uint32_t)42); /* u  small = 42         */
  }
  {
    SL_PRINT_STRING_INFO("u  max   = %u\r\n", (uint32_t)0xFFFFFFFFu); /* u  max   = 4294967295 */
  }
  {
    SL_PRINT_STRING_INFO("u  width [%8u]\r\n", (uint32_t)42); /* u  width [      42]   */
  }
  {
    SL_PRINT_STRING_INFO("u  zero  [%08u]\r\n", (uint32_t)42); /* u  zero  [00000042]   */
  }

  /* ---------------------------------------------------------------- *
   * 4. Hex: %x lowercase, %X uppercase. Default (no width / no flag) *
   *    is 8-digit zero-padded for backwards compatibility.           *
   * ---------------------------------------------------------------- */
  {
    SL_PRINT_STRING_INFO("x  default [%x]\r\n", (uint32_t)0xABCDu); /* x  default [0000abcd] */
  }
  {
    SL_PRINT_STRING_INFO("X  default [%X]\r\n", (uint32_t)0xABCDu); /* X  default [0000ABCD] */
  }
  {
    SL_PRINT_STRING_INFO("x  width   [%4x]\r\n", (uint32_t)0xAu); /* x  width   [   a]     */
  }
  {
    SL_PRINT_STRING_INFO("x  zero    [%04x]\r\n", (uint32_t)0xAu); /* x  zero    [000a]     */
  }
  {
    SL_PRINT_STRING_INFO("X  zero    [%04X]\r\n", (uint32_t)0xAu); /* X  zero    [000A]     */
  }

  /* ---------------------------------------------------------------- *
   * 5. Octal: %o.                                                    *
   * ---------------------------------------------------------------- */
  {
    SL_PRINT_STRING_INFO("o  small = %o\r\n", (uint32_t)8); /* o  small = 10     */
  }
  {
    SL_PRINT_STRING_INFO("o  width [%6o]\r\n", (uint32_t)8); /* o  width [    10] */
  }

  /* ---------------------------------------------------------------- *
   * 6. Pointer: %p always emits "0x" + 8 uppercase hex digits.       *
   * ---------------------------------------------------------------- */
  {
    static const int marker = 0;
    SL_PRINT_STRING_INFO("p  &marker = %p\r\n", (uintptr_t)&marker); /* p  &marker = 0xXXXXXXXX */
  }
  {
    SL_PRINT_STRING_INFO("p  null   = %p\r\n", (uintptr_t)NULL); /* p  null   = 0x00000000 */
  }

  /* ---------------------------------------------------------------- *
   * 7. Character: %c.                                                *
   * ---------------------------------------------------------------- */
  {
    SL_PRINT_STRING_INFO("c  letter = [%c]\r\n", (uint32_t)'Z'); /* c  letter = [Z] */
  }
  {
    SL_PRINT_STRING_INFO("c  digit  = [%c]\r\n", (uint32_t)'7'); /* c  digit  = [7] */
  }

  /* ---------------------------------------------------------------- *
   * 8. String: %s. Pass a real pointer cast to (uintptr_t).          *
   * ---------------------------------------------------------------- */
  {
    static const char *hello = "hello, world";
    SL_PRINT_STRING_INFO("s  msg = '%s'\r\n", (uintptr_t)hello); /* s  msg = 'hello, world' */
  }
  {
    SL_PRINT_STRING_INFO("s  null = '%s'\r\n", (uintptr_t)NULL); /* s  null = '' */
  }

  /* ---------------------------------------------------------------- *
   * 9. Float: %f / %F via SL_LOG_FLOAT_BITS().                       *
   *    Default precision is 6, max 9.                                *
   * ---------------------------------------------------------------- */
  {
    SL_PRINT_STRING_INFO("f  default     = %f\r\n", SL_LOG_FLOAT_BITS(3.14159265f)); /* 3.141592    */
  }
  {
    SL_PRINT_STRING_INFO("f  prec 0      = %.0f\r\n", SL_LOG_FLOAT_BITS(3.7f)); /* 3           */
  }
  {
    SL_PRINT_STRING_INFO("f  prec 2      = %.2f\r\n", SL_LOG_FLOAT_BITS(2.718f)); /* 2.71        */
  }
  {
    SL_PRINT_STRING_INFO("f  prec 9 max  = %.9f\r\n", SL_LOG_FLOAT_BITS(0.123456789f)); /* 0.123456789 */
  }
  {
    SL_PRINT_STRING_INFO("f  negative    = %.2f\r\n", SL_LOG_FLOAT_BITS(-12.5f)); /* -12.50      */
  }
  {
    SL_PRINT_STRING_INFO("F  uppercase   = %.2F\r\n", SL_LOG_FLOAT_BITS(1.25f)); /* 1.25        */
  }

  /* Float salvage path: pass a raw float (not bit-cast). The backend
   * detects the resulting denormal bit pattern and recovers the integer
   * part instead of printing 0.000000. */
  {
    SL_PRINT_STRING_INFO("f  salvage     = %.2f\r\n", (uint32_t)2); /* 2.00 */
  }

  /* Float specials. */
  {
    union {
      float f;
      uint32_t u;
    } posinf = { .u = 0x7F800000u };
    union {
      float f;
      uint32_t u;
    } neginf = { .u = 0xFF800000u };
    union {
      float f;
      uint32_t u;
    } nan_v = { .u = 0x7FC00000u };
    SL_PRINT_STRING_INFO("f  +inf = %f\r\n", posinf.u); /* inf  */
    SL_PRINT_STRING_INFO("f  -inf = %f\r\n", neginf.u); /* -inf */
    SL_PRINT_STRING_INFO("f  nan  = %f\r\n", nan_v.u);  /* nan  */
  }
  /* Out-of-uint32-range value: prints "ovf". */
  {
    SL_PRINT_STRING_INFO("f  ovf  = %f\r\n", SL_LOG_FLOAT_BITS(5e9f)); /* ovf */
  }

  /* ---------------------------------------------------------------- *
   * 10. Literal '%%'.                                                *
   * ---------------------------------------------------------------- */
  {
    SL_PRINT_STRING_INFO("literal percent: 100%%\r\n"); /* literal percent: 100% */
  }

  /* ---------------------------------------------------------------- *
   * 11. Length modifiers (h, hh, l, ll, z, t, j) - accepted but      *
   *     ignored; arg is still uint32_t.                              *
   * ---------------------------------------------------------------- */
  {
    SL_PRINT_STRING_INFO("len  ld   = %ld\r\n", (uint32_t)-1); /* -1       */
  }
  {
    SL_PRINT_STRING_INFO("len  lld  = %lld\r\n", (uint32_t)123); /* 123      */
  }
  {
    SL_PRINT_STRING_INFO("len  hhx  = %hhx\r\n", (uint32_t)0xFFu); /* 000000ff */
  }
  {
    SL_PRINT_STRING_INFO("len  zu   = %zu\r\n", (uint32_t)4096); /* 4096     */
  }

  /* ---------------------------------------------------------------- *
   * 12. Mixed-specifier line (typical real-world usage).             *
   * ---------------------------------------------------------------- */
  {
    static const char *tag = "ADC";
    SL_PRINT_STRING_INFO("[%s] count=%d addr=%p st=0x%04X t=%.2f\r\n",
                         (uintptr_t)tag,
                         (uint32_t)-1,
                         (uintptr_t)&tag,
                         (uint32_t)0x3u,
                         SL_LOG_FLOAT_BITS(23.75f));
    /* Expected:
     *   [ADC] count=-1 addr=0xXXXXXXXX st=0x0003 t=23.75
     */
  }

  /* ---------------------------------------------------------------- *
   * 13. Diagnostic markers (intentional misuse, must NOT crash).     *
   *     - Too few args -> visible "<%d?>" marker for the missing one.*
   *     - Unknown specifier -> echoed literally.                     *
   * ---------------------------------------------------------------- */
  {
    SL_PRINT_STRING_WARN("diag  too-few-args: a=%d b=%d c=%d\r\n", (uint32_t)1, (uint32_t)2);
    /* a=1 b=2 c=<%d?> */
  }
  {
    SL_PRINT_STRING_WARN("diag  unknown spec: %q done\r\n", (uint32_t)0); /* unknown spec: %q done */
  }

  /* ---------------------------------------------------------------- *
   * 14. Width-overflow safety check (parser must clamp).             *
   *     "%260d" used to wrap to width=4; now clamps to 99 cleanly.   *
   * ---------------------------------------------------------------- */
  {
    SL_PRINT_STRING_DEBUG("wide [%260d]\r\n", (uint32_t)1);
    /* Expected: a single line with "1" preceded by ~99 spaces, no overflow. */
  }

  /* ---------------------------------------------------------------- *
   * 15. Event-mode path (TYPE = 'E'). Up to SL_LOG_CONFIG_ARG args.  *
   *     Header should read [.|E|TIMESTAMP] and body shows hex args.  *
   * ---------------------------------------------------------------- */
  {
    SL_PRINT_EVENT_INFO(0x12345678u); /* event no args */
  }
  {
    SL_PRINT_EVENT_INFO(0xDEADBEEFu, (uint32_t)0xAABBCCDDu); /* event 1 arg   */
  }
  {
    SL_PRINT_EVENT_DEBUG(0xCAFEBABEu, (uint32_t)0x11111111u, (uint32_t)0x22222222u); /* event 2 args  */
  }
}
#endif /* SL_CATALOG_SI91X_LOG_BACKEND_IOSTREAM_FORMATTED_PRESENT */

/*******************************************************************************
   **************************** Init Entry ****************************************
   ******************************************************************************/
// Public function called by system startup to create the main app task
void logger_example_init(void)
{
  // Start application orchestrator
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}
