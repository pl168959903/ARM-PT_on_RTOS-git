
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

void W25QXX_ReadID( W25QXX_T* w25qxx ) {
    w25qxx->Select();
    w25qxx->Write( 0x9f );
    *( uint8_t* )( &w25qxx->ID + 2 ) = w25qxx->Read();
    *( uint8_t* )( &w25qxx->ID + 1 ) = w25qxx->Read();
    *( uint8_t* )( &w25qxx->ID + 0 ) = w25qxx->Read();
    w25qxx->Deselect();
}

void W25QXX_ReadUniqID( W25QXX_T* w25qxx ) {
    uint8_t i;
    w25qxx->Select();
    w25qxx->Write( 0x4B );
    for ( i = 0; i < 4; i++ )
        W25QXX_dummy();
    for ( i = 0; i < 8; i++ )
        w25qxx->UniqID[ i ] = w25qxx->Read();
    w25qxx->Deselect();
}

void W25QXX_WriteEnable( W25QXX_T* w25qxx ) {
    w25qxx->Select();
    w25qxx->Write( 0x06 );
    w25qxx->Deselect();
}

void W25QXX_WriteDisable( W25QXX_T* w25qxx ) {
    w25qxx->Select();
    w25qxx->Write( 0x04 );
    w25qxx->Deselect();
}

void W25QXX_VolatileSRWriteEnable( W25QXX_T* w25qxx ) {
    w25qxx->Select();
    w25qxx->Write( 0x50 );
    w25qxx->Deselect();
}
uint8_t W25QXX_ReadStatusRegister( W25QXX_T* w25qxx, uint8_t reg ) {
    uint8_t temp;
    w25qxx->Select();
    if ( reg == 1 ) {
        w25qxx->Write( 0x05 );
        w25qxx->StatusRegister[ 0 ] = w25qxx->Read();
    }
    else if ( reg == 2 ) {
        w25qxx->Write( 0x35 );
        w25qxx->StatusRegister[ 1 ] = w25qxx->Read();
    }
    else if ( reg == 3 ) {
        w25qxx->Write( 0x15 );
        w25qxx->StatusRegister[ 2 ] = w25qxx->Read();
    }
    w25qxx->Deselect();
    return w25qxx->StatusRegister[ reg - 1 ];
}

void W25QXX_WriteStatusRegister( W25QXX_T* w25qxx, uint8_t reg, uint8_t data ) {
    w25qxx->Select();
    if ( reg == 1 ) {
        w25qxx->Write( 0x01 );
        w25qxx->StatusRegister[ 0 ] = data;
    }
    else if ( reg == 2 ) {
        w25qxx->Write( 0x31 );
        w25qxx->StatusRegister[ 1 ] = data;
    }
    else if ( reg == 3 ) {
        w25qxx->Write( 0x11 );
    }
    w25qxx->Write( data );
    w25qxx->Deselect();
    w25qxx->StatusRegister[ reg - 1 ] = data;
}

void W25QXX_WaitForWriteEnd( W25QXX_T* w25qxx ) {
    w25qxx->Select();
    w25qxx->Write( 0x05 );
    do {
        w25qxx->StatusRegister[0] = w25qxx->Read();
    } while ( ( w25qxx->StatusRegister[0] & 0x01 ) );
    w25qxx->Deselect();
}
void W25QXX_EraseChip( W25QXX_T* w25qxx ) {
    W25QXX_WriteEnable(w25qxx);
    w25qxx->Select();
    w25qxx->Write( 0xC7 );
    w25qxx->Deselect();
    W25QXX_WaitForWriteEnd(w25qxx);
}

void W25QXX_EraseSector( uint32_t Addr ) {
    W25QXX_WriteEnable(w25qxx);
    w25qxx->Select();
    w25qxx->Write( 0x20 );
    w25qxx->Write( ( Addr & 0x00FF0000 ) >> 16 );
    w25qxx->Write( ( Addr & 0x0000FF00 ) >> 8 );
    w25qxx->Write( ( Addr & 0x000000FF ) >> 0 );
    w25qxx->Deselect();
    W25QXX_WaitForWriteEnd(w25qxx);
}

void W25QXX_EraseHalfBlock( uint32_t Addr ) {
    W25QXX_WriteEnable(w25qxx);
    w25qxx->Select();
    w25qxx->Write( 0x52 );
    w25qxx->Write( ( Addr & 0x00FF0000 ) >> 16 );
    w25qxx->Write( ( Addr & 0x0000FF00 ) >> 8 );
    w25qxx->Write( ( Addr & 0x000000FF ) >> 0 );
    w25qxx->Deselect();
    W25QXX_WaitForWriteEnd(w25qxx);
}

void W25QXX_EraseFullBlock( uint32_t Addr ) {
    W25QXX_WriteEnable(w25qxx);
    w25qxx->Select();
    w25qxx->Write( 0xD8 );
    w25qxx->Write( ( Addr & 0x00FF0000 ) >> 16 );
    w25qxx->Write( ( Addr & 0x0000FF00 ) >> 8 );
    w25qxx->Write( ( Addr & 0x000000FF ) >> 0 );
    w25qxx->Deselect();
    W25QXX_WaitForWriteEnd(w25qxx);
}

uint8_t W25QXX_IsEmpty( uint32_t StartAddress, uint32_t Count ) {
    uint32_t i;
    uint8_t  temp;
    w25qxx->Select();
    w25qxx->Write( 0x0B );
    w25qxx->Write( ( StartAddress & 0x00FF0000 ) >> 16 );
    w25qxx->Write( ( StartAddress & 0x0000FF00 ) >> 8 );
    w25qxx->Write( ( StartAddress & 0x000000FF ) >> 0 );
    W25QXX_dummy();
    for ( i = StartAddress; i <= StartAddress + Count; i++ ) {
        temp = w25qxx->Read();
        if ( temp != 0xFF ) {
            w25qxx->Deselect();
            return 0;
        }
    }
    w25qxx->Deselect();
    return 1;
}

void w25qxx->WriteData( uint8_t dataBuf[], uint32_t writeCount, uint32_t startAddr ) {
    uint32_t i, wtCnt, bufSum = 0, stAddr = startAddr;
    if ( writeCount == 0 )
        return;
    do {
        wtCnt = w25qxx.PageSize - ( stAddr % w25qxx.PageSize );
        wtCnt = ( writeCount < wtCnt ) ? writeCount : wtCnt;
#ifdef W25QXX_DEBUG
        {
            printf( "Start Write \n" );
            printf( "Addr : 0x%08X\n", stAddr );
            printf( "wtCnt : %d\n", wtCnt );
            printf( "bufSum : %d\n", bufSum );
        }
#endif
        W25QXX_WriteEnable(w25qxx);
        w25qxx->Select();
        w25qxx->Write( 0x02 );
        w25qxx->Write( ( stAddr & 0x00FF0000 ) >> 16 );
        w25qxx->Write( ( stAddr & 0x0000FF00 ) >> 8 );
        w25qxx->Write( ( stAddr & 0x000000FF ) >> 0 );
        for ( i = bufSum; i < ( bufSum + wtCnt ); i++ )
            w25qxx->Write( dataBuf[ i ] );
        w25qxx->Deselect();
        W25QXX_WaitForWriteEnd(w25qxx);
        bufSum += wtCnt;
        stAddr += wtCnt;
        writeCount -= wtCnt;
    } while ( writeCount > 0 );
}
void w25qxx->ReadData( uint8_t dataBuf[], uint32_t readCount, uint32_t startAddr ) {
    uint32_t i;
    if ( readCount == 0 )
        return;
    w25qxx->Select();
    w25qxx->Write( 0x0B );
    w25qxx->Write( ( startAddr & 0x00FF0000 ) >> 16 );
    w25qxx->Write( ( startAddr & 0x0000FF00 ) >> 8 );
    w25qxx->Write( ( startAddr & 0x000000FF ) >> 0 );
    W25QXX_dummy();
    for ( i = 0; i < readCount; i++ ) {
        dataBuf[ i ] = w25qxx->Read();
    }
    w25qxx->Deselect();
}
void W25QXX_CopeData( uint32_t sourceAddr, uint32_t destinationAddr, uint32_t dataCount ) {
    uint8_t* dataTemp;
    uint32_t A_Addr = sourceAddr;
    uint32_t B_Addr = destinationAddr;
    uint32_t wtCnt;
    if ( dataCount == 0 )
        return;
    dataTemp = malloc( w25qxx.PageSize * sizeof( uint8_t ) );
    do {
        wtCnt = ( dataCount > w25qxx.PageSize ) ? w25qxx.PageSize : dataCount;
        wtCnt = ( ( w25qxx.PageSize - ( B_Addr % w25qxx.PageSize ) ) < wtCnt ) ? ( w25qxx.PageSize - ( B_Addr % w25qxx.PageSize ) ) : wtCnt;
        w25qxx->ReadData( dataTemp, wtCnt, A_Addr );
        w25qxx->WriteData( dataTemp, wtCnt, B_Addr );
        A_Addr += wtCnt;
        B_Addr += wtCnt;
        dataCount -= wtCnt;
    } while ( dataCount > 0 );
    free( dataTemp );
}
void W25QXX_EraseData( uint32_t startAddr, uint32_t Count ) {
    uint32_t erAddr = startAddr, F_Cnt, L_Cnt, E_Cnt, base;
    do {
        if ( !( erAddr % w25qxx.BlockSize ) && ( Count >= w25qxx.BlockSize ) ) {
            W25QXX_EraseFullBlock( erAddr );
#ifdef W25QXX_DEBUG
            {
                printf( "Start Erase Full block\n" );
                printf( "erAddr : 0x%08X   Count : %d\n", erAddr, Count );
            }
#endif
            Count -= w25qxx.BlockSize;
            erAddr += w25qxx.BlockSize;
        }
        else if ( !( erAddr % ( w25qxx.BlockSize / 2 ) ) && ( Count >= ( w25qxx.BlockSize / 2 ) ) ) {
            W25QXX_EraseHalfBlock( erAddr );
#ifdef W25QXX_DEBUG
            {
                printf( "Start Erase Half block\n" );
                printf( "erAddr : 0x%08X   Count : %d\n", erAddr, Count );
            }
#endif
            Count -= ( w25qxx.BlockSize / 2 );
            erAddr += ( w25qxx.BlockSize / 2 );
        }
        else if ( !( erAddr % w25qxx.SectorSize ) && ( Count >= w25qxx.SectorSize ) ) {
            W25QXX_EraseSector( erAddr );
#ifdef W25QXX_DEBUG
            {
                printf( "Start Erase Sector\n" );
                printf( "erAddr : 0x%08X   Count : %d\n", erAddr, Count );
            }
#endif
            Count -= w25qxx.SectorSize;
            erAddr += w25qxx.SectorSize;
        }
        else {
            F_Cnt = erAddr % w25qxx.SectorSize;
            E_Cnt = ( Count < ( w25qxx.SectorSize - F_Cnt ) ) ? Count : ( w25qxx.SectorSize - F_Cnt );
            L_Cnt = w25qxx.SectorSize - ( F_Cnt + E_Cnt );
            base  = erAddr - F_Cnt;
#ifdef W25QXX_DEBUG
            {
                printf( "Start Erase Half Sector\n" );
                printf( "erAddr : 0x%08X   Count : %d\n", erAddr, Count );
                printf( "F_Cnt : %d\n", F_Cnt );
                printf( "E_Cnt : %d\n", E_Cnt );
                printf( "L_Cnt : %d\n", L_Cnt );
                printf( "base : 0x%08X\n", base );
            }
#endif
            W25QXX_EraseSector( W25QXX_TEMP_SECTOR_ADDR );
            W25QXX_CopeData( base, W25QXX_TEMP_SECTOR_ADDR, F_Cnt );
            W25QXX_CopeData( base + F_Cnt + E_Cnt, W25QXX_TEMP_SECTOR_ADDR + F_Cnt + E_Cnt, L_Cnt );
            W25QXX_EraseSector( erAddr );
            W25QXX_CopeData( W25QXX_TEMP_SECTOR_ADDR, base, F_Cnt );
            W25QXX_CopeData( W25QXX_TEMP_SECTOR_ADDR + F_Cnt + E_Cnt, base + F_Cnt + E_Cnt, L_Cnt );
            erAddr += E_Cnt;
            Count -= E_Cnt;
        }
    } while ( Count > 0 );
}

uint8_t W25QXX_INIT( void ) {
    uint32_t ID = w25qxx->ReadID();
    switch ( ID & 0x0000FFFF ) {
    case 0x401A:  //  w25q512
        w25qxx.ID         = W25Q512;
        w25qxx.BlockCount = 1024;
#ifdef W25QXX_DEBUG
        printf( "w25qxx Chip: w25q512\r\n" );
#endif
        break;
    case 0x4019:  //  w25q256
        w25qxx.ID         = W25Q256;
        w25qxx.BlockCount = 512;
#ifdef W25QXX_DEBUG
        printf( "w25qxx Chip: w25q256\r\n" );
#endif
        break;
    case 0x4018:  //  w25q128
        w25qxx.ID         = W25Q128;
        w25qxx.BlockCount = 256;
#ifdef W25QXX_DEBUG
        printf( "w25qxx Chip: w25q128\r\n" );
#endif
        break;
    case 0x4017:  //  w25q64
        w25qxx.ID         = W25Q64;
        w25qxx.BlockCount = 128;
#ifdef W25QXX_DEBUG
        printf( "w25qxx Chip: w25q64\r\n" );
#endif
        break;
    case 0x4016:  //  w25q32
        w25qxx.ID         = W25Q32;
        w25qxx.BlockCount = 64;
#ifdef W25QXX_DEBUG
        printf( "w25qxx Chip: w25q32\r\n" );
#endif
        break;
    case 0x4015:  //  w25q16
        w25qxx.ID         = W25Q16;
        w25qxx.BlockCount = 32;
#ifdef W25QXX_DEBUG
        printf( "w25qxx Chip: w25q16\r\n" );
#endif
        break;
    case 0x4014:  //  w25q80
        w25qxx.ID         = W25Q80;
        w25qxx.BlockCount = 16;
#ifdef W25QXX_DEBUG
        printf( "w25qxx Chip: w25q80\r\n" );
#endif
        break;
    case 0x4013:  //  w25q40
        w25qxx.ID         = W25Q40;
        w25qxx.BlockCount = 8;
#ifdef W25QXX_DEBUG
        printf( "w25qxx Chip: w25q40\r\n" );
#endif
        break;
    case 0x4012:  //  w25q20
        w25qxx.ID         = W25Q20;
        w25qxx.BlockCount = 4;
#ifdef W25QXX_DEBUG
        printf( "w25qxx Chip: w25q20\r\n" );
#endif
        break;
    case 0x4011:  //  w25q10
        w25qxx.ID         = W25Q10;
        w25qxx.BlockCount = 2;
#ifdef W25QXX_DEBUG
        printf( "w25qxx Chip: w25q10\r\n" );
#endif
        break;
    default:
#ifdef W25QXX_DEBUG
        printf( "w25qxx Unknown ID\r\n" );
#endif
        return 0;
    }

    w25qxx.PageSize           = 0x00000100;
    w25qxx.SectorSize         = 0x00001000;
    w25qxx.BlockSize          = 0x00010000;
    w25qxx.CapacityInKiloByte = ( w25qxx.BlockSize * w25qxx.BlockCount ) / 1024;
    w25qxx->ReadUniqID();
    w25qxx->ReadStatusRegister( 1 );
    w25qxx->ReadStatusRegister( 2 );
    w25qxx->ReadStatusRegister( 3 );
#ifdef W25QXX_DEBUG
    printf( "ID : %X\n", ID );
    printf( "Capacity %d\n", w25qxx.CapacityInKiloByte );
#endif
    return 1;
}
