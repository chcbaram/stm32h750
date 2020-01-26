/*
 * millis.c
 *
 *  Created on: 2020. 1. 21.
 *      Author: Baram
 */




#include "millis.h"



bool millisInit(void)
{
  return true;
}

uint32_t millis(void)
{
  return HAL_GetTick();
}

