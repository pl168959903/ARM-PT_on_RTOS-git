
#include "NRF24L01.h"

//---------------------------------------------------------------
// CMD
NRF_T* NRF_New( SPI_Func_T* SpiFuncion) {
    NRF_T* nrfClass;
    nrfClass = ( NRF_T* )NRF_MALLOC( sizeof( NRF_T ) );
    if ( nrfClass != NULL ) {
        nrfClass->spi = SpiFuncion;
    }
    return nrfClass;
}
uint8_t NRF_ReadRegByte( NRF_T* nrf, uint8_t reg ) {
    uint8_t returnData;
    nrf->spi->Select();
    nrf->statusReg = nrf->spi->Write( reg );
    returnData     = nrf->spi->Read();
    nrf->spi->Deselect();
    return returnData;
}
void NRF_WriteRegByte( NRF_T* nrf, uint8_t reg, uint8_t data ) {
    nrf->spi->Select();
    nrf->statusReg = nrf->spi->Write( reg | NRF_CMD_WRITE_REG );
    nrf->spi->Write( data );
    nrf->spi->Deselect();
}
void NRF_ReadRegArray( NRF_T* nrf, uint8_t reg, uint8_t* array, size_t size ) {
    size_t i;
    nrf->spi->Select();
    nrf->statusReg = nrf->spi->Write( reg );
    for ( i = 0; i < size; i++ ) {
        *( array + i ) = nrf->spi->Read();
    }
    nrf->spi->Deselect();
}
void NRF_WriteRegArray( NRF_T* nrf, uint8_t reg, uint8_t* array, size_t size ) {
    size_t i;
    nrf->spi->Select();
    nrf->statusReg = nrf->spi->Write( reg | NRF_CMD_WRITE_REG );
    for ( i = 0; i < size; i++ ) {
        nrf->spi->Write( *( array + i ) );
    }
    nrf->spi->Deselect();
}
void NRF_Rx( NRF_T* nrf, uint8_t* array, uint8_t size ) {
    size_t i;
    nrf->spi->Select();
    nrf->statusReg = nrf->spi->Write( NRF_CMD_RX_PAYLOAD );
    for ( i = 0; i < size; i++ ) {
        *( array + i ) = nrf->spi->Read();
    }
    nrf->spi->Deselect();
}
void NRF_Tx( NRF_T* nrf, uint8_t* array, uint8_t size ) {
    size_t i;
    nrf->spi->Select();
    nrf->statusReg = nrf->spi->Write( NRF_CMD_TX_PATLOAD );
    for ( i = 0; i < size; i++ ) {
        nrf->spi->Write( *( array + i ) );
    }
    nrf->spi->Deselect();
}
void NRF_FlushTx( NRF_T* nrf ) {
    nrf->spi->Select();
    nrf->statusReg = nrf->spi->Write( NRF_CMD_FLUSH_TX );
    nrf->spi->Deselect();
}
void NRF_FlushRx( NRF_T* nrf ) {
    nrf->spi->Select();
    nrf->statusReg = nrf->spi->Write( NRF_CMD_FLUSH_RX );
    nrf->spi->Deselect();
}
void NRF_ReuseTx( NRF_T* nrf ) {
    nrf->spi->Select();
    nrf->spi->Write( NRF_CMD_REUSE_TX );
    nrf->spi->Deselect();
}
uint8_t NRF_Nop( NRF_T* nrf ) {
    nrf->spi->Select();
    nrf->statusReg = nrf->spi->Write( NRF_CMD_NOP );
    nrf->spi->Deselect();
    return nrf->statusReg;
}
//---------------------------------------------------------------
void NRF_PowerDown( NRF_T* nrf ) {
    uint8_t reg;
    reg = NRF_ReadRegByte( nrf, NRF_REG_CFG );
    reg &= ~( NRF_REG_CFG_PWR_UP_MSK );
    NRF_WriteRegByte( nrf, NRF_REG_CFG, reg );
}
void NRF_PowerOn( NRF_T* nrf ) {
    uint8_t reg;
    reg = NRF_ReadRegByte( nrf, NRF_REG_CFG );
    reg |=  NRF_REG_CFG_PWR_UP_MSK;
    NRF_WriteRegByte( nrf, NRF_REG_CFG, reg );
}
void NRF_RxMode( NRF_T* nrf ) {
    uint8_t reg;
    reg = NRF_ReadRegByte( nrf, NRF_REG_CFG );
    reg |=  NRF_REG_CFG_PRIM_RX_MSK;
    NRF_WriteRegByte( nrf, NRF_REG_CFG, reg );
}
void NRF_TxMode( NRF_T* nrf ) {
    uint8_t reg;
    reg = NRF_ReadRegByte( nrf, NRF_REG_CFG );
    reg &=  ~(NRF_REG_CFG_PRIM_RX_MSK);
    NRF_WriteRegByte( nrf, NRF_REG_CFG, reg );
}
uint8_t NRF_RstIrq( NRF_T* nrf ) {
    uint8_t statusReg;
    statusReg = NRF_ReadRegByte( nrf, NRF_REG_STATUS );
    NRF_WriteRegByte( nrf, NRF_REG_STATUS, statusReg );
    return statusReg;
}
