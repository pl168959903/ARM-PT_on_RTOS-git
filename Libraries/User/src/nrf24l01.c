
#include "NRF24L01.h"

//---------------------------------------------------------------
// CMD
NRF_T* NRF_New( SPI_Func_T* SpiFuncion, FUNC_PTR* SetCE, FUNC_PTR* ResetCE ) {
    NRF_T* nrfClass;
    nrfClass = ( NRF_T* )NRF_MALLOC( sizeof( NRF_T ) );
    if ( nrfClass != NULL ) {
        nrfClass->spi     = SpiFuncion;
        nrfClass->SetCE   = SetCE;
        nrfClass->ResetCE = ResetCE;
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
    nrf->statusReg = nrf->spi->Write( reg | NRF_CMD_W_REGISTER );
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
    nrf->statusReg = nrf->spi->Write( reg | NRF_CMD_W_REGISTER );
    for ( i = 0; i < size; i++ ) {
        nrf->spi->Write( *( array + i ) );
    }
    nrf->spi->Deselect();
}
void NRF_RxPayload( NRF_T* nrf, uint8_t* array, uint8_t size ) {
    size_t i;
    nrf->spi->Select();
    nrf->statusReg = nrf->spi->Write( NRF_CMD_R_RX_PAYLOAD );
    for ( i = 0; i < size; i++ ) {
        *( array + i ) = nrf->spi->Read();
    }
    nrf->spi->Deselect();
}
void NRF_TxPayload( NRF_T* nrf, uint8_t* array, uint8_t size ) {
    size_t i;
    nrf->spi->Select();
    nrf->statusReg = nrf->spi->Write( NRF_CMD_W_TX_PAYLOAD );
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
    nrf->spi->Write( NRF_CMD_REUSE_TX_PL );
    nrf->spi->Deselect();
}
uint8_t NRF_ReadRxPayloadWide( NRF_T* nrf ) {
    uint8_t returnData;
    nrf->spi->Select();
    nrf->statusReg = nrf->spi->Write( NRF_CMD_R_RX_PL_WID );
    returnData     = nrf->spi->Read();
    nrf->spi->Deselect();
    return returnData;
}
void NRF_AckPayload( NRF_T* nrf, uint8_t* array, uint8_t size, uint8_t ch ) {
    size_t i;
    nrf->spi->Select();
    nrf->statusReg = nrf->spi->Write( NRF_CMD_W_ACK_PAYLOAD | ( ch & 0x7 ) );
    for ( i = 0; i < size; i++ ) {
        nrf->spi->Write( *( array + i ) );
    }
    nrf->spi->Deselect();
}
void NRF_TxWithoutAutoAck( NRF_T* nrf, uint8_t* array, uint8_t size ) {
    size_t i;
    nrf->spi->Select();
    nrf->statusReg = nrf->spi->Write( NRF_CMD_W_TX_PAYLOAD_NOACK );
    for ( i = 0; i < size; i++ ) {
        nrf->spi->Write( *( array + i ) );
    }
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
    reg |= NRF_REG_CFG_PWR_UP_MSK;
    NRF_WriteRegByte( nrf, NRF_REG_CFG, reg );
}
void NRF_RxMode( NRF_T* nrf ) {
    uint8_t reg;
    nrf->ResetCE();
    reg = NRF_ReadRegByte( nrf, NRF_REG_CFG );
    reg |= NRF_REG_CFG_PRIM_RX_MSK;
    NRF_WriteRegByte( nrf, NRF_REG_CFG, reg );
    nrf->SetCE();
}
void NRF_TxMode( NRF_T* nrf ) {
    uint8_t reg;
    nrf->ResetCE();
    reg = NRF_ReadRegByte( nrf, NRF_REG_CFG );
    reg &= ~( NRF_REG_CFG_PRIM_RX_MSK );
    NRF_WriteRegByte( nrf, NRF_REG_CFG, reg );
    nrf->SetCE();
}
uint8_t NRF_RstIrq( NRF_T* nrf ) {
    uint8_t statusReg;
    statusReg = NRF_ReadRegByte( nrf, NRF_REG_STATUS );
    NRF_WriteRegByte( nrf, NRF_REG_STATUS, statusReg );
    return statusReg;
}
void NRF_RegPrintf( NRF_T* nrf ) {
    uint8_t i;
    uint8_t reg[ 5 ] = { 0 };
    printf( "--------------------------------------\n" );
    for ( i = 0; i <= 0x09; i++ ) {
        reg[ 0 ] = NRF_ReadRegByte( nrf, i );
        printf( "Reg : 0x%02X = 0x%02X\n", i, reg[ 0 ] );
    }
    for ( i = 0x0A; i <= 0x0B; i++ ) {
        NRF_ReadRegArray( nrf, i, reg, 5 );
        printf( "Reg : 0x%02X = 0x%02X%02X%02X%02X%02X\n", i, reg[ 4 ], reg[ 3 ], reg[ 2 ], reg[ 1 ], reg[ 0 ] );
    }
    for ( i = 0x0C; i <= 0x0F; i++ ) {
        reg[ 0 ] = NRF_ReadRegByte( nrf, i );
        printf( "Reg : 0x%02X = 0x%02X\n", i, reg[ 0 ] );
    }
    for ( i = 0x10; i <= 0x10; i++ ) {
        NRF_ReadRegArray( nrf, i, reg, 5 );
        printf( "Reg : 0x%02X = 0x%02X%02X%02X%02X%02X\n", i, reg[ 4 ], reg[ 3 ], reg[ 2 ], reg[ 1 ], reg[ 0 ] );
    }
    for ( i = 0x11; i <= 0x17; i++ ) {
        reg[ 0 ] = NRF_ReadRegByte( nrf, i );
        printf( "Reg : 0x%02X = 0x%02X\n", i, reg[ 0 ] );
    }
    for ( i = 0x1C; i <= 0x1D; i++ ) {
        reg[ 0 ] = NRF_ReadRegByte( nrf, i );
        printf( "Reg : 0x%02X = 0x%02X\n", i, reg[ 0 ] );
    }
}
