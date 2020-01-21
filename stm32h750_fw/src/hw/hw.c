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

  ledInit();
}
