//******************************************************************************
//******************************************************************************
//
// Copyright (c) Freescale Semiconductor, Inc 2011.
//
// FILE NAME      : mmcau_sha1_functions.c
// VERSION        : $Id: mmcau_sha1_functions.c.rca 1.2 Sat Jan  8 01:22:22 2011 rzsx60 Experimental $
// TYPE           : Source C library code
// DEPARTMENT     : MSG R&D Core and Platforms
// AUTHOR         : David Schimke
// AUTHOR'S EMAIL : David.Schimke@freescale.com
// -----------------------------------------------------------------------------
// Release history
// VERSION  Date       AUTHOR           DESCRIPTION
//          08-2010    David Schimke    Initial Release
//          01-2011    David Schimke    Header added, comments cleaned up
//
//******************************************************************************
//******************************************************************************

#include "cau2_defines.h"
#include "dma_proto.h"
#include "dma.h"
#include "sha1_address_map.h"

static volatile unsigned long *const MMCAU_PPB_DIRECT   = (unsigned long *) 0xe0081000;
static volatile unsigned long *const MMCAU_PPB_INDIRECT = (unsigned long *) 0xe0081800;

#define mmcau_1_cmd(c1)        (0x80000000 + (c1<<22))
#define mmcau_2_cmds(c1,c2)    (0x80100000 + (c1<<22) + (c2<<11))
#define mmcau_3_cmds(c1,c2,c3) (0x80100200 + (c1<<22) + (c2<<11) + c3)
#define byterev(d)             (d>>24)|(d>>8)&0xff00|(d<<8)&0xff0000|(d<<24)


//******************************************************************************
//******************************************************************************
//
// SHA1: Initializes the SHA1 state variables
//   arguments
//           *sha1_state pointer to 160-bit block of SHA1 state variables:
//                           a,b,c,d,e
//
//   calling convention
//   int     mmcau_sha1_initialize_output (const u32 *sha1_state)

void mmcau_sha1_initialize_output (u32 sha1_state[])
{

//  initialize the hash variables, both in memory and the CAU

    sha1_state[0] = 0x67452301;                           // initialize a
    sha1_state[1] = 0xefcdab89;                           // initialize b
    sha1_state[2] = 0x98badcfe;                           // initialize c
    sha1_state[3] = 0x10325476;                           // initialize d
    sha1_state[4] = 0xc3d2e1f0;                           // initialize e

}

//******************************************************************************
//******************************************************************************
//
// SHA1: Perform the hash and generate SHA1 state variables for one or more
//       input message blocks
//  arguments
//           *msg_data   pointer to start of input message data
//           num_blks    number of 512-bit blocks to process
//           *sha1_state pointer to 160-bit block of SHA1 state variables:
//                           a,b,c,d,e
//
//   NOTE    Input message and state variable output blocks must not overlap
//
//   calling convention
//   int     mmcau_sha1_hash (const unsigned char *msg_data,
//                            const int            num_blks,
//                            u32        *sha1_state)

void mmcau_sha1_hash_n (u32 msg_data[], int num_blks,
                        u32 sha1_state[])
{
    int i, j, k, n;
    int w[80];

    u32 sha1_k[] = {0x5a827999,0x6ed9eba1,0x8f1bbcdc,0xca62c1d6};
//  initialize the hash variables, in CAU

    *(MMCAU_PPB_INDIRECT + (LDR+CA4)) = sha1_state[4];             // CA4 = e
    *(MMCAU_PPB_INDIRECT + (LDR+CA3)) = sha1_state[3];             // CA3 = d
    *(MMCAU_PPB_INDIRECT + (LDR+CA2)) = sha1_state[2];             // CA2 = c
    *(MMCAU_PPB_INDIRECT + (LDR+CA1)) = sha1_state[1];             // CA1 = b
    *(MMCAU_PPB_INDIRECT + (LDR+CA0)) = sha1_state[0];             // CA0 = a

    k = 0;
    for (n = 0; n < num_blks; n++)
    {
        i = 0;
        *(MMCAU_PPB_DIRECT) = mmcau_1_cmd(MVRA+CA0);               // a -> CAA
        *(MMCAU_PPB_INDIRECT + (ROTL+CAA)) = 5;                    // rotate 5

        for (j = 0; j < 16; j++, k++)
        {
            w[i] = byterev(msg_data[k]);                           // w[i]=m[k]
            *(MMCAU_PPB_DIRECT) = mmcau_2_cmds(HASH+HFC,ADRA+CA4); // + Ch(),+ e
            *(MMCAU_PPB_INDIRECT + (ADR+CAA))  = sha1_k[0];        // + k[0]
            *(MMCAU_PPB_INDIRECT + (ADR+CAA))  = w[i++];           // + w[i]
            *(MMCAU_PPB_DIRECT) = mmcau_1_cmd(SHS);                // shift regs
        }

        for (j = 0; j < 4; j++)
        {
            *(MMCAU_PPB_DIRECT) = mmcau_2_cmds(HASH+HFC,ADRA+CA4); // + Ch(),+ e
            *(MMCAU_PPB_INDIRECT + (ADR+CAA))  = sha1_k[0];        // + k[0]
            *(MMCAU_PPB_INDIRECT + (LDR+CA5))  = w[i-16];          //CA5=w[i-16]
            *(MMCAU_PPB_INDIRECT + (XOR+CA5))  = w[i-14];          //xor w[i-14]
            *(MMCAU_PPB_INDIRECT + (XOR+CA5))  = w[i-8];           // xor w[i-8]
            *(MMCAU_PPB_INDIRECT + (XOR+CA5))  = w[i-3];           // xor w[i-3]
            *(MMCAU_PPB_INDIRECT + (ROTL+CA5)) = 1;                // rotate 1
            w[i++] = *(MMCAU_PPB_INDIRECT + (STR+CA5));            // store w[i]
            *(MMCAU_PPB_DIRECT) = mmcau_2_cmds(ADRA+CA5,SHS);      // +w[i],shft
        }

        for (j = 0; j < 20; j++)
        {
            *(MMCAU_PPB_DIRECT) = mmcau_2_cmds(HASH+HFP,ADRA+CA4); //+Parity(),e
            *(MMCAU_PPB_INDIRECT + (ADR+CAA))  = sha1_k[1];        // + k[1]
            *(MMCAU_PPB_INDIRECT + (LDR+CA5))  = w[i-16];          //CA5=w[i-16]
            *(MMCAU_PPB_INDIRECT + (XOR+CA5))  = w[i-14];          //xor w[i-14]
            *(MMCAU_PPB_INDIRECT + (XOR+CA5))  = w[i-8];           // xor w[i-8]
            *(MMCAU_PPB_INDIRECT + (XOR+CA5))  = w[i-3];           // xor w[i-3]
            *(MMCAU_PPB_INDIRECT + (ROTL+CA5)) = 1;                // rotate 1
            w[i++] = *(MMCAU_PPB_INDIRECT + (STR+CA5));            // store w[i]
            *(MMCAU_PPB_DIRECT) = mmcau_2_cmds(ADRA+CA5,SHS);      // +w[i],shft
        }

        for (j = 0; j < 20; j++)
        {
            *(MMCAU_PPB_DIRECT) = mmcau_2_cmds(HASH+HFM,ADRA+CA4); // Maj(), +e
            *(MMCAU_PPB_INDIRECT + (ADR+CAA))  = sha1_k[2];        // + k[2]
            *(MMCAU_PPB_INDIRECT + (LDR+CA5))  = w[i-16];          //CA5=w[i-16]
            *(MMCAU_PPB_INDIRECT + (XOR+CA5))  = w[i-14];          //xor w[i-14]
            *(MMCAU_PPB_INDIRECT + (XOR+CA5))  = w[i-8];           // xor w[i-8]
            *(MMCAU_PPB_INDIRECT + (XOR+CA5))  = w[i-3];           // xor w[i-3]
            *(MMCAU_PPB_INDIRECT + (ROTL+CA5)) = 1;                // rotate 1
            w[i++] = *(MMCAU_PPB_INDIRECT + (STR+CA5));            // store w[i]
            *(MMCAU_PPB_DIRECT) = mmcau_2_cmds(ADRA+CA5,SHS);      // +w[i],shft
        }

        for (j = 0; j < 20; j++)
        {
            *(MMCAU_PPB_DIRECT) = mmcau_2_cmds(HASH+HFP,ADRA+CA4); //+Parity(),e
            *(MMCAU_PPB_INDIRECT + (ADR+CAA))  = sha1_k[3];        // + k[3]
            *(MMCAU_PPB_INDIRECT + (LDR+CA5))  = w[i-16];          //CA5=w[i-16]
            *(MMCAU_PPB_INDIRECT + (XOR+CA5))  = w[i-14];          //xor w[i-14]
            *(MMCAU_PPB_INDIRECT + (XOR+CA5))  = w[i-8];           // xor w[i-8]
            *(MMCAU_PPB_INDIRECT + (XOR+CA5))  = w[i-3];           // xor w[i-3]
            *(MMCAU_PPB_INDIRECT + (ROTL+CA5)) = 1;                // rotate  1
            w[i++] = *(MMCAU_PPB_INDIRECT + (STR+CA5));            // store w[i]
            *(MMCAU_PPB_DIRECT) = mmcau_2_cmds(ADRA+CA5,SHS);      // +w[i],shft
        }

        *(MMCAU_PPB_INDIRECT + (ADR+CA0))  = sha1_state[0];        // + sha1[0]
        *(MMCAU_PPB_INDIRECT + (ADR+CA1))  = sha1_state[1];        // + sha1[1]
        *(MMCAU_PPB_INDIRECT + (ADR+CA2))  = sha1_state[2];        // + sha1[2]
        *(MMCAU_PPB_INDIRECT + (ADR+CA3))  = sha1_state[3];        // + sha1[3]
        *(MMCAU_PPB_INDIRECT + (ADR+CA4))  = sha1_state[4];        // + sha1[4]

        sha1_state[4] = *(MMCAU_PPB_INDIRECT + (STR+CA4));         //st sha1[4]
        sha1_state[3] = *(MMCAU_PPB_INDIRECT + (STR+CA3));         //st sha1[3]
        sha1_state[2] = *(MMCAU_PPB_INDIRECT + (STR+CA2));         //st sha1[2]
        sha1_state[1] = *(MMCAU_PPB_INDIRECT + (STR+CA1));         //st sha1[1]
        sha1_state[0] = *(MMCAU_PPB_INDIRECT + (STR+CA0));         //st sha1[0]
    }

}

//******************************************************************************
//******************************************************************************
//
// SHA1: Updates SHA1 state variables for one or more input message blocks
//   arguments
//           *msg_data   pointer to start of input message data
//           num_blks    number of 512-bit blocks to process
//           *sha1_state pointer to 160-bit block of SHA1 state variables:
//                           a,b,c,d,e
//
//   calling convention
//   int     mmcau_sha1_update (const unsigned char *msg_data,
//                              const int            num_blks,
//                              u32        *sha1_state)

void mmcau_sha1_update(u32 msg_data[], int num_blks,
                       u32 sha1_state[])
{
    int i;

    mmcau_sha1_initialize_output (sha1_state);

    mmcau_sha1_hash_n (msg_data, num_blks, sha1_state);
}

#ifdef SHA1_HW_ACCELERATOR
/*
 * SHA-1 HMAC process buffer
 */

void sha1_hmac_do_mic(unsigned char *data,unsigned int datalen)
{
#if 0
  dma_desc_t sha1_dma_desc;
  //! following code is not functional
  uint32 beat_size = (4 << DMA_DESC_BSZ_OFST);  

  sha1_dma_desc.src_addr = (uint32)data;
  sha1_dma_desc.dst_addr = (uint32)(0x20400040 | BIT(31));
  sha1_dma_desc.control = (((datalen + 3) & ~3) | DEV_TYP_DST(DEV_IPSEC) | 
                                   DMA_CTRL0_LINK_LIST |
                                   DMA_CTRL0_TRANS_EN  |
                                   DMA_CTRL0_16BIT_TRANS |
                                   beat_size  );
  /* set End of chain bit for last descriptor*/
  sha1_dma_desc.control |= ( DMA_DESC_EOC );

  while(*(volatile uint16 *)DMA_CH3_STATUS & BIT(0));

   //! Set the base address registers for Tx descriptors
  DMA_CH3_LISTA_ADDR_L = ((((uint32)&sha1_dma_desc) ) & 0xFFFF);
  DMA_CH3_LISTA_ADDR_H = ((((uint32)&sha1_dma_desc)) >> 16);

  DMA_CH3_CTRL = (DMA_CTRL0_TRGR | DMA_CTRL0_LINK_LIST | DMA_CTRL0_16BIT_TRANS | DMA_CTRL0_TRANS_EN ); 


  //! wait sha1 staus register to go high
  while(*(volatile uint16 *)0x20400028 & BIT(1));
#else
  uint32 *word_address = data;
  uint8 i =0;
  //! wait sha1 staus register to go high
  while(SHA1_STS_REG & BIT(1));
  
  for(i=0 ;i < (datalen/4) ; i++)
  {
    SHA1_DIN_FIFO_DATA = word_address[i];
  }
#endif
  
  return ; 
}
#endif

//******************************************************************************
//******************************************************************************
//
// SHA1: Performs SHA1 hash algorithm on a single input message block
//           *msg_data   pointer to start of input message data
//           *sha1_state pointer to 160-bit block of SHA1 state variables:
//                           a,b,c,d,e
//
//   calling convention
//   int     mmcau_sha1_update (const unsigned char *msg_data,
//                              u32        *sha1_state)

void mmcau_sha1_hash (u32 msg_data[], u32 sha1_state[])
{
/*
#ifdef SHA1_HW_ACCELERATOR
  sha1_hmac_do_mic((uint8*)msg_data, 64);
#else
  mmcau_sha1_hash_n (msg_data, 1, sha1_state);
#endif*/

  //! wait sha1 staus register to go high
  while(SHA1_STS_REG & BIT(1));
  
  SHA1_DIN_FIFO_DATA = msg_data[0];
  SHA1_DIN_FIFO_DATA = msg_data[1];
  SHA1_DIN_FIFO_DATA = msg_data[2];
  SHA1_DIN_FIFO_DATA = msg_data[3];
  SHA1_DIN_FIFO_DATA = msg_data[4];
  SHA1_DIN_FIFO_DATA = msg_data[5];
  SHA1_DIN_FIFO_DATA = msg_data[6];
  SHA1_DIN_FIFO_DATA = msg_data[7];
  SHA1_DIN_FIFO_DATA = msg_data[8];
  SHA1_DIN_FIFO_DATA = msg_data[9];
  SHA1_DIN_FIFO_DATA = msg_data[10];
  SHA1_DIN_FIFO_DATA = msg_data[11];
  SHA1_DIN_FIFO_DATA = msg_data[12];
  SHA1_DIN_FIFO_DATA = msg_data[13];
  SHA1_DIN_FIFO_DATA = msg_data[14];
  SHA1_DIN_FIFO_DATA = msg_data[15];

}
