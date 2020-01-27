/*
 * ap.cpp
 *
 *  Created on: 2020. 1. 21.
 *      Author: Baram
 */




#include "ap.h"
#include "boot/boot.h"



#define BOOT_MODE_JUMP_FW     0
#define BOOT_MODE_LOADER      1
#define BOOT_MODE_ROMBOOT     2


uint8_t boot_mode = BOOT_MODE_LOADER;

static cmd_t cmd_boot;




void apInit(void)
{
  hwInit();

  cmdifOpen(_DEF_UART1, 57600);

  cmdInit(&cmd_boot);
  cmdBegin(&cmd_boot, _DEF_UART2, 57600);


  boot_mode = resetGetCount();


  switch(boot_mode)
  {
    case BOOT_MODE_LOADER:
      logPrintf("boot begin...\r\n");
      break;

    case BOOT_MODE_ROMBOOT:
      logPrintf("jump system boot...\r\n");
      delay(100);
      resetToSysBoot();
      break;

    default:
      logPrintf("jump fw...\r\n");

      qspiEnableMemoryMappedMode();

      if (bootVerifyCrc() != true)
      {
        logPrintf("fw crc    \t\t: Fail\r\n");
        logPrintf("boot begin...\r\n");

        qspiReset();
        boot_mode = BOOT_MODE_LOADER;
      }
      else
      {
        delay(100);
        bootJumpToFw();
      }

      break;
  }

  usbInit();
  usbBegin(USB_CDC_MODE);
  vcpInit();
}

void apMain(void)
{
  uint32_t pre_time;

  while(1)
  {
    cmdifMain();

    if (cmdReceivePacket(&cmd_boot) == true)
    {
      bootProcessCmd(&cmd_boot);
    }

    if (millis()-pre_time >= 100)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }
  }
}





