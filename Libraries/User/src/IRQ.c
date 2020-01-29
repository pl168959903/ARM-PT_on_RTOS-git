#include "Nano103.h"
#include <USER_LIB.h>
#include <stdio.h>

uint8_t IRQ_timer0_flag_1 = 0;
uint8_t IRQ_timer0_flag_2 = 0;


void NVIC_INIT(void) {
    NVIC_EnableIRQ(GPABC_IRQn);
    NVIC_EnableIRQ(TMR0_IRQn);
    NVIC_EnableIRQ(UART1_IRQn);
}

 
void GPABC_IRQHandler(void) {
    //Buttion
    if(GPIO_GET_INT_FLAG(PA, BIT2)) {
        g_button_flag = 1;
        GPIO_CLR_INT_FLAG(PA, BIT2);
    }
    //ext int
    if(GPIO_GET_INT_FLAG(PA, BIT14)) {
        g_ext_int_flag = 1;
        GPIO_CLR_INT_FLAG(PA, BIT14);
    }
    //nRF IRQ
    if(GPIO_GET_INT_FLAG(PB, BIT3)) {
        g_nrf_irq_flag = 1;
        GPIO_CLR_INT_FLAG(PB, BIT3);
    }
}


void TMR0_IRQHandler(void) {
    if(IRQ_timer0_flag_1) FIFO_countTime();
    if(!(IRQ_timer0_flag_1 | IRQ_timer0_flag_2)) TIMER_Stop(TIMER0);
    TIMER_ClearIntFlag(TIMER0);
}

void UART1_IRQHandler(void) {
    uint32_t intSt = UART1->INTSTS;
    uint8_t dataTemp[1];
    if(intSt & UART_INTSTS_RXTOIF_Msk) {
        while(!UART_GET_RX_EMPTY(UART1)) {
            UART_Read(UART1, dataTemp, 1);
            FIFO_data_byte_in(&FIFO_uart1_buf, dataTemp);
        }
        UART_ClearIntFlag(UART1, UART_INTSTS_RXTOIF_Msk);
    } else if(intSt & UART_INTSTS_RDAIF_Msk) {
        while(!UART_GET_RX_EMPTY(UART1)) {
            UART_Read(UART1, dataTemp, 1);
            FIFO_data_byte_in(&FIFO_uart1_buf, dataTemp);
        }
        UART_ClearIntFlag(UART1, UART_INTSTS_RDAIF_Msk);
    }
}

