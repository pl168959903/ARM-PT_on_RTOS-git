#include "Nano103.h"
#include <USER_LIB.h>
#include <stdio.h>

void NVIC_INIT(void) {
    NVIC_EnableIRQ(GPABC_IRQn);
    NVIC_EnableIRQ(TMR0_IRQn);
    NVIC_EnableIRQ(UART1_IRQn);
}

 
void GPABC_IRQHandler(void) {
    //Buttion
    if(GPIO_GET_INT_FLAG(PA, BIT2)) {
        g_u8ButtonInterruptFlag = 1;
        GPIO_CLR_INT_FLAG(PA, BIT2);
    }
    //ext int
    if(GPIO_GET_INT_FLAG(PA, BIT14)) {
        g_u8ExtInterruptFlag = 1;
        GPIO_CLR_INT_FLAG(PA, BIT14);
    }
    //nRF IRQ
    if(GPIO_GET_INT_FLAG(PB, BIT3)) {
        g_u8NrfInterruptFlag = 1;
        GPIO_CLR_INT_FLAG(PB, BIT3);
    }
}


void TMR0_IRQHandler(void) {
    
    TIMER_ClearIntFlag(TIMER0);
}

void UART1_IRQHandler(void) {
    uint32_t intSt = UART1->INTSTS;
    uint8_t dataTemp[1];
    if(intSt & UART_INTSTS_RXTOIF_Msk) {
        while(!UART_GET_RX_EMPTY(UART1)) {
            UART_Read(UART1, dataTemp, 1);
            FIFO_ByteIn(&g_stUart1_buf, dataTemp);
        }
        UART_ClearIntFlag(UART1, UART_INTSTS_RXTOIF_Msk);
    } else if(intSt & UART_INTSTS_RDAIF_Msk) {
        while(!UART_GET_RX_EMPTY(UART1)) {
            UART_Read(UART1, dataTemp, 1);
            FIFO_ByteIn(&g_stUart1_buf, dataTemp);
        }
        UART_ClearIntFlag(UART1, UART_INTSTS_RDAIF_Msk);
    }
}

