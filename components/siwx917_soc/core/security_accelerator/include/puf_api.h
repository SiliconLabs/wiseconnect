/*******************************************************************************
* @file  puf_api.h
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
#ifndef _PUF_API_
#define _PUF_API_
//#include "rsi_chip.h"
#include "stdint.h"
#include "base_types.h"

typedef enum rsi_error_e {
  RSI_ERROR_NONE                         = 0,
  RSI_ERROR_TIMEOUT                      = -1,
  RSI_ERROR_INVALID_PARAM                = -2,
  RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE = -3,
  RSI_ERROR_PKT_ALLOCATION_FAILURE       = -4,
  RSI_ERROR_COMMAND_NOT_SUPPORTED        = -5,
  RSI_ERROR_INSUFFICIENT_BUFFER          = -6,
  RSI_ERROR_IN_OS_OPERATION              = -7
} rsi_error_t;

#ifdef _CHIP_9118
#define QK_BASE_ADD      0x20400600
#define QK_AES_BASE_ADD  0x20400500
#define IID_RP_INTF_REGS 0x20400400
#else
#define QK_BASE_ADD      0x00070000
#define QK_AES_BASE_ADD  0x00078000
#define IID_RP_INTF_REGS 0x00078100
#endif

#define QK_CONTROL_REG     *(volatile uint32_t *)(QK_BASE_ADD + 0x0)
#define QK_INDEX_REG       *(volatile uint32_t *)(QK_BASE_ADD + 0x4)
#define QK_SIZE_REG        *(volatile uint32_t *)(QK_BASE_ADD + 0x8)
#define QK_TARGET_REG      *(volatile uint32_t *)(QK_BASE_ADD + 0xC)
#define QK_STATUS_REG      *(volatile uint32_t *)(QK_BASE_ADD + 0x20)
#define QK_ALLOW_REG       *(volatile uint32_t *)(QK_BASE_ADD + 0x28)
#define QK_INPUT_REG       *(volatile uint32_t *)(QK_BASE_ADD + 0x40)
#define QK_CODE_INPUT_REG  *(volatile uint32_t *)(QK_BASE_ADD + 0x44)
#define QK_CODE_OUTPUT_REG *(volatile uint32_t *)(QK_BASE_ADD + 0x48)
#define QK_XKPD_OUTPUT_REG *(volatile uint32_t *)(QK_BASE_ADD + 0x4C)
#define QK_OP_INDEX_REG    *(volatile uint32_t *)(QK_BASE_ADD + 0x60)
#define QK_OUTPUT_REG      *(volatile uint32_t *)(QK_BASE_ADD + 0x64)
#define QK_IF_STATUS_REG   *(volatile uint32_t *)(QK_BASE_ADD + 0xDC)
#define QK_TEST_REG        *(volatile uint32_t *)(QK_BASE_ADD + 0xE0)
#define QK_VER_REG         *(volatile uint32_t *)(QK_BASE_ADD + 0xFC)

#define QK_AES_KEY_CONTROL_REG *(volatile uint32_t *)(QK_AES_BASE_ADD + 0x0)
#define QK_AES_MODE_REG        *(volatile uint32_t *)(QK_AES_BASE_ADD + 0x4)
#define QK_AES_ACTION_REG      *(volatile uint32_t *)(QK_AES_BASE_ADD + 0x8)
#define QK_AES_STATUS_REG      *(volatile uint32_t *)(QK_AES_BASE_ADD + 0x20)
#define QK_AES_KEY_REG         *(volatile uint32_t *)(QK_AES_BASE_ADD + 0x40)
#define QK_AES_DIN_REG         *(volatile uint32_t *)(QK_AES_BASE_ADD + 0x44)
#define QK_AES_DOUT_REG        *(volatile uint32_t *)(QK_AES_BASE_ADD + 0x48)
#define QK_AES_IF_STATUS_REG   *(volatile uint32_t *)(QK_AES_BASE_ADD + 0xDC)

#define IID_BIST_CTRL_REG   *(volatile uint32_t *)(IID_RP_INTF_REGS + 0x0)
#define IID_BIST_STATUS_REG *(volatile uint32_t *)(IID_RP_INTF_REGS + 0x4)
#define IID_CTRL_REG        *(volatile uint32_t *)(IID_RP_INTF_REGS + 0x8)

#define WKE_CONTROL_REG *(volatile uint32_t *)(IID_RP_INTF_REGS + 0xC)
#define WKE_STATUS_REG  *(volatile uint32_t *)(IID_RP_INTF_REGS + 0x10)
#define WKE_DATA_REG    *(volatile uint32_t *)(IID_RP_INTF_REGS + 0x1C)

//#define MISC_CFG_BASE_ADDR	     		  0x46008000
#define MISC_CFG_BASE_ADDR 0x41050000
//#define CHIP_MODE_CNFG_REG_BASE_ADDR  0x24048600
//FIXME: ADD EQUIVALENT CHIP CONFIG REG
#define CHIP_MODE_CNFG_REG *(volatile uint32_t *)(MISC_CFG_BASE_ADDR + 0x3E)
//#define CHIP_MODE_CNFG_REG		    	*(volatile uint32_t *) (CHIP_MODE_CNFG_REG_BASE_ADDR + 0x0C)

//#define MISC_SOFT_SET_REG           *(volatile uint16_t *) (MISC_CFG_BASE_ADDR + 0x28)

//CHIP MODE CONFIG reg
#define BLCK_ENROLL      0x20
#define BLCK_SET_KEY     0x40
#define BLCK_KEY_REG     0x80
#define BLCK_KEY_VIA_REG 0x80
#define DISABLE_IID      0x100

#define BLOCK_IID         0x00
#define BLOCK_ENROLL      0x01
#define BLOCK_SET_KEY     0x02
#define BLOCK_GET_KEY_REG 0x03
#define QK_RETURN_SUCCESS 0x00

#define KEY_CODE_SIZE       11 /*Key code size*/
#define KEY_CODE_SIZE_BYTES 44 /*Key code size*/
#define MAX_KEY_SIZE_BYTES  32 /*Key size*/
#define USER_KEY_SIZE_8     8  /*user key size of 8 Words(256 bits)*/
#define USER_KEY_SIZE_4     4  /*user key size of 4 Words(128 bits)*/
#define IID_256_BIT         0x04
#define IID_128_BIT         0x02
#define BITS_256            256
#define BITS_128            128
#define INVALID_KEYSIZE     0x00

#define AUTHEN_DATA_SIZE     4   /*Authentication data size 4 words*/
#define XKPD_SIZE            19  /*External key programming data 19 Words*/
#define ACTIVATION_CODE_SIZE 298 /*Activation code size is 298 words*/

/*  AES Key Control Register */
//#define QK_AES_KEY_CHANGE_REQ 0x01				//bit 0, 1 clears the current key and starts a request for a new key
//#define QK_AES_KEY_SIZE_128	  0x02				//bit 0, 1 clears the current key and starts a request for a new ke
#define QK_AES_KEY_CHANGE_REQ_128B_INTF \
  0x01 //clears the current key and starts a request for a new key of 128bit through interface
#define QK_AES_KEY_CHANGE_REQ_256B_INTF \
  0x03 //clears the current key and starts a request for a new key of 256bit through interface
#define QK_AES_KEY_CHANGE_REQ_128B_REG \
  0x81 //clears the current key and starts a request for a new key of 128bit through register
#define QK_AES_KEY_CHANGE_REQ_256B_REG \
  0x83                          //clears the current key and starts a request for a new key of 128bit through register
#define AES_KEY_SRC_REG    0x80 //bit 7: 1:sets key source as register
#define AES_KEY_SRC_INTF   0x00 //bit 7: 0:sets key source as Interface
#define AES_KEY_SIZE_256   0x02 //Bit 2: 1: sets the AES key size to 256bits
#define AES_KEY_SIZE_128   0x00 //Bit 2: 0: sets the AES key size to 128bits
#define AES_KEY_CHANGE_REQ 0x01 //Bit 1: 1: sets new request for key change

/*   AES Mode Register    */
#define QK_AES_MODE_NONE 0x00 //aes disabled
#define QK_AES_MODE_ECB  0x01 // aes in ECB
#define QK_AES_MODE_CBC  0x02 // aes in CBC

/*AES Action Register-  R/W accessible */
#define QK_AES_ACTION_IDLE   0x00 // AES Idle
#define QK_AES_ACTION_ENCODE 0x01 // AES Encode
#define QK_AES_ACTION_DECODE 0x02 // AES Decode
#define QK_AES_ACTION_CLEAR  0x03 // AES Clear

/*AES Status Register - Read-only register*/
#define QK_AES_BUSY        0x01 //bit0, 1: Indicates that AES core is processing data
#define QK_AES_CLEAR_DONE  0x04 //bit 2, 1: Indicates that the Clear action is finished
#define QK_AES_KEY_PRESENT 0x08 //bit 3, 1: Indicates the presence of a complete key
#define QK_AES_KEY_RQST    0x20 //bit 5, 1:Key must be provided
#define QK_AES_DATA_RQST   0x40 //bit 6, 1: Data must be provided
#define QK_AES_DATA_AVAIL  0x80 //bit 7, 1: Data is available
#define QK_ERROR_BUSY      0x05
#define QK_BUSY_OK         0x03

/*AES Test Register*/
#define QK_AES_BIST_ENABLE  0x01 //bit0, 1:Isolates the iid_aes module and runs a BIST
#define QK_AES_BIST_RUNNING 0x10 //bit4, 1:BIST is in progress or finishing up
#define QK_AES_BIST_ACTIVE  0x20 //bit5, 1:Indicates that BIST is running
#define QK_AES_BIST_OK      0x40 //bit6, 1:Indicates that BIST has passed
#define QK_AES_BIST_ERROR   0x80 //bit7, 1:Indicates that BIST has failed

/* Quiddikey Status Register -QK_SR - bit Status*/

#define QK_BUSY     0x01 /* bit 0 1 indicates operation is in progress*/
#define QK_OK       0x02 /* bit 1, 1 indicates last operation was successful*/
#define QK_ERROR    0x04 /* bit 2, 1 indicates Quiddikey is in error no operation can be performed*/
#define QK_XO_AV    0x08 /* bit 3 , 1 indiactes next part of XKPD ia available*/
#define QK_KI_REQ   0x10 /* bit 4, 1 indicates request for next part of key*/
#define QK_KO_AV    0x20 /* bit 5, 1 indicates next part of key is available */
#define QK_CI_REQ   0x40 /* bit 6 , 1 indicates request for next part of AC/KC */
#define QK_CO_AV    0x80 /* bit 7, 1 indicates next part of AC?KC is available*/
#define QK_ERROR_OK 0x05

/* Quiddikey Control Register -QK_CR - bit Status*/
/* Control register defines which command must be executed next, Command bit neds to be
   set 1 for the each operation*/
#define QK_ZEROIZE 0x01 /* bit 0 1 indicates begin zeroize operation and go to error state*/
#define QK_ENROLL  0x02 /* bit 1, 1 indicates begin enroll operation*/
#define QK_START   0x04 /* bit 2, 1 indicates begin start operation*/
#define QK_SET_IK  0x08 /* bit 3 , 1 indiactes begin Set Intrinsic Key Operation*/
#define QK_SET_UK  0x10 /* bit 4, 1 indicates begin Set User Key operation*/
#define QK_SET_XK  0x20 /* bit 5, 1 indicates begin Set External Key operation */
#define QK_GET_KEY 0x40 /* bit 6 , 1 indicates begin Get Key operation */

/*Quiddikey Allow Register*/
#define QK_ALLOW_ENROLL  0x01 //bit0, 1: Enroll Operation is allowed
#define QK_ALLOW_START   0x02 //bit1, 1:Start Operation is allowed
#define QK_ALLOW_SET_KEY 0x04 //bit2, 1:Set Key Operation is allowed
#define QK_ALLOW_GET_KEY 0x08 //bit3, 1:Get Key Operation is allowed
#define QK_ALLOW_BIST    0x80 //bit7, 1:BIST is allowed to be started

/*Quiddikey Test Register*/
#define QK_AES_BIST_ENABLE  0x01 //bit0, 1:Isolates the iid_aes module and runs a BIST
#define QK_AES_BIST_RUNNING 0x10 //bit4, 1:BIST is in progress or finishing up
#define QK_AES_BIST_ACTIVE  0x20 //bit5, 1:Indicates that BIST is running
#define QK_AES_BIST_OK      0x40 //bit6, 1:Indicates that BIST has passed
#define QK_AES_BIST_ERROR   0x80 //bit7, 1:Indicates that BIST has failed

/*IID_BIST_CTRL_REG*/
//description not available
#define IID_BIST_QK_BIST_EN  0x01
#define IID_BIST_AES_BIST_EN 0x02
#define IID_BIST_KH_BIST_EN  0x04

/*IID_CTRL_REG*/
#define IID_MAX_KEY_256 0x01 //bit0, 1: 256 bit key, 0: 128 bit key
#define IID_KH_SURCE_AES \
  0x02 //bit1, 1:KH will source the key to AES engine, 0 : QK key output is connected to AES key input
#define IID_KH_LATCH_KEY_QK \
  0x04 //bit2, 1: KH will latch the key given by QK, 0: QK key output is connected to AES key input
#define IID_KH_OUT_RESET     0x08  //bit3, 1:Out of reset, 0: KH will be in reset
#define IID_KH_KEY_256       0x10  //bit4, 1:256 bit key, 0: 128  bit key
#define IID_KH_ADDR          0x100 //bit8, 1:Lower addr bit key, 0: Higher  addr
#define IID_KH_CLOCK_RATIO_1 0x20  //bits 5,6,7 indicates the division factor to be used for generating kh_clk
#define IID_RESET_DEFAULT    0x00  //reset value

/*WKE_CTRL_REG*/
#define WKE_KC_DONE 0x20 //bit 5, This has to be set after reading key code
#define WKE_SET_KEY 0x10 //bit 4, This has to be set after key available from AES
#define WKE_COMP    0x08 //bit 3, 1: will start compare the data from AES engine with the data provided by firmware
#define WKE_FLUSH   0x04 //bit2, 1:  WKE will flush out the data from AES.
#define WKE_KEY_256 0x02 //bit1, 0:128bit size, 1: 256 bit size
#define WKE_ENABLE  0x01 //bit 0, 1: Enables WKE
#define WKE_KEY_128 0x00

/*WKE_STATUS_REG*/
#define WKE_ACTIVE         0x01 //bit 0, 1: when WKE is active
#define WKE_KEY_FEED_ON    0x02 //bit 1, 1: when feeding key to AES
#define WKE_FLUS_ON        0x04 //bit 2, 1: when WKE flushing out the data from AES
#define WKE_CMPR_ON        0x08 //bit 3, 1: when WKE is comparing the data from AES
#define WKE_SET_KEY_ON     0x10 //bit 4, 1: when WKE is doing set key operation with QK
#define WKE_KEY_READY      0x20 //bit 5, 0: Firmware has to load the authentication, which will be compared with AES output,
#define WKE_CMP_DATA_READY 0x40 //bit 6, todo : bug in the user manual with same decription
#define WKE_CMP_FAIL       0x80 //bit 7, 1: when comparison failed

/* Used in GetKey operation as a parameter to be passed */
#define QK_TARGET_AS_INTF    0x00
#define QK_TARGET_AS_REG     0x01
#define QK_TARGET_AS_KH      0x02
#define QK_TARGET_AS_KH_INTF 0x00

/*IID_BIST_STATUS_REG*/
//no clear description in manual
#define BIST_QK_ACTIVE    0x01  //bit 0,
#define BIST_QK_ERROR     0x02  //bit 1,
#define BIST_QK_OK        0x04  //bit 2,
#define BIST_QK_RUNNING   0x08  //bit 3,
#define BIST_AES_ACTIVE   0x10  //bit 4,
#define BIST_AES_ERROR    0x20  //bit 5,
#define BIST_AES_OK       0x40  //bit 6,
#define BIST_AES_RUNNING  0x80  //bit 7,
#define BIST_KH_STATUS_OK 0x100 //bit 8

//states for PUF
#define PUF_STARTED      BIT(0)
#define PUF_KEY_LOADED   BIT(1)
#define PUF_ENABLE_CLOCK BIT(8)

//AES Modes
#define MODE_ECB 0x00
#define MODE_CBC BIT(0)

#define MODE_KEY_SIZE_128 0x00
#define MODE_KEY_SIZE_256 BIT(1)

#define MODE_KEY_FROM_PUF 0x00
#define MODE_KEY_FROM_AES BIT(2)
#define MODE_KEY_FROM_KH  0x00

#define MODE_IV_SIZE_128 0x00
#define MODE_IV_SIZE_256 BIT(3)

#define MODE_ENCRYPT BIT(4)
#define MODE_DECRYPT 0x00

#define AES_STRUCT_SIZE_WITHOUT_DATA (1 + 32 + 32 + 2) //(1B mode + 32B key+ 32B iv+ 2B size)

#define NWP_AHB_ADDR 0x41300000

#define CHIP_CONFIG_WIRELESS *(volatile uint32_t *)(NWP_AHB_ADDR + 0x608)
#define CHIP_CONFIG_MCU      *(volatile uint32_t *)(NWP_AHB_ADDR + 0x60C)

#ifdef _CHIP_9118
//! wireless chip modes
#define BLOCK_PUF_IID_GET_KEY_REG BIT(13)
#define BLOCK_PUF_IID_SET_KEY     BIT(12)
#define BLOCK_PUF_IID_ENROLL      BIT(11)
#define DISABLE_PUF               BIT(10)
#endif

/*Error codes*/
#define QK_INVALID_KEYCODE         -202 /*Keycode is invalid */
#define QK_RETURN_ERROR            -203 /*Operation failed*/
#define QK_RETURN_OP_NOT_ALLOWED   -204 /*OPeration not allowed */
#define QK_RETURN_QK_ERROR         -205 /*Qk is in error state */
#define QK_RETURN_INVALID_PARAM    -206 /*Invalid input parameter */
#define QK_RETURN_INVALID_QK_INPUT -207 /*Given input to QK is invalid or incorrect */
#define QK_RETURN_INVALID_MODE     -208 /*Given input mode is invalid */
#define QK_RETURN_OP_BLOCKED       -209 /*OPeration is blocked */
#define QK_RETURN_AUTH_FAIL        -210 /*Authentication failed in key update */
#define QK_PACKET_ERROR            -211 /*Error in packet */

#define RSI_INLINE_EN_DE_CRTYPT 1
#define RSI_EN_DE_CRTYPT        0

/* Declarations */
extern int32_t puf_block(uint8_t type);
extern int32_t puf_get_key(uint32_t *kcdata, uint32_t *mast_key, uint8_t intf_mode, uint32_t *key_size);
extern int32_t puf_intf_key_frm_kh(uint32_t key_len, uint32_t intf_mode);
extern int32_t puf_set_uk(uint32_t *ukdata, uint8_t keyindex, uint16_t keysize, uint32_t *kcdata);
extern int32_t puf_start(uint32_t *actv_code);
extern int32_t puf_enroll(uint32_t *actv_code);
extern int32_t puf_wait_for_init();
extern int32_t data_aes_mac(uint8_t mode,
                            uint32_t *plain_data,
                            uint32_t *iv,
                            uint32_t *key,
                            uint32_t data_size,
                            uint32_t *mac);
extern int32_t data_aes_encry_decry(uint8_t mode,
                                    uint32_t *in_data,
                                    uint32_t offset_in_flash,
                                    uint32_t *iv,
                                    uint32_t *key,
                                    uint32_t data_size,
                                    uint32_t *out_data,
                                    uint32_t *nonce,
                                    uint32_t type);
extern int32_t puf_feature_block_check(uint8_t feature);
extern int32_t data_aes_encry_decry_qspi(uint8_t mode,
                                         uint32_t *in_data,
                                         uint32_t offset_in_flash,
                                         uint32_t *iv,
                                         uint32_t *key,
                                         uint32_t data_size,
                                         uint32_t *out_data,
                                         uint32_t *nonce);
extern uint16_t puf_error(uint32_t status);
extern int32_t puf_set_intrensic_key(uint8_t keyindex, uint16_t keysize, uint32_t *keycode);
#endif
