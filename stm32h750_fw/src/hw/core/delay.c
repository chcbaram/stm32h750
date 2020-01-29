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
#ifdef _USE_HW_RTOS
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
    osDelay(ms);
  }
  else
  {
    HAL_Delay(ms);
  }
#else
  HAL_Delay(ms);
#endif
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
