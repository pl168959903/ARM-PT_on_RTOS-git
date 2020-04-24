#include "Project.h"

void GPABC_IRQHandler( void ) {
    // Buttion
    if ( GPIO_GET_INT_FLAG( PA, BIT2 ) ) {
        GPIO_CLR_INT_FLAG( PA, BIT2 );
    }
    // ext int
    if ( GPIO_GET_INT_FLAG( PA, BIT14 ) ) {
        // printf( "INT\n" );
        g_u8GetPictureReadyFlag = 1;
        GPIO_CLR_INT_FLAG( PA, BIT14 );
    }
    // nRF IRQ
    if ( GPIO_GET_INT_FLAG( PB, BIT3 ) ) {
        NRF_Nop( g_stNrf0 );
        if ( g_stNrf0->statusReg & NRF_REG_STATUS_RX_DR_MSK ) {

            uint8_t  i;
            uint8_t  data[ 32 ];
            uint32_t len = NRF_ReadRxPayloadWide( g_stNrf0 );
            NRF_RxPayload( g_stNrf0, data, len );
            for ( i = 0; i < len; i++ ) {
                FIFO_ByteIn( g_stNrfRx, ( data + i ) );
            }
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

void TMR0_IRQHandler( void ) {

    TIMER_ClearIntFlag( TIMER0 );
}

void RTC_IRQHandler( void ) {
    if ( g_u32GetPictureTime > 0 ) {
        if ( g_u32GetPictureCount++ >= g_u32GetPictureTime ) {
            g_u8GetPictureReadyFlag = 1;
            g_u32GetPictureCount    = 0;
        }
    }
    g_u8RtcTickFlag = 1;

    RTC_CLEAR_TICK_INT_FLAG();
}

void UART0_IRQHandler( void ) {
    uint32_t intSt = UART0->INTSTS;
    uint8_t  dataTemp[ 1 ];
    if ( intSt & UART_INTSTS_RXTOIF_Msk ) {
        while ( !UART_GET_RX_EMPTY( UART0 ) ) {
            if ( dataTemp[ 0 ] == '$' ) {
                FIFO_Rst( g_stGpsRx );
            }
            UART_Read( UART0, dataTemp, 1 );
            FIFO_ByteIn( g_stGpsRx, dataTemp );
        }
        g_u8GpsDataReadyFlag = 1;
        UART_ClearIntFlag( UART0, UART_INTSTS_RXTOIF_Msk );
    }
    else if ( intSt & UART_INTSTS_RDAIF_Msk ) {
        while ( !UART_GET_RX_EMPTY( UART0 ) ) {
            if ( dataTemp[ 0 ] == '$' ) {
                FIFO_Rst( g_stGpsRx );
            }
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
        while ( !UART_GET_RX_EMPTY( UART1 ) ) {
            g_stOv528_s0->Uart->Read( dataTemp, 1 );
        }
        UART_ClearIntFlag( UART1, UART_INTSTS_RXTOIF_Msk );
    }
    else if ( intSt & UART_INTSTS_RDAIF_Msk ) {
        while ( !UART_GET_RX_EMPTY( UART1 ) ) {
            g_stOv528_s0->Uart->Read( dataTemp, 1 );
        }
        UART_ClearIntFlag( UART1, UART_INTSTS_RDAIF_Msk );
    }
}
