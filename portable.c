#include "Nano103.h"
#include "USER_LIB.h"
#include "bridging.h"
#include <stdbool.h>

uint8_t SPI0_WriteByte( uint8_t data ) {
    SPI_WRITE_TX0( USER_CFG_NRF24L01_0_SPI, data );
    SPI_TRIGGER( USER_CFG_NRF24L01_0_SPI );
    while ( SPI_IS_BUSY( USER_CFG_NRF24L01_0_SPI ) ) {};
    return ( uint8_t )( SPI_READ_RX0( USER_CFG_NRF24L01_0_SPI ) );
}
uint8_t SPI0_ReadByte( void ) {
    SPI_TRIGGER( USER_CFG_NRF24L01_0_SPI );
    while ( SPI_IS_BUSY( USER_CFG_NRF24L01_0_SPI ) ) {};
    return ( uint8_t )( SPI_READ_RX0( USER_CFG_NRF24L01_0_SPI ) );
}
void SPI0_SS( void ) {
    SPI_SET_SS0_LOW( SPI0 );
}
void SPI0_DSS( void ) {
    SPI_SET_SS0_HIGH( SPI0 );
}
void SPI0_Dummy( void ) {
    SPI_TRIGGER( USER_CFG_NRF24L01_0_SPI );
    while ( SPI_IS_BUSY( USER_CFG_NRF24L01_0_SPI ) )
        ;
}

SPI_Func_T st_SPI0 = { SPI0_WriteByte, SPI0_ReadByte, SPI0_SS, SPI0_DSS, SPI0_Dummy };

// Camera
uint32_t CameraUartWrite( uint8_t* array, uint32_t size ) {
    return UART_Write( UART1, array, size );
}

uint32_t CameraUartRead( uint8_t* array, uint32_t size ) {
    uint32_t receive;
    receive = UART_Read( UART1, array, 1 );
    FIFO_ByteIn( g_stOv528_s0->fifoBuf, array );
    return receive;
}

void CameraDelay( uint32_t time ) {
    DelayUs( 1000 * time );
}

// External Flash
uint8_t ExtFlashSpiWrite( uint8_t data ) {
    SPI_WRITE_TX0( USER_CFG_W25XX_0_SPI, data );
    SPI_TRIGGER( USER_CFG_W25XX_0_SPI );
    while ( SPI_IS_BUSY( USER_CFG_W25XX_0_SPI ) )
        ;
    return ( ( uint8_t )SPI_READ_RX0( USER_CFG_W25XX_0_SPI ) );
}

uint8_t ExtFlashSpiRead( void ) {
    SPI_TRIGGER( USER_CFG_W25XX_0_SPI );
    while ( SPI_IS_BUSY( USER_CFG_W25XX_0_SPI ) )
        ;
    return ( ( uint8_t )SPI_READ_RX0( USER_CFG_W25XX_0_SPI ) );
}

void ExtFlashSpiDummy( void ) {
    SPI_TRIGGER( USER_CFG_W25XX_0_SPI );
    while ( SPI_IS_BUSY( USER_CFG_W25XX_0_SPI ) )
        ;
}

void ExtFlashSpiSelect( void ) {
    SPI_SET_SS0_LOW( USER_CFG_W25XX_0_SPI );
}

void ExtFlashSpiDeselect( void ) {
    SPI_SET_SS0_HIGH( USER_CFG_W25XX_0_SPI );
}
