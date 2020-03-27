
#include "W25QXX.h"

//#define W25QXX_DEBUG

W25QXX_T* W25QXX_New( SpiWriteFunction funcWrite, SpiReadFunction funcRead, SpiDummyFunction funcDummy, SpiSelectFunction funcSelect, SpiDeselectFunction funcDeselect ) {
    W25QXX_T* w25qxxClass;
    w25qxxClass = ( W25QXX_T* )W25QXX_MALLOC( sizeof( W25QXX_T ) );
    if ( w25qxxClass != NULL ) {
        w25qxxClass->Write    = funcWrite;
        w25qxxClass->Read     = funcRead;
        w25qxxClass->Dummy    = funcDummy;
        w25qxxClass->Select   = funcSelect;
        w25qxxClass->Deselect = funcDeselect;
    }
    return w25qxxClass;
}
bool W25QXX_StructInit( W25QXX_T* w25qxx ) {
    if ( w25qxx == NULL )
        return false;
    W25QXX_ReadJedecId( w25qxx );
    W25QXX_ReadUniqID( w25qxx );
    W25QXX_ReadStatusRegister1( w25qxx );
    W25QXX_ReadStatusRegister2( w25qxx );
    W25QXX_ReadStatusRegister3( w25qxx );
    if ( w25qxx->jedecIdManufacturer != W25QXX_MANUFACTURER_ID )
        return false;
    else
        return true;
}
void W25QXX_WriteEnable( W25QXX_T* w25qxx ) {
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_WRITE_ENABLE );
    w25qxx->Deselect();
}
void W25QXX_VolatileSRWriteEnable( W25QXX_T* w25qxx ) {
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_VOLATILE_SR_WRITE_ENABLE );
    w25qxx->Deselect();
}
void W25QXX_WriteDisable( W25QXX_T* w25qxx ) {
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_WRITE_DISABLE );
    w25qxx->Deselect();
}
uint8_t W25QXX_ReadStatusRegister1( W25QXX_T* w25qxx ) {
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_READ_STATUS_REGISTER_1 );
    w25qxx->StatusRegister[ 0 ] = w25qxx->Read();
    w25qxx->Deselect();
    return w25qxx->StatusRegister[ 0 ];
}
uint8_t W25QXX_ReadStatusRegister2( W25QXX_T* w25qxx ) {
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_READ_STATUS_REGISTER_2 );
    w25qxx->StatusRegister[ 1 ] = w25qxx->Read();
    w25qxx->Deselect();
    return w25qxx->StatusRegister[ 1 ];
}
uint8_t W25QXX_ReadStatusRegister3( W25QXX_T* w25qxx ) {
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_READ_STATUS_REGISTER_3 );
    w25qxx->StatusRegister[ 2 ] = w25qxx->Read();
    w25qxx->Deselect();
    return w25qxx->StatusRegister[ 2 ];
}
void W25QXX_WriteStatusRegister1( W25QXX_T* w25qxx, uint8_t data ) {
    W25QXX_WaitBusy( w25qxx );
    W25QXX_VolatileSRWriteEnable( w25qxx );
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_WRITE_STATUS_REGISTER_1 );
    w25qxx->Write( data );
    w25qxx->Deselect();
    w25qxx->StatusRegister[ 0 ] = data;
}
void W25QXX_WriteStatusRegister2( W25QXX_T* w25qxx, uint8_t data ) {
    W25QXX_WaitBusy( w25qxx );
    W25QXX_VolatileSRWriteEnable( w25qxx );
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_WRITE_STATUS_REGISTER_2 );
    w25qxx->Write( data );
    w25qxx->Deselect();
    w25qxx->StatusRegister[ 1 ] = data;
}
void W25QXX_WriteStatusRegister3( W25QXX_T* w25qxx, uint8_t data ) {
    W25QXX_WaitBusy( w25qxx );
    W25QXX_VolatileSRWriteEnable( w25qxx );
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_WRITE_STATUS_REGISTER_3 );
    w25qxx->Write( data );
    w25qxx->Deselect();
    w25qxx->StatusRegister[ 2 ] = data;
}
void W25QXX_EraseFullChip( W25QXX_T* w25qxx ) {
    W25QXX_WaitBusy( w25qxx );
    W25QXX_WriteEnable( w25qxx );
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_CHIP_ERASE );
    w25qxx->Deselect();
}
void W25QXX_EraseSector( W25QXX_T* w25qxx, uint32_t Addr ) {
    W25QXX_WaitBusy( w25qxx );
    W25QXX_WriteEnable( w25qxx );
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_SECTOR_ERASE_4KB );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 2 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 1 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 0 ] );
    w25qxx->Deselect();
}
void W25QXX_EraseHalfBlock( W25QXX_T* w25qxx, uint32_t Addr ) {
    W25QXX_WaitBusy( w25qxx );
    W25QXX_WriteEnable( w25qxx );
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_BLOCK_ERASE_32KB );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 2 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 1 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 0 ] );
    w25qxx->Deselect();
}
void W25QXX_EraseBlock( W25QXX_T* w25qxx, uint32_t Addr ) {
    W25QXX_WaitBusy( w25qxx );
    W25QXX_WriteEnable( w25qxx );
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_BLOCK_ERASE_64KB );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 2 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 1 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 0 ] );
    w25qxx->Deselect();
}
void W25QXX_ReadByte( W25QXX_T* w25qxx, uint32_t Addr, uint8_t* Array, size_t size ) {
    size_t i;
    W25QXX_WaitBusy( w25qxx );
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_READ_DATA );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 2 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 1 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 0 ] );
    for ( i = 0; i < size; i++ ) {
        *( uint8_t* )( ( size_t )Array + i ) = w25qxx->Read();
    }
    w25qxx->Deselect();
}
void W25QXX_FastReadByte( W25QXX_T* w25qxx, uint32_t Addr, uint8_t* Array, size_t size ) {
    size_t i;
    W25QXX_WaitBusy( w25qxx );
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_FAST_READ );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 2 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 1 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 0 ] );
    w25qxx->Dummy();
    for ( i = 0; i < size; i++ ) {
        *( uint8_t* )( ( size_t )Array + i ) = w25qxx->Read();
    }
    w25qxx->Deselect();
}
void W25QXX_ReadSfdp( W25QXX_T* w25qxx, uint8_t* Array ) {
    size_t i;
    W25QXX_WaitBusy( w25qxx );
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_READ_SFDP_REGISTER );
    w25qxx->Write( 0x00 );
    w25qxx->Write( 0x00 );
    w25qxx->Write( 0x00 );
    w25qxx->Dummy();
    for ( i = 0; i < 256; i++ ) {
        *( Array + i ) = w25qxx->Read();
    }
    w25qxx->Deselect();
}
void W25QXX_Suspend( W25QXX_T* w25qxx ) {
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_ERASE_PROGRAM_SUSPEND );
    w25qxx->Deselect();
}
void W25QXX_Resume( W25QXX_T* w25qxx ) {
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_ERASE_PROGRAM_RESUME );
    w25qxx->Deselect();
}
void W25QXX_PowerDown( W25QXX_T* w25qxx ) {
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_POWER_DOWN );
    w25qxx->Deselect();
}
bool W25QXX_WakeUp( W25QXX_T* w25qxx ) {
    uint8_t ID = 0;
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_RELEASE_POWER_DOWN_ID );
    w25qxx->Dummy();
    w25qxx->Dummy();
    w25qxx->Dummy();
    ID = w25qxx->Read();
    if ( ID == w25qxx->jedecIdCapacity )
        return true;
    else
        return false;
}
void W25QXX_ReadJedecId( W25QXX_T* w25qxx ) {
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_JEDEC_ID );
    w25qxx->jedecIdManufacturer = w25qxx->Read();
    w25qxx->jedecIdMemoryType   = w25qxx->Read();
    w25qxx->jedecIdCapacity     = w25qxx->Read();
    w25qxx->Deselect();
}
void W25QXX_ReadUniqID( W25QXX_T* w25qxx ) {
    uint8_t i;
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_READ_UNIQUE_ID );
    for ( i = 0; i < 4; i++ )
        w25qxx->Dummy();
    for ( i = 0; i < 8; i++ )
        w25qxx->UniqID[ i ] = w25qxx->Read();
    w25qxx->Deselect();
}
void W25QXX_SorftWareReset( W25QXX_T* w25qxx ) {
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_ENABLE_RESET );
    w25qxx->Deselect();

    w25qxx->Select();
    w25qxx->Write( W25Q_INS_RESET_DEVICE );
    w25qxx->Deselect();
}
void W25QXX_ProgramPage( W25QXX_T* w25qxx, uint32_t Addr, uint8_t* Array, size_t size ) {
    size_t i;
    W25QXX_WaitBusy( w25qxx );
    W25QXX_WriteEnable( w25qxx );
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_PAGE_PROGRAM );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 2 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 1 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 0 ] );
    for ( i = 0; i < size; i++ ) {
        w25qxx->Write( *( uint8_t* )( ( size_t )Array + i ) );
    }
    w25qxx->Deselect();
}
//---------------------------------------
// Lock Memrary Function
void W25QXX_WpsGlobalBlockLock( W25QXX_T* w25qxx ) {
    W25QXX_WriteEnable( w25qxx );
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_GLOBAL_BLOCK_LOCK );
    w25qxx->Deselect();
}
void W25QXX_WpsGlobalBlockUnLock( W25QXX_T* w25qxx ) {
    W25QXX_WriteEnable( w25qxx );
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_GLOBAL_BLOCK_UNLOCK );
    w25qxx->Deselect();
}
void W25QXX_WpsIndividualLock( W25QXX_T* w25qxx, uint32_t Addr ) {
    W25QXX_WriteEnable( w25qxx );
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_INDIVIDUAL_BLOCK_LOCK );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 2 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 1 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 0 ] );
    w25qxx->Deselect();
}
void W25QXX_WpsIndividualUnLock( W25QXX_T* w25qxx, uint32_t Addr ) {
    W25QXX_WriteEnable( w25qxx );
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_INDIVIDUAL_BLOCK_UNLOCK );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 2 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 1 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 0 ] );
    w25qxx->Deselect();
}
bool W25QXX_WpsReadLock( W25QXX_T* w25qxx, uint32_t Addr ) {
    uint8_t temp;
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_READ_BLOCK_LOCK );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 2 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 1 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 0 ] );
    temp = w25qxx->Read();
    w25qxx->Deselect();
    if ( (temp & 0x01) != 0 )
        return true;
    else
        return false;
}

//---------------------------------------
// Security Register
void W25QXX_EraseSecurityRegisters( W25QXX_T* w25qxx, uint32_t Addr ) {
    W25QXX_WaitBusy( w25qxx );
    W25QXX_WriteEnable( w25qxx );
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_ERASE_SECURITY_REGISTER );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 2 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 1 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 0 ] );
    w25qxx->Deselect();
}
void W25QXX_WriteSecurityRegisters( W25QXX_T* w25qxx, uint32_t Addr, uint8_t* Array, size_t size ) {
    size_t i;
    W25QXX_WaitBusy( w25qxx );
    W25QXX_WriteEnable( w25qxx );
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_PROGRAM_SECURITY_REGISTER );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 2 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 1 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 0 ] );
    for ( i = 0; i < size; i++ ) {
        w25qxx->Write( *( uint8_t* )( ( size_t )Array + i ) );
    }
    w25qxx->Deselect();
}
void W25QXX_ReadSecurityRegisters( W25QXX_T* w25qxx, uint32_t Addr, uint8_t* Array, size_t size ) {
    size_t i;
    W25QXX_WaitBusy( w25qxx );
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_READ_SECURITY_REGISTER );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 2 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 1 ] );
    w25qxx->Write( ( ( uint8_t* )&Addr )[ 0 ] );
    w25qxx->Dummy();
    for ( i = 0; i < size; i++ ) {
        *( uint8_t* )( ( size_t )Array + i ) = w25qxx->Read();
    }
    w25qxx->Deselect();
}

//---------------------------------------
// Status Register
void W25QXX_WaitBusy( W25QXX_T* w25qxx ) {
    w25qxx->Select();
    w25qxx->Write( W25Q_INS_READ_STATUS_REGISTER_1 );
    do {
        w25qxx->StatusRegister[ 0 ] = w25qxx->Read();
    } while ( ( *( uint32_t* )( w25qxx->StatusRegister ) & W25Q_STATUS_REG_BUSY_MSK ) );
    w25qxx->Deselect();
}
//---------------------------------------
// Application
void W25QXX_WriteAlignmentSectors( W25QXX_T* w25qxx, uint32_t Addr, uint8_t* Array, size_t size ) {
    Addr = Addr & ~( ( uint32_t )W25QXX_SECTORS_ALIGNMENT_MSK );
}
