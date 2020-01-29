#include "Nano103.h"
#include <USER_LIB.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

uint32_t FIFO_cmd_countTime = 0;

DATA_FIFO   FIFO_uart0_buf = {{0}, 0, 0, 0};
DATA_FIFO   FIFO_uart1_buf = {{0}, 0, 0, 0};

void FIFO_count_start(void) {
    /*-----------------------------------------------interface----------------------------------------------------------*/
    IRQ_timer0_flag_1 = 1;
    if(!TIMER_IS_ACTIVE(TIMER0)) TIMER_Start(TIMER0);
    /*------------------------------------------------------------------------------------------------------------------*/
}
void FIFO_count_stop(void) {
    /*-----------------------------------------------interface----------------------------------------------------------*/
    IRQ_timer0_flag_1 = 0;
    /*------------------------------------------------------------------------------------------------------------------*/
}

void FIFO_countTime(void) {
    FIFO_cmd_countTime++;
}
void FIFO_reset_countTime(void) {
    FIFO_cmd_countTime = 0;
}


void FIFO_data_byte_in(DATA_FIFO *buf_st, uint8_t *data) {
    buf_st->buf[buf_st->head] = *data;
		if(buf_st->size < FIFO_BUF_SIZE)
		{
			buf_st->size++;
			buf_st->head = (buf_st->head < FIFO_BUF_SIZE - 1) ? buf_st->head + 1 : 0;
		}
		else
		{
			buf_st->head = (buf_st->head < FIFO_BUF_SIZE - 1) ? buf_st->head + 1 : 0;
			buf_st->tail = (buf_st->tail < FIFO_BUF_SIZE - 1) ? buf_st->tail + 1 : 0;
		}
}

void FIFO_data_Byte_out(DATA_FIFO *buf_st, uint8_t *data) {
    if(buf_st->size > 0) {
        *data = buf_st->buf[buf_st->tail];
        buf_st->size -= (buf_st->size < FIFO_BUF_SIZE) ? 1 : 0;
        buf_st->tail = (buf_st->tail < FIFO_BUF_SIZE - 1) ? buf_st->tail + 1 : 0;
    }
}

void FIFO_reset(DATA_FIFO *buf_st) {
    buf_st->head = 0;
    buf_st->size = 0;
    buf_st->tail = 0;
}

uint8_t FIFO_IsEmpty(DATA_FIFO *buf_st) {
    if(buf_st->size == 0) return 1;
    else return 0;
}
uint8_t FIFO_read_byte(DATA_FIFO *buf_st, int32_t cursor) {
    return buf_st->buf[(cursor + buf_st->tail) % FIFO_BUF_SIZE];
}

uint8_t FIFO_waitData(DATA_FIFO *buf_st, uint32_t dataSize, uint32_t TimeOut) {
    FIFO_reset_countTime();
    FIFO_count_start();
    while(1) {
        if(FIFO_cmd_countTime > TimeOut) {
            FIFO_count_stop();
            return 0;
        }
        if((buf_st->size) >= dataSize) {
            FIFO_count_stop();
            return 1;
        }
    }
}

uint8_t FIFO_cmdCheck(DATA_FIFO *buf_st, uint8_t Command[], uint32_t sum, uint32_t TimeOut_ms) {
    uint32_t fifo_cursor = 0;
    uint32_t cmd_cursor = 0;
    uint32_t cmd_len = strlen((const char*)Command);
		uint32_t waitTime = TimeOut_ms * FIFO_timerFreq;
		if(TimeOut_ms % 1000) {
			waitTime /= 1000;
			waitTime++;
		} else waitTime /= 1000;
    
    FIFO_reset_countTime();
    FIFO_count_start();

    while(1) {
        if(FIFO_cmd_countTime > waitTime) {
            FIFO_count_stop();
            return 0;
        }
        if(buf_st->size > fifo_cursor) {
            if(Command[cmd_cursor] == FIFO_read_byte(buf_st, fifo_cursor)) {
                cmd_cursor++;
                if(cmd_cursor == (cmd_len < sum ? cmd_len : sum)) {
                    FIFO_count_stop();
                    return 1;
                }
            } else cmd_cursor = 0;
            fifo_cursor++;
        }
    }
}


