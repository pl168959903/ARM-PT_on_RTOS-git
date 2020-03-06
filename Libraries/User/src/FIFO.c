/*
    __             _____   __________
   / /_  __  __   /__  /  / ____/ __ \
  / __ \/ / / /     / /  / / __/ /_/ /
 / /_/ / /_/ /     / /__/ /_/ / _, _/
/_.___/\__, /     /____/\____/_/ |_|
      /____/
*/

#include "FIFO.h"
#include <String.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
volatile uint32_t FIFO_CntTime = 0;

/*--------------------------------------------------------------------------------------------------------------------*/
// Buffer work

/**
 * @brief  建立新的FIFO物件
 * @note
 * @param  bufSize: FIFO大小
 * @retval 物件指標
 */
FIFO_T* FIFO_New( uint32_t bufSize ) {

    FIFO_T* buf_st = ( FIFO_T* )malloc( sizeof( FIFO_T ) );
    if ( buf_st == NULL ) printf( "Error malloc memery!!\nError pointer : %s", "buf_st" );
    buf_st->buf = ( uint8_t* )malloc( sizeof( uint8_t ) * bufSize );
    if ( buf_st->buf == NULL ) printf( "Error malloc memery!!\nError pointer : %s", "buf_st->buf" );
    buf_st->effSize = bufSize;
    buf_st->head    = 0;
    buf_st->tail    = 0;
    buf_st->size    = 0;
    return buf_st;
}

/**
 * @brief  FIFO 字節輸入
 * @note
 * @param  buf_st: FIFO 結構體
 * @param  dataIn: 資料入口
 * @retval None
 */
void FIFO_ByteIn( FIFO_T* buf_st, uint8_t* dataIn ) {
    if ( buf_st->size < buf_st->effSize ) {
        buf_st->buf[ buf_st->head ] = *dataIn;
        buf_st->size++;
        buf_st->head = ( buf_st->head < buf_st->effSize - 1 ) ? buf_st->head + 1 : 0;
    }
    else {
        // FIFO overflow
    }
}

/**
 * @brief  FIFO 字節輸出
 * @note
 * @param  buf_st: FIFO 結構體
 * @param  dataOut: 資料出口
 * @retval None
 */
void FIFO_ByteOut( FIFO_T* buf_st, uint8_t* dataOut ) {
    if ( buf_st->size > 0 ) {
        *dataOut = buf_st->buf[ buf_st->tail ];
        buf_st->size--;
        buf_st->tail = ( buf_st->tail < buf_st->effSize - 1 ) ? buf_st->tail + 1 : 0;
    }
    else {
        // FIFO empty
    }
}

/**
 * @brief  FIFO 重置
 * @note
 * @param  buf_st: FIFO 結構體
 * @retval None
 */
void FIFO_Rst( FIFO_T* buf_st ) {
    buf_st->head = 0;
    buf_st->size = 0;
    buf_st->tail = 0;
}

/**
 * @brief  判斷該FIFO是否為空
 * @note
 * @param  buf_st: FIFO 結構體
 * @retval 為空 : 1 ; 不為空 : 0
 */
uint8_t FIFO_IsEmpty( FIFO_T* buf_st ) {
    if ( buf_st->size == 0 )
        return 1;
    else
        return 0;
}

/**
 * @brief 讀取FIFO上某個偏移字節
 * @note
 * @param  buf_st: FIFO 結構體
 * @param  offset: 偏移量
 * @retval 資料回傳
 */
uint8_t FIFO_ReadData( FIFO_T* buf_st, int32_t offset ) {
    return buf_st->buf[ ( offset + buf_st->tail ) % buf_st->effSize ];
}

/*--------------------------------------------------------------------------------------------------------------------*/
// Buffer policy

/**
 * @brief  在時間內等待dataSize數量的資料
 * @note
 * @param  buf_st: FIFO 結構體
 * @param  dataSize: 等待資料量
 * @param  TimeOut: 時間限制
 * @retval 成功 : 1 ; 失敗 : 0
 */
uint8_t FIFO_WaitData( FIFO_T* buf_st, uint32_t dataSize, uint32_t timeOut ) {
    uint32_t timeStamp = FIFO_CntTime;
    uint32_t CntTime;
    while ( 1 ) {
        CntTime = timeStamp > FIFO_CntTime ? ( ( UINT32_MAX - timeStamp ) + FIFO_CntTime ) : FIFO_CntTime - timeStamp;
        if ( CntTime > timeOut ) return 0;
        if ( ( buf_st->size ) >= dataSize ) return 1;
    }
}

/**
 * @brief  在時間內比對FIFO內是否含有Command字串
 * @note
 * @param  buf_st: FIFO 結構體
 * @param  Command[]: 比對字串
 * @param  checkSzie: 比對字串量
 * @param  timeOut: 時間限制
 * @retval 成功 : 1 ; 失敗 : 0
 */
uint8_t FIFO_CmdCheck( FIFO_T* buf_st, uint8_t Command[], uint32_t checkSzie, uint32_t timeOut ) {
    uint32_t timeStamp = FIFO_CntTime;
    uint32_t CntTime;
    uint32_t fifo_c  = 0;
    uint32_t cmd_c   = 0;
    uint32_t cmd_len = strlen( ( const char* )Command );

    while ( 1 ) {
        CntTime = timeStamp > FIFO_CntTime ? ( ( UINT32_MAX - timeStamp ) + FIFO_CntTime ) : FIFO_CntTime - timeStamp;
        if ( CntTime > timeOut ) return 0;

        fifo_c = 0;
        while ( ( buf_st->size - fifo_c ) >= cmd_len ) {
            for ( cmd_c = 0; cmd_c < cmd_len; cmd_c++ ) {
                if ( ( Command[ cmd_c ] != FIFO_ReadData( buf_st, fifo_c ) ) ) break;
                if ( ( cmd_c + 1 ) == ( cmd_len < checkSzie ? cmd_len : checkSzie ) ) return 1;
            }
            fifo_c++;
        }
    }
}

/**
 * @brief  FIFO 計數器觸發
 * @note
 * @retval None
 */
void FIFO_CntTImeTrigger( void ) {
    FIFO_CntTime = ( FIFO_CntTime >= UINT64_MAX ) ? 0 : ( FIFO_CntTime + 1 );
}
