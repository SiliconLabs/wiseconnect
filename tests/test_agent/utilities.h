#pragma once

#include <stdint.h>

// encode a buffer to a base64-encoded string.
// Note:  caller must call free on returned pointer
char *base64_encode(const uint8_t *binary_data, uint32_t length);
