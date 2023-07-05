/*******************************************************************************
 * @file  sdmem_fatfs_exp.c
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

//Include Files

#include <stdio.h>
#include <string.h>
#include "ff.h"
#include "diskio.h"
#include "rsi_board.h"
#include "rsi_chip.h"

/* Private typedef ------------------------------------------------------------------------------------------------*/

/* Private define -------------------------------------------------------------------------------------------------*/

/* Private macro --------------------------------------------------------------------------------------------------*/
#define BUFFER_SIZE            (4096)
#define SOC_OPER_FREQUENCY_SIM 32000000 /*32Mhz*/
#define SYSTIC_TIMER_CONFIG    1000
#define SOC_PLL_CLK            50000000
#define SOC_PLL_REF_CLK        40000000
/* Private variables ----------------------------------------------------------------------------------------------*/
static FATFS fileSystem; /* File system object */
static FIL fileObject;   /* File object */

static uint8_t Write_Buffer[BUFFER_SIZE];
static uint8_t Read_Buffer[BUFFER_SIZE];

/*==============================================*/
/**
 * @brief  Main program.
 * @param  None
 * @return None
 */
int main(void)
{
  /* At this stage the microcontroller clock setting is already configured,
	 * this is done through SystemInit() function which is called from startup
	 * file (startup_rs1xxxx.s) before to branch to application main.
	 * To reconfigure the default setting of SystemInit() function, refer to
	 * startup_rs1xxxx.c file
	 */
  int i;
  FRESULT error;
  UINT bytesWritten;
  UINT bytesRead;
  const TCHAR driverNumberBuffer[3U] = { DEV_SD + '0', ':', '/' };
  int32_t status                     = RSI_OK;
  uint32_t comp                      = 0;

  /*Configures the system default clock and power configurations*/
  SystemCoreClockUpdate();

  /* Initialized board UART */
  DEBUGINIT();

  printf("\r\nThis is a file system example\r\n");

  /*Configure Source clock and  module clock for SDMEM */
  status = RSI_CLK_SetSocPllFreq(M4CLK, SOC_PLL_CLK, SOC_PLL_REF_CLK);
  if (status != RSI_OK) {
    DEBUGOUT("\r\nFailed to Set SOC PLL Clock to 50Mhz,Error Code : %d\r\n", status);
    return status;
  } else {
    DEBUGOUT("\r\nSet SOC PLL Clock to 50Mhz\r\n");
  }

  status = RSI_CLK_M4SocClkConfig(M4CLK, M4_SOCPLLCLK, 0);
  if (status != RSI_OK) {
    DEBUGOUT("\r\nFailed to Set M4 Clock as SOC PLL clock,Error Code : %d\r\n", status);
    return status;
  } else {
    DEBUGOUT("\r\nSet M4 Clock as SOC PLL clock\r\n");
  }

  status = RSI_CLK_SdMemClkConfig(M4CLK, 1, SDMEM_SOCPLLCLK, 1);
  if (status != RSI_OK) {
    DEBUGOUT("\r\nFailed to Set SOC PLL Clock to SDMEM,Error Code : %d\r\n", status);
    return status;
  } else {
    DEBUGOUT("\r\nSet SOC PLL Clock to SDMEM\r\n");
  }

  /*Systic Config*/
  SysTick_Config(SOC_OPER_FREQUENCY_SIM / SYSTIC_TIMER_CONFIG);

  for (i = 0; i < BUFFER_SIZE; i++) {
    Write_Buffer[i] = i + 1;
  }

  printf("\r\nPlease insert a memory card into sd slot\r\n");

  if (f_mount(&fileSystem, driverNumberBuffer, 0U)) {
    printf("\r\nMount failed\r\n");
    while (1)
      ;
  }

#if (_FS_RPATH >= 2U)
  error = f_chdrive((char const *)&driverNumberBuffer[0U]);
  if (error) {
    printf("\r\nChange drive failed\r\n");
    return -1;
  }
#endif
  printf("\r\nCreate folder in card\r\n");
  error = f_mkdir(_T("/folder_1"));
  if (error) {
    if (error == FR_EXIST) {
      printf("\r\nFolder already exists\r\n");
    } else {
      printf("\r\nMake folder failed\r\n");
      return -1;
    }
  }
  printf("\r\nCreate a file in created folder\r\n");
  error = f_open(&fileObject, _T("/folder_1/file_1.dat"), (FA_WRITE | FA_READ | FA_CREATE_ALWAYS));
  if (error) {
    if (error == FR_EXIST) {
      printf("\r\nThis file already exists\r\n");
    } else {
      printf("\r\nOpen file failed \r\n");
      return -1;
    }
  }
  printf("\r\nWrite data to above created file\r\n");
  error = f_write(&fileObject, Write_Buffer, sizeof(Write_Buffer), &bytesWritten);
  if ((error) || (bytesWritten != sizeof(Write_Buffer))) {
    printf("\r\nWrite file failed\r\n");
    return -1;
  }
  /* Move the file pointer */
  if (f_lseek(&fileObject, 0U)) {
    printf("\r\nSet file pointer position failed\r\n");
    return -1;
  }

  printf("\r\nRead data from above created file\r\n");
  memset(Read_Buffer, 0, sizeof(Read_Buffer));
  error = f_read(&fileObject, Read_Buffer, sizeof(Read_Buffer), &bytesRead);
  if ((error) || (bytesRead != sizeof(Read_Buffer))) {
    printf("\r\nRead data from file failed\r\n");
    return -1;
  }
  printf("\r\nCompare both read and write data in sd card\r\n");
  for (comp = 0; comp < sizeof(Write_Buffer); comp++) {
    if (Write_Buffer[comp] == Read_Buffer[comp]) {
      continue;
    } else {
      break;
    }
  }

  if (comp == sizeof(Write_Buffer)) {
    DEBUGOUT("\r\nData Comparison Success\r\n");
    DEBUGOUT("\r\nTest Case Pass\r\n");
  } else {
    DEBUGOUT("\r\nData Comparison Fail\r\n");
    DEBUGOUT("\r\nTest Case Fail\r\n");
  }

  if (f_close(&fileObject)) {
    printf("\r\nClose file failed\r\n");
    return -1;
  }

  printf("\r\nFile closed\r\n");

  while (1)
    ;
}
