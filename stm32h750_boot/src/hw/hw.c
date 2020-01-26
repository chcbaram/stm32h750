/*
 * hw.c
 *
 *  Created on: 2020. 1. 21.
 *      Author: Baram
 */




#include "hw.h"



#define FW_TAG_SIZE     1024



__attribute__((section(".version"))) uint8_t boot_name[32] = "STM32H750_B/D";
__attribute__((section(".version"))) uint8_t boot_ver[32]  = "B200126R1";


static bool verifyFw(void);
static void jumpToFw(void);

void hwInit(void)
{
  bspInit();

  delayInit();
  millis();

  cmdifInit();
  ledInit();
  gpioInit();
  uartInit();
  uartOpen(_DEF_UART1, 57600);

  logPrintf("\n\n[ Bootloader Begin... ]\r\n");
  logPrintf("Booting..Name \t\t: %s\r\n", boot_name);
  logPrintf("Booting..Ver  \t\t: %s\r\n", boot_ver);


  qspiInit();
  flashInit();

  qspiEnableMemoryMappedMode();

  if (verifyFw() == true)
  {
    uint32_t *p_addr = (uint32_t *)(QSPI_ADDR_START + FW_TAG_SIZE + 4);

    logPrintf("Jump To F/W \t\t: 0x%X\r\n\r\n", (int)*p_addr);
    delay(50);
    jumpToFw();
  }
  else
  {
    logPrintf("Verify F/W  \t\t: Fail\r\n");
  }
}



bool verifyFw(void)
{
  void (**jump_func)(void) = (void (**)(void))(QSPI_ADDR_START + FW_TAG_SIZE + 4);


  if ((uint32_t)(*jump_func) != 0xFFFFFFFF)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void jumpToFw(void)
{
  void (**jump_func)(void) = (void (**)(void))(QSPI_ADDR_START + FW_TAG_SIZE + 4);

  bspDeInit();
  __set_MSP(*(__IO uint32_t*) (QSPI_ADDR_START + FW_TAG_SIZE));
  (*jump_func)();
}
