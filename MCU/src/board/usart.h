#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

/* API */
void InitUsart(USART_TypeDef *USARTx);
void UsartSetBaudrate(USART_TypeDef *USARTx, uint32_t baudrate);
void UsartTransmit(USART_TypeDef *USARTx, uint8_t *data, int16_t len);
void UsartReceive(USART_TypeDef *USARTx, uint8_t *data, int16_t len);
uint8_t UsartTxRxByte(USART_TypeDef *USARTx, uint8_t data_byte);
#endif  // __USART_H
