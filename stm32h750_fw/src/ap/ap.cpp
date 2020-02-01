/*
 * ap.cpp
 *
 *  Created on: 2020. 1. 21.
 *      Author: Baram
 */




#include "ap.h"




extern uint32_t _flash_tag_addr;
extern uint32_t _flash_fw_addr;



__attribute__((section(".tag"))) flash_tag_t fw_tag =
    {
     // fw info
     //
     0xAAAA5555,        // magic_number
     "V200129R1",       // version_str
     "STM32H750_B/D",   // board_str
     "Firmware",        // name
     __DATE__,
     __TIME__,
     (uint32_t)&_flash_tag_addr,
     (uint32_t)&_flash_fw_addr,


     // tag info
     //
    };




void testCmdif(void);




void apInit(void)
{
  hwInit();

  cmdifOpen(_DEF_UART1, 57600);

  cmdifAdd("test", testCmdif);
}

void apMain(void)
{
  uint32_t pre_time;
  uint16_t x = 0;
  uint16_t y = 0;

  while(1)
  {
    cmdifMain();

    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);

      //logPrintf("X : %d\n", (int)joypadGetX());
      //logPrintf("Y : %d\n", (int)joypadGetY());
    }


    if (lcdDrawAvailable() > 0)
    {
      lcdClearBuffer(black);
      lcdPrintf(0,  0, white, "%d ms", ili9225GetFpsTime());
      lcdPrintf(0, 16, white, "%d fps", ili9225GetFps());

      lcdDrawFillRect(x, 32, 30, 30, red);
      lcdDrawFillRect(lcdGetWidth()-x, 62, 30, 30, green);
      lcdDrawFillRect(x + 30, 92, 30, 30, blue);

      x += 2;

      x %= lcdGetWidth();
      y %= lcdGetHeight();

      lcdRequestDraw();
    }
  }
}





void testCmdif(void)
{
  bool ret = true;
  uint8_t tx_buf[256];


  if (cmdifGetParamCnt() == 1 && cmdifHasString("info", 0) == true)
  {
    cmdifPrintf("cmdif test\n");
  }
  else if (cmdifGetParamCnt() == 1 && cmdifHasString("spi", 0) == true)
  {
    cmdifPrintf("cmdif spi\n");

    spiDmaTransfer(_DEF_SPI1, tx_buf, 256, 100);
  }
  else
  {
    ret = false;
  }

  if (ret == false)
  {
    cmdifPrintf( "test info \n");
  }
}
