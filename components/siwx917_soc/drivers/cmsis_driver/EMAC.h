/* -------------------------------------------------------------------------- 
 * Copyright (c) 2013-2018 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Date:        25. Dec 2018
 * $Revision:    V2.8
 *
 * Project:      Ethernet Media Access (MAC) Definitions 
 * -------------------------------------------------------------------------- */

#include <string.h>

#include "Driver_ETH_MAC.h"

#define ETH_MII 0
/* EMAC Driver state flags */
#define EMAC_FLAG_INIT  (1 << 0) // Driver initialized
#define EMAC_FLAG_POWER (1 << 1) // Driver power on

/* TDES0 - DMA Descriptor TX Packet Control/Status */
#define DMA_TX_OWN 0x80000000U // Own bit 1=DMA,0=CPU

/* TDES1 - DMA Descriptor TX Packet Control/Status */
#define DMA_TX_IC         0x80000000U // Interrupt on completition
#define DMA_TX_LS         0x40000000U // Last segment
#define DMA_TX_FS         0x20000000U // First segment
#define DMA_TX_DC         0x04000000U // Disable CRC
#define DMA_TX_DP         0x00800000U // Disable pad
#define DMA_TX_CIC        0x18000000U // Checksum insertion control
#define DMA_TX_CIC_IP     0x08000000U //Checksum insertion control for IP header only
#define DMA_TX_CIC_BYPASS 0x00000000U
#define DMA_TX_TER        0x02000000U // Transmit end of ring
#define DMA_TX_TCH        0x01000000U // Second address chained
#define DMA_TX_TTSS       0x00020000U // Transmit time stamp status
#define DMA_TX_IHE        0x00010000U // IP header error status
#define DMA_TX_ES         0x00008000U // Error summary
#define DMA_TX_JT         0x00004000U // Jabber timeout
#define DMA_TX_FF         0x00002000U // Frame flushed
#define DMA_TX_IPE        0x00001000U // IP payload error
#define DMA_TX_LC         0x00000800U // Loss of carrier
#define DMA_TX_NC         0x00000400U // No carrier
#define DMA_TX_LCOL       0x00000200U // Late collision
#define DMA_TX_EC         0x00000100U // Excessive collision
#define DMA_TX_VF         0x00000080U // VLAN frame
#define DMA_TX_CC         0x00000078U // Collision count
#define DMA_TX_ED         0x00000004U // Excessive deferral
#define DMA_TX_UF         0x00000002U // Underflow error
#define DMA_TX_DB         0x00000001U // Deferred bit

/* TDES1 - DMA Descriptor TX Packet Control */
#define DMA_TX_TBS2 0x1FFF0000U // Transmit buffer 2 size
#define DMA_TX_TBS1 0x00001FFFU // Transmit buffer 1 size

/* RDES0 - DMA Descriptor RX Packet Status */
#define DMA_RX_OWN   0x80000000U // Own bit 1=DMA,0=CPU
#define DMA_RX_AFM   0x40000000U // Destination address filter fail
#define DMA_RX_FL    0x3FFF0000U // Frame length mask
#define DMA_RX_ES    0x00008000U // Error summary
#define DMA_RX_DE    0x00004000U // Descriptor error
#define DMA_RX_SAF   0x00002000U // Source address filter fail
#define DMA_RX_LE    0x00001000U // Length error
#define DMA_RX_OE    0x00000800U // Overflow error
#define DMA_RX_VLAN  0x00000400U // VLAN tag
#define DMA_RX_FS    0x00000200U // First descriptor
#define DMA_RX_LS    0x00000100U // Last descriptor
#define DMA_RX_IPHCE 0x00000080U // IPv4 header checksum error
#define DMA_RX_LC    0x00000040U // late collision
#define DMA_RX_FT    0x00000020U // Frame type
#define DMA_RX_RWT   0x00000010U // Receive watchdog timeout
#define DMA_RX_RE    0x00000008U // Receive error
#define DMA_RX_DRE   0x00000004U // Dribble bit error
#define DMA_RX_CE    0x00000002U // CRC error
#define DMA_RX_RMAM  0x00000001U // Rx MAC adr.match/payload cks.error

/* RDES1 - DMA Descriptor RX Packet Control */
#define DMA_RX_DIC  0x80000000U // Disable interrupt on completion
#define DMA_RX_RBS2 0x1FFF0000U // Receive buffer 2 size
#define DMA_RX_RER  0x02000000U // Receive end of ring
#define DMA_RX_RCH  0x01000000U // Second address chained
#define DMA_RX_RBS1 0x00001FFFU // Receive buffer 1 size

/* Ethernet MAC configuration register */
#define ETH_MACCR_WD        0x00800000U /* Watchdog disable */
#define ETH_MACCR_JD        0x00400000U /* Jabber disable */
#define ETH_MACCR_IFG       0x000E0000U /* Inter-frame gap mask*/
#define ETH_MACCR_IFG_96Bit 0x00000000U /* Minimum IFG between frames during transmission is 96Bit */
#define ETH_MACCR_IFG_88Bit 0x00020000U /* Minimum IFG between frames during transmission is 88Bit */
#define ETH_MACCR_IFG_80Bit 0x00040000U /* Minimum IFG between frames during transmission is 80Bit */
#define ETH_MACCR_IFG_72Bit 0x00060000U /* Minimum IFG between frames during transmission is 72Bit */
#define ETH_MACCR_IFG_64Bit 0x00080000U /* Minimum IFG between frames during transmission is 64Bit */
#define ETH_MACCR_IFG_56Bit 0x000A0000U /* Minimum IFG between frames during transmission is 56Bit */
#define ETH_MACCR_IFG_48Bit 0x000C0000U /* Minimum IFG between frames during transmission is 48Bit */
#define ETH_MACCR_IFG_40Bit 0x000E0000U /* Minimum IFG between frames during transmission is 40Bit */
#define ETH_MACCR_DCRS      0x00010000U /* Disable Carrier sense during transmission  */
#define ETH_MACCR_PS        0x00008000U /* PORT Select */
#define ETH_MACCR_FES       0x00000010U /* Fast ethernet speed */
#define ETH_MACCR_DO        0x00002000U /* Disable Receive own */
#define ETH_MACCR_LM        0x00001000U /* loopback mode */
#define ETH_MACCR_DM        0x00000800U /* Duplex mode */
#define ETH_MACCR_IPC       0x00000400U /* IP Checksum offload */
#define ETH_MACCR_DR        0x00000200U /* Disable Retry */
#define ETH_MACCR_APCS      0x00000080U /* Automatic Pad/CRC stripping */
#define ETH_MACCR_BL \
  0x00000060U /* Back-off limit mask   The random integer (r) of slot time delays where r takes the value in the range 0 = r < 2k*/

#define ETH_MACCR_BL_10 0x00000000U /* k = min (n, 10) */
#define ETH_MACCR_BL_8  0x00000020U /* k = min (n, 8) */
#define ETH_MACCR_BL_4  0x00000040U /* k = min (n, 4) */
#define ETH_MACCR_BL_1  0x00000060U /* k = min (n, 1) */
#define ETH_MACCR_DC    0x00000010U /* Defferal check */
#define ETH_MACCR_TE    0x00000008U /* Transmitter enable */
#define ETH_MACCR_RE    0x00000004U /* Receiver enable */

/* Ethernet MAC Frame Filter Register */
#define ETH_MACFFR_RA           0x80000000U /* Receive all */
#define ETH_MACFFR_HPF          0x00000400U /* Hash or perfect filter */
#define ETH_MACFFR_SAF          0x00000200U /* Source address filter enable */
#define ETH_MACFFR_SAIF         0x00000100U /* SA inverse filtering */
#define ETH_MACFFR_PCF          0x000000C0U /* Pass control frames: 3 cases */
#define ETH_MACFFR_PCF_BlockAll 0x00000040U /* MAC filters all control frames from reaching the application */
#define ETH_MACFFR_PCF_ForwardAll \
  0x00000080U /* MAC forwards all control frames to application even if they fail the Address Filter */
#define ETH_MACFFR_PCF_ForwardPassedAddrFilter \
  0x000000C0U                       /* MAC forwards control frames that pass the Address Filter. */
#define ETH_MACFFR_DBF  0x00000020U /* Disable Broadcast Frames */
#define ETH_MACFFR_PM   0x00000010U /* Pass all mutlicast */
#define ETH_MACFFR_DAIF 0x00000008U /* DA Inverse filtering */
#define ETH_MACFFR_HMC  0x00000004U /* Hash multicast */
#define ETH_MACFFR_HUC  0x00000002U /* Hash unicast */
#define ETH_MACFFR_PR   0x00000001U /* Promiscuous mode */

/*Ethernet MAC Hash Table High Register */
#define ETH_MACHTHR_HTH 0xFFFFFFFFU /* Hash table high */

/*Ethernet MAC Hash Table Low Register */
#define ETH_MACHTLR_HTL 0xFFFFFFFFU /* Hash table low */

/*Ethernet MAC MII Address Register */
#define ETH_MACMIIAR_PA        0x0000F800U /* Physical layer address */
#define ETH_MACMIIAR_MR        0x000007C0U /* MII register in the selected PHY */
#define ETH_MACMIIAR_CR        0x0000001CU /* CR clock range: 6 cases */
#define ETH_MACMIIAR_CR_Div42  0x00000000U /* HCLK:60-100 MHz; MDC clock= HCLK/42   */
#define ETH_MACMIIAR_CR_Div62  0x00000004U /* HCLK:100-150 MHz; MDC clock= HCLK/62  */
#define ETH_MACMIIAR_CR_Div16  0x00000008U /* HCLK:20-35 MHz; MDC clock= HCLK/16    */
#define ETH_MACMIIAR_CR_Div26  0x0000000CU /* HCLK:35-60 MHz; MDC clock= HCLK/26    */
#define ETH_MACMIIAR_CR_Div102 0x00000010U /* HCLK:150-180 MHz; MDC clock= HCLK/102 */
#define ETH_MACMIIAR_MW        0x00000002U /* MII write */
#define ETH_MACMIIAR_MB        0x00000002U /* MII busy  */

/*Ethernet MAC MII Data Register */
#define ETH_MACMIIDR_GD 0x0000FFFFU /* MII data: read/write data from/to PHY */

/*Ethernet MAC Flow Control Register */
#define ETH_MACFCR_PT           0xFFFF0000U /* Pause time */
#define ETH_MACFCR_DZPQ         0x00000080U /* Disable Zero-Quanta Pause */
#define ETH_MACFCR_PLT          0x00000030U /* Pause low threshold */
#define ETH_MACFCR_PLT_Minus4   0x00000000U /* Pause time minus 4 slot times   */
#define ETH_MACFCR_PLT_Minus28  0x00000010U /* Pause time minus 28 slot times  */
#define ETH_MACFCR_PLT_Minus144 0x00000020U /* Pause time minus 144 slot times */
#define ETH_MACFCR_PLT_Minus256 0x00000030U /* Pause time minus 256 slot times */
#define ETH_MACFCR_UP           0x00000008U /* Unicast pause frame detect */
#define ETH_MACFCR_RFE          0x00000004U /* Receive flow control enable */
#define ETH_MACFCR_TFE          0x00000002U /* Transmit flow control enable */
#define ETH_MACFCR_FCBBPA       0x00000001U /* Flow control busy/backpressure activate */

/*Ethernet MAC VLAN Tag Register */
#define ETH_MACVLANTR_VLANTI 0x0000FFFFU /* VLAN tag identifier (for receive frames) */

/*Ethernet MAC Remote Wake-UpFrame Filter Register */
#define ETH_MACRWUFFR_D 0xFFFFFFFF /* Wake-up frame filter register data */

/*Ethernet MAC PMT Control and Status Register */
#define ETH_MACPMTCSR_WFFRPR 0x80000000 /* Wake-Up Frame Filter Register Pointer Reset */
#define ETH_MACPMTCSR_GU     0x00000200 /* Global Unicast                              */
#define ETH_MACPMTCSR_WFR    0x00000040 /* Wake-Up Frame Received                      */
#define ETH_MACPMTCSR_MPR    0x00000020 /* Magic Packet Received                       */
#define ETH_MACPMTCSR_WFE    0x00000004 /* Wake-Up Frame Enable                        */
#define ETH_MACPMTCSR_MPE    0x00000002 /* Magic Packet Enable                         */
#define ETH_MACPMTCSR_PD     0x00000001 /* Power Down                                  */

/*Ethernet MAC Status Register */
#define ETH_MACSR_MMCTS 0x00000040U /* MMC transmit status       */
#define ETH_MACSR_MMCRS 0x00000020U /* MMC receive status        */
#define ETH_MACSR_MMCS  0x00000010U /* MMC status                */
#define ETH_MACSR_PMTS  0x00000008U /* PMT status        */

/* Ethernet MAC Interrupt Mask Register */
#define ETH_MACIMR_PMTIM 0x00000008U /* PMT interrupt mask                */

/* Ethernet MAC Address0 High Register */
#define ETH_MACA0HR_MACA0H 0x0000FFFFU /* MAC address0 high */

/* Ethernet MAC Address0 Low Register */
#define ETH_MACA0LR_MACA0L 0xFFFFFFFFU /* MAC address0 low */

/* Ethernet MAC Address1 High Register */
#define ETH_MACA1HR_AE             0x80000000U /* Address enable */
#define ETH_MACA1HR_SA             0x40000000U /* Source address */
#define ETH_MACA1HR_MBC            0x3F000000U /* Mask byte control: bits to mask for comparison of the MAC Address bytes */
#define ETH_MACA1HR_MBC_HBits15_8  0x20000000U /* Mask MAC Address high reg bits [15:8] */
#define ETH_MACA1HR_MBC_HBits7_0   0x10000000U /* Mask MAC Address high reg bits [7:0]  */
#define ETH_MACA1HR_MBC_LBits31_24 0x08000000U /* Mask MAC Address low reg bits [31:24] */
#define ETH_MACA1HR_MBC_LBits23_16 0x04000000U /* Mask MAC Address low reg bits [23:16] */
#define ETH_MACA1HR_MBC_LBits15_8  0x02000000U /* Mask MAC Address low reg bits [15:8]  */
#define ETH_MACA1HR_MBC_LBits7_0   0x01000000U /* Mask MAC Address low reg bits [7:0]   */
#define ETH_MACA1HR_MACA1H         0x0000FFFFU /* MAC address1 high */

/* Ethernet MAC Address1 Low Register */
#define ETH_MACA1LR_MACA1L 0xFFFFFFFFU /* MAC address1 low */

/* Ethernet MAC Address2 High Register */
#define ETH_MACA2HR_AE             0x80000000U /* Address enable */
#define ETH_MACA2HR_SA             0x40000000U /* Source address */
#define ETH_MACA2HR_MBC            0x3F000000U /* Mask byte control */
#define ETH_MACA2HR_MBC_HBits15_8  0x20000000U /* Mask MAC Address high reg bits [15:8] */
#define ETH_MACA2HR_MBC_HBits7_0   0x10000000U /* Mask MAC Address high reg bits [7:0]  */
#define ETH_MACA2HR_MBC_LBits31_24 0x08000000U /* Mask MAC Address low reg bits [31:24] */
#define ETH_MACA2HR_MBC_LBits23_16 0x04000000U /* Mask MAC Address low reg bits [23:16] */
#define ETH_MACA2HR_MBC_LBits15_8  0x02000000U /* Mask MAC Address low reg bits [15:8]  */
#define ETH_MACA2HR_MBC_LBits7_0   0x01000000U /* Mask MAC Address low reg bits [70]    */
#define ETH_MACA2HR_MACA2H         0x0000FFFFU /* MAC address1 high */

/*Ethernet MAC Address2 Low Register */
#define ETH_MACA2LR_MACA2L 0xFFFFFFFFU /* MAC address2 low */

/* Ethernet MAC Address3 High Register */
#define ETH_MACA3HR_AE             0x80000000U /* Address enable */
#define ETH_MACA3HR_SA             0x40000000U /* Source address */
#define ETH_MACA3HR_MBC            0x3F000000U /* Mask byte control */
#define ETH_MACA3HR_MBC_HBits15_8  0x20000000U /* Mask MAC Address high reg bits [15:8] */
#define ETH_MACA3HR_MBC_HBits7_0   0x10000000U /* Mask MAC Address high reg bits [7:0]  */
#define ETH_MACA3HR_MBC_LBits31_24 0x08000000U /* Mask MAC Address low reg bits [31:24] */
#define ETH_MACA3HR_MBC_LBits23_16 0x04000000U /* Mask MAC Address low reg bits [23:16] */
#define ETH_MACA3HR_MBC_LBits15_8  0x02000000U /* Mask MAC Address low reg bits [15:8]  */
#define ETH_MACA3HR_MBC_LBits7_0   0x01000000U /* Mask MAC Address low reg bits [70]    */
#define ETH_MACA3HR_MACA3H         0x0000FFFFU /* MAC address3 high */

/* Ethernet MAC Address3 Low Register */
#define ETH_MACA3LR_MACA3L 0xFFFFFFFFU /* MAC address3 low */

/* Ethernet MMC Contol Register */
#define ETH_MMCCR_MCF 0x00000008U /* MMC Counter Freeze           */
#define ETH_MMCCR_ROR 0x00000004U /* Reset on Read                */
#define ETH_MMCCR_CSR 0x00000002U /* Counter Stop Rollover        */
#define ETH_MMCCR_CR  0x00000001U /* Counters Reset               */

/* Ethernet MMC Receive Interrupt Register */
#define ETH_MMCRIR_RFCES 0x00000020U /* Set when Rx crc error counter reaches half the maximum value */

/* MMC Transmit Interrupt Register */
#define ETH_MMCTIR_TGFS   0x00200000U /* Set when Tx good frame count counter reaches half the maximum value */
#define ETH_MMCTIR_TGFSCS 0x00004000U /* Set when Tx good single col counter reaches half the maximum value */

/*Ethernet MMC Receive Interrupt Mask Register */
#define ETH_MMCRIMR_RFCEM 0x00000020U /* Mask the interrupt when Rx crc error counter reaches half the maximum value */

/*Ethernet MMC Transmit Interrupt Mask Register */
#define ETH_MMCTIMR_TGFM \
  0x00200000U /* Mask the interrupt when Tx good frame count counter reaches half the maximum value */
#define ETH_MMCTIMR_TGFSCM \
  0x00004000U /* Mask the interrupt when Tx good single col counter reaches half the maximum value */

/* DMA Bus Mode Register */
#define ETH_DMABMR_AAB        0x02000000U /* Address-Aligned beats */
#define ETH_DMABMR_FPM        0x01000000U /* 4xPBL mode */
#define ETH_DMABMR_USP        0x00800000U /* Use separate PBL */
#define ETH_DMABMR_RDP        0x007E0000U /* RxDMA PBL */
#define ETH_DMABMR_RDP_1Beat  0x00020000U /* maximum number of beats to be transferred in one RxDMA transaction is 1 */
#define ETH_DMABMR_RDP_2Beat  0x00040000U /* maximum number of beats to be transferred in one RxDMA transaction is 2 */
#define ETH_DMABMR_RDP_4Beat  0x00080000U /* maximum number of beats to be transferred in one RxDMA transaction is 4 */
#define ETH_DMABMR_RDP_8Beat  0x00100000U /* maximum number of beats to be transferred in one RxDMA transaction is 8 */
#define ETH_DMABMR_RDP_16Beat 0x00200000U /* maximum number of beats to be transferred in one RxDMA transaction is 16 */
#define ETH_DMABMR_RDP_32Beat 0x00400000U /* maximum number of beats to be transferred in one RxDMA transaction is 32 */
#define ETH_DMABMR_RDP_4xPBL_4Beat \
  0x01020000U /* maximum number of beats to be transferred in one RxDMA transaction is 4 */
#define ETH_DMABMR_RDP_4xPBL_8Beat \
  0x01040000U /* maximum number of beats to be transferred in one RxDMA transaction is 8 */
#define ETH_DMABMR_RDP_4xPBL_16Beat \
  0x01080000U /* maximum number of beats to be transferred in one RxDMA transaction is 16 */
#define ETH_DMABMR_RDP_4xPBL_32Beat \
  0x01100000U /* maximum number of beats to be transferred in one RxDMA transaction is 32 */
#define ETH_DMABMR_RDP_4xPBL_64Beat \
  0x01200000U /* maximum number of beats to be transferred in one RxDMA transaction is 64 */
#define ETH_DMABMR_RDP_4xPBL_128Beat \
  0x01400000U                           /* maximum number of beats to be transferred in one RxDMA transaction is 128 */
#define ETH_DMABMR_FB       0x00010000U /* Fixed Burst */
#define ETH_DMABMR_RTPR     0x0000C000U /* Rx Tx priority ratio */
#define ETH_DMABMR_RTPR_1_1 0x00000000U /* Rx Tx priority ratio */
#define ETH_DMABMR_RTPR_2_1 0x00004000U /* Rx Tx priority ratio */
#define ETH_DMABMR_RTPR_3_1 0x00008000U /* Rx Tx priority ratio */
#define ETH_DMABMR_RTPR_4_1 0x0000C000U /* Rx Tx priority ratio */
#define ETH_DMABMR_PBL      0x00003F00U /* Programmable burst length */
#define ETH_DMABMR_PBL_1Beat \
  0x00000100U /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 1 */
#define ETH_DMABMR_PBL_2Beat \
  0x00000200U /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 2 */
#define ETH_DMABMR_PBL_4Beat \
  0x00000400U /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
#define ETH_DMABMR_PBL_8Beat \
  0x00000800U /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
#define ETH_DMABMR_PBL_16Beat \
  0x00001000U /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
#define ETH_DMABMR_PBL_32Beat \
  0x00002000U /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */
#define ETH_DMABMR_PBL_4xPBL_4Beat \
  0x01000100U /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
#define ETH_DMABMR_PBL_4xPBL_8Beat \
  0x01000200U /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
#define ETH_DMABMR_PBL_4xPBL_16Beat \
  0x01000400U /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
#define ETH_DMABMR_PBL_4xPBL_32Beat \
  0x01000800U /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */
#define ETH_DMABMR_PBL_4xPBL_64Beat \
  0x01001000U /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 64 */
#define ETH_DMABMR_PBL_4xPBL_128Beat \
  0x01002000U /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 128 */
#define ETH_DMABMR_DSL 0x0000007CU /* Descriptor Skip Length */
#define ETH_DMABMR_DA  0x00000002U /* DMA arbitration scheme */
#define ETH_DMABMR_SR  0x00000001U /* Software reset */

/* DMA Transmit Poll Demand Register */
#define ETH_DMATPDR_TPD 0xFFFFFFFFU /* Transmit poll demand */

/* DMA Receive Poll Demand Register */
#define ETH_DMARPDR_RPD 0xFFFFFFFFU

/* DMA Receive Descriptor List Address Register */
#define ETH_DMARDLAR_SRL 0xFFFFFFFFU /* Start of receive list */

/* DMA Transmit Descriptor List Address Register */
#define ETH_DMATDLAR_STL 0xFFFFFFFFU /* Start of transmit list */

/* DMA Status Register */
#define ETH_DMASR_PMTS             0x10000000U /* PMT status */
#define ETH_DMASR_MMCS             0x08000000U /* MMC status */
#define ETH_DMASR_EBS              0x03800000U /* Error bits status */
#define ETH_DMASR_EBS_DescAccess   0x02000000U /* Error bits 0-data buffer, 1-desc. access */
#define ETH_DMASR_EBS_ReadTransf   0x01000000U /* Error bits 0-write trnsfer, 1-read transfer */
#define ETH_DMASR_EBS_DataTransfTx 0x00800000U /* Error bits 0-Data transfer by Rx DMA, 1-Tx DMA */
#define ETH_DMASR_TPS              0x00700000U /* Transmit process state */
#define ETH_DMASR_TPS_Stopped      0x00000000U /* Stopped - Reset or Stop Tx Command issued  */
#define ETH_DMASR_TPS_Fetching     0x00100000U /* Running - fetching the Tx descriptor */
#define ETH_DMASR_TPS_Waiting      0x00200000U /* Running - waiting for status */
#define ETH_DMASR_TPS_Reading      0x00300000U /* Running - reading the data from host memory */
#define ETH_DMASR_TPS_Suspended    0x00600000U /* Suspended - Tx Descriptor unavailabe */
#define ETH_DMASR_TPS_Closing      0x00700000U /* Running - closing Rx descriptor */
#define ETH_DMASR_RPS              0x000E0000U /* Receive process state */
#define ETH_DMASR_RPS_Stopped      0x00000000U /* Stopped - Reset or Stop Rx Command issued */
#define ETH_DMASR_RPS_Fetching     0x00020000U /* Running - fetching the Rx descriptor */
#define ETH_DMASR_RPS_Waiting      0x00060000U /* Running - waiting for packet */
#define ETH_DMASR_RPS_Suspended    0x00080000U /* Suspended - Rx Descriptor unavailable */
#define ETH_DMASR_RPS_Closing      0x000A0000U /* Running - closing descriptor */
#define ETH_DMASR_RPS_Queuing      0x000E0000U /* Running - queuing the recieve frame into host memory */
#define ETH_DMASR_NIS              0x00010000U /* Normal interrupt summary */
#define ETH_DMASR_AIS              0x00008000U /* Abnormal interrupt summary */
#define ETH_DMASR_ERI              0x00004000U /* Early receive interrupt */
#define ETH_DMASR_FBEI             0x00002000U /* Fatal bus error Interrupt */
#define ETH_DMASR_ETI              0x00000400U /* Early transmit Interrupt */
#define ETH_DMASR_RWT              0x00000200U /* Receive watchdog timeout status */
#define ETH_DMASR_RPSS             0x00000100U /* Receive process stopped status */
#define ETH_DMASR_RBU              0x00000080U /* Receive buffer unavailable status */
#define ETH_DMASR_RI               0x00000040U /* Receive interrupt status */
#define ETH_DMASR_TUNF             0x00000020U /* Transmit underflow status */
#define ETH_DMASR_ROVF             0x00000010U /* Receive overflow status */
#define ETH_DMASR_TJT              0x00000008U /* Transmit jabber timeout status */
#define ETH_DMASR_TBU              0x00000004U /* Transmit buffer unavailable status */
#define ETH_DMASR_TPSS             0x00000002U /* Transmit process stopped status */
#define ETH_DMASR_TI               0x00000001U /* Transmit interrupt status */

/* DMA Operation Mode Register */
#define ETH_DMAOMR_DT           0x04000000U /* Disable Dropping of TCP/IP checksum error frames */
#define ETH_DMAOMR_RSF          0x02000000U /* Receive store and forward */
#define ETH_DMAOMR_DFF          0x01000000U /* Disable flushing of received frames */
#define ETH_DMAOMR_TSF          0x00200000U /* Transmit store and forward */
#define ETH_DMAOMR_FTF          0x00100000U /* Flush transmit FIFO */
#define ETH_DMAOMR_TTC          0x0001C000U /* Transmit threshold control */
#define ETH_DMAOMR_TTC_64Bytes  0x00000000U /* threshold level of the MTL Transmit FIFO is 64 Bytes */
#define ETH_DMAOMR_TTC_128Bytes 0x00004000U /* threshold level of the MTL Transmit FIFO is 128 Bytes */
#define ETH_DMAOMR_TTC_192Bytes 0x00008000U /* threshold level of the MTL Transmit FIFO is 192 Bytes */
#define ETH_DMAOMR_TTC_256Bytes 0x0000C000U /* threshold level of the MTL Transmit FIFO is 256 Bytes */
#define ETH_DMAOMR_TTC_40Bytes  0x00010000U /* threshold level of the MTL Transmit FIFO is 40 Bytes */
#define ETH_DMAOMR_TTC_32Bytes  0x00014000U /* threshold level of the MTL Transmit FIFO is 32 Bytes */
#define ETH_DMAOMR_TTC_24Bytes  0x00018000U /* threshold level of the MTL Transmit FIFO is 24 Bytes */
#define ETH_DMAOMR_TTC_16Bytes  0x0001C000U /* threshold level of the MTL Transmit FIFO is 16 Bytes */
#define ETH_DMAOMR_ST           0x00002000U /* Start/stop transmission command */
#define ETH_DMAOMR_FEF          0x00000080U /* Forward error frames */
#define ETH_DMAOMR_FUF          0x00000040U /* Forward undersized good frames */
#define ETH_DMAOMR_RTC          0x00000018U /* receive threshold control */
#define ETH_DMAOMR_RTC_64Bytes  0x00000000U /* threshold level of the MTL Receive FIFO is 64 Bytes */
#define ETH_DMAOMR_RTC_32Bytes  0x00000008U /* threshold level of the MTL Receive FIFO is 32 Bytes */
#define ETH_DMAOMR_RTC_96Bytes  0x00000010U /* threshold level of the MTL Receive FIFO is 96 Bytes */
#define ETH_DMAOMR_RTC_128Bytes 0x00000018U /* threshold level of the MTL Receive FIFO is 128 Bytes */
#define ETH_DMAOMR_OSF          0x00000004U /* operate on second frame */
#define ETH_DMAOMR_SR           0x00000002U /* Start/stop receive */

/* DMA Interrupt Enable Register */
#define ETH_DMAIER_NIE   0x00010000U /* Normal interrupt summary enable */
#define ETH_DMAIER_AIE   0x00008000U /* Abnormal interrupt summary enable */
#define ETH_DMAIER_ERIE  0x00004000U /* Early receive interrupt enable */
#define ETH_DMAIER_FBIE  0x00002000U /* Fatal bus error interrupt enable */
#define ETH_DMAIER_ETIE  0x00000400U /* Early transmit interrupt enable */
#define ETH_DMAIER_RWIE  0x00000200U /* Receive watchdog timeout interrupt enable */
#define ETH_DMAIER_RSIE  0x00000100U /* Receive process stopped interrupt enable */
#define ETH_DMAIER_RUIE  0x00000080U /* Receive buffer unavailable interrupt enable */
#define ETH_DMAIER_RIE   0x00000040U /* Receive interrupt enable */
#define ETH_DMAIER_TUIE  0x00000020U /* Transmit Underflow interrupt enable */
#define ETH_DMAIER_ROIE  0x00000010U /* Receive Overflow interrupt enable */
#define ETH_DMAIER_TJIE  0x00000008U /* Transmit jabber timeout interrupt enable */
#define ETH_DMAIER_TBUIE 0x00000004U /* Transmit buffer unavailable interrupt enable */
#define ETH_DMAIER_TSIE  0x00000002U /* Transmit process stopped interrupt enable */
#define ETH_DMAIER_TIE   0x00000001U /* Transmit interrupt enable */

/* DMA Missed Frame and Buffer Overflow Counter Register */
#define ETH_DMAMFBOCR_OFOC 0x10000000U /* Overflow bit for FIFO overflow counter */
#define ETH_DMAMFBOCR_MFA  0x0FFE0000U /* Number of frames missed by the application */
#define ETH_DMAMFBOCR_OMFC 0x00010000U /* Overflow bit for missed frame counter */
#define ETH_DMAMFBOCR_MFC  0x0000FFFFU /* Number of frames missed by the controller */

/* DMA Current Host Transmit Descriptor Register */
#define ETH_DMACHTDR_HTDAP 0xFFFFFFFFU /* Host transmit descriptor address pointer */

/* DMA Current Host Receive Descriptor Register */
#define ETH_DMACHRDR_HRDAP 0xFFFFFFFFU /* Host receive descriptor address pointer */

/* DMA Current Host Transmit Buffer Address Register */
#define ETH_DMACHTBAR_HTBAP 0xFFFFFFFFU /* Host transmit buffer address pointer */

/* DMA Current Host Receive Buffer Address Register */
#define ETH_DMACHRBAR_HRBAP 0xFFFFFFFFU /* Host receive buffer address pointer */

typedef struct _ETH_PIN_config {
  uint8_t port;
  uint8_t pin;
  uint8_t mode;
  uint8_t pad_sel;
} ETH_PIN_Config;

/* EMAC Driver Control Information */
typedef struct {
  ARM_ETH_MAC_SignalEvent_t cb_event; // Event callback
  uint8_t flags;                      // Control and state flags
  uint8_t tx_index;                   // Transmit descriptor index
  uint8_t rx_index;                   // Receive descriptor index
#if (EMAC_CHECKSUM_OFFLOAD)
  bool tx_cks_offload; // Checksum offload enabled/disabled
#endif
#if (EMAC_TIME_STAMP)
  uint8_t tx_ts_index; // Transmit Timestamp descriptor index
#endif
  uint8_t *frame_end; // End of assembled frame fragments
} EMAC_CTRL;
