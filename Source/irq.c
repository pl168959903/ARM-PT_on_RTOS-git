#include "Project.h"

void GPABC_IRQHandler( void ) {
    // Buttion
    if ( GPIO_GET_INT_FLAG( PA, BIT2 ) ) {
        GPIO_CLR_INT_FLAG( PA, BIT2 );
    }
    // ext int
    if ( GPIO_GET_INT_FLAG( PA, BIT14 ) ) {
        GPIO_CLR_INT_FLAG( PA, BIT14 );
    }
    // nRF IRQ
    if ( GPIO_GET_INT_FLAG( PB, BIT3 ) ) {
        NRF_Nop(g_stNrf0);
        if ( g_stNrf0->statusReg & NRF_REG_STATUS_RX_DR_MSK ) {
            printf("RX_DR\n");
        }
        if ( g_stNrf0->statusReg & NRF_REG_STATUS_TX_DS_MSK ) {
						printf("TX_DS\n");
        }
        if ( g_stNrf0->statusReg & NRF_REG_STATUS_MAX_RT_MSK ) {
						printf("MAX_RT\n");
        }
				NRF_WriteRegByte( g_stNrf0, NRF_REG_STATUS, g_stNrf0->statusReg );
        GPIO_CLR_INT_FLAG( PB, BIT3 );
    }
}

void TMR0_IRQHandler( void ) {

    TIMER_ClearIntFlag( TIMER0 );
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
