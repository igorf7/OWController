#include "spi.h"

/*!
 \brief Initializes the SPI peripheral module
*/
void InitSpi(SPI_TypeDef *SPIx)
{
    SPI_InitTypeDef SPI_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;
    uint32_t PORT_PeriphClock;
    GPIO_TypeDef *SPI_Port;
    uint16_t SPI_MOSI_pin, SPI_MISO_pin, SPI_SCK_pin;
    
    if (SPIx == SPI1) {
        SPI_Port = GPIOA;
        SPI_MOSI_pin = GPIO_Pin_7;
        SPI_MISO_pin = GPIO_Pin_6;
        SPI_SCK_pin = GPIO_Pin_5;
        PORT_PeriphClock = RCC_APB2Periph_GPIOA;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    }
    else if (SPIx == SPI2) {
        SPI_Port = GPIOB;
        SPI_MOSI_pin = GPIO_Pin_15;
        SPI_MISO_pin = GPIO_Pin_14;
        SPI_SCK_pin = GPIO_Pin_13;
        PORT_PeriphClock = RCC_APB2Periph_GPIOB;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    }
    else {
        return; //module selection error
    }
    
    RCC_APB2PeriphClockCmd(PORT_PeriphClock, ENABLE);
    GPIO_InitStruct.GPIO_Pin = SPI_SCK_pin | SPI_MOSI_pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = SPI_MISO_pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_Port, &GPIO_InitStruct);
    
    /* Configure the SPI module */
    SPI_I2S_DeInit(SPIx);
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(SPIx, &SPI_InitStruct);
    SPI_Cmd(SPIx, ENABLE);
}

/*!
 \brief Send/Receive byte via SPI
*/
uint8_t SPI_TxRx_byte(SPI_TypeDef *SPIx, uint8_t tx_data)
{
    uint8_t rx_data;
    while (!(SPIx->SR & SPI_I2S_FLAG_TXE));
    SPIx->DR = tx_data;
    while (!(SPIx->SR & SPI_I2S_FLAG_RXNE));
    rx_data = SPIx->DR; // reading data clears flag RXNE
    
    return rx_data;
}
