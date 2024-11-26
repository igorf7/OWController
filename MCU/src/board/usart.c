#include "usart.h"

static USART_InitTypeDef USART_InitStruct;

/*!
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
 */
void UsartSetBaudrate(USART_TypeDef *USARTx, uint32_t baudrate)
{ 
    /* Write to USART BRR */
    USARTx->BRR = SystemCoreClock / baudrate;
}

/*!
 */
void UsartTransmit(USART_TypeDef *USARTx, uint8_t *data, int16_t len, uint32_t timeout)
{    
    while (len--)
    {
        while(!(USARTx->SR & USART_FLAG_TXE))
        {
            if (--timeout == 0) break;
        }
        USARTx->DR = *data++;
    }
}

/*!
 */
void UsartReceive(USART_TypeDef *USARTx, uint8_t *data, int16_t len, uint32_t timeout)
{
    while (len--)
    {
        while(!(USARTx->SR & USART_FLAG_RXNE))
        {
            if (--timeout == 0) break;
        }
        *data++ = USARTx->DR;
    }
}

/*!
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
