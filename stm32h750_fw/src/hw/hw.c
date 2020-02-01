/*
 * hw.c
 *
 *  Created on: 2020. 1. 21.
 *      Author: Baram
 */




#include "hw.h"


extern flash_tag_t fw_tag;




void hwInit(void)
{
  bspInit();

  delayInit();
  millis();

  resetInit();
  cmdifInit();
  swtimerInit();

  ledInit();
  gpioInit();
  pwmInit();
  spiInit();
  adcInit();
  joypadInit();
  buttonInit();
  uartInit();
  uartOpen(_DEF_UART1, 57600);
  uartOpen(_DEF_UART2, 57600);


  logPrintf("\n\n[ Firmware Begin... ]\r\n");
  logPrintf("Booting..Board\t\t: %s\r\n", fw_tag.board_str);
  logPrintf("Booting..Name \t\t: %s\r\n", fw_tag.name_str);
  logPrintf("Booting..Ver  \t\t: %s\r\n", fw_tag.version_str);

  rtcInit();

  //qspiInit();
  //flashInit();

#if HW_USE_CDC == 1
  usbInit();
  usbBegin(USB_CDC_MODE);
  vcpInit();
#endif

  if (sdInit() == true)
  {
    fatfsInit();

#if HW_USE_MSC == 1
    //usbInit();
    //usbBegin(USB_MSC_MODE);
#endif
  }

  logPrintf("Start...\r\n");


  dacInit();
  speakerInit();
  ili9225Init();
  lcdInit();
}

void hwJumpToBoot(void)
{
  rtcWriteBackupData(_HW_DEF_RTC_BOOT_MODE, (1<<7));
  resetRunSoftReset();
}
