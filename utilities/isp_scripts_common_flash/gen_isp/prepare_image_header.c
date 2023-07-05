#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define BIT(x) (1 << x)
typedef unsigned int crc_t;
#define M4_IMAGE BIT(0)
#if 0
#define MIC        BIT(2)
#define ENCRYPTION BIT(1)
#define SIGNATURE  BIT(3)
#endif
unsigned int checksum_addition(unsigned char *buf, unsigned int size, unsigned int prev_sum);
crc_t crc_calc(unsigned char *data_in, unsigned int num);

typedef unsigned char uint8;
//! char is mapped to int8
typedef char int8;
//! unsigned short int is mapped as uint16
typedef unsigned short int uint16;
//! short int is mapped as int16
typedef short int int16;
//! unsigned int is mapped as uint32
typedef unsigned int uint32;
//! int is mapped as uint32
typedef int int32;

typedef unsigned char byte;

typedef unsigned short word16;

typedef unsigned long word32;

typedef unsigned long long word64;

typedef unsigned long word;

#ifdef M4_CRC
uint32 crc;

unsigned int data[] = {
#include "../demo_image_hex"
};
#else
typedef struct fwupreq_s {
  uint16 control_flags;
  uint16 sha_type;
  uint32 magic_no;
  uint32 image_size;
  uint32 fw_version;
  uint32 flash_location;
  uint32 crc;
  uint32 mic[4];
  uint32 counter;
  uint32 reserved1[5];
} fwupreq_t;

unsigned int array1[] = {
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
#include "../pmemdata_qspi_hex"
};

int local_mic[4];
unsigned int array2[sizeof(array1)];
#endif

void main(int argc, char *argv[])
{
#ifdef M4_CRC
  unsigned int length = 0, i = 0;
  length = sizeof(data);

  crc = checksum_addition((unsigned char *)data, 236, 1);

  for (i = 0; i < (length / 4); i++) {
    if (i == 59) {
      printf("0x%x,\n", crc);
    } else {
      printf("0x%x,\n", data[i]);
    }
  }
#else
  fwupreq_t *fwupreq;

  fwupreq             = (fwupreq_t *)array1;
  unsigned int crc    = 0;
  unsigned int length = 0, i = 0;
  fwupreq->magic_no       = 0x900d900d;
  fwupreq->control_flags  = BIT(0);
  fwupreq->reserved1[4]   = 0x900d900d;
  fwupreq->flash_location = atoi(argv[1]);
  length                  = sizeof(array1);

  fwupreq->image_size = length - sizeof(fwupreq_t);

#ifdef ENCRYPTION
  fwupreq->control_flags |= ENCRYPTION;
  encrypt_image_32(&array1[16], fwupreq->image_size, &array2[16]);
#endif

#ifdef SIGNATURE
  fwupreq->control_flags |= SIGNATURE;
  fwupreq->image_size += 512;
#endif

#ifdef MIC
  fwupreq->control_flags |= MIC;
  get_mic_16(array1, length, local_mic);
  memcpy(fwupreq->mic, local_mic, 16);
#else
  fwupreq->crc = crc_calc((unsigned char *)array1, length);
#endif
  memcpy(array2, array1, sizeof(fwupreq_t));
#ifdef ENCRYPTION
  for (i = 0; i < (length / 4); i++) {
    printf("0x%x,\n", array2[i]);
  }
#else
  for (i = 0; i < (length / 4); i++) {
    printf("0x%x,\n", array1[i]);
  }
#endif
#endif
}
