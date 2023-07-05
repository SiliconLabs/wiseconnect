/*******************************************************************************
* @file  dma.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
/*************************************************************************
 *
 */

#ifndef _DMA_H
#define _DMA_H

//#include "data_types.h"

//#include "soc_intf.h"
#include "stdint.h"
#if 1
#define DMA_BASE_ADDR 0x21000000

/* channel 1 defines */
#define DMA_CH1_SRC_ADDR_L  *(volatile uint16_t *)(DMA_BASE_ADDR + (0x00))
#define DMA_CH1_SRC_ADDR_H  *(volatile uint16_t *)(DMA_BASE_ADDR + (0x02))
#define DMA_CH1_DEST_ADDR_L *(volatile uint16_t *)(DMA_BASE_ADDR + (0x04))
#define DMA_CH1_DEST_ADDR_H *(volatile uint16_t *)(DMA_BASE_ADDR + (0x06))
#define DMA_CH1_TRANS_LEN   *(volatile uint16_t *)(DMA_BASE_ADDR + (0x08))
#define DMA_CH1_CTRL        *(volatile uint16_t *)(DMA_BASE_ADDR + (0x0a))
#define DMA_CH1_CTRL_1      *(volatile uint16_t *)(DMA_BASE_ADDR + (0x0c))
#define DMA_CH1_STATUS      *(volatile uint16_t *)(DMA_BASE_ADDR + (0xe))
typedef struct dma_ch_list_addr_s {
  volatile uint16_t dma_ch1_lista_addr_l;
  volatile uint16_t dma_ch1_lista_addr_h;
  volatile uint16_t dma_ch1_ctrl_2;
  volatile uint16_t dma_ch2_lista_addr_l;
  volatile uint16_t dma_ch2_lista_addr_h;
  volatile uint16_t dma_ch2_ctrl2;
  volatile uint16_t dma_ch3_lista_addr_l;
  volatile uint16_t dma_ch3_lista_addr_h;
  //!Channel3 Cntrl reg is supposed to be after address of chan3.
  //!But it is not defined in h file. So Interting dummy for continous address
  volatile uint16_t dummy1;
  volatile uint16_t dma_ch4_lista_addr_l;
  volatile uint16_t dma_ch4_lista_addr_h;
} dma_ch_list_addr_t;

#define DMA_CH1_LISTA_ADDR_L     *(volatile uint16_t *)(DMA_BASE_ADDR + 0x80)
#define DMA_CH1_LISTA_ADDR_H     *(volatile uint16_t *)(DMA_BASE_ADDR + 0x82)
#define DMA_CH1_CTRL_2           *(volatile uint16_t *)(DMA_BASE_ADDR + 0x84)
#define DMA_CH1_LISTB_ADDR_L     *(volatile uint16_t *)(DMA_BASE_ADDR + 0xB0)
#define DMA_CH1_LISTB_ADDR_H     *(volatile uint16_t *)(DMA_BASE_ADDR + 0xB2)
#define DMA_CH1_PING_TCP_CSUM    *(volatile uint16_t *)(DMA_BASE_ADDR + 0xD0)
#define DMA_CH1_PONG_TCP_CSUM    *(volatile uint16_t *)(DMA_BASE_ADDR + 0xE0)
#define DMA_CH1_PING_TCP_PKT_LEN *(volatile uint16_t *)(DMA_BASE_ADDR + 0xE4)
#define DMA_CH1_PONG_TCP_PKT_LEN *(volatile uint16_t *)(DMA_BASE_ADDR + 0xE8)

/* channel 2 defines */
#define DMA_CH2_SRC_ADDR_L  *(volatile uint16_t *)(DMA_BASE_ADDR + (0x10))
#define DMA_CH2_SRC_ADDR_H  *(volatile uint16_t *)(DMA_BASE_ADDR + (0x12))
#define DMA_CH2_DEST_ADDR_L *(volatile uint16_t *)(DMA_BASE_ADDR + (0x14))
#define DMA_CH2_DEST_ADDR_H *(volatile uint16_t *)(DMA_BASE_ADDR + (0x16))
#define DMA_CH2_TRANS_LEN   *(volatile uint16_t *)(DMA_BASE_ADDR + (0x18))
#define DMA_CH2_CTRL        *(volatile uint16_t *)(DMA_BASE_ADDR + (0x1a))
#define DMA_CH2_CTRL_1      *(volatile uint16_t *)(DMA_BASE_ADDR + (0x1c))
#define DMA_CH2_STATUS      *(volatile uint16_t *)(DMA_BASE_ADDR + (0x1e))

#define DMA_CH2_CTRL2            *(volatile uint16_t *)(DMA_BASE_ADDR + 0x8A)
#define DMA_CH2_LISTA_ADDR_L     *(volatile uint16_t *)(DMA_BASE_ADDR + 0x86)
#define DMA_CH2_LISTA_ADDR_H     *(volatile uint16_t *)(DMA_BASE_ADDR + 0x88)
#define DMA_CH2_LISTB_ADDR_L     *(volatile uint16_t *)(DMA_BASE_ADDR + 0xB4)
#define DMA_CH2_LISTB_ADDR_H     *(volatile uint16_t *)(DMA_BASE_ADDR + 0xB6)
#define DMA_CH2_PING_TCP_CSUM    *(volatile uint16_t *)(DMA_BASE_ADDR + 0xD2)
#define DMA_CH2_PONG_TCP_CSUM    *(volatile uint16_t *)(DMA_BASE_ADDR + 0xE2)
#define DMA_CH2_PING_TCP_PKT_LEN *(volatile uint16_t *)(DMA_BASE_ADDR + 0xE6)
#define DMA_CH2_PONG_TCP_PKT_LEN *(volatile uint16_t *)(DMA_BASE_ADDR + 0xEA)

#define DMA_TCP_CKSM_OFFSET *(volatile uint16_t *)(DMA_BASE_ADDR + 0xF0)

/* Channel3 registers */
#define DMA_CH3_SRC_ADDR_L   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x20)
#define DMA_CH3_SRC_ADDR_H   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x22)
#define DMA_CH3_DST_ADDR_L   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x24)
#define DMA_CH3_DST_ADDR_H   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x26)
#define DMA_CH3_TRANS_LEN    *(volatile uint16_t *)(DMA_BASE_ADDR + 0x28)
#define DMA_CH3_CTRL0        *(volatile uint16_t *)(DMA_BASE_ADDR + 0x2A)
#define DMA_CH3_CTRL1        *(volatile uint16_t *)(DMA_BASE_ADDR + 0x2C)
#define DMA_CH3_STATUS       *(volatile uint16_t *)(DMA_BASE_ADDR + 0x2E)
#define DMA_CH3_LISTA_ADDR_L *(volatile uint16_t *)(DMA_BASE_ADDR + 0x8C)
#define DMA_CH3_LISTA_ADDR_H *(volatile uint16_t *)(DMA_BASE_ADDR + 0x8E)

/* Channel4 registers */
#define DMA_CH4_SRC_ADDR_L   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x30)
#define DMA_CH4_SRC_ADDR_H   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x32)
#define DMA_CH4_DST_ADDR_L   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x34)
#define DMA_CH4_DST_ADDR_H   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x36)
#define DMA_CH4_TRANS_LEN    *(volatile uint16_t *)(DMA_BASE_ADDR + 0x38)
#define DMA_CH4_CTRL0        *(volatile uint16_t *)(DMA_BASE_ADDR + 0x3A)
#define DMA_CH4_CTRL1        *(volatile uint16_t *)(DMA_BASE_ADDR + 0x3C)
#define DMA_CH4_STATUS       *(volatile uint16_t *)(DMA_BASE_ADDR + 0x3E)
#define DMA_CH4_LISTA_ADDR_L *(volatile uint16_t *)(DMA_BASE_ADDR + 0x92)
#define DMA_CH4_LISTA_ADDR_H *(volatile uint16_t *)(DMA_BASE_ADDR + 0x94)

/* Channel5 registers */
#define DMA_CH5_SRC_ADDR_L   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x40)
#define DMA_CH5_SRC_ADDR_H   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x42)
#define DMA_CH5_DST_ADDR_L   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x44)
#define DMA_CH5_DST_ADDR_H   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x46)
#define DMA_CH5_TRANS_LEN    *(volatile uint16_t *)(DMA_BASE_ADDR + 0x48)
#define DMA_CH5_CTRL0        *(volatile uint16_t *)(DMA_BASE_ADDR + 0x4A)
#define DMA_CH5_CTRL1        *(volatile uint16_t *)(DMA_BASE_ADDR + 0x4C)
#define DMA_CH5_STATUS       *(volatile uint16_t *)(DMA_BASE_ADDR + 0x4E)
#define DMA_CH5_LISTA_ADDR_L *(volatile uint16_t *)(DMA_BASE_ADDR + 0x98)
#define DMA_CH5_LISTA_ADDR_H *(volatile uint16_t *)(DMA_BASE_ADDR + 0x9A)

/* Channel6 registers */
#define DMA_CH6_SRC_ADDR_L   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x50)
#define DMA_CH6_SRC_ADDR_H   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x52)
#define DMA_CH6_DST_ADDR_L   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x54)
#define DMA_CH6_DST_ADDR_H   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x56)
#define DMA_CH6_TRANS_LEN    *(volatile uint16_t *)(DMA_BASE_ADDR + 0x58)
#define DMA_CH6_CTRL0        *(volatile uint16_t *)(DMA_BASE_ADDR + 0x5A)
#define DMA_CH6_CTRL1        *(volatile uint16_t *)(DMA_BASE_ADDR + 0x5C)
#define DMA_CH6_STATUS       *(volatile uint16_t *)(DMA_BASE_ADDR + 0x5E)
#define DMA_CH6_LISTA_ADDR_L *(volatile uint16_t *)(DMA_BASE_ADDR + 0x9E)
#define DMA_CH6_LISTA_ADDR_H *(volatile uint16_t *)(DMA_BASE_ADDR + 0xA0)

/* Channel7 registers */
#define DMA_CH7_SRC_ADDR_L   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x60)
#define DMA_CH7_SRC_ADDR_H   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x62)
#define DMA_CH7_DST_ADDR_L   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x64)
#define DMA_CH7_DST_ADDR_H   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x66)
#define DMA_CH7_TRANS_LEN    *(volatile uint16_t *)(DMA_BASE_ADDR + 0x68)
#define DMA_CH7_CTRL0        *(volatile uint16_t *)(DMA_BASE_ADDR + 0x6A)
#define DMA_CH7_CTRL1        *(volatile uint16_t *)(DMA_BASE_ADDR + 0x6C)
#define DMA_CH7_STATUS       *(volatile uint16_t *)(DMA_BASE_ADDR + 0x6E)
#define DMA_CH7_LISTA_ADDR_L *(volatile uint16_t *)(DMA_BASE_ADDR + 0xA4)
#define DMA_CH7_LISTA_ADDR_H *(volatile uint16_t *)(DMA_BASE_ADDR + 0xA6)

/* Channel8 registers */
#define DMA_CH8_SRC_ADDR_L   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x70)
#define DMA_CH8_SRC_ADDR_H   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x72)
#define DMA_CH8_DST_ADDR_L   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x74)
#define DMA_CH8_DST_ADDR_H   *(volatile uint16_t *)(DMA_BASE_ADDR + 0x76)
#define DMA_CH8_TRANS_LEN    *(volatile uint16_t *)(DMA_BASE_ADDR + 0x78)
#define DMA_CH8_CTRL0        *(volatile uint16_t *)(DMA_BASE_ADDR + 0x7A)
#define DMA_CH8_CTRL1        *(volatile uint16_t *)(DMA_BASE_ADDR + 0x7C)
#define DMA_CH8_STATUS       *(volatile uint16_t *)(DMA_BASE_ADDR + 0x7E)
#define DMA_CH8_LISTA_ADDR_L *(volatile uint16_t *)(DMA_BASE_ADDR + 0xAA)
#define DMA_CH8_LISTA_ADDR_H *(volatile uint16_t *)(DMA_BASE_ADDR + 0xAC)

/* Generic DMA channel registers */
#define DMA_CHn_SRC_ADDR_L(n) (*(volatile uint16_t *)(DMA_BASE_ADDR + (0x10 * (n - 1)) + 0x0))
#define DMA_CHn_SRC_ADDR_H(n) (*(volatile uint16_t *)(DMA_BASE_ADDR + (0x10 * (n - 1)) + 0x2))
#define DMA_CHn_DST_ADDR_L(n) (*(volatile uint16_t *)(DMA_BASE_ADDR + (0x10 * (n - 1)) + 0x4))
#define DMA_CHn_DST_ADDR_H(n) (*(volatile uint16_t *)(DMA_BASE_ADDR + (0x10 * (n - 1)) + 0x6))
#define DMA_CHn_TRANS_LEN(n)  (*(volatile uint16_t *)(DMA_BASE_ADDR + (0x10 * (n - 1)) + 0x8))
#define DMA_CHn_CTRL0(n)      (*(volatile uint16_t *)(DMA_BASE_ADDR + (0x10 * (n - 1)) + 0xA))
#define DMA_CHn_CTRL1(n)      (*(volatile uint16_t *)(DMA_BASE_ADDR + (0x10 * (n - 1)) + 0xC))
#define DMA_CHn_STATUS(n)     (*(volatile uint16_t *)(DMA_BASE_ADDR + (0x10 * (n - 1)) + 0xE))

#define DMA_CHn_CTRL2(n)          (*(volatile uint16_t *)(DMA_BASE_ADDR + (0x6 * (n - 1)) + 0x84))
#define DMA_CHn_DESC1_BASE_L(n)   (*(volatile uint16_t *)(DMA_BASE_ADDR + (0x6 * (n - 1)) + 0x80))
#define DMA_CHn_DESC1_BASE_H(n)   (*(volatile uint16_t *)(DMA_BASE_ADDR + (0x6 * (n - 1)) + 0x82))
#define DMA_CHn_DESC2_BASE_L(n)   (*(volatile uint16_t *)(DMA_BASE_ADDR + (0x6 * (n - 1)) + 0xB0))
#define DMA_CHn_DESC2_BASE_H(n)   (*(volatile uint16_t *)(DMA_BASE_ADDR + (0x6 * (n - 1)) + 0xB2))
#define DMA_ADDR_INCR_DISABLE_REG (*(volatile uint16_t *)(DMA_BASE_ADDR + 0xDC))

#define CHANNEL_NUM_MASK                 0xFF
#define SRC_ADDR_DONT_INC                BIT(8)
#define DES_ADDR_DONT_INC                BIT(9)
#define WAIT_FOR_DMA_DONE_BEFORE_PROGRAM BIT(10)
#define WAIT_FOR_DMA_DONE_AFTER_PROGRAM  BIT(11)
#define DMA_INTERRUPT_MODE               BIT(12)

/* Control register0 bit definitions */
#define DMA_CTRL0_TRGR            BIT(9)
#define DMA_CTRL0_TCP_CSUM_ENABLE BIT(10)
#define DMA_CTRL0_BYTE_MODE       BIT(11)
#define DMA_CTRL0_LOCK_TRANS      BIT(8)
#define DMA_CTRL0_LINK_LIST       BIT(7)
#define DMA_CTRL0_STOP_TRANS      BIT(6)
#define DMA_CTRL0_PRI_DST         BIT(5)
#define DMA_CTRL0_PRI_SRC         BIT(4)
#define DMA_CTRL0_16BIT_TRANS     BIT(3)
#define DMA_CTRL0_COMP_INT_EN     BIT(2)
#define DMA_CTRL0_ERR_INT_EN      BIT(1)
#define DMA_CTRL0_TRANS_EN        BIT(0)

/* Control register1 bit definitions */
#define DMA_CTRL1_BRST_SZ_OFST      12
#define DMA_CTRL1_MAX_TRANS_SZ_OFST 6
#define DMA_CTRL1_DST_DEV_CODE_OFST 3
#define DMA_CTRL1_SRC_DEV_CODE_OFST 0

/* control register2 bit definitions */
#define DMA_CTRL2_TCP_CSUM_ENABLE BIT(10)
#define DMA_CTRL2_TRGR            BIT(6)
#define DMA_CTRL2_LOCK_TRANS      BIT(5)
#define DMA_CTRL2_LINK_LIST       BIT(4)
#define DMA_CTRL2_TRANS_STOP      BIT(3)
#define DMA_CTRL2_COMP_INT_EN     BIT(2)
#define DMA_CTRL2_ERR_INT_EN      BIT(1)
#define DMA_CTRL2_TRANS_EN        BIT(0)

/* status register bits */
#define DMA_STAT_STOP_DONE1  BIT(10)
#define DMA_STAT_TRANS_DONE1 BIT(9)
#define DMA_STAT_ERR1        BIT(8)
#define DMA_STAT_BUSY1       BIT(7)
#define DMA_STAT_STOP_DONE0  BIT(6)
#define DMA_STAT_TSIZE_COMP  BIT(5)
#define DMA_STAT_AEMPTY_INTR BIT(4)
#define DMA_STAT_AFULL_INTR  BIT(3)
#define DMA_STAT_TRANS_DONE0 BIT(2)
#define DMA_STAT_ERR0        BIT(1)
#define DMA_STAT_BUSY0       BIT(0)

/* IP Header offset in packet */
/* words 8(desc) + 3(dst) + 3(src)  +  1(type)  */
#define TCP_CKSUM_OFFSET 15

/* Channel assignment */
#define DMA_HOST_TO_TA_CHAN_NUM 1
#define DMA_TA_TO_HOST_CHAN_NUM 2

#define DMA_BEATS_16 0x00
#define DMA_BEATS_1  0x01
#define DMA_BEATS_2  0x02
#define DMA_BEATS_3  0x03
#define DMA_BEATS_4  0x04
#define DMA_BEATS_8  0x08

#define DMA_CHANNEL_NUM_1 0
#define DMA_CHANNEL_NUM_2 1
#define DMA_CHANNEL_NUM_3 2
#define DMA_CHANNEL_NUM_4 3
#define DMA_CHANNEL_NUM_5 4
#define DMA_CHANNEL_NUM_6 5
#define DMA_CHANNEL_NUM_7 6
#define DMA_CHANNEL_NUM_8 7

#endif
#endif
