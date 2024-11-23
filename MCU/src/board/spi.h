#ifndef SPI_H
#define SPI_H

#include "stm32f10x.h"
#include "stm32f10x_spi.h"

/* SPI API */
void InitSpi(SPI_TypeDef *SPIx);
uint8_t SPI_TxRx_byte(SPI_TypeDef *SPIx, uint8_t tx_data);

#endif // SPI_H
