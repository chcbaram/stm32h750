/*
 * ap.cpp
 *
 *  Created on: 2020. 1. 21.
 *      Author: Baram
 */




#include "ap.h"





void apInit(void)
{
  hwInit();
}

void apMain(void)
{
  uint32_t pre_time;

  while(1)
  {
    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
      uartPrintf(_DEF_UART1, "tx\n");
    }

    if (uartAvailable(_DEF_UART1) > 0)
    {
      uartPrintf(_DEF_UART1, "rx : 0x%X\n", uartRead(_DEF_UART1));
    }
  }
}
