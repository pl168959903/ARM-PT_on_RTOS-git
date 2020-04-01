
#include "NRF24L01.h"

volatile uint8_t NRF_Status_Reg;
volatile uint8_t NRF_Int_Flag = 0;

uint8_t rx_p0_add[ 5 ] = { 0x44, 0x43, 0x10, 0x10, 0x01 };
uint8_t tx_p0_add[ 5 ] = { 0x44, 0x43, 0x10, 0x10, 0x01 };

//---------------------------------------------------------------

NRF_T* NRF_New( SPI_Func_T* SpiFuncion ) {
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
    nrf->statusReg = nrf->spi->Write( reg );
    nrf->spi->Write( data );
    nrf->spi->Deselect();
}

void NRF_ReadRegArray( NRF_T* nrf, uint8_t reg, uint8_t* array, size_t size ) {
    size_t i;
    nrf->spi->Select();
    nrf->statusReg = nrf->spi->Write( reg | NRF_CMD_WRITE_REG );
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

uint8_t NRF_Rx( NRF_T* nrf, uint8_t* array ) {
    size_t i;
    nrf->spi->Select();
    nrf->spi->Write( NRF_CMD_RX_PAYLOAD );
    for ( i = 0; i < nrf->rxPayload; i++ ) {
        *( array + i ) = nrf->spi->Read();
    }
    nrf->spi->Deselect();
    return nrf->rxPayload;
}

uint8_t NRF_Tx( NRF_T* nrf, uint8_t* array ) {
    size_t i;
    nrf->spi->Select();
    nrf->spi->Write( NRF_CMD_TX_PATLOAD );
    for ( i = 0; i < nrf->txPayload, i++ ) {
        nrf->spi->Write( *( array + i ) );
    }
    nrf->spi->Deselect();
    return nrf->txPayload;
}

void NRF_FlushTx(NRF_T* nrf){
    nrf->spi->Select();
    nrf->spi->Write(NRF_CMD_FLUSH_TX);
    nrf->spi->Deselect();
}

void NRF_FlushRx(NRF_T* nrf){
    nrf->spi->Select();
    nrf->spi->Write(NRF_CMD_FLUSH_RX);
    nrf->spi->Deselect();
}

void NRF_ReuseTx( NRF_T* nrf){
    nrf->spi->Select();
    nrf->spi->Write(NRF_CMD_REUSE_TX);
    nrf->spi->Deselect();
}

void NRF_Nop( NRF_T* nrf){
    nrf->spi->Select();
    nrf->spi->Write(NRF_CMD_NOP);
    nrf->spi->Deselect();
}

//---------------------------------------------------------------

uint8_t NRF_Reset_IRQ( void ) {
    uint8_t st;
    NRF_Reg_Read_Byte( NRF_REG_STATUS, &st );
    NRF_Reg_Write_Byte( NRF_REG_STATUS, st );
    return st;
}
void NRF_Clr_FiFo( void ) {
    NRF_Reg_Write_Byte( NRF_CMD_CLR_RX_FIFO, 0xFF );
    NRF_Reg_Write_Byte( NRF_CMD_CLR_TX_FIFO, 0xFF );
}

void NRF_Tx_Mode( void ) {
    NRF_CE_PIN = 1;
    NRF_Reset_IRQ();
    NRF_Clr_FiFo();
    NRF_Reg_Write_Arg( NRF_REG_TX_ADDR, 5, tx_p0_add );
    NRF_Reg_Write_Arg( NRF_REG_ADDR_P0, 5, rx_p0_add );
    NRF_Reg_Write_Byte( NRF_REG_AUTO_ACK, 0x01 );  // 0x01
    NRF_Reg_Write_Byte( NRF_REG_EN_ADDR, 0x01 );
    NRF_Reg_Write_Byte( NRF_REG_RESEND, 0x1A );  // 0x1A
    NRF_Reg_Write_Byte( NRF_REG_RF_CH, 0x70 );
    NRF_Reg_Write_Byte( NRF_REG_RF_CONFIG, 0x0F );
    NRF_Reg_Write_Byte( NRF_REG_CONFIG, 0x0E );
    NRF_CE_PIN = 0;
}
void NRF_Rx_Mode( void ) {
    NRF_CE_PIN = 0;
    NRF_Reset_IRQ();
    NRF_Clr_FiFo();
    NRF_Reg_Write_Arg( NRF_REG_ADDR_P0, 5, rx_p0_add );
    NRF_Reg_Write_Byte( NRF_REG_AUTO_ACK, 0x01 );  // 0x01
    NRF_Reg_Write_Byte( NRF_REG_EN_ADDR, 0x01 );
    NRF_Reg_Write_Byte( NRF_REG_RX_PW_P0, RX_PLOAD_WIDTH );
    NRF_Reg_Write_Byte( NRF_REG_RF_CH, 0x70 );
    NRF_Reg_Write_Byte( NRF_REG_RF_CONFIG, 0x0F );
    NRF_Reg_Write_Byte( NRF_REG_CONFIG, 0x0F );
    NRF_CE_PIN = 1;
}
uint8_t NRF_Tx_Package( uint8_t mode, uint8_t* buf, uint8_t data_PW ) {
    volatile uint8_t st = 0;
    NRF_CE_PIN          = 1;
    NRF_Int_Flag        = 0;
    NRF_Reg_Write_Arg( NRF_CMD_TX_PATLOAD, data_PW, buf );
    while ( !NRF_Int_Flag ) {};
    if ( mode == NRF_MODE_1 ) {
        NRF_Reg_Write_Byte( NRF_CMD_CLR_TX_FIFO, 0xFF );
    }
    else if ( mode == NRF_MODE_2 ) {
        do {
            st = NRF_Reset_IRQ();
        } while ( !( st & TX_OK ) );
    }
    NRF_CE_PIN = 0;
    return st;
}
uint8_t NRF_Rx_package( uint8_t* buf ) {
    volatile uint8_t st;
    st = NRF_Reset_IRQ();
    if ( st & RX_OK ) {
        NRF_Reg_Read_Arg( NRF_CMD_RX_PAYLOAD, RX_PLOAD_WIDTH, buf );
        NRF_Reg_Write_Byte( NRF_CMD_CLR_RX_FIFO, 0xFF );
        return 1;
    }
    return 0;
}
