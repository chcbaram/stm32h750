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

  while(1)
  {
    cmdifMain();

    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }
  }
}





void testCmdif(void)
{
  bool ret = true;


  if (cmdifGetParamCnt() == 1 && cmdifHasString("info", 0) == true)
  {
    cmdifPrintf("cmdif test\n");
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
