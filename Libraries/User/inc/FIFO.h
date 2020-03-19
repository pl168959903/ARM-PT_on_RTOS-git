
#ifndef __FIFO_H__
#define __FIFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#define FIFO_USE_VMEMALLOC 1

#if FIFO_USE_VMEMALLOC
    #include "vMemAlloc.h"
    #define FIFO_MALLOC( size ) vMemAlloc( size )
    #define FIFO_FREE( ptr ) vMemFree( ptr )
#else
    #define FIFO_MALLOC( size ) malloc( size )
    #define FIFO_FREE( ptr ) free( ptr )
#endif  // FIFO_USE_VMEMALLOC

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// FIFO 結構體
typedef struct {
    size_t   size;     // 有效資料量
    size_t   head;     // 資料標頭
    size_t   tail;     // 資料標尾
    size_t   effSize;  // 有效空間量
    uint8_t* buf;      // FIFO空間指標
} FIFO_T;

FIFO_T* FIFO_New( size_t bufSize, uint8_t* fifoBuf );
bool    FIFO_ByteIn( FIFO_T* buf_st, uint8_t* dataIn );
bool    FIFO_ByteOut( FIFO_T* buf_st, uint8_t* dataOut );
void    FIFO_Rst( FIFO_T* buf_st );
bool FIFO_IsEmpty( FIFO_T* buf_st );
uint8_t FIFO_ReadData( FIFO_T* buf_st, size_t offset );
bool FIFO_WaitData( FIFO_T* buf_st, size_t dataSize, size_t timeOut );
bool FIFO_CmdCheck( FIFO_T* buf_st, uint8_t Command[], size_t shiftByte, size_t checkSize, size_t timeOut );
void    FIFO_CntTImeTrigger( void );

#ifdef __cplusplus
}
#endif

#endif  //__FIFO_H__
