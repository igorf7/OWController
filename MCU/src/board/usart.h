/*!
 \file   usart.h
 \date   November-2024 (created)
 \brief  USART driver header file
 */
#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

#define CR1_OVER8_Set             ((u16)0x8000)  /* USART OVER8 mode Enable Mask */
#define CR1_OVER8_Reset           ((u16)0x7FFF)  /* USART OVER8 mode Disable Mask */

/* API */
void InitUsart(USART_TypeDef *USARTx);
void UsartSetBaudrate(USART_TypeDef *USARTx, uint32_t baudrate);
uint8_t UsartTxRxByte(USART_TypeDef *USARTx, uint8_t data_byte);
#endif  // __USART_H
