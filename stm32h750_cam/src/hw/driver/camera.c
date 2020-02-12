/*
 * camera.c
 *
 *  Created on: 2020. 2. 12.
 *      Author: Baram
 */




#include "camera.h"
#include "i2c.h"
#include "ov7725.h"
#include "gpio.h"

static uint8_t i2c_ch = _DEF_I2C1;

static camera_t   sensor = {0};
static DCMI_HandleTypeDef  hcamera_dcmi;
static bool is_init = false;
static bool is_requested = false;

const int resolution[][2] = {
    {0,    0   },
    // C/SIF Resolutions
    {88,   72  },    /* QQCIF     */
    {176,  144 },    /* QCIF      */
    {352,  288 },    /* CIF       */
    {88,   60  },    /* QQSIF     */
    {176,  120 },    /* QSIF      */
    {352,  240 },    /* SIF       */
    // VGA Resolutions
    {40,   30  },    /* QQQQVGA   */
    {80,   60  },    /* QQQVGA    */
    {160,  120 },    /* QQVGA     */
    {320,  240 },    /* QVGA      */
    {640,  480 },    /* VGA       */
    {60,   40  },    /* HQQQVGA   */
    {120,  80  },    /* HQQVGA    */
    {240,  160 },    /* HQVGA     */
    // FFT Resolutions
    {64,   32  },    /* 64x32     */
    {64,   64  },    /* 64x64     */
    {128,  64  },    /* 128x64    */
    {128,  128 },    /* 128x64    */
    // Other
    {128,  160 },    /* LCD       */
    {128,  160 },    /* QQVGA2    */
    {720,  480 },    /* WVGA      */
    {752,  480 },    /* WVGA2     */
    {800,  600 },    /* SVGA      */
    {1024, 768 },    /* XGA       */
    {1280, 1024},    /* SXGA      */
    {1600, 1200},    /* UXGA      */
    {1280, 720 },    /* HD        */
    {1920, 1080},    /* FHD       */
    {2560, 1440},    /* QHD       */
    {2048, 1536},    /* QXGA      */
    {2560, 1600},    /* WQXGA     */
    {2592, 1944},    /* WQXGA2    */
};




bool cameraInit(void)
{
  if (i2cIsDeviceReady(i2c_ch, OV7725_SLV_ADDR) == true)
  {
    sensor.slv_addr = OV7725_SLV_ADDR;

    // Clear sensor chip ID.
    sensor.chip_id = 0;

    if (i2cReadByte2(i2c_ch, sensor.slv_addr, OV_CHIP_ID, &sensor.chip_id, 100) == true)
    {
      logPrintf("OV7725 CHIP_ID \t\t: 0x%X\n", sensor.chip_id);
    }
    else
    {
      logPrintf("OV7725 CHIP_ID \t\t: Fail\n");
      return false;
    }


    if (ov7725Open(&sensor) == true)
    {
      logPrintf("OV7725 Open \t\t: OK\n");
    }
    else
    {
      logPrintf("OV7725 Open \t\t: Fail\n");
      return false;
    }


    cameraReset();
    cameraSetPixformat(PIXFORMAT_RGB565);
    cameraSetFramesize(FRAMESIZE_HQVGA);
    //cameraSetFramesize(FRAMESIZE_QVGA);

    //sensor.set_auto_exposure(&sensor, false, 2000);


    is_init = true;
  }
  else
  {
    return false;
  }


  return true;
}

int cameraShutdown(int enable)
{
  if (enable)
  {
    gpioPinWrite(1, _DEF_HIGH);
  }
  else
  {
    gpioPinWrite(1, _DEF_LOW);
  }

  delay(10);
  return 0;
}

int cameraSetFramesize(framesize_t framesize)
{
  if (sensor.framesize == framesize) {
      // No change
      return 0;
  }

  // Call the sensor specific function
  if (sensor.set_framesize == NULL
      || sensor.set_framesize(&sensor, framesize) != 0) {
      // Operation not supported
      return -1;
  }

  // Set framebuffer size
  sensor.framesize = framesize;

  return 0;
}

int cameraSetFramerate(framerate_t framerate)
{
  if (sensor.framerate == framerate) {
     /* no change */
      return 0;
  }

  /* call the sensor specific function */
  if (sensor.set_framerate == NULL
      || sensor.set_framerate(&sensor, framerate) != 0) {
      /* operation not supported */
      return -1;
  }

  /* set the frame rate */
  sensor.framerate = framerate;

  return 0;
}

int cameraSetPixformat(pixformat_t pixformat)
{
  //uint32_t jpeg_mode = DCMI_JPEG_DISABLE;

  if (sensor.pixformat == pixformat) {
      // No change
      return 0;
  }

  if (sensor.set_pixformat == NULL
      || sensor.set_pixformat(&sensor, pixformat) != 0) {
      // Operation not supported
      return -1;
  }

  // Set pixel format
  sensor.pixformat = pixformat;

  // Set JPEG mode
  //if (pixformat == PIXFORMAT_JPEG) {
  //    jpeg_mode = DCMI_JPEG_ENABLE;
  //}

  // Skip the first frame.
  //MAIN_FB()->bpp = -1;

  //return dcmi_config(jpeg_mode);
  return 0;
}





static void DCMI_MspInit(DCMI_HandleTypeDef *hdcmi);
static void DCMI_MspDeInit(DCMI_HandleTypeDef *hdcmi);


bool cameraReset()
{
  cameraShutdown(true);
  delay(50);

  DCMI_MspInit(&hcamera_dcmi);

  /* DCMI configuration */
#if 0
  hcamera_dcmi.Instance              = DCMI;
  hcamera_dcmi.Init.CaptureRate      = DCMI_CR_ALL_FRAME;
  hcamera_dcmi.Init.HSPolarity       = DCMI_HSPOLARITY_LOW;
  hcamera_dcmi.Init.SynchroMode      = DCMI_SYNCHRO_HARDWARE;
  hcamera_dcmi.Init.VSPolarity       = DCMI_VSPOLARITY_HIGH;
  hcamera_dcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
  hcamera_dcmi.Init.PCKPolarity      = DCMI_PCKPOLARITY_RISING;
#else
  hcamera_dcmi.Instance              = DCMI;
  hcamera_dcmi.Init.CaptureRate      = DCMI_CR_ALL_FRAME;
  hcamera_dcmi.Init.HSPolarity       = DCMI_HSPOLARITY_LOW;
  hcamera_dcmi.Init.SynchroMode      = DCMI_SYNCHRO_HARDWARE;
  hcamera_dcmi.Init.VSPolarity       = DCMI_VSPOLARITY_HIGH;
  hcamera_dcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
  hcamera_dcmi.Init.PCKPolarity      = DCMI_PCKPOLARITY_RISING;
#endif

  hcamera_dcmi.Init.ByteSelectMode  = DCMI_BSM_ALL;         // Capture all received bytes
  hcamera_dcmi.Init.ByteSelectStart = DCMI_OEBS_ODD;        // Ignored
  hcamera_dcmi.Init.LineSelectMode  = DCMI_LSM_ALL;         // Capture all received lines
  hcamera_dcmi.Init.LineSelectStart = DCMI_OELS_ODD;        // Ignored


  if(HAL_DCMI_Init(&hcamera_dcmi) != HAL_OK)
  {
    return false;
  }


  // Reset the sesnor state
  sensor.sde         = 0;
  sensor.pixformat   = 0;
  sensor.framesize   = 0;
  sensor.framerate   = 0;
  sensor.gainceiling = 0;

  cameraShutdown(false);
  delay(50);

  // Call sensor-specific reset function
  if (sensor.reset(&sensor) != 0) {
      return false;
  }

  return true;
}

static int32_t getBpp(pixformat_t pixformat)
{
  int32_t bpp = 0;


  switch(pixformat)
  {
    case PIXFORMAT_BINARY:
    case PIXFORMAT_GRAYSCALE:
    case PIXFORMAT_BAYER:
    case PIXFORMAT_JPEG:
      bpp = 1;
      break;

    case PIXFORMAT_RGB565:
    case PIXFORMAT_YUV422:
      bpp = 2;
      break;

    default:
      bpp = 0;
      break;
  }

  return bpp;
}

bool cameraIsAvailble(void)
{
  if (is_requested == true)
  {
    return false;
  }
  return true;
}

bool cameraStart(uint8_t *pBff, uint32_t Mode)
{
  bool ret = true;
  int32_t x_res;
  int32_t y_res;


  x_res = resolution[sensor.framesize][0];
  y_res = resolution[sensor.framesize][1];

  is_requested = true;

  if(HAL_DCMI_Start_DMA(&hcamera_dcmi, Mode, (uint32_t)pBff, (uint32_t)(x_res * y_res * getBpp(sensor.pixformat))/4) != HAL_OK)
  {
    ret = false;
  }

  return ret;
}


bool cameraStop(void)
{
  bool ret = true;

  if(HAL_DCMI_Stop(&hcamera_dcmi) != HAL_OK)
  {
    ret = false;
  }

  return ret;
}

bool cameraSuspend(void)
{
  if(HAL_DCMI_Suspend(&hcamera_dcmi) != HAL_OK)
  {
    return false;
  }
  return true;
}

bool cameraResume(void)
{
  if(HAL_DCMI_Resume(&hcamera_dcmi) != HAL_OK)
  {
    return false;
  }

  is_requested = true;
  return true;
}

bool cameraDeInit(void)
{
  bool ret = true;;

  hcamera_dcmi.Instance = DCMI;

  if(cameraStop() != true)
  {
    ret = false;
  }
  else if(HAL_DCMI_DisableCROP(&hcamera_dcmi)!= HAL_OK)
  {
    ret = false;
  }
  else if(HAL_DCMI_DeInit(&hcamera_dcmi) != HAL_OK)
  {
    ret = false;
  }
  else
  {
    DCMI_MspDeInit(&hcamera_dcmi);
  }

  return ret;
}


/**
  * @brief  Initializes the DCMI MSP.
  * @param  hdcmi  DCMI handle
  * @retval None
  */
static void DCMI_MspInit(DCMI_HandleTypeDef *hdcmi)
{
  static DMA_HandleTypeDef hdma_handler;
  GPIO_InitTypeDef GPIO_InitStruct;

  /*** Enable peripherals and GPIO clocks ***/
  /* Enable DMA2 clock */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /*** Configure the GPIO ***/
  /* DCMI clock enable */
  __HAL_RCC_DCMI_CLK_ENABLE();

  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /**DCMI GPIO Configuration
  PE4     ------> DCMI_D4
  PE5     ------> DCMI_D6
  PE6     ------> DCMI_D7
  PA4     ------> DCMI_HSYNC
  PA6     ------> DCMI_PIXCLK
  PC6     ------> DCMI_D0
  PC7     ------> DCMI_D1
  PD3     ------> DCMI_D5
  PB7     ------> DCMI_VSYNC
  PE0     ------> DCMI_D2
  PE1     ------> DCMI_D3
  */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_0
                        |GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);



  /*** Configure the DMA ***/
  /* Set the parameters to be configured */
  hdma_handler.Init.Request             = DMA_REQUEST_DCMI;
  hdma_handler.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_handler.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_handler.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_handler.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  hdma_handler.Init.Mode                = DMA_CIRCULAR;
  hdma_handler.Init.Priority            = DMA_PRIORITY_LOW;
  hdma_handler.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
  hdma_handler.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_handler.Init.MemBurst            = DMA_MBURST_SINGLE;
  hdma_handler.Init.PeriphBurst         = DMA_PBURST_SINGLE;

  hdma_handler.Instance = DMA2_Stream1;

  /* Associate the initialized DMA handle to the DCMI handle */
  __HAL_LINKDMA(hdcmi, DMA_Handle, hdma_handler);

  /*** Configure the NVIC for DCMI and DMA ***/
  /* NVIC configuration for DCMI transfer complete interrupt */
  HAL_NVIC_SetPriority(DCMI_IRQn, 0x05, 0);
  HAL_NVIC_EnableIRQ(DCMI_IRQn);

  /* NVIC configuration for DMA2D transfer complete interrupt */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0x05, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

  /* Configure the DMA stream */
  (void)HAL_DMA_Init(hdcmi->DMA_Handle);
}

/**
  * @brief  DeInitializes the DCMI MSP.
  * @param  hdcmi  DCMI handle
  * @retval None
  */
static void DCMI_MspDeInit(DCMI_HandleTypeDef *hdcmi)
{
  /* Disable NVIC  for DCMI transfer complete interrupt */
  HAL_NVIC_DisableIRQ(DCMI_IRQn);

  /* Disable NVIC for DMA2 transfer complete interrupt */
  HAL_NVIC_DisableIRQ(DMA2_Stream1_IRQn);

  /* Peripheral clock disable */
  __HAL_RCC_DCMI_CLK_DISABLE();

  /**DCMI GPIO Configuration
  PE4     ------> DCMI_D4
  PE5     ------> DCMI_D6
  PE6     ------> DCMI_D7
  PA4     ------> DCMI_HSYNC
  PA6     ------> DCMI_PIXCLK
  PC6     ------> DCMI_D0
  PC7     ------> DCMI_D1
  PD3     ------> DCMI_D5
  PB7     ------> DCMI_VSYNC
  PE0     ------> DCMI_D2
  PE1     ------> DCMI_D3
  */
  HAL_GPIO_DeInit(GPIOE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_0
                        |GPIO_PIN_1);

  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4|GPIO_PIN_6);

  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6|GPIO_PIN_7);

  HAL_GPIO_DeInit(GPIOD, GPIO_PIN_3);

  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

  /* DCMI DMA DeInit */
  HAL_DMA_DeInit(hdcmi->DMA_Handle);
}


void DCMI_IRQHandler(void)
{
  HAL_DCMI_IRQHandler(&hcamera_dcmi);

  if (hcamera_dcmi.ErrorCode > 0)
  {
    logPrintf("DCMI Err %d\n", (int)hcamera_dcmi.ErrorCode);
  }

}

void DMA2_Stream1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(hcamera_dcmi.DMA_Handle);
  if (hcamera_dcmi.DMA_Handle->ErrorCode > 0)
  {
    logPrintf("DMA Err %d\n", (int)hcamera_dcmi.DMA_Handle->ErrorCode);
  }
}

/**
  * @brief  Line event callback
  * @param  hdcmi  pointer to the DCMI handle
  * @retval None
  */
void HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdcmi);
  //BSP_CAMERA_LineEventCallback(0);
}

/**
  * @brief  Frame event callback
  * @param  hdcmi pointer to the DCMI handle
  * @retval None
  */
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdcmi);


  static uint32_t pre_time;
  static int32_t fps;
  static int32_t time;



  time = millis()-pre_time;
  if (time > 0)
  {
    fps = 1000/time;
  }
  logPrintf("%d ms,  %d fps\n", time, fps);
  pre_time = millis();

  is_requested = false;
  //BSP_CAMERA_FrameEventCallback(0);
}

/**
  * @brief  Vsync event callback
  * @param  hdcmi pointer to the DCMI handle
  * @retval None
  */
void HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef *hdcmi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdcmi);
}

/**
  * @brief  Error callback
  * @param  hdcmi pointer to the DCMI handle
  * @retval None
  */
void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef *hdcmi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdcmi);

  logPrintf("error %d\n", hdcmi->ErrorCode);
  HAL_DCMI_DeInit(&hcamera_dcmi);
  HAL_DCMI_Init(&hcamera_dcmi);
  //BSP_CAMERA_ErrorCallback(0);
}
