/*
 * i2c.c
 *
 *  Created on: 2020. 2. 11.
 *      Author: Baram
 */




#include "i2c.h"
#include "cmdif.h"
#include "delay.h"


#ifdef _USE_HW_I2C

#ifdef _USE_HW_CMDIF
static void i2cCmdifInit(void);
static void i2cCmdif(void);
#endif



static uint32_t i2c_timeout[I2C_MAX_CH];
static uint32_t i2c_errcount[I2C_MAX_CH];
static uint32_t i2c_freq[I2C_MAX_CH];

static bool is_init = false;
static bool is_begin[I2C_MAX_CH];


static I2C_HandleTypeDef hi2c4;

typedef struct
{
  I2C_HandleTypeDef *p_hi2c;

  GPIO_TypeDef *scl_port;
  int           scl_pin;

  GPIO_TypeDef *sda_port;
  int           sda_pin;
} i2c_tbl_t;

static i2c_tbl_t i2c_tbl[I2C_MAX_CH] =
    {
        { &hi2c4, GPIOB, GPIO_PIN_6,  GPIOD, GPIO_PIN_13},
    };


bool i2cInit(void)
{
  uint32_t i;


  for (i=0; i<I2C_MAX_CH; i++)
  {
    i2c_timeout[i] = 10;
    i2c_errcount[i] = 0;
    is_begin[i] = false;
  }

#ifdef _USE_HW_CMDIF
  i2cCmdifInit();
#endif

  is_init = true;
  return true;
}

bool i2cBegin(uint8_t ch, uint32_t freq_khz)
{
  bool ret = false;

  I2C_HandleTypeDef *p_handle = i2c_tbl[ch].p_hi2c;

  if (ch >= I2C_MAX_CH)
  {
    return false;
  }


  switch(ch)
  {
    case _DEF_I2C1:
      i2c_freq[ch] = freq_khz;

      p_handle->Instance             = I2C4;
      //p_handle->Init.Timing          = ((uint32_t)0x009034B6);
      p_handle->Init.Timing          = ((uint32_t)0x10C0ECFF);
      p_handle->Init.OwnAddress1     = 0x00;
      p_handle->Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
      p_handle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
      p_handle->Init.OwnAddress2     = 0xFF;
      p_handle->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
      p_handle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
      p_handle->Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

      HAL_I2C_DeInit(p_handle);
      if(HAL_I2C_Init(p_handle) != HAL_OK)
      {
      }

      /* Enable the Analog I2C Filter */
      HAL_I2CEx_ConfigAnalogFilter(p_handle,I2C_ANALOGFILTER_ENABLE);

      /* Configure Digital filter */
      HAL_I2CEx_ConfigDigitalFilter(p_handle, 0);

      ret = true;
      is_begin[ch] = true;
      break;
  }

  return ret;
}

void i2cReset(uint8_t ch)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  i2c_tbl_t *p_pin = &i2c_tbl[ch];


  GPIO_InitStruct.Pin       = p_pin->scl_pin;
  GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(p_pin->scl_port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin       = p_pin->sda_pin;
  GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  HAL_GPIO_Init(p_pin->sda_port, &GPIO_InitStruct);

  /*
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
  for (uint8_t i=0; i<9; i++)
  {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
  */

  HAL_GPIO_WritePin(p_pin->scl_port, p_pin->scl_pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(p_pin->sda_port, p_pin->sda_pin, GPIO_PIN_SET);
  delayUs(5);

  for (int i = 0; i < 9; i++)
  {

    HAL_GPIO_WritePin(p_pin->scl_port, p_pin->scl_pin, GPIO_PIN_RESET);
    delayUs(5);
    HAL_GPIO_WritePin(p_pin->scl_port, p_pin->scl_pin, GPIO_PIN_SET);
    delayUs(5);
  }

  HAL_GPIO_WritePin(p_pin->scl_port, p_pin->scl_pin, GPIO_PIN_RESET);
  delayUs(5);
  HAL_GPIO_WritePin(p_pin->sda_port, p_pin->sda_pin, GPIO_PIN_RESET);
  delayUs(5);

  HAL_GPIO_WritePin(p_pin->scl_port, p_pin->scl_pin, GPIO_PIN_SET);
  delayUs(5);
  HAL_GPIO_WritePin(p_pin->sda_port, p_pin->sda_pin, GPIO_PIN_SET);
}

bool i2cIsDeviceReady(uint8_t ch, uint8_t dev_addr)
{
  I2C_HandleTypeDef *p_handle = i2c_tbl[ch].p_hi2c;


  __disable_irq();
  if (HAL_I2C_IsDeviceReady(p_handle, dev_addr << 1, 10, 10) == HAL_OK)
  {
    __enable_irq();
    return true;
  }
  __enable_irq();

  return false;
}

bool i2cRecovery(uint8_t ch)
{
  bool ret;

  i2cReset(ch);

  ret = i2cBegin(ch, i2c_freq[ch]);

  return ret;
}

bool i2cReadByte (uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t timeout)
{
  return i2cReadBytes(ch, dev_addr, reg_addr, p_data, 1, timeout);
}

bool i2cReadByte2 (uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t timeout)
{
  bool ret = false;
  I2C_HandleTypeDef *p_handle = i2c_tbl[ch].p_hi2c;

  if (ch >= I2C_MAX_CH)
  {
    return false;
  }


  if (HAL_I2C_Master_Transmit(p_handle, (uint16_t)(dev_addr << 1), (uint8_t *)&reg_addr, 1, timeout) == HAL_OK)
  {
    if (HAL_I2C_Master_Receive(p_handle, (uint16_t)(dev_addr << 1), p_data, 1, timeout) == HAL_OK)
    {
      ret = true;
    }
  }
  else
  {
    ret = false;
  }

  return ret;
}

bool i2cReadBytes(uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
  bool ret;
  HAL_StatusTypeDef i2c_ret;
  I2C_HandleTypeDef *p_handle = i2c_tbl[ch].p_hi2c;

  if (ch >= I2C_MAX_CH)
  {
    return false;
  }

  i2c_ret = HAL_I2C_Mem_Read(p_handle, (uint16_t)(dev_addr << 1), reg_addr, I2C_MEMADD_SIZE_8BIT, p_data, length, timeout);

  if( i2c_ret == HAL_OK )
  {
    ret = true;
  }
  else
  {
    ret = false;
  }

  return ret;
}

bool i2cRead16Byte (uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t timeout)
{
  return i2cRead16Bytes(ch, dev_addr, reg_addr, p_data, 1, timeout);
}

bool i2cRead16Bytes(uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
  bool ret;
  HAL_StatusTypeDef i2c_ret;
  I2C_HandleTypeDef *p_handle = i2c_tbl[ch].p_hi2c;

  if (ch >= I2C_MAX_CH)
  {
    return false;
  }

  i2c_ret = HAL_I2C_Mem_Read(p_handle, (uint16_t)(dev_addr << 1), reg_addr, I2C_MEMADD_SIZE_16BIT, p_data, length, timeout);

  if( i2c_ret == HAL_OK )
  {
    ret = true;
  }
  else
  {
    ret = false;
  }

  return ret;
}

bool i2cReadData(uint8_t ch, uint16_t dev_addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
  bool ret;
  HAL_StatusTypeDef i2c_ret;
  I2C_HandleTypeDef *p_handle = i2c_tbl[ch].p_hi2c;

  if (ch >= I2C_MAX_CH)
  {
    return false;
  }

  i2c_ret = HAL_I2C_Master_Receive(p_handle, (uint16_t)(dev_addr << 1), p_data, length, timeout);

  if( i2c_ret == HAL_OK )
  {
    ret = true;
  }
  else
  {
    ret = false;
  }

  return ret;
}

bool i2cWriteByte (uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t data, uint32_t timeout)
{
  return i2cWriteBytes(ch, dev_addr, reg_addr, &data, 1, timeout);
}

bool i2cWriteByte2(uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t data, uint32_t timeout)
{
  bool ret;
  HAL_StatusTypeDef i2c_ret;
  I2C_HandleTypeDef *p_handle = i2c_tbl[ch].p_hi2c;
  uint8_t buf[] = {reg_addr, data};

  if (ch >= I2C_MAX_CH)
  {
    return false;
  }

  i2c_ret = HAL_I2C_Master_Transmit(p_handle, (uint16_t)(dev_addr << 1), buf, 2, timeout);

  if(i2c_ret == HAL_OK)
  {
    ret = true;
  }
  else
  {
    ret = false;
  }

  return ret;
}

bool i2cWriteBytes(uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
  bool ret;
  HAL_StatusTypeDef i2c_ret;
  I2C_HandleTypeDef *p_handle = i2c_tbl[ch].p_hi2c;

  if (ch >= I2C_MAX_CH)
  {
    return false;
  }

  i2c_ret = HAL_I2C_Mem_Write(p_handle, (uint16_t)(dev_addr << 1), reg_addr, I2C_MEMADD_SIZE_8BIT, p_data, length, timeout);

  if(i2c_ret == HAL_OK)
  {
    ret = true;
  }
  else
  {
    ret = false;
  }

  return ret;
}

bool i2cWrite16Byte (uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t data, uint32_t timeout)
{
  return i2cWrite16Bytes(ch, dev_addr, reg_addr, &data, 1, timeout);
}

bool i2cWrite16Bytes(uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
  bool ret;
  HAL_StatusTypeDef i2c_ret;
  I2C_HandleTypeDef *p_handle = i2c_tbl[ch].p_hi2c;

  if (ch >= I2C_MAX_CH)
  {
    return false;
  }

  i2c_ret = HAL_I2C_Mem_Write(p_handle, (uint16_t)(dev_addr << 1), reg_addr, I2C_MEMADD_SIZE_16BIT, p_data, length, timeout);

  if(i2c_ret == HAL_OK)
  {
    ret = true;
  }
  else
  {
    ret = false;
  }

  return ret;
}
bool i2cWriteData(uint8_t ch, uint16_t dev_addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
  bool ret;
  HAL_StatusTypeDef i2c_ret;
  I2C_HandleTypeDef *p_handle = i2c_tbl[ch].p_hi2c;

  if (ch >= I2C_MAX_CH)
  {
    return false;
  }


  i2c_ret = HAL_I2C_Master_Transmit(p_handle, (uint16_t)(dev_addr << 1), p_data, length, timeout);

  if(i2c_ret == HAL_OK)
  {
    ret = true;
  }
  else
  {
    ret = false;
  }

  return ret;
}

void i2cSetTimeout(uint8_t ch, uint32_t timeout)
{
  i2c_timeout[ch] = timeout;
}

uint32_t i2cGetTimeout(uint8_t ch)
{
  return i2c_timeout[ch];
}

void i2cClearErrCount(uint8_t ch)
{
  i2c_errcount[ch] = 0;
}

uint32_t i2cGetErrCount(uint8_t ch)
{
  return i2c_errcount[ch];
}







#ifdef _USE_HW_CMDIF
void i2cCmdifInit(void)
{
  cmdifAdd("i2c", i2cCmdif);
}


void i2cCmdif(void)
{
  bool ret = true;
  bool i2c_ret;

  uint8_t print_ch;
  uint8_t ch;
  uint16_t dev_addr;
  uint16_t reg_addr;
  uint16_t length;

  uint32_t i;
  uint8_t i2c_data[128];
  uint32_t pre_time;


  if (cmdifGetParamCnt() == 2)
  {
    print_ch = (uint16_t) cmdifGetParam(1);

    print_ch = constrain(print_ch, 1, I2C_MAX_CH);
    print_ch -= 1;

    if(cmdifHasString("scan", 0) == true)
    {
      for (i=0x00; i<= 0x7F; i++)
      {
        if (i2cIsDeviceReady(print_ch, i) == true)
        {
          cmdifPrintf("I2C CH%d Addr 0x%X : OK\n", print_ch+1, i);
        }
        else
        {
          cmdifPrintf("I2C CH%d Addr 0x%X : --\n", print_ch+1, i);
        }
      }
    }
    else if(cmdifHasString("begin", 0) == true)
    {
      i2c_ret = i2cBegin(print_ch, 400);
      if (i2c_ret == true)
      {
        cmdifPrintf("I2C CH%d Begin OK\n", print_ch + 1);
      }
      else
      {
        cmdifPrintf("I2C CH%d Begin Fail\n", print_ch + 1);
      }
    }
  }
  else if (cmdifGetParamCnt() == 5)
  {
    print_ch = (uint16_t) cmdifGetParam(1);
    print_ch = constrain(print_ch, 1, I2C_MAX_CH);

    dev_addr = (uint16_t) cmdifGetParam(2);
    reg_addr = (uint16_t) cmdifGetParam(3);
    length   = (uint16_t) cmdifGetParam(4);
    ch       = print_ch - 1;

    if(cmdifHasString("read", 0) == true)
    {
      for (i=0; i<length; i++)
      {
        i2c_ret = i2cReadByte(ch, dev_addr, reg_addr+i, i2c_data, 100);

        if (i2c_ret == true)
        {
          cmdifPrintf("%d I2C - 0x%02X : 0x%02X\n", print_ch, reg_addr+i, i2c_data[0]);
        }
        else
        {
          cmdifPrintf("%d I2C - Fail \n", print_ch);
          break;
        }
      }
    }
    if(cmdifHasString("read2", 0) == true)
    {
      for (i=0; i<length; i++)
      {
        i2c_ret = i2cReadByte2(ch, dev_addr, reg_addr+i, i2c_data, 100);

        if (i2c_ret == true)
        {
          cmdifPrintf("%d I2C - 0x%02X : 0x%02X\n", print_ch, reg_addr+i, i2c_data[0]);
        }
        else
        {
          cmdifPrintf("%d I2C - Fail \n", print_ch);
          break;
        }
      }
    }
    else if(cmdifHasString("read16", 0) == true)
    {
      for (i=0; i<length; i++)
      {
        i2c_ret = i2cRead16Byte(ch, dev_addr, reg_addr+i, i2c_data, 100);

        if (i2c_ret == true)
        {
          cmdifPrintf("%d I2C - 0x%02X : 0x%02X\n", print_ch, reg_addr+i, i2c_data[0]);
        }
        else
        {
          cmdifPrintf("%d I2C - Fail \n", print_ch);
          break;
        }
      }
    }
    else if(cmdifHasString("write", 0) == true)
    {
      pre_time = micros();
      i2c_ret = i2cWriteByte(ch, dev_addr, reg_addr, (uint8_t)length, 100);

      if (i2c_ret == true)
      {
        cmdifPrintf("%d I2C - 0x%02X : 0x%02X, %d us\n", print_ch, reg_addr, length, micros()-pre_time);
      }
      else
      {
        cmdifPrintf("%d I2C - Fail \n", print_ch);
      }
    }
    else if(cmdifHasString("write2", 0) == true)
    {
      pre_time = micros();
      i2c_ret = i2cWriteByte2(ch, dev_addr, reg_addr, (uint8_t)length, 100);

      if (i2c_ret == true)
      {
        cmdifPrintf("%d I2C - 0x%02X : 0x%02X, %d us\n", print_ch, reg_addr, length, micros()-pre_time);
      }
      else
      {
        cmdifPrintf("%d I2C - Fail \n", print_ch);
      }
    }
    else if(cmdifHasString("write16", 0) == true)
    {
      pre_time = micros();
      i2c_ret = i2cWrite16Byte(ch, dev_addr, reg_addr, (uint8_t)length, 100);

      if (i2c_ret == true)
      {
        cmdifPrintf("%d I2C - 0x%02X : 0x%02X, %d us\n", print_ch, reg_addr, length, micros()-pre_time);
      }
      else
      {
        cmdifPrintf("%d I2C - Fail \n", print_ch);
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
    cmdifPrintf( "i2c begin channel[1~%d]\n", I2C_MAX_CH);
    cmdifPrintf( "i2c read channel dev_addr reg_addr length\n");
    cmdifPrintf( "i2c read2 channel dev_addr reg_addr length\n");
    cmdifPrintf( "i2c read16 channel dev_addr reg_addr length\n");
    cmdifPrintf( "i2c write channel dev_addr reg_addr data\n");
    cmdifPrintf( "i2c write2 channel dev_addr reg_addr data\n");
    cmdifPrintf( "i2c write16 channel dev_addr reg_addr data\n");
  }
}




void I2C4_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&hi2c4);
}

void I2C4_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&hi2c4);
}




void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};


  if(hi2c->Instance == I2C4)
  {
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C4 GPIO Configuration
    PD13     ------> I2C4_SDA
    PB6     ------> I2C4_SCL
    */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C4;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_I2C4;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C4 clock enable */
    __HAL_RCC_I2C4_CLK_ENABLE();



    /* I2C4 interrupt Init */
    HAL_NVIC_SetPriority(I2C4_EV_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ(I2C4_EV_IRQn);
    HAL_NVIC_SetPriority(I2C4_ER_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ(I2C4_ER_IRQn);
  }
}

/**
  * @brief I2C MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param hi2c: I2C handle pointer
  * @retval None
  */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
  if(hi2c->Instance==I2C2)
  {
    /* Peripheral clock disable */
    __HAL_RCC_I2C4_CLK_DISABLE();

    /**I2C4 GPIO Configuration
    PD13     ------> I2C4_SDA
    PB6     ------> I2C4_SCL
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_13);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    /* I2C4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(I2C4_EV_IRQn);
    HAL_NVIC_DisableIRQ(I2C4_ER_IRQn);
  }
}

#endif


#endif
