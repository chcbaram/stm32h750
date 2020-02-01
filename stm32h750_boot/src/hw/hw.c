/*
 * hw.c
 *
 *  Created on: 2020. 1. 21.
 *      Author: Baram
 */




#include "hw.h"




__attribute__((section(".version"))) uint8_t boot_name[32] = "STM32H750_B/D";
__attribute__((section(".version"))) uint8_t boot_ver[32]  = "B200202R1";



void hwInit(void)
{
  bspInit();

  resetInit();
  delayInit();
  millis();

  swtimerInit();
  cmdifInit();
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

  logPrintf("\n\n[ Bootloader Begin... ]\r\n");
  logPrintf("Booting..Name \t\t: %s\r\n", boot_name);
  logPrintf("Booting..Ver  \t\t: %s\r\n", boot_ver);

  resetLog();
  rtcInit();
  resetWaitCount();


  qspiInit();
  flashInit();


  if (sdInit() == true)
  {
    fatfsInit();
  }


  ili9225Init();
  lcdInit();

  lcdPrintf(0, 0*16, white, "%s", boot_name);
  lcdPrintf(0, 1*16, white, "%s", boot_ver);
  lcdUpdateDraw();
}







