
#include "CJ_OV528.h"
#include "FIFO.h"
#include <stdint.h>

uint8_t OV528_ERR_CODE;

struct OV528 OV528;

void OV528_ErrHook( uint8_t err ) {
    switch ( err ) {
    case OV528_ERR_SNYC:
        break;
    case OV528_ERR_INIT:
        break;
    case OV528_ERR_GETPICTUE:
        break;
    case OV528_ERR_SNAPSHOUT:
        break;
    case OV528_ERR_SETBAUDRATE:
        break;
    case OV528_ERR_POWNDOWN:
        break;
    case OV528_ERR_SETPACKETSZIE:
        break;
    case OV528_ERR_GETPACKET:
        break;
    default:
        break;
    }

    if ( err - 1 ) printf( "Camena err!!	ERR CODE : %X\n", err );
    // while(1) {};
}

uint8_t OV528_SNYC( OV528_T* OV528 ) {
    uint8_t cmdCheck;

    // send data
    Array_assign( OV528->RS232_CMD, 6, 0xAA, OV528_CMD_ID_SNYC, 0x00, 0x00, 0x00, 0x00 );
    FIFO_reset( OV528->buf );
    OV528->writeData( OV528->RS232_CMD, 6 );
    OV528->writeData( OV528->RS232_CMD, 6 );

    // check ack
    Array_assign( OV528->RS232_CMD, 6, 0xAA, OV528_CMD_ID_ACK, 0x00, 0x00, 0x00, 0x00 );
    cmdCheck = FIFO_cmdCheck( OV528->buf, OV528->RS232_CMD, 6, 0 );
    if ( cmdCheck )
        goto Exit;
    else
        goto ERR;

ERR:
// Error Hook
#if OV528_USE_ERR_HOOK
    OV528_ErrHook( OV528_ERR_SNYC );
#endif  // OV528_USE_ERR_HOOK

    FIFO_reset( OV528->buf );
    OV528_DELAY_MS( 1 );
    return 0;

Exit:
    FIFO_reset( OV528->buf );
    OV528_DELAY_MS( 1 );
    return 1;
}

uint8_t OV528_INIT( OV528_T* OV528, uint8_t color, uint8_t PR, uint8_t JPEGR ) {
    uint8_t cmdCheck;

    // send data
    Array_assign( OV528->RS232_CMD, 6, 0xAA, OV528_CMD_ID_INIT, 0x00, color, PR, JPEGR );
    FIFO_reset( OV528->buf );
    OV528->writeData( OV528->RS232_CMD, 6 );

    // check ack
    Array_assign( OV528->RS232_CMD, 6, 0xAA, OV528_CMD_ID_ACK, OV528_CMD_ID_INIT, 0x00, 0x00, 0x00 );
    cmdCheck = FIFO_cmdCheck( OV528->buf, OV528->RS232_CMD, 6, 250 );
    if ( cmdCheck )
        goto Exit;
    else
        goto ERR;

ERR :
// Error Hook
#if OV528_USE_ERR_HOOK
{
    OV528_ErrHook( OV528_ERR_INIT );
}
#endif

    FIFO_reset( OV528->buf );
    OV528_DELAY_MS( 1 );
    return 0;

Exit:
    OV528->color             = color;
    OV528->previewResolution = PR;
    OV528->JPEGResolution    = JPEGR;

    FIFO_reset( OV528->buf );
    OV528_DELAY_MS( 1 );
    return 1;
}

uint8_t OV528_GetPictue( OV528_T* OV528, uint8_t imageType ) {
    uint8_t cmdCheck;

    // send data
    Array_assign( OV528->RS232_CMD, 6, 0xAA, OV528_CMD_ID_GET_PICTURE, imageType, 0x00, 0x00, 0x00 );
    FIFO_reset( OV528->buf );
    OV528->writeData( OV528->RS232_CMD, 6 );

    // check ack
    Array_assign( OV528->RS232_CMD, 6, 0xAA, OV528_CMD_ID_ACK, OV528_CMD_ID_GET_PICTURE, 0x00, 0x00, 0x00 );
    cmdCheck = FIFO_cmdCheck( OV528->buf, OV528->RS232_CMD, 6, 250 );
    if ( !cmdCheck ) goto ERR;

    // check image config
    Array_assign( OV528->RS232_CMD, 3, 0xAA, OV528_CMD_ID_DATA, OV528->imageType );
    cmdCheck = FIFO_cmdCheck( OV528->buf, OV528->RS232_CMD, 3, 1000 );
    if ( cmdCheck ) {
        if ( FIFO_waitData( OV528->buf, 12, 1 ) ) {
            goto Exit;
        }
    }
    goto ERR;

ERR :
#if OV528_USE_ERR_HOOK
{
    OV528_ErrHook( OV528_ERR_GETPICTUE );  // Error Hook
}
#endif

    FIFO_reset( OV528->buf );
    OV528_DELAY_MS( 1 );
    return 0;

Exit:
    OV528->imageType   = imageType;
    OV528->imageSize   = ( uint32_t )( FIFO_read_byte( OV528->buf, 9 ) << 0 ) | ( uint32_t )( FIFO_read_byte( OV528->buf, 10 ) << 8 ) | ( uint32_t )( FIFO_read_byte( OV528->buf, 11 ) << 16 );
    OV528->imagePacket = OV528->imageSize / ( OV528->packetSize - 6 );
    if ( OV528->imageSize % ( OV528->packetSize - 6 ) ) OV528->imagePacket++;

    FIFO_reset( OV528->buf );
    OV528_DELAY_MS( 1 );
    return 1;
}

uint8_t OV528_Snapshout( OV528_T* OV528, uint8_t Compressed, uint16_t SkipFrame ) {
    uint8_t cmdCheck;

    // send data
    Array_assign( OV528->RS232_CMD, 6, 0xAA, OV528_CMD_ID_SNAPSHOT, Compressed, ( uint8_t )( ( SkipFrame & 0x00FF ) >> 0 ), ( uint8_t )( ( SkipFrame & 0xFF00 ) >> 8 ), 0x00 );
    FIFO_reset( OV528->buf );
    OV528->writeData( OV528->RS232_CMD, 6 );

    // check ack
    Array_assign( OV528->RS232_CMD, 6, 0xAA, OV528_CMD_ID_ACK, OV528_CMD_ID_SNAPSHOT, 0x00, 0x00, 0x00 );
    cmdCheck = FIFO_cmdCheck( OV528->buf, OV528->RS232_CMD, 6, 250 );
    if ( cmdCheck )
        goto Exit;
    else
        goto ERR;

ERR :
// Error Hook
#if OV528_USE_ERR_HOOK
{
    OV528_ErrHook( OV528_ERR_SNAPSHOUT );
}
#endif

    FIFO_reset( OV528->buf );
    OV528_DELAY_MS( 1 );
    return 0;

Exit:
    OV528->IsCompressed = Compressed;
    OV528->skipFrame    = SkipFrame;

    FIFO_reset( OV528->buf );
    OV528_DELAY_MS( 1 );
    return 1;
}

uint8_t OV528_SetBaudRate( OV528_T* OV528, uint32_t BAUD ) {
    uint8_t  cmdCheck;
    uint32_t div1 = ( 14745600 / 4 / BAUD ) / 2;
    uint32_t div2 = 2;
    while ( div1 > 256 ) {
        div1 /= 2;
        div2 *= 2;
    }

    // send data
    Array_assign( OV528->RS232_CMD, 6, 0xAA, OV528_CMD_ID_SET_BAUD_RATE, ( uint8_t )( div1 - 1 ), ( uint8_t )( div2 - 1 ), 0x00, 0x00 );
    FIFO_reset( OV528->buf );
    OV528->writeData( OV528->RS232_CMD, 6 );

    // check ack
    Array_assign( OV528->RS232_CMD, 6, 0xAA, OV528_CMD_ID_ACK, OV528_CMD_ID_SET_BAUD_RATE, 0x00, 0x00, 0x00 );
    if ( FIFO_cmdCheck( OV528->buf, OV528->RS232_CMD, 6, 250 ) )
        goto Exit;
    else
        goto ERR;

ERR :
#if OV528_USE_ERR_HOOK
{
    OV528_ErrHook( OV528_ERR_SETBAUDRATE );  // Error Hook
}
#endif

    FIFO_reset( OV528->buf );
    OV528_DELAY_MS( 1 );
    return 0;

Exit:
    OV528->baudRate = 3686400 / div1 / div2;

    FIFO_reset( OV528->buf );
    OV528_DELAY_MS( 1 );
    return 1;
}

uint8_t OV528_PowerDown( OV528_T* OV528 ) {
    uint8_t cmdCheck;

    // send data
    Array_assign( OV528->RS232_CMD, 6, 0xAA, OV528_CMD_ID_POWER_DOWN, 0x00, 0x00, 0x00, 0x00 );
    FIFO_reset( OV528->buf );
    OV528->writeData( OV528->RS232_CMD, 6 );

    // check ack
    Array_assign( OV528->RS232_CMD, 6, 0xAA, OV528_CMD_ID_ACK, OV528_CMD_ID_POWER_DOWN, 0x00, 0x00, 0x00 );
    if ( FIFO_cmdCheck( OV528->buf, OV528->RS232_CMD, 6, 250 ) )
        goto Exit;
    else
        goto ERR;

ERR :
#if OV528_USE_ERR_HOOK
{
    OV528_ErrHook( OV528_ERR_POWNDOWN );
}
#endif

    FIFO_reset( OV528->buf );
    OV528_DELAY_MS( 1 );
    return 0;

Exit:
    FIFO_reset( OV528->buf );
    OV528_DELAY_MS( 1 );
    return 1;
}

uint8_t OV528_SetPacketSize( OV528_T* OV528, uint16_t size ) {
    uint8_t cmdCheck;

    // send data
    Array_assign( OV528->RS232_CMD, 6, 0xAA, OV528_CMD_ID_SET_PACKAGE_SIZE, 0x08, ( uint8_t )( size & 0x00FF ), ( uint8_t )( size >> 8 ), 0x00 );
    FIFO_reset( OV528->buf );
    OV528->writeData( OV528->RS232_CMD, 6 );

    // check ack
    Array_assign( OV528->RS232_CMD, 6, 0xAA, OV528_CMD_ID_ACK, OV528_CMD_ID_SET_PACKAGE_SIZE, 0x00, 0x00, 0x00 );
    cmdCheck = FIFO_cmdCheck( OV528->buf, OV528->RS232_CMD, 6, 250 );
    if ( cmdCheck )
        goto Exit;
    else
        goto ERR;

ERR :
#if OV528_USE_ERR_HOOK
{
    OV528_ErrHook( OV528_ERR_SETPACKETSZIE );
}
#endif

    FIFO_reset( OV528->buf );
    OV528_DELAY_MS( 1 );
    return 0;

Exit:
    OV528->packetSize = size;

    FIFO_reset( OV528->buf );
    OV528_DELAY_MS( 1 );
    return 1;
}

uint16_t OV528_GetPacket( OV528_T* OV528, uint16_t package_ID, uint8_t package[] ) {
    uint8_t  cmdCheck;
    uint8_t  ID[ 2 ];
    uint16_t i;
    uint16_t size;
    uint16_t checkSum = 0;
    ID[ 0 ]           = ( uint8_t )( package_ID & 0x00FF );
    ID[ 1 ]           = ( uint8_t )( package_ID >> 8 );

    // send data
    Array_assign( OV528->RS232_CMD, 6, 0xAA, OV528_CMD_ID_ACK, 0x00, 0x00, ID[ 0 ], ID[ 1 ] );
    FIFO_reset( OV528->buf );
    OV528->writeData( OV528->RS232_CMD, 6 );

    // check ack
    Array_assign( OV528->RS232_CMD, 3, 0xAA, ID[ 0 ], ID[ 1 ] );
    cmdCheck = FIFO_cmdCheck( OV528->buf, OV528->RS232_CMD, 3, 250 );
    if ( !cmdCheck ) goto ERR;

    cmdCheck = FIFO_waitData( OV528->buf, 4, 250 );
    if ( !cmdCheck ) goto ERR;
    
    if ( ( FIFO_read_byte( OV528->buf, 0 ) == ID[ 0 ] ) && ( FIFO_read_byte( OV528->buf, 1 ) == ID[ 1 ] ) ) {
        size = ( uint16_t )OV528->buf->buf[ 2 ] | ( uint16_t )( OV528->buf->buf[ 3 ] << 8 );
        if ( FIFO_waitData( OV528->buf, size + 6, 3000 ) ) {
            for ( i = 0; i < size + 4; i++ )
                checkSum += OV528->buf->buf[ i ];
            if ( ( ( uint8_t )( checkSum & 0x00ff ) == OV528->buf->buf[ size + 4 ] ) && ( ( uint8_t )( checkSum >> 8 ) == OV528->buf->buf[ size + 5 ] ) ) {
                for ( i = 0; i < size; i++ )
                    package[ i ] = OV528->buf->buf[ i + 4 ];
                goto Exit;
            }
        }
        goto ERR;

    ERR :
#if OV528_USE_ERR_HOOK
    {
        OV528_ErrHook( OV528_ERR_GETPACKET );
    }
#endif
        FIFO_reset( OV528->buf );
        OV528_DELAY_MS( 1 );
        return 0;

    Exit:
        FIFO_reset( OV528->buf );
        OV528_DELAY_MS( 1 );
        return size;
    }
