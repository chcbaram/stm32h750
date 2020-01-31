/*
 * spi.c
 *
 *  Created on: 2020. 1. 30.
 *      Author: Baram
 */





#include "spi.h"
#include "hw.h"




#define SPI_TX_DMA_MAX_LENGTH   0xEFFF
#define SPI_DATA_8BIT           0
#define SPI_DATA_16BIT          1


typedef struct
{
  bool tx_done;
  uint8_t *p_tx_buf;
  uint8_t *p_tx_buf_next;
  uint32_t tx_length_next;
} spi_dma_buf_t;

typedef struct
{
  bool               is_open;
  bool               is_dma_init;
  bool               is_refresh;
  SPI_HandleTypeDef  h_spi;
  DMA_HandleTypeDef  hdma_tx;
  spi_dma_buf_t      dma_tx_buf;

  void              (*func_tx)(void);
} spi_t;





static spi_t spi_tbl[SPI_MAX_CH];



bool spiInit(void)
{
  uint8_t i;


  for(i=0; i<SPI_MAX_CH; i++)
  {
    spi_tbl[i].is_open = false;
    spi_tbl[i].is_dma_init = false;
    spi_tbl[i].is_refresh = false;
    spi_tbl[i].func_tx    = NULL;

    spi_tbl[i].dma_tx_buf.p_tx_buf         = NULL;
    spi_tbl[i].dma_tx_buf.p_tx_buf_next    = NULL;
    spi_tbl[i].dma_tx_buf.tx_done          = false;
    spi_tbl[i].dma_tx_buf.tx_length_next   = 0;
  }

  return true;
}


void spiBegin(uint8_t spi_ch)
{
  spi_t  *p_spi;



  switch(spi_ch)
  {
    case _DEF_SPI1:
      p_spi = &spi_tbl[spi_ch];

      p_spi->h_spi.Instance               = SPI1;
      p_spi->h_spi.Init.Mode              = SPI_MODE_MASTER;
      p_spi->h_spi.Init.Direction         = SPI_DIRECTION_2LINES;
      p_spi->h_spi.Init.DataSize          = SPI_DATASIZE_16BIT;
      p_spi->h_spi.Init.CLKPolarity       = SPI_POLARITY_LOW;
      p_spi->h_spi.Init.CLKPhase          = SPI_PHASE_1EDGE;
      p_spi->h_spi.Init.NSS               = SPI_NSS_SOFT;
      p_spi->h_spi.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
      p_spi->h_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; // 50 Mbit
      p_spi->h_spi.Init.FirstBit          = SPI_FIRSTBIT_MSB;
      p_spi->h_spi.Init.TIMode            = SPI_TIMODE_DISABLE;
      p_spi->h_spi.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
      p_spi->h_spi.Init.CRCPolynomial     = 0;
      p_spi->h_spi.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;
      p_spi->h_spi.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
      p_spi->h_spi.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
      p_spi->h_spi.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
      p_spi->h_spi.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
      p_spi->h_spi.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
      p_spi->h_spi.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
      p_spi->h_spi.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
      p_spi->h_spi.Init.IOSwap = SPI_IO_SWAP_DISABLE;

      HAL_SPI_Init(&p_spi->h_spi);

      p_spi->is_open = true;
      break;
  }
}


void spiTransfer(uint8_t spi_ch, uint8_t *p_tx_data, uint8_t *p_rx_data, uint32_t length)
{

  return;
}


uint8_t spiTransfer8(uint8_t spi_ch, uint8_t data)
{
  uint8_t ret;
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false) return 0;


  HAL_SPI_TransmitReceive(&p_spi->h_spi, &data, &ret, 1, 0xffff);

  return ret;
}


uint16_t spiTransfer16(uint8_t spi_ch, uint16_t data)
{
  uint8_t tBuf[2];
  uint8_t rBuf[2];
  uint16_t ret;
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false) return 0;

  if (p_spi->h_spi.Init.DataSize == SPI_DATASIZE_8BIT)
  {
    tBuf[1] = (uint8_t)data;
    tBuf[0] = (uint8_t)(data>>8);
    HAL_SPI_TransmitReceive(&p_spi->h_spi, (uint8_t *)&tBuf, (uint8_t *)&rBuf, 2, 0xffff);

    ret = rBuf[0];
    ret <<= 8;
    ret += rBuf[1];
  }
  else
  {
    HAL_SPI_TransmitReceive(&p_spi->h_spi, (uint8_t *)&data, (uint8_t *)&ret, 1, 0xffff);
  }
  return ret;
}


void spiSetBitOrder(uint8_t spi_ch, uint8_t bitOrder)
{
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false) return;

  p_spi->h_spi.Init.FirstBit = bitOrder;
  HAL_SPI_Init(&p_spi->h_spi);
}


void spiSetClockDivider(uint8_t spi_ch, uint32_t clockDiv)
{
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false) return;

  p_spi->h_spi.Init.BaudRatePrescaler = clockDiv;
  HAL_SPI_Init(&p_spi->h_spi);
}


void spiSetDataMode(uint8_t spi_ch, uint8_t dataMode)
{
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false) return;


  switch( dataMode )
  {
    // CPOL=0, CPHA=0
    case SPI_MODE0:
      p_spi->h_spi.Init.CLKPolarity = SPI_POLARITY_LOW;
      p_spi->h_spi.Init.CLKPhase    = SPI_PHASE_1EDGE;
      HAL_SPI_Init(&p_spi->h_spi);
      break;

    // CPOL=0, CPHA=1
    case SPI_MODE1:
      p_spi->h_spi.Init.CLKPolarity = SPI_POLARITY_LOW;
      p_spi->h_spi.Init.CLKPhase    = SPI_PHASE_2EDGE;
      HAL_SPI_Init(&p_spi->h_spi);
      break;

    // CPOL=1, CPHA=0
    case SPI_MODE2:
      p_spi->h_spi.Init.CLKPolarity = SPI_POLARITY_HIGH;
      p_spi->h_spi.Init.CLKPhase    = SPI_PHASE_1EDGE;
      HAL_SPI_Init(&p_spi->h_spi);
      break;

    // CPOL=1, CPHA=1
    case SPI_MODE3:
      p_spi->h_spi.Init.CLKPolarity = SPI_POLARITY_HIGH;
      p_spi->h_spi.Init.CLKPhase    = SPI_PHASE_2EDGE;
      HAL_SPI_Init(&p_spi->h_spi);
      break;
  }
}


void spiDmaStartTx(uint8_t spi_ch, uint8_t *p_buf, uint32_t length)
{
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false)     return;
  if (p_spi->is_dma_init == false) return;


  if(length > SPI_TX_DMA_MAX_LENGTH)
  {
    p_spi->dma_tx_buf.tx_done       = false;
    p_spi->dma_tx_buf.tx_length_next= length - SPI_TX_DMA_MAX_LENGTH;
    p_spi->dma_tx_buf.p_tx_buf      =  p_buf;
    p_spi->dma_tx_buf.p_tx_buf_next = &p_buf[SPI_TX_DMA_MAX_LENGTH];

    HAL_SPI_Transmit_DMA(&p_spi->h_spi, p_spi->dma_tx_buf.p_tx_buf, SPI_TX_DMA_MAX_LENGTH);
  }
  else
  {
    p_spi->dma_tx_buf.tx_done       = false;
    p_spi->dma_tx_buf.tx_length_next= 0;
    p_spi->dma_tx_buf.p_tx_buf      = p_buf;
    p_spi->dma_tx_buf.p_tx_buf_next = NULL;

    HAL_SPI_Transmit_DMA(&p_spi->h_spi, p_spi->dma_tx_buf.p_tx_buf, length);
  }
}


bool spiDmaIsTxDone(uint8_t spi_ch)
{
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false)     return true;
  if (p_spi->is_dma_init == false) return true;


  return p_spi->dma_tx_buf.tx_done;
}


void spiDmaSetRefresh(uint8_t spi_ch, bool enable)
{
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false)     return;
  if (p_spi->is_dma_init == false) return;


  p_spi->is_refresh = enable;

}

void spiDmaTransfer(uint8_t spi_ch, void *buf, uint32_t length, uint32_t timeout)
{
  uint32_t t_time;


  spiDmaStartTx(spi_ch, (uint8_t *)buf, length);

  t_time = millis();

  if (timeout == 0) return;

  while(1)
  {
    if(spiDmaIsTxDone(spi_ch))
    {
      break;
    }
    if((millis()-t_time) > timeout)
    {
      break;
    }
  }
}

void spiAttachTxInterrupt(uint8_t spi_ch, void (*func)())
{
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false)     return;
  if (p_spi->is_dma_init == false) return;


  p_spi->func_tx = func;

}


void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  volatile uint16_t length;
  spi_t  *p_spi;

  if (hspi->Instance == spi_tbl[_DEF_SPI1].h_spi.Instance)
  {
    p_spi = &spi_tbl[_DEF_SPI1];

    if(p_spi->dma_tx_buf.tx_length_next > 0)
    {
      p_spi->dma_tx_buf.p_tx_buf = p_spi->dma_tx_buf.p_tx_buf_next;

      if(p_spi->dma_tx_buf.tx_length_next > SPI_TX_DMA_MAX_LENGTH)
      {
        length = SPI_TX_DMA_MAX_LENGTH;
        p_spi->dma_tx_buf.tx_length_next = p_spi->dma_tx_buf.tx_length_next - SPI_TX_DMA_MAX_LENGTH;
        p_spi->dma_tx_buf.p_tx_buf_next = &p_spi->dma_tx_buf.p_tx_buf[SPI_TX_DMA_MAX_LENGTH];
      }
      else
      {
        length = p_spi->dma_tx_buf.tx_length_next;
        p_spi->dma_tx_buf.tx_length_next = 0;
        p_spi->dma_tx_buf.p_tx_buf_next = NULL;
      }
      HAL_SPI_Transmit_DMA(hspi, p_spi->dma_tx_buf.p_tx_buf, length);
    }
    else
    {
      p_spi->dma_tx_buf.tx_done = true;

      if (p_spi->func_tx != NULL)
      {
        (*p_spi->func_tx)();
      }
    }
  }
}
void DMA1_Stream1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(spi_tbl[_DEF_SPI1].h_spi.hdmatx);
}

void SPI1_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&spi_tbl[_DEF_SPI1].h_spi);
}


void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  spi_t  *p_spi;
  GPIO_InitTypeDef  GPIO_InitStruct;




  if (hspi->Instance == spi_tbl[_DEF_SPI1].h_spi.Instance)
  {
    p_spi = &spi_tbl[_DEF_SPI1];

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable SPI clock */
    __HAL_RCC_SPI1_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PD7     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);


    if (p_spi->is_dma_init == false)
    {
      /*##-3- Configure the DMA ##################################################*/

      __HAL_RCC_DMA1_CLK_ENABLE();

      /* Configure the DMA handler for Transmission process */
      p_spi->hdma_tx.Instance                 = DMA1_Stream1;
      p_spi->hdma_tx.Init.Request             = DMA_REQUEST_SPI1_TX;
      p_spi->hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
      p_spi->hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
      p_spi->hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
      p_spi->hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
      p_spi->hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
      p_spi->hdma_tx.Init.Mode                = DMA_NORMAL;
      p_spi->hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
      p_spi->hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;

      HAL_DMA_Init(&p_spi->hdma_tx);

      /* Associate the initialized DMA handle to the the SPI handle */
      __HAL_LINKDMA(hspi, hdmatx, p_spi->hdma_tx);


      /*##-4- Configure the NVIC for DMA #########################################*/
      /* NVIC configuration for DMA transfer complete interrupt (SPI1_TX) */
      HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 5, 1);
      HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

      HAL_NVIC_SetPriority(SPI1_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(SPI1_IRQn);

      p_spi->is_dma_init = true;
    }
  }
}


void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{


  if (hspi->Instance == spi_tbl[_DEF_SPI1].h_spi.Instance)
  {
    /*##-1- Reset peripherals ##################################################*/
    __HAL_RCC_SPI1_CLK_DISABLE();

    /*##-2- Disable peripherals and GPIO Clocks ################################*/
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PD7     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5);
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_7);


    HAL_DMA_DeInit(&spi_tbl[_DEF_SPI1].hdma_tx);
    HAL_NVIC_DisableIRQ(DMA1_Stream1_IRQn);
  }
}
