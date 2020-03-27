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

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint8_t ( *SpiWriteFunction )( uint8_t );
typedef uint8_t ( *SpiReadFunction )( void );
typedef void ( *SpiDummyFunction )( void );
typedef void ( *SpiDeselectFunction )( void );
typedef void ( *SpiSelectFunction )( void );

#define W25QXX_TEMP_SECTOR_ADDR ( w25qxx->CapacityInKiloByte * 1024 - w25qxx->SectorSize )
#define W25QXX_MANUFACTURER_ID 0xEF

#define W25QXX_BLOCK_SIZE 65536
#define W25QXX_BLOCK_ALIGNMENT_MSK 0xFFFF
#define W25QXX_HALF_BLOCK_SIZE 32768
#define W25QXX_HALF_BLOCK_ALIGNMENT_MSK 0x7FFF
#define W25QXX_SECTORS_SIZE 4096
#define W25QXX_SECTORS_ALIGNMENT_MSK 0xFFF
#define W25QXX_PAGE_SIZE 256
#define W25QXX_PAGE_ALIGNMENT_MSK 0xFF

//---------------------------------------
#define W25QXX_SECURITY_REGISTER_1_ADDRESS 0x00001000U
#define W25QXX_SECURITY_REGISTER_2_ADDRESS 0x00002000U
#define W25QXX_SECURITY_REGISTER_3_ADDRESS 0x00003000U
//---------------------------------------

#define W25Q_INS_WRITE_ENABLE 0x06
#define W25Q_INS_VOLATILE_SR_WRITE_ENABLE 0x50
#define W25Q_INS_WRITE_DISABLE 0x04
#define W25Q_INS_READ_STATUS_REGISTER_1 0x05
#define W25Q_INS_WRITE_STATUS_REGISTER_1 0x01
#define W25Q_INS_READ_STATUS_REGISTER_2 0x35
#define W25Q_INS_WRITE_STATUS_REGISTER_2 0x31
#define W25Q_INS_READ_STATUS_REGISTER_3 0x15
#define W25Q_INS_WRITE_STATUS_REGISTER_3 0x11
#define W25Q_INS_ERASE_PROGRAM_SUSPEND 0x75
#define W25Q_INS_ERASE_PROGRAM_RESUME 0x7A
#define W25Q_INS_POWER_DOWN 0xB9
#define W25Q_INS_RELEASE_POWER_DOWN_ID 0xAB
#define W25Q_INS_MANUFACTURER_DEVICE_ID 0x90
#define W25Q_INS_JEDEC_ID 0x9F
#define W25Q_INS_GLOBAL_BLOCK_LOCK 0x7E
#define W25Q_INS_GLOBAL_BLOCK_UNLOCK 0x98
#define W25Q_INS_ENTER_QPI_MODE 0x38
#define W25Q_INS_ENABLE_RESET 0x66
#define W25Q_INS_RESET_DEVICE 0x99
#define W25Q_INS_READ_UNIQUE_ID 0x4B
#define W25Q_INS_PAGE_PROGRAM 0x02
#define W25Q_INS_QUAD_PAGE_PROGRAM 0x32
#define W25Q_INS_SECTOR_ERASE_4KB 0x20
#define W25Q_INS_BLOCK_ERASE_32KB 0x52
#define W25Q_INS_BLOCK_ERASE_64KB 0xD8
#define W25Q_INS_READ_DATA 0x03
#define W25Q_INS_FAST_READ 0x0B
#define W25Q_INS_FAST_READ_DUAL_OUTPUT 0x3B
#define W25Q_INS_FAST_READ_QUAD_OUTPUT 0x6B
#define W25Q_INS_READ_SFDP_REGISTER 0x5A
#define W25Q_INS_ERASE_SECURITY_REGISTER 0x44
#define W25Q_INS_PROGRAM_SECURITY_REGISTER 0x42
#define W25Q_INS_READ_SECURITY_REGISTER 0x48
#define W25Q_INS_INDIVIDUAL_BLOCK_LOCK 0x36
#define W25Q_INS_INDIVIDUAL_BLOCK_UNLOCK 0x39
#define W25Q_INS_READ_BLOCK_LOCK 0x3D
#define W25Q_INS_FAST_READ_DUAL_I_O 0xBB
#define W25Q_INS_MFTR_DEVICE_ID_DUAL_I_O 0x92
#define W25Q_INS_SET_BURST_WITH_WRAP 0x77
#define W25Q_INS_FAST_READ_QUAD_I_O 0xEB
#define W25Q_INS_WORD_READ_QUAD_I_O 0xE7
#define W25Q_INS_OCTAL_WORD_READ_QUAD_I_O 0xE3
#define W25Q_INS_MFTR_DEVICE_ID_QUAD_I_O 0x94
#define W25Q_INS_CHIP_ERASE 0xC7
#define W25Q_INS_SET_READ_PARAMETERS 0xC0
#define W25Q_INS_EXIT_QPI_MODE 0xFF
#define W25Q_INS_BURST_READ_WITH_WRAP 0x0C

//---------------------------------------
#define W25Q_STATUS_REG_BUSY_POS ( 0 )
#define W25Q_STATUS_REG_BUSY_MSK ( 0x1UL << W25Q_STATUS_REG_BUSY_POS )
#define W25Q_STATUS_REG_WEL_POS ( 1 )
#define W25Q_STATUS_REG_WEL_MSK ( 0x1UL << W25Q_STATUS_REG_WEL_POS )
#define W25Q_STATUS_REG_BP_POS ( 2 )
#define W25Q_STATUS_REG_BP_MSK ( 0x7UL << W25Q_STATUS_REG_BP_POS )
#define W25Q_STATUS_REG_TB_POS ( 5 )
#define W25Q_STATUS_REG_TB_MSK ( 0x1UL << W25Q_STATUS_REG_TB_POS )
#define W25Q_STATUS_REG_SEC_POS ( 6 )
#define W25Q_STATUS_REG_SEC_MSK ( 0x1UL << W25Q_STATUS_REG_SEC_POS )
#define W25Q_STATUS_REG_SRP_POS ( 7 )
#define W25Q_STATUS_REG_SRP_MSK ( 0x3UL << W25Q_STATUS_REG_SRP_POS )
#define W25Q_STATUS_REG_QE_POS ( 9 )
#define W25Q_STATUS_REG_QE_MSK ( 0x1UL << W25Q_STATUS_REG_QE_POS )
#define W25Q_STATUS_REG_LB_POS ( 11 )
#define W25Q_STATUS_REG_LB_MSK ( 0x7UL << W25Q_STATUS_REG_LB_POS )
#define W25Q_STATUS_REG_CMP_POS ( 14 )
#define W25Q_STATUS_REG_CMP_MSK ( 0x1UL << W25Q_STATUS_REG_CMP_POS )
#define W25Q_STATUS_REG_SUS_POS ( 15 )
#define W25Q_STATUS_REG_SUS_MSK ( 0x1UL << W25Q_STATUS_REG_SUS_POS )
#define W25Q_STATUS_REG_WPS_POS ( 18 )
#define W25Q_STATUS_REG_WPS_MSK ( 0x1UL << W25Q_STATUS_REG_WPS_POS )
#define W25Q_STATUS_REG_DRV_POS ( 21 )
#define W25Q_STATUS_REG_DRV_MSK ( 0x3UL << W25Q_STATUS_REG_DRV_POS )
#define W25Q_STATUS_REG_RST_POS ( 23 )
#define W25Q_STATUS_REG_RST_MSK ( 0x3UL << W25Q_STATUS_REG_RST_POS )

//---------------------------------------

typedef struct {
    // JEDEC ID
    uint8_t jedecIdManufacturer;
    uint8_t jedecIdMemoryType;
    uint8_t jedecIdCapacity;

    // Uniq ID
    uint8_t UniqID[ 8 ];

    // Status Register
    uint8_t StatusRegister[ 3 ];

    SpiWriteFunction    Write;
    SpiReadFunction     Read;
    SpiDummyFunction    Dummy;
    SpiDeselectFunction Deselect;
    SpiSelectFunction   Select;
} W25QXX_T;

#ifdef __cplusplus
}
#endif

//---------------------------------------
W25QXX_T* W25QXX_New( SpiWriteFunction funcWrite, SpiReadFunction funcRead, SpiDummyFunction funcDummy, SpiSelectFunction funcSelect, SpiDeselectFunction funcDeselect );
bool W25QXX_StructInit( W25QXX_T* w25qxx );
void W25QXX_WriteEnable( W25QXX_T* w25qxx );
void W25QXX_VolatileSRWriteEnable( W25QXX_T* w25qxx );
void W25QXX_WriteDisable( W25QXX_T* w25qxx );
uint8_t W25QXX_ReadStatusRegister1( W25QXX_T* w25qxx );
uint8_t W25QXX_ReadStatusRegister2( W25QXX_T* w25qxx );
uint8_t W25QXX_ReadStatusRegister3( W25QXX_T* w25qxx );
void W25QXX_WriteStatusRegister1( W25QXX_T* w25qxx, uint8_t data );
void W25QXX_WriteStatusRegister2( W25QXX_T* w25qxx, uint8_t data );
void W25QXX_WriteStatusRegister3( W25QXX_T* w25qxx, uint8_t data );
void W25QXX_EraseFullChip( W25QXX_T* w25qxx );
void W25QXX_EraseSector( W25QXX_T* w25qxx, uint32_t Addr );
void W25QXX_EraseHalfBlock( W25QXX_T* w25qxx, uint32_t Addr );
void W25QXX_EraseBlock( W25QXX_T* w25qxx, uint32_t Addr );
void W25QXX_ReadByte( W25QXX_T* w25qxx, uint32_t Addr, uint8_t* Array, size_t size );
void W25QXX_FastReadByte( W25QXX_T* w25qxx, uint32_t Addr, uint8_t* Array, size_t size );
void W25QXX_ReadSfdp( W25QXX_T* w25qxx, uint8_t* Array );
void W25QXX_Suspend( W25QXX_T* w25qxx );
void W25QXX_Resume( W25QXX_T* w25qxx );
void W25QXX_PowerDown( W25QXX_T* w25qxx );
bool W25QXX_WakeUp( W25QXX_T* w25qxx );
void W25QXX_ReadJedecId( W25QXX_T* w25qxx );
void W25QXX_ReadUniqID( W25QXX_T* w25qxx );
void W25QXX_SorftWareReset( W25QXX_T* w25qxx );
void W25QXX_ProgramPage( W25QXX_T* w25qxx, uint32_t Addr, uint8_t* Array, size_t size );
void W25QXX_WpsGlobalBlockLock( W25QXX_T* w25qxx );
void W25QXX_WpsGlobalBlockUnLock( W25QXX_T* w25qxx );
void W25QXX_WpsIndividualLock( W25QXX_T* w25qxx, uint32_t Addr );
void W25QXX_WpsIndividualUnLock( W25QXX_T* w25qxx, uint32_t Addr );
bool W25QXX_WpsReadLock( W25QXX_T* w25qxx, uint32_t Addr );
void W25QXX_EraseSecurityRegisters( W25QXX_T* w25qxx, uint32_t Addr );
void W25QXX_WriteSecurityRegisters( W25QXX_T* w25qxx, uint32_t Addr, uint8_t* Array, size_t size );
void W25QXX_WaitBusy( W25QXX_T* w25qxx );
void W25QXX_WriteAlignmentSectors( W25QXX_T* w25qxx, uint32_t Addr, uint8_t* Array, size_t size );

#endif  // __W25QXX_H__
