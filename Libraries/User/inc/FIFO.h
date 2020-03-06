
#ifndef __FIFO_H__
#define __FIFO_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>	
	
// FIFO 結構體
typedef struct {
    uint8_t *buf;       // FIFO空間指標
    uint32_t size;      // 有效資料量
    uint32_t head;      // 資料標頭
    uint32_t tail;      // 資料標尾
    uint32_t effSize;    // 有效空間量
} FIFO_T;

typedef struct {
    uint8_t buf[512];       // FIFO空間指標
    uint32_t size;      // 有效資料量
    uint32_t head;      // 資料標頭
    uint32_t tail;      // 資料標尾
    uint32_t effSize;    // 有效空間量
} FIFO_T0;

FIFO_T* FIFO_New( uint32_t bufSize );
void FIFO_ByteIn( FIFO_T* buf_st, uint8_t* dataIn );
void FIFO_ByteOut( FIFO_T* buf_st, uint8_t* dataOut );
void FIFO_Rst( FIFO_T* buf_st );
uint8_t FIFO_IsEmpty( FIFO_T* buf_st );
uint8_t FIFO_ReadData( FIFO_T* buf_st, int32_t offset );
uint8_t FIFO_WaitData( FIFO_T* buf_st, uint32_t dataSize, uint32_t timeOut );
uint8_t FIFO_CmdCheck( FIFO_T* buf_st, uint8_t Command[], uint32_t checkSzie, uint32_t timeOut );
void FIFO_CntTImeTrigger( void );


#ifdef __cplusplus
}
#endif

#endif //__FIFO_H__
