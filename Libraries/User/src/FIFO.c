#include <stdint.h>
#include "FIFO.h"

volatile uint32_t* FIFO_CntTime;

/*--------------------------------------------------------------------------------------------------------------------*/
// Buffer work
void FIFO_data_byte_in( DATA_FIFO* buf_st, uint8_t* data ) {
    buf_st->buf[ buf_st->head ] = *data;
    if ( buf_st->size < FIFO_BUF_SIZE ) {
        buf_st->size++;
        buf_st->head = ( buf_st->head < FIFO_BUF_SIZE - 1 ) ? buf_st->head + 1 : 0;
    }
    else {
        buf_st->head = ( buf_st->head < FIFO_BUF_SIZE - 1 ) ? buf_st->head + 1 : 0;
        buf_st->tail = ( buf_st->tail < FIFO_BUF_SIZE - 1 ) ? buf_st->tail + 1 : 0;
    }
}

void FIFO_data_Byte_out( DATA_FIFO* buf_st, uint8_t* data ) {
    if ( buf_st->size > 0 ) {
        *data = buf_st->buf[ buf_st->tail ];
        buf_st->size -= ( buf_st->size < FIFO_BUF_SIZE ) ? 1 : 0;
        buf_st->tail = ( buf_st->tail < FIFO_BUF_SIZE - 1 ) ? buf_st->tail + 1 : 0;
    }
}

void FIFO_reset( DATA_FIFO* buf_st ) {
    buf_st->head = 0;
    buf_st->size = 0;
    buf_st->tail = 0;
}

uint8_t FIFO_IsEmpty( DATA_FIFO* buf_st ) {
    if ( buf_st->size == 0 )
        return 1;
    else
        return 0;
}

uint8_t FIFO_read_byte( DATA_FIFO* buf_st, int32_t cursor ) {
    return buf_st->buf[ ( cursor + buf_st->tail ) % FIFO_BUF_SIZE ];
}

/*--------------------------------------------------------------------------------------------------------------------*/
// Buffer policy
uint8_t FIFO_waitData( DATA_FIFO* buf_st, uint32_t dataSize, uint32_t TimeOut ) {
    uint32_t timeStamp = FIFO_CntTime;
    uint32_t CntTime;
    while ( 1 ) {
        CntTime = timeStamp > FIFO_CntTime ? ( ( UINT32_MAX - timeStamp ) + FIFO_CntTime ) : FIFO_CntTime - timeStamp;
        if ( CntTime > TimeOut ) return 0;
        if ( ( buf_st->size ) >= dataSize ) return 1;
    }
}

uint8_t FIFO_cmdCheck( DATA_FIFO* buf_st, uint8_t Command[], uint32_t sum, uint32_t TimeOut_ms ) {
    uint32_t timeStamp = FIFO_CntTime;
    uint32_t CntTime;
    uint32_t fifo_c  = 0;
    uint32_t cmd_c   = 0;
    uint32_t cmd_len = strlen( ( const char* )Command );

    while ( 1 ) {
        CntTime = timeStamp > FIFO_CntTime ? ( ( UINT32_MAX - timeStamp ) + FIFO_CntTime ) : FIFO_CntTime - timeStamp;
        if ( CntTime > TimeOut_ms ) return 0;

        fifo_c = 0;
        while ( ( buf_st->size - fifo_c ) >= cmd_len ) {
            for ( cmd_c = 0; cmd_c < cmd_len; cmd_c++ ) {
                if ( ( Command[ cmd_c ] != FIFO_read_byte( buf_st, fifo_c ) ) ) break;
                if ( ( cmd_c + 1 ) == ( cmd_len < sum ? cmd_len : sum ) ) return 1;
            }
            fifo_c++;
        }
    }
}
