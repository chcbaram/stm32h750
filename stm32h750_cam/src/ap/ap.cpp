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
     "V200201R1",       // version_str
     "STM32H750_B/D",   // board_str
     "Camera",        // name
     __DATE__,
     __TIME__,
     (uint32_t)&_flash_tag_addr,
     (uint32_t)&_flash_fw_addr,


     // tag info
     //
    };


static void testCmdif(void);
static void threadEmul(void const *argument);



void apInit(void)
{
  hwInit();

  cmdifOpen(_DEF_UART1, 57600);

  cmdifAdd("test", testCmdif);

  osThreadDef(threadEmul, threadEmul, _HW_DEF_RTOS_THREAD_PRI_EMUL, 0, _HW_DEF_RTOS_THREAD_MEM_EMUL);
  if (osThreadCreate(osThread(threadEmul), NULL) != NULL)
  {
    logPrintf("threadEmul \t\t: OK\r\n");
  }
  else
  {
    logPrintf("threadEmul \t\t: Fail\r\n");
    while(1);
  }
}

static __attribute__((section(".sram_d1"))) uint32_t camera_buffer[320 * 240 / 2];


void apMain(void)
{
  uint32_t pre_time;
  uint16_t x = 0;
  uint16_t y = 0;

  cameraStart((uint8_t *)camera_buffer, CAMERA_MODE_CONTINUOUS);
  //cameraStart((uint8_t *)camera_buffer, CAMERA_MODE_SNAPSHOT);

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
#if 0
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
#else
    if (lcdDrawAvailable() > 0) //  && cameraIsAvailble() > 0)
    {
      //cameraStop();
      lcdClearBuffer(black);

      SCB_InvalidateDCache_by_Addr((uint32_t *)camera_buffer, 240*160*2);

      uint16_t *p_buf = (uint16_t *)camera_buffer;
      uint16_t color_h;
      uint16_t color_l;
      uint16_t color;
      for (y=0; y<160; y++)
      {
        for (x=0; x<220; x++)
        {
          color = p_buf[y*240 + x];
          color_h = color >> 8;
          color_l = color << 8;

          lcdDrawPixel(x, y, color_h | color_l);
        }
      }

      lcdPrintf(0,  0, white, "%d ms", ili9225GetFpsTime());
      lcdPrintf(0, 16, white, "%d fps", ili9225GetFps());

      lcdRequestDraw();
      //logPrintf("lcd draw\n");
      //cameraStart((uint8_t *)camera_buffer, CAMERA_MODE_SNAPSHOT);
      delay(10);
    }

    if (buttonGetPressedTime(_DEF_HW_BTN_A) > 50 && buttonGetPressedEvent(_DEF_HW_BTN_A) == true)
    {
      if (cameraIsAvailble() > 0)
      {
        cameraStop();
        cameraStart((uint8_t *)camera_buffer, CAMERA_MODE_SNAPSHOT);
      }
    }
#endif
  }
}


static void threadEmul(void const *argument)
{
  UNUSED(argument);

  while(1)
  {
    delay(100);
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


