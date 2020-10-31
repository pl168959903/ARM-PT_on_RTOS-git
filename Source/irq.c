#include "Project.h"

void GPABC_IRQHandler( void ) {
    // ext int
    if ( GPIO_GET_INT_FLAG( PA, BIT14 ) ) {
        // printf( "INT\n" );
        g_u8GetPictureReadyFlag = 1;
        g_u8ExternalTriggerFlag = 1;
        GPIO_CLR_INT_FLAG( PA, BIT14 );
    }
    // nRF IRQ
    if ( GPIO_GET_INT_FLAG( PB, BIT3 ) ) {
        NRF_Nop( g_stNrf0 );
        // printf("%02X\n",g_stNrf0->statusReg);
        if ( g_stNrf0->statusReg & NRF_REG_STATUS_RX_DR_MSK ) {

            uint8_t  i;
            uint8_t  data[ 32 ];
            uint32_t len = NRF_ReadRxPayloadWide( g_stNrf0 );
            NRF_RxPayload( g_stNrf0, data, len );
            for ( i = 0; i < len; i++ ) { FIFO_ByteIn( g_stNrfRx, ( data + i ) ); }
            // printf( "RX_DR\n" );
            NRF_WriteRegByte( g_stNrf0, NRF_REG_STATUS, g_stNrf0->statusReg );
        }
        if ( g_stNrf0->statusReg & NRF_REG_STATUS_TX_DS_MSK ) {
            // printf( "TX_DS\n" );
        }
        if ( g_stNrf0->statusReg & NRF_REG_STATUS_MAX_RT_MSK ) {
            // printf( "MAX_RT\n" );
        }
        GPIO_CLR_INT_FLAG( PB, BIT3 );
    }
}
void RTC_IRQHandler( void ) {
    if ( RTC_GET_ALARM_INT_FLAG() ) {
        RTC_SetDateAndTime( &g_stRtcTime );
        g_u8GetPictureReadyFlag = 1;
        RTC_CLEAR_ALARM_INT_FLAG();
    }
}
void UART0_IRQHandler( void ) {
    uint32_t intSt = UART0->INTSTS;
    uint8_t  dataTemp[ 1 ];
    if ( intSt & UART_INTSTS_RXTOIF_Msk ) {
        while ( !UART_GET_RX_EMPTY( UART0 ) ) {
            if ( dataTemp[ 0 ] == '$' ) { FIFO_Rst( g_stGpsRx ); }
            UART_Read( UART0, dataTemp, 1 );
            FIFO_ByteIn( g_stGpsRx, dataTemp );
        }
        g_u8GpsDataReadyFlag = 1;
        UART_ClearIntFlag( UART0, UART_INTSTS_RXTOIF_Msk );
    }
    else if ( intSt & UART_INTSTS_RDAIF_Msk ) {
        while ( !UART_GET_RX_EMPTY( UART0 ) ) {
            if ( dataTemp[ 0 ] == '$' ) { FIFO_Rst( g_stGpsRx ); }
            UART_Read( UART0, dataTemp, 1 );
            FIFO_ByteIn( g_stGpsRx, dataTemp );
        }
        UART_ClearIntFlag( UART0, UART_INTSTS_RDAIF_Msk );
    }
}
void UART1_IRQHandler( void ) {
    uint32_t intSt = UART1->INTSTS;
    uint8_t  dataTemp[ 1 ];
    if ( intSt & UART_INTSTS_RXTOIF_Msk ) {
        while ( !UART_GET_RX_EMPTY( UART1 ) ) { g_stOv528_s0->Uart->Read( dataTemp, 1 ); }
        UART_ClearIntFlag( UART1, UART_INTSTS_RXTOIF_Msk );
    }
    else if ( intSt & UART_INTSTS_RDAIF_Msk ) {
        while ( !UART_GET_RX_EMPTY( UART1 ) ) { g_stOv528_s0->Uart->Read( dataTemp, 1 ); }
        UART_ClearIntFlag( UART1, UART_INTSTS_RDAIF_Msk );
    }
}
void CKSD_IRQHandler( void ) {
    uint32_t clkDieFlag = CLK->CLKDSTS;
    if ( clkDieFlag & ( 0x1U << 1 ) ) {
        printf( "LXT Stop!!\n" );
        CLK->CLKDSTS = ( 0x1 << 1 );
    }
}
void HIRC_IRQHandler( void ) {
    uint32_t hirc1Flag = SYS_GET_IRC1TRIM_INT_FLAG();

    if ( hirc1Flag & SYS_IRCTISTS_32KERR_INT ) {
        printf( "LXT Clock error.\n" );
        SYS_CLEAR_IRC1TRIM_INT_FLAG( hirc1Flag );
    }

    if ( hirc1Flag & SYS_IRCTISTS_FAIL_INT ) {
        printf( "HIRC1 : Trim Failure.\n" );
        SYS_CLEAR_IRC1TRIM_INT_FLAG( hirc1Flag );
    }
}


