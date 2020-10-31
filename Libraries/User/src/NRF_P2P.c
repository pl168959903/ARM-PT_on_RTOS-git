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
        NRF_WriteRegByte( nrf, NRF_REG_SETUP_RETR, ( ( 15 << NRF_REG_SETUP_RETR_ARD_POS ) | ( 15 << NRF_REG_SETUP_RETR_ARC_POS ) ) );
        NRF_WriteRegByte( nrf, NRF_REG_RF_CH, rfCh );
        NRF_WriteRegByte( nrf, NRF_REG_RF_SETUP, ( NRF_REG_RF_SETUP_RF_DR_MSK | ( 0x3 << NRF_REG_RF_SETUP_RF_PWR_POS ) | NRF_REG_RF_SETUP_LNA_HCURR_MSK ) );
        NRF_WriteRegByte( nrf, NRF_REG_STATUS, ( NRF_REG_STATUS_RX_DR_MSK | NRF_REG_STATUS_TX_DS_MSK | NRF_REG_STATUS_MAX_RT_MSK ) );
        NRF_WriteRegByte( nrf, NRF_REG_DYNPD, ( 0 ) );
        NRF_WriteRegByte( nrf, NRF_REG_FEATURE, ( 0 ) );

        //清除FIFO
        NRF_FlushRx( g_stNrf0 );
        NRF_FlushTx( g_stNrf0 );

        //清除中斷旗標
        NRF_WriteRegByte( nrf, NRF_REG_STATUS, nrf->statusReg );
    }
}

NRFP2P_CHANNEL_T* nrfP2P_NewChannel( NRF_T* nrf, uint8_t ch, uint8_t* address, bool autoAck, bool dypw ) {
    uint8_t           reg;
    NRFP2P_CHANNEL_T* ptr;
    ptr      = ( NRFP2P_CHANNEL_T* )nrfP2P_MALLOC( sizeof( NRFP2P_CHANNEL_T ) );
    ptr->nrf = nrf;
    ptr->ch  = ch;
    nrf->ResetCE();

    // EN_CH
    reg = NRF_ReadRegByte( nrf, NRF_REG_EN_RXADDR );
    NRF_WriteRegByte( nrf, NRF_REG_EN_RXADDR, reg | ( 0x1 << ( ch + NRF_REG_EN_RXADDR_ERX_P0_POS ) ) );

    // EN_AA
    if ( autoAck == true ) {
        reg = NRF_ReadRegByte( nrf, NRF_REG_EN_AA );
        NRF_WriteRegByte( nrf, NRF_REG_EN_AA, reg | ( 0x1 << ( ch + NRF_REG_EN_AA_P0_POS ) ) );
    }

    // Payload Wide
    if ( dypw == true ) {
        NRF_WriteRegByte( nrf, NRF_REG_DYNPD, ( 0x1 << ( ch + NRF_REG_DYNPD_DPL_P0_POS ) ) );
        reg = NRF_ReadRegByte( nrf, NRF_REG_FEATURE );
        NRF_WriteRegByte( nrf, NRF_REG_FEATURE, ( reg | NRF_REG_FEATURE_EN_DPL_MSK ) );
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

void nrfP2P_SetPayloadWide( NRFP2P_CHANNEL_T* nrfCh, uint8_t wide ) {
    NRF_WriteRegByte( nrfCh->nrf, ( NRF_REG_RX_PW_P0 + nrfCh->ch ), wide );
}

void nrfP2P_EnableTxAutoAck( NRF_T* nrf, uint8_t* destAddress ) {
    uint8_t reg;

    NRF_WriteRegArray( nrf, NRF_REG_RX_ADDR_P0, destAddress, 5 );

    reg = NRF_ReadRegByte( nrf, NRF_REG_EN_AA );
    NRF_WriteRegByte( nrf, NRF_REG_EN_AA, ( reg | NRF_REG_EN_AA_P0_MSK ) );

    reg = NRF_ReadRegByte( nrf, NRF_REG_EN_RXADDR );
    NRF_WriteRegByte( nrf, NRF_REG_EN_RXADDR, ( reg | NRF_REG_EN_RXADDR_ERX_P0_MSK ) );
}

void nrfP2P_EnableTxDypw( NRF_T* nrf ) {
    uint8_t reg;
    reg = NRF_ReadRegByte( nrf, NRF_REG_FEATURE );
    NRF_WriteRegByte( nrf, NRF_REG_FEATURE, ( reg | NRF_REG_FEATURE_EN_DPL_MSK ) );

    reg = NRF_ReadRegByte( nrf, NRF_REG_DYNPD );
    NRF_WriteRegByte( nrf, NRF_REG_DYNPD, ( reg | NRF_REG_DYNPD_DPL_P0_MSK ) );
}

bool nrfP2P_SendPacket( NRF_T* nrf, uint8_t* destAddress, uint8_t* array, uint8_t pw ) {
    uint8_t reg;

    NRF_WriteRegArray( nrf, NRF_REG_TX_ADDR, destAddress, 5 );
    NRF_TxPayload( nrf, array, pw );

    //傳輸、等待中斷
    nrf->SetCE();
    while ( ( ( nrf->statusReg ) & ( NRF_REG_STATUS_TX_DS_MSK | NRF_REG_STATUS_MAX_RT_MSK ) ) == 0 ) {};
    //nrf->ResetCE();

    //清除中斷
    reg = NRF_ReadRegByte( nrf, NRF_REG_STATUS );
    NRF_WriteRegByte( g_stNrf0, NRF_REG_STATUS, reg );

    //傳輸結果
    if ( reg & NRF_REG_STATUS_TX_DS_MSK ) {  //成功
        return true;
    }
    if ( reg & NRF_REG_STATUS_MAX_RT_MSK ) {  //逾時
        return false;
    }

    //例外狀況
    return false;
}

bool nrfP2P_TxReuse( NRF_T* nrf, uint32_t times ) {
    uint8_t  reg;
    uint32_t i = 0;
    do {
        NRF_Nop( g_stNrf0 );
        NRF_ReuseTx( nrf );
        nrf->SetCE();
        while ( ( ( nrf->statusReg ) & ( NRF_REG_STATUS_TX_DS_MSK | NRF_REG_STATUS_MAX_RT_MSK ) ) == 0 ) {};
        nrf->ResetCE();

        reg = NRF_ReadRegByte( nrf, NRF_REG_STATUS );
        NRF_WriteRegByte( g_stNrf0, NRF_REG_STATUS, reg );

    } while ( ( reg & NRF_REG_STATUS_MAX_RT_MSK ) && ( i++ < times ) );

    NRF_FlushTx( nrf );
    if ( reg & NRF_REG_STATUS_MAX_RT_MSK ) {
        
        return false;
    }
    else {
        return true;
    }
}
