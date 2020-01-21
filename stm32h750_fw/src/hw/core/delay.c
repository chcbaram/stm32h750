/*
 * delay.c
 *
 *  Created on: 2020. 1. 21.
 *      Author: Baram
 */




#include "delay.h"



bool delayInit(void)
{
  return true;
}

void delay(uint32_t ms)
{
  HAL_Delay(ms);
}

void delayNs(uint32_t ns)
{

}

void delayUs(uint32_t us)
{
}

void delayMs(uint32_t ms)
{
  HAL_Delay(ms);
}
