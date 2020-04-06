#include "NRF_P2P.h"

uint8_t searchAddr[ 5 ] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE };

void nrfP2p_InitNrf( NRF_T* nrf, uint8_t rfCh ) {
    uint8_t statusReg;
    statusReg = NRF_Nop( nrf );
    if ( statusReg != 0xFF ) {
        NRF_WriteRegByte( nrf, NRF_REG_CFG, ( NRF_REG_CFG_EN_CRC_MSK | NRF_REG_CFG_PWR_UP_MSK | NRF_REG_CFG_PRIM_RX_MSK ) );
        NRF_WriteRegByte( nrf, NRF_REG_EN_AA, ( 0 ) );
        NRF_WriteRegByte( nrf, NRF_REG_EN_RXADDR, ( 0 ) );
        NRF_WriteRegByte( nrf, NRF_REG_SETUP_AW, 0x3 );
        NRF_WriteRegByte( nrf, NRF_REG_SETUP_RETR, ( ( 0 << NRF_REG_SETUP_RETR_ARD_POS ) | ( 15 << NRF_REG_SETUP_RETR_ARC_POS ) ) );
        NRF_WriteRegByte( nrf, NRF_REG_RF_CH, rfCh );
        NRF_WriteRegByte( nrf, NRF_REG_RF_SETUP, ( NRF_REG_RF_SETUP_RF_DR_MSK | ( 0x3 << NRF_REG_RF_SETUP_RF_PWR_POS ) | NRF_REG_RF_SETUP_LNA_HCURR_MSK ) );
        NRF_WriteRegByte( nrf, NRF_REG_STATUS, ( NRF_REG_STATUS_RX_DR_MSK | NRF_REG_STATUS_TX_DS_MSK | NRF_REG_STATUS_MAX_RT_MSK ) );
    }
}

void nrfP2p_NewChannel( NRF_T* nrf, uint8_t ch, uint8_t* addr, size_t bufSize, uint8_t rxpw, bool autoAck ) {
    uint8_t reg;
    reg = NRF_ReadRegByte( nrf, NRF_REG_EN_AA );
    if ( autoAck == true ) {
        NRF_WriteRegByte( nrf, NRF_REG_EN_AA, reg | ( 0x1 << ch ) );
    }
    else {
        NRF_WriteRegByte( nrf, NRF_REG_EN_AA, reg & ~( 0x1 << ch ) );
    }
    reg = NRF_ReadRegByte( nrf, NRF_REG_EN_RXADDR );
    NRF_WriteRegByte( nrf, NRF_REG_EN_RXADDR, ( reg | ( 0x1 << ch ) ) );
    if ( ch > 1 ) {
        NRF_WriteRegArray( nrf, ( NRF_REG_RX_ADDR_P0 + ch ), addr, 5 );
    }
    else {
        NRF_WriteRegByte( nrf, ( NRF_REG_RX_ADDR_P0 + ch ), *( addr + 4 ) );
    }
    NRF_WriteRegByte( nrf, ( NRF_REG_RX_PW_P0 + ch ), rxpw );
    nrf->buf[ ch ] = FIFO_New( bufSize, NULL );
}

bool nrfP2p_SendPacket( NRF_T* nrf, uint8_t* addr, uint8_t* array, uint8_t size, uint8_t txpw ) {
    NRF_WriteRegArray( nrf, NRF_REG_TX_ADDR, addr, 5 );
    NRF_Tx( nrf, array, txpw );
    while ( 1 ) {
        //NRF_Nop( nrf ); //如有中斷程式更新StatusReg，這行可以註解。
        if ( nrf->statusReg & NRF_REG_STATUS_TX_DS_MSK ){
            NRF_WriteRegByte(nrf,NRF_REG_STATUS, nrf->statusReg);
            return true;
        }
        if ( nrf->statusReg & NRF_REG_STATUS_MAX_RT_MSK ){
            NRF_WriteRegByte(nrf,NRF_REG_STATUS, nrf->statusReg);
            return false;
        }   
    }
}
