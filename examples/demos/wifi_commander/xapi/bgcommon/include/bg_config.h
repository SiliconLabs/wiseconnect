#ifndef BG_CONFIG_H_
#define BG_CONFIG_H_

#include <stdint.h>

#define BG_EVENT_HANDLERS_MAX 5
#define BG_EVENT_MESSAGE      0x12345678 //message sent when event arrives

#define BGAPI_PAYLOAD_SIZE 256

//enable debugging if not disabled
#ifndef NDEBUG
#define BG_DEBUG
#endif

//bgbuf config
#define BGBUF_DATA_SIZE         (69)
#define BGBUF_HEADER_RESERVE    (9)
#define BGBUF_TAILER_RESERVE    (0)
#define BGBUF_GENERAL_DATA_SIZE (BGBUF_DATA_SIZE - BGBUF_HEADER_RESERVE - BGBUF_TAILER_RESERVE)

typedef uint8_t bgbuf_size_t;
#define BGBUF_IN_RESERVE     6  /*BGBUFS min reservation for incoming data packets*/
#define BGBUF_OUT_RESERVE    12 /*BGBUFS min reservation for outgoing data packets*/
#define BGBUF_NOFLOW_RESERVE 8  /*BGBUFS min reservation for control packets*/

#define BG_POOL_BGBUF_SZ (sizeof(bgbuf) + BGBUF_DATA_SIZE)
//bgbuf config ends

//BG Message pool size
#define BGMESSAGE_POOL_SIZE 20

#if FEATURE_DEBUG && 0
#if FEATURE_STDLIB
#define DEBUGP(...)                                                  \
  {                                                                  \
    fprintf(stderr, "%s:%d:%s\t", __FILE__, __LINE__, __FUNCTION__); \
    fprintf(stderr, __VA_ARGS__);                                    \
    fflush(stderr);                                                  \
  }
#define DEBUGPR(...)              \
  {                               \
    fprintf(stderr, __VA_ARGS__); \
  }

#else
#define DEBUGP(...)                                             \
  {                                                             \
    tfp_printf("%s:%d:%s\t", __FILE__, __LINE__, __FUNCTION__); \
    tfp_printf(__VA_ARGS__);                                    \
  }
#define DEBUGPR(...)         \
  {                          \
    tfp_printf(__VA_ARGS__); \
  }
#endif
#else
#define DEBUGP(...)
#define DEBUGPR(...)
#endif

#endif /* CONFIG_H_ */
