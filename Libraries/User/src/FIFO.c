/*
    __             _____   __________
   / /_  __  __   /__  /  / ____/ __ \
  / __ \/ / / /     / /  / / __/ /_/ /
 / /_/ / /_/ /     / /__/ /_/ / _, _/
/_.___/\__, /     /____/\____/_/ |_|
      /____/
*/

#include "FIFO.h"

volatile size_t FIFO_CntTime = 0;

/*--------------------------------------------------------------------------------------------------------------------*/
// Buffer work
/**
 * @brief  建立新的FIFO物件
 * @note
 * @param  bufSize: FIFO大小
 * @param  fifoBuf: 指定BUF的指標，若為NULL則自動分配足夠記憶體
 * @retval 物件指標
 */
FIFO_T* FIFO_New( const size_t bufSize, uint8_t* fifoBuf ) {

    FIFO_T* buf_st;
    if ( fifoBuf == NULL ) {
        buf_st = ( FIFO_T* )FIFO_MALLOC( sizeof( FIFO_T ) + sizeof( uint8_t ) * bufSize );
    }
    else {
        buf_st = ( FIFO_T* )FIFO_MALLOC( sizeof( FIFO_T ) );
    }
    if ( buf_st == NULL ) {
        printf( "Error Alloc memery!!\n" );
        return NULL;
    }
    buf_st->size    = 0u;
    buf_st->head    = 0u;
    buf_st->tail    = 0u;
    buf_st->effSize = bufSize;
    printf( "%d", buf_st->effSize );
    buf_st->buf = ( fifoBuf == NULL ) ? ( uint8_t* )( ( size_t )buf_st + sizeof( FIFO_T ) ) : fifoBuf;
    return buf_st;
}

/**
 * @brief  FIFO 字節輸入
 * @note
 * @param  buf_st: FIFO 結構體
 * @param  dataIn: 資料入口
 * @retval 成功 : 1 ; 失敗 : 0
 */
bool FIFO_ByteIn( FIFO_T* buf_st, uint8_t* dataIn ) {
    if ( buf_st->size < buf_st->effSize ) {
        buf_st->buf[ buf_st->head ] = *dataIn;
        buf_st->size++;
        buf_st->head = ( buf_st->head < buf_st->effSize - 1 ) ? buf_st->head + 1 : 0;
    }
    else {
        // FIFO overflow
        return false;
    }
    return true;
}

/**
 * @brief  FIFO 字節輸出
 * @note
 * @param  buf_st: FIFO 結構體
 * @param  dataOut: 資料出口
 * @retval 成功 : 1 ; 失敗 : 0
 */
bool FIFO_ByteOut( FIFO_T* buf_st, uint8_t* dataOut ) {
    if ( buf_st->size > 0 ) {
        *dataOut = buf_st->buf[ buf_st->tail ];
        buf_st->size--;
        buf_st->tail = ( buf_st->tail < buf_st->effSize - 1 ) ? buf_st->tail + 1 : 0;
    }
    else {
        // FIFO empty
        return false;
    }
    return true;
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
bool FIFO_IsEmpty( FIFO_T* buf_st ) {
    if ( buf_st->size == 0 )
        return true;
    else
        return false;
}

/**
 * @brief 讀取FIFO上某個偏移字節
 * @note
 * @param  buf_st: FIFO 結構體
 * @param  offset: 偏移量
 * @retval 資料回傳
 */
uint8_t FIFO_ReadData( FIFO_T* buf_st, size_t offset ) {
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
bool FIFO_WaitData( FIFO_T* buf_st, size_t dataSize, size_t timeOut ) {
    size_t timeStamp = FIFO_CntTime;
    size_t CntTime;
    while ( 1 ) {
        CntTime = timeStamp > FIFO_CntTime ? ( ( UINT32_MAX - timeStamp ) + FIFO_CntTime ) : FIFO_CntTime - timeStamp;
        if ( CntTime > timeOut )
            return false;
        if ( ( buf_st->size ) >= dataSize )
            return true;
    }
}

/**
 * @brief  在時間內比對FIFO內是否含有Command字串
 * @note
 * @param  buf_st: FIFO 結構體
 * @param  Command[]: 比對字串
 * @param  fifoShift:  比對FIFO的起始偏移
 * @param  fifoRange:  比對FIFO的起始範圍
 * @param  checkSize: 比對字串量
 * @param  timeOut: 時間限制
 * @retval 成功 : 1 ; 失敗 : 0
 */
bool FIFO_CmdCheck( FIFO_T* buf_st, uint8_t Command[], size_t fifoShift, size_t fifoRange, size_t checkSize, size_t timeOut ) {
    size_t timeStamp = FIFO_CntTime;
    size_t CntTime;
    size_t fifo_c  = 0;
    size_t cmd_c   = 0;
    size_t cmd_len = ( size_t )strlen( ( const char* )Command );

    //取最小的檢查大小
    cmd_len        = ( checkSize < cmd_len ) ? checkSize : cmd_len;
    //檢查FIFO範圍是否大於command的檢查大小，如果沒有，函式將永遠無法成立，直接return false
    if(fifoRange < cmd_len) return false;

    //檢查迴圈開始
    while ( 1 ) {

        //檢查時間戳記
        CntTime = timeStamp > FIFO_CntTime ? ( ( UINT32_MAX - timeStamp ) + FIFO_CntTime ) : FIFO_CntTime - timeStamp;
        if ( CntTime > timeOut )
            return false;

        //
        fifo_c = 0;
        //檢查BUF大小至少要大於偏移
        if ( buf_st->size <= fifoShift )
            continue;
        //檢查BUF
        if ( buf_st->size > fifoShift ) {
            while ( ( buf_st->size - fifo_c ) >= cmd_len ) {
                for ( cmd_c = 0; cmd_c < cmd_len; cmd_c++ ) {
                    printf( "Command[%d] = 0x%X ; FIFO_ReadData( buf_st, %d ) = 0x%X\n", cmd_c, Command[ cmd_c ], fifo_c, FIFO_ReadData( buf_st, fifo_c ) );
                    if ( ( Command[ cmd_c ] != FIFO_ReadData( buf_st, fifo_c + cmd_c ) ) )
                        break;
                    if ( ( cmd_c + 1 ) == ( cmd_len < checkSize ? cmd_len : checkSize ) )
                        return true;
                }
                fifo_c++;
            }
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
