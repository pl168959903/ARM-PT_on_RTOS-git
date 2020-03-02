/*
    __             _____   __________
   / /_  __  __   /__  /  / ____/ __ \
  / __ \/ / / /     / /  / / __/ /_/ /
 / /_/ / /_/ /     / /__/ /_/ / _, _/
/_.___/\__, /     /____/\____/_/ |_|
      /____/
*/
#include <stdint.h>
#include <stdio.h>

#include "CJ_OV528.h"

#if OV528_USE_ERR_HOOK
/**
 * @brief  例外事件鉤子函數
 * @note
 * @param  errCode: 錯誤號
 * @retval None
 */

void OV528_ErrHook( uint8_t errCode ) {
    switch ( errCode ) {
    case OV528_ERR_SNYC: break;
    case OV528_ERR_INIT: break;
    case OV528_ERR_GETPICTUE: break;
    case OV528_ERR_SNAPSHOUT: break;
    case OV528_ERR_SETBAUDRATE: break;
    case OV528_ERR_POWNDOWN: break;
    case OV528_ERR_SETPACKETSZIE: break;
    case OV528_ERR_GETPACKET: break;
    default: break;
    }
    // Device header
    if ( errCode - 1 ) printf( "Camena err!!	ERR CODE : %X\n", errCode );
    // while(1) {};
}
#endif  // OV528_USE_ERR_HOOK

void OV528_StructInit(OV528_T* OV528, uint8_t ID, FIFO_T * FIFO_st, )

/**
 * @brief  設定指令
 * ! @note   私有函數
 * @param  OV528: OV528 結構體
 * @param  byte 1-5: 指令
 * @retval None
 */
void OV528_SetCmd( OV528_T* OV528, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5 ) {
    OV528->cmd[ 0 ] = 0xAA;
    OV528->cmd[ 1 ] = byte1;
    OV528->cmd[ 2 ] = byte2;
    OV528->cmd[ 3 ] = byte3;
    OV528->cmd[ 4 ] = byte4;
    OV528->cmd[ 5 ] = byte5;
}

/**
 * @brief  同步函數
 * @note   
 * @param  OV528: OV528 結構體
 * @retval
 */
uint8_t OV528_SNYC( OV528_T* OV528 ) {
    uint8_t cmdCheck;

    // send data
    OV528_SetCmd( OV528, OV528_CMD_ID_SNYC, 0x00, 0x00, 0x00, 0x00 );
    FIFO_Rst( OV528->buf );
    OV528->WriteData( OV528->cmd, 6 );
    OV528->WriteData( OV528->cmd, 6 );

    // check ack
    OV528_SetCmd( OV528, OV528_CMD_ID_ACK, 0x00, 0x00, 0x00, 0x00 );
    cmdCheck = FIFO_CmdCheck( OV528->buf, OV528->cmd, 6, 0 );
    if ( cmdCheck )
        goto Exit;
    else
        goto ERR;

    ERR:
    // Error Hook
    #if OV528_USE_ERR_HOOK
    OV528_ErrHook( OV528_ERR_SNYC );
    #endif  // OV528_USE_ERR_HOOK

    FIFO_Rst( OV528->buf );
    OV528->Delay( 1 );
    return 0;

    Exit:
    FIFO_Rst( OV528->buf );
    OV528->Delay( 1 );
    return 1;
}

/**
 * @brief 初始化
 * @note
 * @param  OV528: OV528 結構體
 *
 * @param  color: 顏色
 * 參數:
 * OV528_INIT_4_BIT_GRAY ; OV528_INIT_2_BIT_GRAY ; OV528_INIT_8_BIT_GRAY ; OV528_INIT_2_COLOR ; OV528_INIT_JPEG
 *
 * @param  PR: 預覽圖大小
 * 參數:
 * OV528_INIT_PR_80_60 ; OV528_INIT_PR_160_120
 *
 * @param  JPEGR: JPEG 大小
 * 參數:
 * OV528_INIT_JPEG_80_64 ; OV528_INIT_JPEG_160_128 ; OV528_INIT_JPEG_320_240 ; OV528_INIT_JPEG_640_480
 *
 * @retval 成功 : 1 ; 失敗 : 0
 */
uint8_t OV528_Init( OV528_T* OV528, uint8_t color, uint8_t PR, uint8_t JPEGR ) {
    uint8_t cmdCheck;

    // send data
    OV528_SetCmd( OV528, OV528_CMD_ID_INIT, 0x00, color, PR, JPEGR );
    FIFO_Rst( OV528->buf );
    OV528->WriteData( OV528->cmd, 6 );

    // check ack
    OV528_SetCmd( OV528, OV528_CMD_ID_ACK, OV528_CMD_ID_INIT, 0x00, 0x00, 0x00 );
    cmdCheck = FIFO_CmdCheck( OV528->buf, OV528->cmd, 6, 250 );
    if ( cmdCheck )
        goto Exit;
    else
        goto ERR;

    ERR :
    // Error Hook
    #if OV528_USE_ERR_HOOK
    OV528_ErrHook( OV528_ERR_INIT );
    #endif

    FIFO_Rst( OV528->buf );
    OV528->Delay( 1 );
    return 0;

    Exit:
    OV528->color             = color;
    OV528->previewResolution = PR;
    OV528->JPEGResolution    = JPEGR;

    FIFO_Rst( OV528->buf );
    OV528->Delay( 1 );
    return 1;
}

/**
 * @brief  拍照
 * @note
 * @param  OV528: OV528 結構體
 *
 * @param  imageType: 拍照類型
 * 參數:
 * OV528_GET_PICTURE_SNAPSHOT ; OV528_GET_PICTURE_PR ; OV528_GET_PICTURE_JPEG_PR
 *
 * @retval 成功 : 1 ; 失敗 : 0
 */
uint8_t OV528_GetPictue( OV528_T* OV528, uint8_t imageType ) {
    uint8_t cmdCheck;

    // send data
    OV528_SetCmd( OV528, OV528_CMD_ID_GET_PICTURE, imageType, 0x00, 0x00, 0x00 );
    FIFO_Rst( OV528->buf );
    OV528->WriteData( OV528->cmd, 6 );

    // check ack
    OV528_SetCmd( OV528, OV528_CMD_ID_ACK, OV528_CMD_ID_GET_PICTURE, 0x00, 0x00, 0x00 );
    cmdCheck = FIFO_CmdCheck( OV528->buf, OV528->cmd, 6, 250 );
    if ( !cmdCheck ) goto ERR;

    // check image config
    OV528_SetCmd( OV528, OV528_CMD_ID_DATA, OV528->imageType, 0, 0, 0 );
    cmdCheck = FIFO_CmdCheck( OV528->buf, OV528->cmd, 3, 1000 );
    if ( cmdCheck ) {
        if ( FIFO_WaitData( OV528->buf, 12, 1 ) ) { goto Exit; }
    }
    goto ERR;

    ERR :
    #if OV528_USE_ERR_HOOK
    OV528_ErrHook( OV528_ERR_GETPICTUE );  // Error Hook
    #endif

    FIFO_Rst( OV528->buf );
    OV528->Delay( 1 );
    return 0;

    Exit:
    OV528->imageType   = imageType;
    OV528->imageSize   = ( uint32_t )( FIFO_ReadData( OV528->buf, 9 ) << 0 ) | ( uint32_t )( FIFO_ReadData( OV528->buf, 10 ) << 8 ) | ( uint32_t )( FIFO_ReadData( OV528->buf, 11 ) << 16 );
    OV528->imagePacket = OV528->imageSize / ( OV528->packetSize - 6 );
    if ( OV528->imageSize % ( OV528->packetSize - 6 ) ) OV528->imagePacket++;

    FIFO_Rst( OV528->buf );
    OV528->Delay( 1 );
    return 1;
}

/**
 * @brief  快照相關設定
 * @note
 * @param  OV528: OV528 結構體
 *
 * @param  Compressed: 壓縮
 * 參數:
 * OV528_SNAPSHOT_COMPRESSED ; OV528_SNAPSHOT_UNCOMPRESSED
 *
 * @param  SkipFrame: 跳過幀
 * @retval 成功 : 1 ; 失敗 : 0
 */
uint8_t OV528_Snapshout( OV528_T* OV528, uint8_t Compressed, uint16_t SkipFrame ) {
    uint8_t cmdCheck;

    // send data
    OV528_SetCmd( OV528, OV528_CMD_ID_SNAPSHOT, Compressed, ( uint8_t )( ( SkipFrame & 0x00FF ) >> 0 ), ( uint8_t )( ( SkipFrame & 0xFF00 ) >> 8 ), 0x00 );
    FIFO_Rst( OV528->buf );
    OV528->WriteData( OV528->cmd, 6 );

    // check ack
    OV528_SetCmd( OV528, OV528_CMD_ID_ACK, OV528_CMD_ID_SNAPSHOT, 0x00, 0x00, 0x00 );
    cmdCheck = FIFO_CmdCheck( OV528->buf, OV528->cmd, 6, 250 );
    if ( cmdCheck )
        goto Exit;
    else
        goto ERR;

    ERR :
    // Error Hook
    #if OV528_USE_ERR_HOOK
    OV528_ErrHook( OV528_ERR_SNAPSHOUT );
    #endif

    FIFO_Rst( OV528->buf );
    OV528->Delay( 1 );
    return 0;

    Exit:
    OV528->IsCompressed = Compressed;
    OV528->skipFrame    = SkipFrame;

    FIFO_Rst( OV528->buf );
    OV528->Delay( 1 );
    return 1;
}

/**
 * @brief  設定 UART 鮑率
 * @note
 * @param  OV528: OV528 結構體
 * @param  BAUD: 鮑率
 * @retval 成功 : 1 ; 失敗 : 0
 */
uint8_t OV528_SetBaudRate( OV528_T* OV528, uint32_t BAUD ) {
    uint8_t  cmdCheck;
    uint32_t div1 = ( 14745600 / 4 / BAUD ) / 2;
    uint32_t div2 = 2;
    while ( div1 > 256 ) {
        div1 /= 2;
        div2 *= 2;
    }

    // send data
    OV528_SetCmd( OV528, OV528_CMD_ID_SET_BAUD_RATE, ( uint8_t )( div1 - 1 ), ( uint8_t )( div2 - 1 ), 0x00, 0x00 );
    FIFO_Rst( OV528->buf );
    OV528->WriteData( OV528->cmd, 6 );

    // check ack
    OV528_SetCmd( OV528, OV528_CMD_ID_ACK, OV528_CMD_ID_SET_BAUD_RATE, 0x00, 0x00, 0x00 );
    cmdCheck = FIFO_CmdCheck( OV528->buf, OV528->cmd, 6, 250 );
    if ( cmdCheck )
        goto Exit;
    else
        goto ERR;

    ERR :
    #if OV528_USE_ERR_HOOK
    OV528_ErrHook( OV528_ERR_SETBAUDRATE );  // Error Hook
    #endif

    FIFO_Rst( OV528->buf );
    OV528->Delay( 1 );
    return 0;

    Exit:
    OV528->baudRate = 3686400 / div1 / div2;

    FIFO_Rst( OV528->buf );
    OV528->Delay( 1 );
    return 1;
}

/**
 * @brief  休眠
 * @note
 * @param  OV528: OV528 結構體
 * @retval 成功 : 1 ; 失敗 : 0
 */
uint8_t OV528_PowerDown( OV528_T* OV528 ) {
    uint8_t cmdCheck;

    // send data
    OV528_SetCmd( OV528, OV528_CMD_ID_POWER_DOWN, 0x00, 0x00, 0x00, 0x00 );
    FIFO_Rst( OV528->buf );
    OV528->WriteData( OV528->cmd, 6 );

    // check ack
    OV528_SetCmd( OV528, OV528_CMD_ID_ACK, OV528_CMD_ID_POWER_DOWN, 0x00, 0x00, 0x00 );
    cmdCheck = FIFO_CmdCheck( OV528->buf, OV528->cmd, 6, 250 );
    if ( cmdCheck )
        goto Exit;
    else
        goto ERR;

    ERR :
    #if OV528_USE_ERR_HOOK
    OV528_ErrHook( OV528_ERR_POWNDOWN );
    #endif

    FIFO_Rst( OV528->buf );
    OV528->Delay( 1 );
    return 0;

    Exit:
    FIFO_Rst( OV528->buf );
    OV528->Delay( 1 );
    return 1;
}

/**
 * @brief  設定傳輸封包大小
 * @note
 * @param  OV528: OV528 結構體
 * @param  size: 封包大小
 * @retval 成功 : 1 ; 失敗 : 0
 */
uint8_t OV528_SetPacketSize( OV528_T* OV528, uint16_t size ) {
    uint8_t cmdCheck;

    // send data
    OV528_SetCmd( OV528, OV528_CMD_ID_SET_PACKAGE_SIZE, 0x08, ( uint8_t )( size & 0x00FF ), ( uint8_t )( size >> 8 ), 0x00 );
    FIFO_Rst( OV528->buf );
    OV528->WriteData( OV528->cmd, 6 );

    // check ack
    OV528_SetCmd( OV528, OV528_CMD_ID_ACK, OV528_CMD_ID_SET_PACKAGE_SIZE, 0x00, 0x00, 0x00 );
    cmdCheck = FIFO_CmdCheck( OV528->buf, OV528->cmd, 6, 250 );
    if ( cmdCheck )
        goto Exit;
    else
        goto ERR;

    ERR :
    #if OV528_USE_ERR_HOOK
    OV528_ErrHook( OV528_ERR_SETPACKETSZIE );
    #endif

    FIFO_Rst( OV528->buf );
    OV528->Delay( 1 );
    return 0;

    Exit:
    OV528->packetSize = size;

    FIFO_Rst( OV528->buf );
    OV528->Delay( 1 );
    return 1;
}

/**
 * @brief  讀取封包
 * @note
 * @param  OV528: OV528 結構體
 * @param  package_ID: 封包ID
 * @param  package[]: 儲存陣列
 * @retval 成功 : 封包大小 ; 失敗 : 0
 */
uint16_t OV528_GetPacket( OV528_T* OV528, uint16_t package_ID, uint8_t package[] ) {
    uint8_t  ID[ 2 ];
    uint16_t i;
    uint16_t size;
    uint16_t checkSum = 0;
    ID[ 0 ]           = ( uint8_t )( package_ID & 0x00FF );
    ID[ 1 ]           = ( uint8_t )( package_ID >> 8 );

    // send data
    OV528_SetCmd( OV528, OV528_CMD_ID_ACK, 0x00, 0x00, ID[ 0 ], ID[ 1 ] );
    FIFO_Rst( OV528->buf );
    OV528->WriteData( OV528->cmd, 6 );

    if ( FIFO_WaitData( OV528->buf, 4, 250 ) )                                                                   // wait for packet head data
        if ( ( FIFO_ReadData( OV528->buf, 0 ) == ID[ 0 ] ) && ( FIFO_ReadData( OV528->buf, 1 ) == ID[ 1 ] ) ) {  // check ID
            size = ( uint16_t )OV528->buf->buf[ 2 ] | ( uint16_t )( OV528->buf->buf[ 3 ] << 8 );                 // get data size
            if ( FIFO_WaitData( OV528->buf, size + 6, 3000 ) ) {
                for ( i = 0; i < size + 4; i++ ) checkSum += OV528->buf->buf[ i ];
                // check sum
                if ( ( ( uint8_t )( checkSum & 0x00ff ) == OV528->buf->buf[ size + 4 ] ) && ( ( uint8_t )( checkSum >> 8 ) == OV528->buf->buf[ size + 5 ] ) ) {
                    for ( i = 0; i < size; i++ ) package[ i ] = OV528->buf->buf[ i + 4 ];
                    goto Exit;
                }
            }
        }
    goto ERR;

    ERR :
    #if OV528_USE_ERR_HOOK
    OV528_ErrHook( OV528_ERR_GETPACKET );
    #endif
    FIFO_Rst( OV528->buf );
    OV528->Delay( 1 );
    return 0;

    Exit:
    FIFO_Rst( OV528->buf );
    OV528->Delay( 1 );
    return size;
}
