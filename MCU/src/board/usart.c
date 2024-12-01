/*!
 \file   usart.c
 \date   November-2024 (created)
 \brief  USART driver
 */
#include "usart.h"

static USART_InitTypeDef USART_InitStruct;

/*!
 \brief Initializes the USART module
 \param USARTx - USART module
 */
void InitUsart(USART_TypeDef *USARTx)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_TypeDef *OneWirePort;
    uint16_t OneWirePin;
    
    if (USARTx == USART1) {
        OneWirePin = GPIO_Pin_9;
        OneWirePort = GPIOA;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    }
    else if (USARTx == USART2) {
        OneWirePin = GPIO_Pin_2;
        OneWirePort = GPIOA;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    }
    else return;
    
    /*  */
    GPIO_InitStruct.GPIO_Pin = OneWirePin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OneWirePort, &GPIO_InitStruct);
    /*  */
    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USARTx, &USART_InitStruct);
    
    USART_Cmd(USARTx, ENABLE);
    USART_HalfDuplexCmd(USARTx, ENABLE);
}

/*!
 \brief Sets the USART baud rate
 \param USARTx - USART module
 \param baudrate - USART baud rate, bit/sec
 */
void UsartSetBaudrate(USART_TypeDef *USARTx, uint32_t baudrate)
{
    uint32_t integerdivider = 0x00;
    uint32_t fractionaldivider = 0x00;
    uint32_t tmpreg = 0x00, apbclock = 0x00;
    RCC_ClocksTypeDef RCC_ClocksStatus;
    
    uint32_t usartxbase = (uint32_t)USARTx;
    
    RCC_GetClocksFreq(&RCC_ClocksStatus);
    
    if (usartxbase == USART1_BASE)
    {
        apbclock = RCC_ClocksStatus.PCLK2_Frequency;
    }
    else
    {
        apbclock = RCC_ClocksStatus.PCLK1_Frequency;
    }
  
    /* Determine the integer part */
    if ((USARTx->CR1 & CR1_OVER8_Set) != 0)
    {
        /* Integer part computing in case Oversampling mode is 8 Samples */
        integerdivider = ((25 * apbclock) / (2 * baudrate));    
    }
    else /* if ((USARTx->CR1 & CR1_OVER8_Set) == 0) */
    {
        /* Integer part computing in case Oversampling mode is 16 Samples */
        integerdivider = ((25 * apbclock) / (4 * baudrate));    
    }
    tmpreg = (integerdivider / 100) << 4;

    /* Determine the fractional part */
    fractionaldivider = integerdivider - (100 * (tmpreg >> 4));

    /* Implement the fractional part in the register */
    if ((USARTx->CR1 & CR1_OVER8_Set) != 0)
    {
        tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
    }
    else /* if ((USARTx->CR1 & CR1_OVER8_Set) == 0) */
    {
        tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
    }
  
    /* Write to USART BRR */
    USARTx->BRR = (uint16_t)tmpreg;
}

/*!
 \brief Transmits/Receives data byte via USART
 \param USARTx - USART module
 \param data_byte - data byte to transmit
 \param timeout - timeout waiting for ready flag
 \retval received data byte
 */
uint8_t UsartTxRxByte(USART_TypeDef *USARTx, uint8_t data_byte, uint32_t timeout)
{
    volatile uint32_t tx_timeout = timeout;
    volatile uint32_t rx_timeout = timeout;
    
    while(!(USARTx->SR & USART_FLAG_TXE))
    {
        if (--tx_timeout == 0) break;
    }
    USARTx->DR = (data_byte & (uint16_t)0x01FF);
    
    while(!(USARTx->SR & USART_FLAG_RXNE))
    {
        if (--rx_timeout == 0) break;
    }
    return (uint8_t)(USARTx->DR & (uint16_t)0x01FF);
}
