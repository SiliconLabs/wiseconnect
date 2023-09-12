#include "../offset.h"

typedef unsigned char uint8;
//! char is mapped to int8
typedef signed char int8;
//! unsigned short int is mapped as uint16
typedef unsigned short int uint16;
//! short int is mapped as int16
typedef short int int16;
//! unsigned int is mapped as uint32
typedef unsigned int uint32;
//! int is mapped as uint32
typedef int int32;

typedef struct bootload_entry_s {
  struct control_s {
    uint32 len : 24; /* Length of the transfer to the dst_addr */
    uint32 reserved : 3;
    uint32 spi_32bitmode : 1; /* if set spi reads will be done in 32 bit mode and dma writes will be in 32 bit mode */
    uint32 release_ta_softreset : 1; /* If set TA will be released from soft reset */
    uint32 start_from_rom_pc : 1;    /* if set TA PC starts from ROM, else will start from zero */
    uint32
      spi_8bitmode : 1;    /* if spi_32bitmode is reset, this bit is considered. If set, spi read will be in 8 bit mode,
                                             else will be in 16 bit mode. DMA writes will always be in 16 bit mode. */
    uint32 last_entry : 1; /* If set this is treated as last entry for transfer */
  } control;
  uint32 dst_addr; /* Destination address */
} __attribute__((__packed__)) bootload_entry_t;
typedef struct bootload_ds_s {
  uint16 fixed_pattern;
  uint16 offset;
  uint32 ivt_offset;
  struct bootload_entry_s bl_entry[7];
} __attribute__((__packed__)) bootload_ds_t;
struct bootload_ds_s secondbootload_ds __attribute__((section(".secondboot_ds_mem")));
struct bootload_ds_s secondbootload_ds = {
  .fixed_pattern                            = _fixed_pattern,
  .offset                                   = _offset,
  .ivt_offset                               = _ivt_offset,
  .bl_entry[0].control.len                  = _control_len,
  .bl_entry[0].control.reserved             = _bl_entry_control_reserved,
  .bl_entry[0].control.spi_32bitmode        = _bl_entry_control_spi_32bitmode,
  .bl_entry[0].control.release_ta_softreset = _bl_entry_control_release_ta_softreset,
  .bl_entry[0].control.start_from_rom_pc    = _bl_entry_control_start_from_rom_pc,
  .bl_entry[0].control.spi_8bitmode         = _bl_entry_control_spi_8bitmode,
  .bl_entry[0].control.last_entry           = _bl_entry_control_last_entry,
  .bl_entry[0].dst_addr                     = _bl_entry_dst_addr,
};

int main()
{
  return 0;
}
