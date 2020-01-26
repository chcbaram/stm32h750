/*
 * ap.cpp
 *
 *  Created on: 2020. 1. 21.
 *      Author: Baram
 */




#include "ap.h"



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
