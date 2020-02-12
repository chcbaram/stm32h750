/*
 * speaker.c
 *
 *  Created on: 2020. 2. 1.
 *      Author: Baram
 */




#include "speaker.h"
#include "dac.h"
#include "gpio.h"


static uint8_t volume = 100;


bool speakerInit(void)
{

  speakerDisable();

  return true;
}

void speakerEnable(void)
{
  //gpioPinWrite(_PIN_GPIO_SPK_EN, _DEF_LOW);
}

void speakerDisable(void)
{
  //gpioPinWrite(_PIN_GPIO_SPK_EN, _DEF_HIGH);
}

void speakerSetVolume(uint8_t volume_data)
{
  if (volume != volume_data)
  {
    volume = volume_data;
  }
}

uint8_t speakerGetVolume(void)
{
  return volume;
}


void speakerStart(uint32_t hz)
{
  dacSetup(hz);
  dacStart();
}

void speakerStop(void)
{
  dacStop();
}

void speakerReStart(void)
{
  dacStart();
}

uint32_t speakerAvailable(void)
{
  return dacAvailable();
}

uint32_t speakerGetBufLength(void)
{
  return dacGetBufLength();
}

void speakerPutch(uint8_t data)
{
  dacPut16(map(data, 0, 255, 0, volume*4095/100));
}

void speakerWrite(uint8_t *p_data, uint32_t length)
{
  uint32_t i;


  for (i=0; i<length; i++)
  {
    speakerPutch(p_data[i]);
  }
}
