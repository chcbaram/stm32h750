/*
 * spi.h
 *
 *  Created on: 2020. 1. 30.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_SPI_H_
#define SRC_COMMON_HW_INCLUDE_SPI_H_


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_SPI

#define SPI_MAX_CH       HW_SPI_MAX_CH




#define SPI_MODE0           0
#define SPI_MODE1           1
#define SPI_MODE2           2
#define SPI_MODE3           3





bool spiInit(void);

void     spiBegin(uint8_t spi_ch);
void     spiTransfer(uint8_t spi_ch, uint8_t *p_tx_data, uint8_t *p_rx_data, uint32_t length);
uint8_t  spiTransfer8(uint8_t spi_ch, uint8_t data);
uint16_t spiTransfer16(uint8_t spi_ch, uint16_t data);
void spiDmaTransfer(uint8_t spi_ch, void *buf, uint32_t length, uint32_t timeout) ;

void spiSetBitOrder(uint8_t spi_ch, uint8_t bitOrder);
void spiSetClockDivider(uint8_t spi_ch, uint32_t clockDiv);
void spiSetDataMode(uint8_t spi_ch, uint8_t dataMode);


void spiDmaStartTx(uint8_t spi_ch, uint8_t *p_buf, uint32_t length);
bool spiDmaIsTxDone(uint8_t spi_ch);
void spiDmaSetRefresh(uint8_t spi_ch, bool enable);
void spiAttachTxInterrupt(uint8_t spi_ch, void (*func)());

#ifdef __cplusplus
 }
#endif

#endif


#endif /* SRC_COMMON_HW_INCLUDE_SPI_H_ */
