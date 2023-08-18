#pragma once
#include "si91x_device.h"
#ifndef SL_NVM3_PRESENT
#include "rsi_m4.h"
#endif

#ifndef EXT_IRQ_COUNT
#define EXT_IRQ_COUNT 96 // 98
#endif

#ifndef SRAM_BASE
#define SRAM_BASE 0x0
#endif

#ifndef SRAM_SIZE
#define SRAM_SIZE 0x2FC00
#endif
