/*
 *
 *    Atollic TrueSTUDIO Minimal System calls file
 *
 *    For more information about which c-functions
 *    need which of these lowlevel functions
 *    please consult the Newlib libc-manual
 *
 * */
#ifndef IOSTREAM_USART
#include <sys/stat.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include "rsi_board.h"

#define IO_MAXLINE 20U //maximun read length
extern int errno;
typedef int (*PUTCHAR_FUNC)(int a);
register char *stack_ptr __asm("sp");
extern void Serial_send(uint8_t ch);
extern char __HeapBase[];
extern char __HeapLimit[];

char *__env[1] = { 0 };
char **environ = __env;
uint8_t buf[20];

/*! @brief Specification modifier flags for scanf. */
enum _debugconsole_scanf_flag {
  kSCANF_Suppress   = 0x2U,  /*!< Suppress Flag. */
  kSCANF_DestMask   = 0x7cU, /*!< Destination Mask. */
  kSCANF_DestChar   = 0x4U,  /*!< Destination Char Flag. */
  kSCANF_DestString = 0x8U,  /*!< Destination String FLag. */
  kSCANF_DestSet    = 0x10U, /*!< Destination Set Flag. */
  kSCANF_DestInt    = 0x20U, /*!< Destination Int Flag. */
  kSCANF_DestFloat  = 0x30U, /*!< Destination Float Flag. */
  kSCANF_LengthMask = 0x1f00U,
  /*!< Length Mask Flag. */    /*PRINTF_FLOAT_ENABLE */
  kSCANF_TypeSinged = 0x2000U, /*!< TypeSinged Flag. */
};
void initialise_monitor_handles()
{
}

int _getpid(void)
{
  return 1;
}

int _kill(int pid, int sig)
{
  (void)pid;
  (void)sig;
  errno = EINVAL;
  return -1;
}

void _exit(int status)
{
  _kill(status, -1);
  while (1) {
  } /* Make sure we hang here */
}

#if defined SYSCALLS_WRITE
int _write(int file, char *ptr, int len)
{
  int todo;
  (void)file;

  for (todo = 0; todo < len; todo++) {
#ifdef DEBUG_SERIAL
    Serial_send(*ptr++);

#else
    Board_UARTPutChar(*ptr++);

#endif
  }

  return len;
}
#endif //SYSCALLS_WRITE

#ifndef SL_WIFI_COMPONENT_INCLUDED
void *_sbrk(int incr)
{
  static char *heap_end = __HeapBase;
  char *prev_heap_end;

  if ((heap_end + incr) > __HeapLimit) {
    // Not enough heap
    return (void *)-1;
  }

  prev_heap_end = heap_end;
  heap_end += incr;

  return prev_heap_end;
}
#endif

int _close(int file)
{
  (void)file;
  return -1;
}

int _fstat(int file, struct stat *st)
{
  (void)file;
  st->st_mode = S_IFCHR;
  return 0;
}

int _isatty(int file)
{
  (void)file;
  return 1;
}

int _lseek(int file, int ptr, int dir)
{
  (void)file;
  (void)ptr;
  (void)dir;
  return 0;
}

static uint32_t read_exclude_space(const char **s)
{
  uint8_t count = 0;
  uint8_t c;

  c = **s;
  while ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r') || (c == '\v') || (c == '\f')) {
    count++;
    (*s)++;
    c = **s;
  }
  return count;
}

static int scanf_data_format(const char *line_ptr, char *format, va_list args_ptr)
{
  uint8_t base;
  int8_t neg;
  /* Identifier for the format string. */
  char *c = format;
  char temp;
  char *buff;
  /* Flag telling the conversion specification. */
  uint32_t flag = 0;
  /* Filed width for the matching input streams. */
  uint32_t field_width;
  /* How many arguments are assigned except the suppress. */
  uint32_t nassigned = 0;
  /* How many characters are read from the input streams. */
  uint32_t n_decode = 0;

  int32_t val;

  const char *s;
  /* Identifier for the input string. */
  const char *p = line_ptr;

  /* Return EOF error before any conversion. */
  if (*p == '\0') {
    return -1;
  }

  /* Decode directives. */
  while ((*c) && (*p)) {
    /* Ignore all white-spaces in the format strings. */
    if (read_exclude_space((const char **)&c)) {
      n_decode += read_exclude_space(&p);
    } else if ((*c != '%') || ((*c == '%') && (*(c + 1) == '%'))) {
      /* Ordinary characters. */
      c++;
      if (*p == *c) {
        n_decode++;
        p++;
        c++;
      } else {
        /* Match failure. Misalignment with C99, the unmatched characters need to be pushed back to stream.
         * However, it is deserted now. */
        break;
      }
    } else {
      /* convernsion specification */
      c++;
      /* Reset. */
      flag        = 0;
      field_width = 0;
      base        = 0;

      /* Loop to get full conversion specification. */
      while ((*c) && (!(flag & kSCANF_DestMask))) {
        switch (*c) {

          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            if (field_width) {
              /* Match failure. */
              return (int)nassigned;
            }
            do {
              field_width = field_width * 10 + *c - '0';
              c++;
            } while ((*c >= '0') && (*c <= '9'));
            break;
          case 'd':
            base = 10;
            flag |= kSCANF_TypeSinged;
            flag |= kSCANF_DestInt;
            c++;
            break;
          case 'u':
            base = 10;
            flag |= kSCANF_DestInt;
            c++;
            break;
          case 'o':
            base = 8;
            flag |= kSCANF_DestInt;
            c++;
            break;
          case 'x':
          case 'X':
            base = 16;
            flag |= kSCANF_DestInt;
            c++;
            break;
          case 'i':
            base = 0;
            flag |= kSCANF_DestInt;
            c++;
            break;

          case 'c':
            flag |= kSCANF_DestChar;
            if (!field_width) {
              field_width = 1;
            }
            c++;
            break;
          case 's':
            flag |= kSCANF_DestString;
            c++;
            break;
          default:
            return (int)nassigned;
        }
      }

      if (!(flag & kSCANF_DestMask)) {
        /* Format strings are exhausted. */
        return (int)nassigned;
      }

      if (!field_width) {
        /* Large than length of a line. */
        field_width = 99;
      }

      /* Matching strings in input streams and assign to argument. */
      switch (flag & kSCANF_DestMask) {
        case kSCANF_DestChar:
          s    = (const char *)p;
          buff = va_arg(args_ptr, char *);
          while ((field_width--) && (*p)) {
            if (!(flag & kSCANF_Suppress)) {
              *buff++ = *p++;
            } else {
              p++;
            }
            n_decode++;
          }

          if ((!(flag & kSCANF_Suppress)) && (s != p)) {
            nassigned++;
          }
          break;
        case kSCANF_DestString:
          n_decode += read_exclude_space(&p);
          s    = p;
          buff = va_arg(args_ptr, char *);
          while ((field_width--) && (*p != '\0') && (*p != ' ') && (*p != '\t') && (*p != '\n') && (*p != '\r')
                 && (*p != '\v') && (*p != '\f')) {
            if (flag & kSCANF_Suppress) {
              p++;
            } else {
              *buff++ = *p++;
            }
            n_decode++;
          }

          if ((!(flag & kSCANF_Suppress)) && (s != p)) {
            /* Add NULL to end of string. */
            *buf = '\0';
            nassigned++;
          }
          break;
        case kSCANF_DestInt:
          n_decode += read_exclude_space(&p);
          s   = p;
          val = 0;
          if ((base == 0) || (base == 16)) {
            if ((s[0] == '0') && ((s[1] == 'x') || (s[1] == 'X'))) {
              base = 16;
              if (field_width >= 1) {
                p += 2;
                n_decode += 2;
                field_width -= 2;
              }
            }
          }

          if (base == 0) {
            if (s[0] == '0') {
              base = 8;
            } else {
              base = 10;
            }
          }

          neg = 1;
          switch (*p) {
            case '-':
              neg = -1;
              n_decode++;
              p++;
              field_width--;
              break;
            case '+':
              neg = 1;
              n_decode++;
              p++;
              field_width--;
              break;
            default:
              break;
          }

          while ((*p) && (field_width--)) {
            if ((*p <= '9') && (*p >= '0')) {
              temp = *p - '0';
            } else if ((*p <= 'f') && (*p >= 'a')) {
              temp = *p - 'a' + 10;
            } else if ((*p <= 'F') && (*p >= 'A')) {
              temp = *p - 'A' + 10;
            } else {
              temp = base;
            }

            if (temp >= base) {
              break;
            } else {
              val = base * val + temp;
            }
            p++;
            n_decode++;
          }
          val *= neg;
          if (!(flag & kSCANF_Suppress)) {

            /* The default type is the type int. */
            if (flag & kSCANF_TypeSinged) {
              *va_arg(args_ptr, signed int *) = (signed int)val;
            } else {
              *va_arg(args_ptr, unsigned int *) = (unsigned int)val;
            }

            nassigned++;
          }
          break;

        default:
          return (int)nassigned;
      }
    }
  }
  return (int)nassigned;
}
int _read(char *fmt_ptr, ...)
{

  char temp_buf[IO_MAXLINE + 1];
  va_list ap;
  uint32_t i;
  char result;

  va_start(ap, fmt_ptr);
  temp_buf[0] = '\0';

  for (i = 0; i < (uint32_t)IO_MAXLINE; i++) {
#ifdef DEBUG_SERIAL
    temp_buf[i] = result = Serial_receive();
#else
    temp_buf[i] = result = (char)Board_UARTGetChar();
#endif
    if ((result == '\r') || (result == '\n')) {
      /* End of Line. */
      if (i == 0) {
        temp_buf[i] = '\0';
        i           = (uint32_t)-1;
      } else {
        break;
      }
    }
  }

  if ((i == IO_MAXLINE)) {
    temp_buf[i] = '\0';
  } else {
    temp_buf[i + 1] = '\0';
  }
  result = (char)scanf_data_format(temp_buf, fmt_ptr, ap);
  va_end(ap);

  return result;
}

int _open(char *path, int flags, ...)
{
  (void)path;
  (void)flags;
  /* Pretend like we always fail */
  return -1;
}

int _wait(int *status)
{
  (void)status;
  errno = ECHILD;
  return -1;
}

int _unlink(char *name)
{
  (void)name;
  errno = ENOENT;
  return -1;
}

int _times(struct tms *buff)
{
  (void)buff;
  return -1;
}

int _stat(char *file, struct stat *st)
{
  (void)file;
  st->st_mode = S_IFCHR;
  return 0;
}

int _link(char *old_link, char *new_link)
{
  (void)old_link; //This statement is added only to resolve compilation warning, value is unchanged
  (void)new_link; //This statement is added only to resolve compilation warning, value is unchanged
  errno = EMLINK;
  return -1;
}

int _fork(void)
{
  errno = EAGAIN;
  return -1;
}

int _execve(char *name, char **argv, char **env)
{
  (void)name;
  (void)argv;
  (void)env;
  errno = ENOMEM;
  return -1;
}
#endif // IOSTREAM_USART