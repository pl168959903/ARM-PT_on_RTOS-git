#include "NRF_P2P.h"
#include "project.h"

void nrfP2P_InitNrf( NRF_T* nrf, uint8_t rfCh ) {
    uint8_t statusReg;
    statusReg = NRF_Nop( nrf );
    if ( statusReg != 0xFF ) {
        nrf->ResetCE();

        //寫暫存器
        NRF_WriteRegByte( nrf, NRF_REG_CFG, ( NRF_REG_CFG_EN_CRC_MSK | NRF_REG_CFG_PWR_UP_MSK | NRF_REG_CFG_PRIM_RX_MSK ) );
        NRF_WriteRegByte( nrf, NRF_REG_EN_AA, ( 0 ) );
        NRF_WriteRegByte( nrf, NRF_REG_EN_RXADDR, ( 0 ) );
        NRF_WriteRegByte( nrf, NRF_REG_SETUP_AW, 0x3 );
        NRF_WriteRegByte( nrf, NRF_REG_SETUP_RETR, ( ( 0 << NRF_REG_SETUP_RETR_ARD_POS ) | ( 15 << NRF_REG_SETUP_RETR_ARC_POS ) ) );
        NRF_WriteRegByte( nrf, NRF_REG_RF_CH, rfCh );
        NRF_WriteRegByte( nrf, NRF_REG_RF_SETUP, ( NRF_REG_RF_SETUP_RF_DR_MSK | ( 0x3 << NRF_REG_RF_SETUP_RF_PWR_POS ) | NRF_REG_RF_SETUP_LNA_HCURR_MSK ) );
        NRF_WriteRegByte( nrf, NRF_REG_STATUS, ( NRF_REG_STATUS_RX_DR_MSK | NRF_REG_STATUS_TX_DS_MSK | NRF_REG_STATUS_MAX_RT_MSK ) );
        NRF_WriteRegByte( nrf, NRF_REG_DYNPD, ( 0 ) );
        NRF_WriteRegByte( nrf, NRF_REG_FEATURE, ( 0 ) );

        //清除中斷旗標
        NRF_WriteRegByte( nrf, NRF_REG_STATUS, nrf->statusReg);
    }
}

nrfP2P_CHANNEL_T* nrfP2P_NewChannel( NRF_T* nrf, uint8_t ch, uint8_t* address, uint8_t* destAddr, uint8_t rxpw, uint8_t txpw, bool autoAck ) {
    uint8_t           reg;
    nrfP2P_CHANNEL_T* ptr;
    ptr           = ( nrfP2P_CHANNEL_T* )nrfP2P_MALLOC( sizeof( nrfP2P_CHANNEL_T ) );
    ptr->nrf      = nrf;
    ptr->ch       = ch;
    ptr->address  = address;
    ptr->destAddr = destAddr;
    ptr->rxpw     = rxpw;
    ptr->txpw     = txpw;
    ptr->autoAck  = autoAck;

    nrf->ResetCE();
    // EN_AA
    if ( autoAck == true ) {
        reg = NRF_ReadRegByte( nrf, NRF_REG_EN_AA );
        NRF_WriteRegByte( nrf, NRF_REG_EN_AA, reg | ( 0x1 << ch ) | NRF_REG_EN_AA_P0_MSK );
        reg = NRF_ReadRegByte( nrf, NRF_REG_EN_RXADDR );
        NRF_WriteRegByte( nrf, NRF_REG_EN_RXADDR, ( reg | ( 0x1 << ch ) | NRF_REG_EN_RXADDR_ERX_P0_MSK ) );
        NRF_WriteRegByte( nrf, ( NRF_REG_RX_PW_P0 + ch ), ( rxpw & NRF_REG_RX_PW_P0_MSK ) );
        NRF_WriteRegByte( nrf, NRF_REG_RX_PW_P0, ( txpw & NRF_REG_RX_PW_P0_MSK ) );
    }
    else {
        reg = NRF_ReadRegByte( nrf, NRF_REG_EN_AA );
        NRF_WriteRegByte( nrf, NRF_REG_EN_AA, reg & ~( 0x1 << ch ) );
        reg = NRF_ReadRegByte( nrf, NRF_REG_EN_RXADDR );
        NRF_WriteRegByte( nrf, NRF_REG_EN_RXADDR, ( reg | ( 0x1 << ch ) | NRF_REG_EN_RXADDR_ERX_P0_MSK ) );
        NRF_WriteRegByte( nrf, ( NRF_REG_RX_PW_P0 + ch ), ( rxpw & NRF_REG_RX_PW_P0_MSK ) );
    }

    if ( ch > 1 ) {
        uint8_t temp[ 5 ];
        memcpy( temp, address, 5 );
        NRF_WriteRegByte( nrf, ( NRF_REG_RX_ADDR_P0 + ch ), ( uint8_t )*address );
        temp[ 0 ] = NRF_ReadRegByte( nrf, NRF_REG_RX_ADDR_P1 );
        NRF_WriteRegArray( nrf, NRF_REG_RX_ADDR_P1, temp, 5 );
    }
    else {
        NRF_WriteRegArray( nrf, ( NRF_REG_RX_ADDR_P0 + ch ), address, 5 );
    }
    return ptr;
}

bool nrfP2P_SendPacket( nrfP2P_CHANNEL_T* ptr, uint8_t* array ) {

    // Set p0 ack channel;
    if ( ptr->autoAck == true ) {
        NRF_WriteRegArray( ptr->nrf, NRF_REG_RX_ADDR_P0, ptr->destAddr, 5 );
    }
    // Send Data
    NRF_WriteRegArray( ptr->nrf, NRF_REG_TX_ADDR, ptr->destAddr, 5 );
    ptr->nrf->ResetCE();
    NRF_Tx( ptr->nrf, array, ptr->txpw );
    ptr->nrf->SetCE();
    DelayUs( 50 );
    ptr->nrf->ResetCE();
    // Interrupt
    while ( 1 ) {
        // NRF_Nop( ptr->nrf );  //如有中斷程式更新StatusReg，這行可以註解。
        if ( ptr->nrf->statusReg & NRF_REG_STATUS_TX_DS_MSK ) {
            NRF_WriteRegByte( ptr->nrf, NRF_REG_STATUS, ptr->nrf->statusReg );
            return true;
        }
        if ( ptr->nrf->statusReg & NRF_REG_STATUS_MAX_RT_MSK ) {
            NRF_FlushTx( ptr->nrf );
            NRF_WriteRegByte( ptr->nrf, NRF_REG_STATUS, ptr->nrf->statusReg );
            return false;
        }
    }
}

bool nrfP2P_ReceivePacket( nrfP2P_CHANNEL_T* ptr, uint8_t* array ) {
    uint8_t i;
    // NRF_Nop(ptr->nrf); //如有中斷程式更新StatusReg，這行可以註解。
    if ( ptr->nrf->statusReg & NRF_REG_STATUS_RX_DR_MSK ) {
        if ( ( ( ptr->nrf->statusReg & NRF_REG_STATUS_RX_P_NO_MSK ) >> NRF_REG_STATUS_RX_P_NO_POS ) == ptr->ch ) {
            for ( i = 0; i < ptr->rxpw; i++ ) {
                NRF_Rx( ptr->nrf, array, ptr->rxpw );
            }
            NRF_WriteRegByte( ptr->nrf, NRF_REG_STATUS, ptr->nrf->statusReg );
            return true;
        }
    }
    return false;
}
