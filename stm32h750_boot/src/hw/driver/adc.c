/*
 * adc.c
 *
 *  Created on: 2020. 2. 1.
 *      Author: Baram
 */




#include "adc.h"
#include "gpio.h"
#include "cmdif.h"



#define JOYPAD_X_Pin          GPIO_PIN_7
#define JOYPAD_X_GPIO_Port    GPIOA
#define JOYPAD_Y_Pin          GPIO_PIN_5
#define JOYPAD_Y_GPIO_Port    GPIOC
#define BTN_HOME_Pin          GPIO_PIN_9
#define BTN_HOME_GPIO_Port    GPIOA
#define BTN_MENU_Pin          GPIO_PIN_0
#define BTN_MENU_GPIO_Port    GPIOD
#define BTN_SELECT_Pin        GPIO_PIN_4
#define BTN_SELECT_GPIO_Port  GPIOD
#define BTN_START_Pin         GPIO_PIN_6
#define BTN_START_GPIO_Port   GPIOD
#define BTN_B_Pin             GPIO_PIN_5
#define BTN_B_GPIO_Port       GPIOB
#define BTN_A_Pin             GPIO_PIN_8
#define BTN_A_GPIO_Port       GPIOB



typedef struct
{
  bool                    is_init;
  ADC_HandleTypeDef      *hADCx;
  uint32_t                adc_channel;
} adc_tbl_t;



static adc_tbl_t adc_tbl[ADC_MAX_CH];
static __attribute__((section(".sram_d3"))) uint16_t adc_data[ADC_MAX_CH];

static ADC_HandleTypeDef hadc1;
static DMA_HandleTypeDef hdma_adc1;

#ifdef _USE_HW_CMDIF
static void adcCmdif(void);
#endif


bool adcInit(void)
{
  uint32_t i;
  uint32_t ch;



  for (i=0; i<ADC_MAX_CH; i++)
  {
    adc_tbl[i].is_init = false;
  }


  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 2;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }



  if (HAL_ADC_Start_DMA(&hadc1,
                        (uint32_t *)adc_data,
                        ADC_MAX_CH
                       ) != HAL_OK)
  {
    Error_Handler();
  }


  // JOY_X
  ch = 0;
  adc_tbl[ch].is_init     = true;
  adc_tbl[ch].hADCx       = &hadc1;
  adc_tbl[ch].adc_channel = ADC_CHANNEL_7;


  // JOY_Y
  ch = 1;
  adc_tbl[ch].is_init     = true;
  adc_tbl[ch].hADCx       = &hadc1;
  adc_tbl[ch].adc_channel = ADC_CHANNEL_8;



  delay(50);

#ifdef _USE_HW_CMDIF
  cmdifAdd("adc", adcCmdif);
#endif

  return true;
}

uint32_t adcRead(uint8_t ch)
{
  uint32_t adc_value;

  if (adc_tbl[ch].is_init != true)
  {
    return 0;
  }

  adc_value = adc_data[ch];

  return adc_value;
}

uint32_t adcRead8(uint8_t ch)
{
  return adcRead(ch)>>4;
}

uint32_t adcRead10(uint8_t ch)
{
  return adcRead(ch)>>2;
}

uint32_t adcRead12(uint8_t ch)
{
  return adcRead(ch);
}

uint32_t adcRead16(uint8_t ch)
{
  return adcRead(ch)<<4;
}

uint32_t adcReadVoltage(uint8_t ch)
{
  return adcConvVoltage(ch, adcRead(ch));
}

uint32_t adcReadCurrent(uint8_t ch)
{

  return adcConvCurrent(ch, adcRead(ch));
}

uint32_t adcConvVoltage(uint8_t ch, uint32_t adc_value)
{
  uint32_t ret = 0;

  switch(ch)
  {
    case 0:
    case 1:
      ret  = (uint32_t)((adc_value * 3300 * 10) / (4095*10));
      ret += 5;
      ret /= 10;
      break;

    case 2:
      ret  = (uint32_t)((adc_value * 3445 * 26) / (4095*10));
      ret += 5;
      ret /= 10;
      break;

  }

  return ret;
}

uint32_t adcConvCurrent(uint8_t ch, uint32_t adc_value)
{
  return 0;
}

uint8_t  adcGetRes(uint8_t ch)
{
  return 0;
}




void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
    /* ADC1 clock enable */
    __HAL_RCC_ADC12_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PA7     ------> ADC1_INP7
    PC5     ------> ADC1_INP8
    */
    GPIO_InitStruct.Pin = JOYPAD_X_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(JOYPAD_X_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = JOYPAD_Y_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(JOYPAD_Y_GPIO_Port, &GPIO_InitStruct);


    /* ADC1 DMA Init */
    /* ADC1 Init */
    __HAL_RCC_DMA1_CLK_ENABLE();

    hdma_adc1.Instance = DMA1_Stream2;
    hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc1);
  }
}


void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{
  if(adcHandle->Instance==ADC1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_ADC12_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PA7     ------> ADC1_INP7
    PC5     ------> ADC1_INP8
    */
    HAL_GPIO_DeInit(JOYPAD_X_GPIO_Port, JOYPAD_X_Pin);

    HAL_GPIO_DeInit(JOYPAD_Y_GPIO_Port, JOYPAD_Y_Pin);

    /* ADC1 DMA DeInit */
    HAL_DMA_DeInit(adcHandle->DMA_Handle);
  }
}



#ifdef _USE_HW_CMDIF
//-- adcCmdif
//
void adcCmdif(void)
{
  bool ret = true;


  if (cmdifGetParamCnt() == 1)
  {
    if (cmdifHasString("show", 0) == true)
    {
      while(cmdifRxAvailable() == 0)
      {
        for (int i=0; i<ADC_MAX_CH; i++)
        {
          cmdifPrintf("%04d ", adcRead(i));
        }
        cmdifPrintf("\r\n");
        delay(50);
      }
    }
    else
    {
      ret = false;
    }
  }
  else if (cmdifGetParamCnt() == 2)
  {
    if (cmdifHasString("show", 0) == true && cmdifHasString("voltage", 1) == true)
    {
      while(cmdifRxAvailable() == 0)
      {
        for (int i=0; i<ADC_MAX_CH; i++)
        {
          uint32_t adc_data;

          adc_data = adcReadVoltage(i);

          cmdifPrintf("%d.%d ", adc_data/10, adc_data%10);
        }
        cmdifPrintf("\r\n");
        delay(50);
      }
    }
    else
    {
      ret = false;
    }
  }
  else
  {
    ret = false;
  }


  if (ret == false)
  {
    cmdifPrintf( "adc show\n");
    cmdifPrintf( "adc show voltage\n");
  }
}
#endif
