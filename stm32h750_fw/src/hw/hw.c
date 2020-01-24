/*
 * hw.c
 *
 *  Created on: 2020. 1. 21.
 *      Author: Baram
 */




#include "hw.h"





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
  uartOpen(_DEF_UART2, 57600);


  qspiInit();
  flashInit();

#if HW_USE_CDC == 1
  usbInit();
  usbBegin(USB_CDC_MODE);
  vcpInit();
#endif

  if (sdInit() == true)
  {
    fatfsInit();

#if HW_USE_MSC == 1
    usbInit();
    usbBegin(USB_MSC_MODE);
#endif
  }
}
