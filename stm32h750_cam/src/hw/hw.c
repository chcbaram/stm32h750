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
  microsInit();
  millisInit();

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


  if (sdInit() == true)
  {
    fatfsInit();
  }


  logPrintf("Start...\r\n");


  dacInit();
  speakerInit();
  ili9225Init();
  lcdInit();
  i2cInit();
  ov7725Init();
  cameraInit();

  if (sdIsDetected() == true && buttonGetPressed(_DEF_HW_BTN_HOME) == true)
  {
    usbInit();
    usbBegin(USB_MSC_MODE);

    while(1)
    {
      lcdPrintf(0,  0, white, "USB Storage Mode");
      lcdPrintf(0, 16, white, "*");
      lcdUpdateDraw();
      delay(100);
    }
  }
  else
  {
    usbInit();
    usbBegin(USB_CDC_MODE);
    vcpInit();
  }
}

void hwJumpToBoot(void)
{
  rtcWriteBackupData(_HW_DEF_RTC_BOOT_MODE, (1<<7));
  resetRunSoftReset();
}
