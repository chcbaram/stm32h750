/*
 * joypad.c
 *
 *  Created on: 2020. 2. 1.
 *      Author: Baram
 */




#include "joypad.h"



#include "adc.h"


#define JOYPAD_ADC_MAX_COUNT     5
#define JOYPAD_DEAD_ZONE         100


static uint8_t  adc_ch_x = 0;
static uint8_t  adc_ch_y = 1;

static int32_t x_adc_value = 0;
static int32_t y_adc_value = 0;

static int32_t x_adc_offset = 0;
static int32_t y_adc_offset = 0;


static int32_t x_value = 0;
static int32_t y_value = 0;

static int32_t adc_data_x[JOYPAD_ADC_MAX_COUNT];
static int32_t adc_data_y[JOYPAD_ADC_MAX_COUNT];



static void joypadOffsetUpdate(void);


bool joypadInit(void)
{
  uint32_t i;

  for (i=0; i<JOYPAD_ADC_MAX_COUNT; i++)
  {
    adc_data_x[i] = 0;
    adc_data_y[i] = 0;
  }

  joypadOffsetUpdate();

  return true;
}

void joypadOffsetUpdate(void)
{
  uint32_t i;
  int32_t sum;


  for (i=0; i<JOYPAD_ADC_MAX_COUNT; i++)
  {
    adc_data_x[i] = adcRead(adc_ch_x);
    adc_data_y[i] = adcRead(adc_ch_y);
    delay(10);
  }


  sum = 0;
  for (i=0; i<JOYPAD_ADC_MAX_COUNT; i++)
  {
    sum += adc_data_x[i];
  }
  x_adc_value = sum / JOYPAD_ADC_MAX_COUNT;

  sum = 0;
  for (i=0; i<JOYPAD_ADC_MAX_COUNT; i++)
  {
    sum += adc_data_y[i];
  }
  y_adc_value = sum / JOYPAD_ADC_MAX_COUNT;


  x_adc_offset = x_adc_value;
  y_adc_offset = y_adc_value;
}


void joypadUpdate(void)
{
  int32_t value;


  x_adc_value = adcRead(adc_ch_x);
  y_adc_value = adcRead(adc_ch_y);

  value = constrain(x_adc_value-x_adc_offset, -2000, 2000);
  if (value >  JOYPAD_DEAD_ZONE)      value -= JOYPAD_DEAD_ZONE;
  else if (value < -JOYPAD_DEAD_ZONE) value += JOYPAD_DEAD_ZONE;
  else                                value  = 0;
  x_value = map(value, -2000, 2000, -100, 100);

  value = constrain(y_adc_value-y_adc_offset, -2000, 2000);
  if (value >  JOYPAD_DEAD_ZONE)      value -= JOYPAD_DEAD_ZONE;
  else if (value < -JOYPAD_DEAD_ZONE) value += JOYPAD_DEAD_ZONE;
  else                                value  = 0;
  y_value = map(value, -2000, 2000 , -100, 100);
}

int32_t joypadGetX(void)
{
  joypadUpdate();

  return x_value;
}

int32_t joypadGetY(void)
{
  joypadUpdate();

  return y_value;
}



bool joypadGetPressedButton(uint8_t ch)
{
  bool ret = false;

  joypadUpdate();

  switch(ch)
  {
    case 0:
      if (x_value > 50) ret = true;
      break;

    case 1:
      if (x_value < -50) ret = true;
      break;

    case 2:
      if (y_value > 50) ret = true;
      break;

    case 3:
      if (y_value < -50) ret = true;
      break;
  }


  return ret;
}
