#ifndef __W25QXX_H__
#define __W25QXX_H__

#ifdef __cplusplus
extern "C" {
#endif

#define W25QXX_USE_VMEMALLOC 1

#if W25QXX_USE_VMEMALLOC
#include "vMemAlloc.h"
#define W25QXX_MALLOC( size ) vMemAlloc( size )
#define W25QXX_FREE( ptr ) vMemFree( ptr )
#else
#define W25QXX_MALLOC( size ) malloc( size )
#define W25QXX_FREE( ptr ) free( ptr )
#endif  // W25QXX_USE_VMEMALLOC

#include <stdint.h>
#include <stdlib.h>

typedef uint8_t ( *SpiWriteFunction )( uint8_t );
typedef uint8_t ( *SpiReadFunction )( void );
typedef void ( *SpiDummyFunction )( void );
typedef void ( *SpiDeselectFunction )( void );
typedef void ( *SpiSelectFunction )( void );

#define W25QXX_TEMP_SECTOR_ADDR ( w25qxx.CapacityInKiloByte * 1024 - w25qxx.SectorSize )
//#define W25QXX_DEBUG
//---------------------------------------

typedef enum {
    W25Q10 = 1,
    W25Q20,
    W25Q40,
    W25Q80,
    W25Q16,
    W25Q32,
    W25Q64,
    W25Q128,
    W25Q256,
    W25Q512,

} W25QXX_ID_t;

typedef struct {
    uint32_t ID;
    uint8_t  UniqID[ 8 ];

    uint32_t PageSize;
    uint32_t SectorSize;
    uint32_t BlockSize;

    uint32_t BlockCount;

    uint32_t CapacityInKiloByte;

    uint8_t StatusRegister[3];

    SpiWriteFunction    Write;
    SpiReadFunction     Read;
    SpiDummyFunction    Dummy;
    SpiDeselectFunction Deselect;
    SpiSelectFunction   Select;
} W25QXX_T;

uint32_t w25qxx_ReadID( void );
void     w25qxx_ReadUniqID( void );
void     w25qxx_WriteEnable( void );
void     w25qxx_WriteDisable( void );
void     w25qxx_VolatileSRWriteEnable( void );
uint8_t  w25qxx_ReadStatusRegister( uint8_t );
void     w25qxx_WriteStatusRegister( uint8_t, uint8_t );
void     w25qxx_WaitForWriteEnd( void );
void     w25qxx_EraseChip( void );
void     w25qxx_EraseSector( uint32_t );
void     w25qxx_EraseHalfBlock( uint32_t );
void     w25qxx_EraseFullBlock( uint32_t );
uint8_t  w25qxx_IsEmpty( uint32_t, uint32_t );
void     w25qxx_WriteData( uint8_t[], uint32_t, uint32_t );
void     w25qxx_ReadData( uint8_t[], uint32_t, uint32_t );
void     w25qxx_EraseData( uint32_t, uint32_t );
void     w25qxx_CopeData( uint32_t, uint32_t, uint32_t );
uint8_t  w25qxx_INIT( void );

#ifdef __cplusplus
}
#endif

#endif  // __W25QXX_H__
