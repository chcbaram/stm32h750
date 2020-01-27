/*
 * reset.c
 *
 *  Created on: 2020. 1. 27.
 *      Author: Baram
 */




#include "reset.h"

#ifdef _USE_HW_RTC
#include "rtc.h"

#ifdef _USE_HW_LED
#include "led.h"
#endif

static uint8_t reset_count  = 0;
#endif

static uint8_t reset_status = 0x00;
static uint8_t reset_bits   = 0x00;



void resetInit(void)
{
  uint8_t ret = 0;

  if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
  {
   ret |= (1<<_DEF_RESET_PIN);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST) != RESET)
  {
   ret |= (1<<_DEF_RESET_POWER);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
  {
   ret |= (1<<_DEF_RESET_POWER);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDG1RST) != RESET)
  {
   ret |= (1<<_DEF_RESET_WDG);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDG1RST) != RESET)
  {
   ret |= (1<<_DEF_RESET_WDG);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET)
  {
   ret |= (1<<_DEF_RESET_SOFT);
  }

  reset_bits = ret;

  __HAL_RCC_CLEAR_RESET_FLAGS();


  if (ret & (1<<_DEF_RESET_WDG))
  {
    reset_status = _DEF_RESET_WDG;
  }
  else if (ret & (1<<_DEF_RESET_SOFT))
  {
    reset_status = _DEF_RESET_SOFT;
  }
  else if (ret & (1<<_DEF_RESET_POWER))
  {
    reset_status = _DEF_RESET_POWER;
  }
  else
  {
    reset_status = _DEF_RESET_PIN;
  }
}

void resetLog(void)
{
  if (reset_bits & (1<<_DEF_RESET_POWER))
  {
    logPrintf("ResetFrom \t\t: Power\r\n");
  }
  if (reset_bits & (1<<_DEF_RESET_PIN))
  {
    logPrintf("ResetFrom \t\t: Pin\r\n");
  }
  if (reset_bits & (1<<_DEF_RESET_WDG))
  {
    logPrintf("ResetFrom \t\t: Wdg\r\n");
  }
  if (reset_bits & (1<<_DEF_RESET_SOFT))
  {
    logPrintf("ResetFrom \t\t: Soft\r\n");
  }
}

void resetRunSoftReset(void)
{
  HAL_NVIC_SystemReset();
}

void resetClearFlag(void)
{
  __HAL_RCC_CLEAR_RESET_FLAGS();
}

uint8_t resetGetStatus(void)
{
  return reset_status;
}

uint8_t resetGetBits(void)
{
  return reset_bits;
}

void resetToSysBoot(void)
{
  void (*SysMemBootJump)(void);
  volatile uint32_t addr;

  //addr = 0x1FF00000;  // F7
  addr = 0x1FF09800;  // H7


  bspDeInit();

  SysMemBootJump = (void (*)(void)) (*((uint32_t *)(addr + 4)));
  __set_MSP(*(uint32_t *)addr);

  SysMemBootJump();
}


#ifdef _USE_HW_RTC
void resetWaitCount(void)
{
  rtcWriteBackupData(RESET_BOOT_SRC, resetGetBits());

  logPrintf("ResetBits \t\t: 0x%X\n", (int)rtcReadBackupData(RESET_BOOT_SRC));


  if (resetGetBits() == (1<<_DEF_RESET_PIN))
  {
    reset_count = (uint8_t)rtcReadBackupData(RESET_BOOT_CNT);
    logPrintf("ResetCount \t\t: %d\r\n", (int)reset_count);

    rtcWriteBackupData(RESET_BOOT_CNT, rtcReadBackupData(RESET_BOOT_CNT) + 1);
#ifdef _USE_HW_LED
    ledOn(_DEF_LED1);
#endif
    delay(500);
    rtcWriteBackupData(RESET_BOOT_CNT, 0);
#ifdef _USE_HW_LED
    ledOff(_DEF_LED1);
#endif
    rtcWriteBackupData(RESET_BOOT_MODE, 0);
  }
  else
  {
    logPrintf("ResetCount \t\t: %d\r\n", (int)reset_count);
    rtcWriteBackupData(RESET_BOOT_CNT, 0);
  }

  logPrintf("ResetMode \t\t: %d\r\n", (int)rtcReadBackupData(RESET_BOOT_MODE));
}

uint8_t resetGetCount(void)
{
  return reset_count;
}
#endif
