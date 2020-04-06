/*
    __             _____   __________
   / /_  __  __   /__  /  / ____/ __ \
  / __ \/ / / /     / /  / / __/ /_/ /
 / /_/ / /_/ /     / /__/ /_/ / _, _/
/_.___/\__, /     /____/\____/_/ |_|
      /____/
*/

#include "CJ_OV528.h"

/**
 * @brief  建立新的OV528物件
 * @note
 * @param  ID: 物件ID
 * @param  FIFO_st: FIFO 暫存結構
 * @param  WriteFunction UART寫入函式的函式指標
 * @param  DelayFunction 延時函式的函式指標
 * @retval 物件指標
 */
// OV528_T* OV528_New( uint16_t ID, FIFO_T* FIFO_st, UartFunction funcWrite, UartFunction funcRead, DelayFunction funcDelay ) {
OV528_T* OV528_New( uint16_t ID, size_t bufSize, UART_Func_T* UartFuncion, DelayFunction Delay ) {
    OV528_T* OV528_class = ( OV528_T* )OV528_MALLOC( sizeof( OV528_T ) );
    if ( OV528_class != NULL ) {
        OV528_class->ID      = ID;
        OV528_class->fifoBuf = FIFO_New( bufSize, NULL );
        OV528_class->Uart    = UartFuncion;
        OV528_class->Delay   = Delay;
    }
    return OV528_class;
}

/**
 * @brief  同步函數
 * @note
 * @param  OV528: OV528 結構體
 * @retval 成功 : True ; 失敗 : false
 */
bool OV528_SNYC( OV528_T* OV528 ) {
    bool    cmdCheck;
    uint8_t command[ 6 ]    = { 0xAA, OV528_CMD_ID_SNYC, 0x00, 0x00, 0x00, 0x00 };
    uint8_t commandAck[ 6 ] = { 0xAA, OV528_CMD_ID_ACK, OV528_CMD_ID_SNYC, 0x00, 0x00, 0x00 };

    if ( OV528 == NULL ) {
        return false;
    }

    // send data
    FIFO_Rst( OV528->fifoBuf );
    OV528->Uart->Write( command, 6 );
    OV528->Uart->Write( command, 6 );

    cmdCheck = FIFO_CmdCheck( OV528->fifoBuf, commandAck, 0, 12, 6, 500, false );

    FIFO_Rst( OV528->fifoBuf );
    OV528->Delay( 1 );
    return cmdCheck;
}

/**
 * @brief 初始化
 * @note
 * @param  OV528: OV528 結構體
 *
 * @param  color: 顏色
 * 參數:
 * OV528_INIT_4_BIT_GRAY ; OV528_INIT_2_BIT_GRAY ; OV528_INIT_8_BIT_GRAY ; OV528_INIT_2_COLOR ; OV528_INIT_JPEG
 *`
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

bool OV528_Init( OV528_T* OV528, uint8_t color, uint8_t PR, uint8_t JPEGR ) {
    bool    cmdCheck;
    uint8_t command[ 6 ]    = { 0xAA, OV528_CMD_ID_INIT, 0x00, 0x00, 0x00, 0x00 };
    uint8_t commandAck[ 6 ] = { 0xAA, OV528_CMD_ID_ACK, OV528_CMD_ID_INIT, 0x00, 0x00, 0x00 };

    if ( OV528 == NULL ) {
        return false;
    }

    command[ 3 ] = color;
    command[ 4 ] = PR;
    command[ 5 ] = JPEGR;

    // send data
    FIFO_Rst( OV528->fifoBuf );
    OV528->Uart->Write( command, 6 );

    // check ack
    cmdCheck = FIFO_CmdCheck( OV528->fifoBuf, commandAck, 0, 6, 6, 500, false );
    if ( cmdCheck == true ) {
        OV528->color             = color;
        OV528->previewResolution = PR;
        OV528->JPEGResolution    = JPEGR;
    }

    FIFO_Rst( OV528->fifoBuf );
    OV528->Delay( 1 );
    return cmdCheck;
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
bool OV528_GetPictue( OV528_T* OV528, uint8_t imageType ) {
    bool    cmdCheck;
    uint8_t command[ 6 ]    = { 0xAA, OV528_CMD_ID_GET_PICTURE, 0x00, 0x00, 0x00, 0x00 };
    uint8_t commandAck[ 6 ] = { 0xAA, OV528_CMD_ID_ACK, OV528_CMD_ID_GET_PICTURE, 0x00, 0x00, 0x00 };

    if ( OV528 == NULL ) {
        return false;
    }

    command[ 2 ] = imageType;

    // send data
    FIFO_Rst( OV528->fifoBuf );
    OV528->Uart->Write( command, 6 );

    // check ack
    cmdCheck = FIFO_CmdCheck( OV528->fifoBuf, commandAck, 0, 6, 6, 500, false );
    if ( !cmdCheck )
        return false;

    // check image config
    commandAck[ 1 ] = OV528_CMD_ID_DATA;
    commandAck[ 2 ] = OV528->imageType;
    cmdCheck        = FIFO_CmdCheck( OV528->fifoBuf, commandAck, 6, 6, 6, 500, true );

    if ( cmdCheck == true ) {
        OV528->imageType = imageType;
        OV528->imageSize =
            ( uint32_t )( FIFO_ReadData( OV528->fifoBuf, 9 ) << 0 ) | ( uint32_t )( FIFO_ReadData( OV528->fifoBuf, 10 ) << 8 ) | ( uint32_t )( FIFO_ReadData( OV528->fifoBuf, 11 ) << 16 );
        OV528->imagePacket = OV528->imageSize / ( OV528->packetSize - 6 );
        if ( OV528->imageSize % ( OV528->packetSize - 6 ) )
            OV528->imagePacket++;
    }
    FIFO_Rst( OV528->fifoBuf );
    OV528->Delay( 1 );
    return cmdCheck;
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
bool OV528_Snapshout( OV528_T* OV528, uint8_t Compressed, uint16_t SkipFrame ) {
    uint8_t cmdCheck;
    uint8_t command[ 6 ]    = { 0xAA, OV528_CMD_ID_SNAPSHOT, 0x00, 0x00, 0x00, 0x00 };
    uint8_t commandAck[ 6 ] = { 0xAA, OV528_CMD_ID_ACK, OV528_CMD_ID_SNAPSHOT, 0x00, 0x00, 0x00 };

    if ( OV528 == NULL ) {
        return false;
    }

    command[ 2 ] = Compressed;
    command[ 3 ] = ( uint8_t )( ( SkipFrame & 0x00FF ) >> 0 );
    command[ 4 ] = ( uint8_t )( ( SkipFrame & 0xFF00 ) >> 8 );

    // send data
    FIFO_Rst( OV528->fifoBuf );
    OV528->Uart->Write( command, 6 );

    // check ack
    cmdCheck = FIFO_CmdCheck( OV528->fifoBuf, commandAck, 0, 6, 6, 500, false );

    if ( cmdCheck == true ) {
        OV528->IsCompressed = Compressed;
        OV528->skipFrame    = SkipFrame;
    }

    FIFO_Rst( OV528->fifoBuf );
    OV528->Delay( 1 );
    return cmdCheck;
}

/**
 * @brief  設定 UART 鮑率
 * @note
 * @param  OV528: OV528 結構體
 * @param  BAUD: 鮑率
 * @retval 成功 : 1 ; 失敗 : 0
 */
bool OV528_SetBaudRate( OV528_T* OV528, uint32_t BAUD ) {
    uint8_t  cmdCheck;
    uint8_t  command[ 6 ]    = { 0xAA, OV528_CMD_ID_SET_BAUD_RATE, 0x00, 0x00, 0x00, 0x00 };
    uint8_t  commandAck[ 6 ] = { 0xAA, OV528_CMD_ID_ACK, OV528_CMD_ID_SET_BAUD_RATE, 0x00, 0x00, 0x00 };
    uint32_t div1, div2;

    if ( OV528 == NULL ) {
        return false;
    }

    div1 = ( 14745600 / 4 / BAUD ) / 2;
    div2 = 2;
    while ( div1 > 256 ) {
        div1 /= 2;
        div2 *= 2;
    }

    command[ 2 ] = ( uint8_t )( div1 - 1 );
    command[ 3 ] = ( uint8_t )( div2 - 1 );

    // send data
    FIFO_Rst( OV528->fifoBuf );
    OV528->Uart->Write( command, 6 );

    // check ack
    cmdCheck = FIFO_CmdCheck( OV528->fifoBuf, commandAck, 0, 6, 6, 500, false );
    if ( cmdCheck == true ) {
        OV528->baudRate = 3686400 / div1 / div2;
    }

    FIFO_Rst( OV528->fifoBuf );
    OV528->Delay( 1 );
    return cmdCheck;
}

/**
 * @brief  休眠
 * @note
 * @param  OV528: OV528 結構體
 * @retval 成功 : 1 ; 失敗 : 0
 */
bool OV528_PowerDown( OV528_T* OV528 ) {
    uint8_t cmdCheck;
    uint8_t command[ 6 ]    = { 0xAA, OV528_CMD_ID_POWER_DOWN, 0x00, 0x00, 0x00, 0x00 };
    uint8_t commandAck[ 6 ] = { 0xAA, OV528_CMD_ID_ACK, OV528_CMD_ID_POWER_DOWN, 0x00, 0x00, 0x00 };

    if ( OV528 == NULL ) {
        return false;
    }

    // send data
    FIFO_Rst( OV528->fifoBuf );
    OV528->Uart->Write( command, 6 );

    // check ack
    cmdCheck = FIFO_CmdCheck( OV528->fifoBuf, commandAck, 0, 6, 6, 500, false );

    FIFO_Rst( OV528->fifoBuf );
    OV528->Delay( 1 );
    return cmdCheck;
}

/**
 * @brief  設定傳輸封包大小
 * @note
 * @param  OV528: OV528 結構體
 * @param  size: 封包大小
 * @retval 成功 : 1 ; 失敗 : 0
 */
bool OV528_SetPacketSize( OV528_T* OV528, uint16_t size ) {
    uint8_t cmdCheck;
    uint8_t command[ 6 ]    = { 0xAA, OV528_CMD_ID_SET_PACKAGE_SIZE, 0x08, 0x00, 0x00, 0x00 };
    uint8_t commandAck[ 6 ] = { 0xAA, OV528_CMD_ID_ACK, OV528_CMD_ID_SET_PACKAGE_SIZE, 0x00, 0x00, 0x00 };

    if ( OV528 == NULL ) {
        return false;
    }

    command[ 3 ] = ( uint8_t )( size & 0x00FF );
    command[ 4 ] = ( uint8_t )( size >> 8 );

    // send data
    FIFO_Rst( OV528->fifoBuf );
    OV528->Uart->Write( command, 6 );

    // check ack
    cmdCheck = FIFO_CmdCheck( OV528->fifoBuf, commandAck, 0, 6, 6, 500, false );
    if ( cmdCheck == true ) {
        OV528->packetSize = size;
    }
    FIFO_Rst( OV528->fifoBuf );
    OV528->Delay( 1 );
    return cmdCheck;
}

/**
 * @brief  讀取封包
 * @note
 * @param  OV528: OV528 結構體
 * @param  package_ID: 封包ID
 * @param  outPutArray 儲存陣列
 * @retval 成功 : 封包大小 ; 失敗 : 0
 */
uint16_t OV528_GetPacket( OV528_T* OV528, uint16_t package_ID, uint8_t* outPutArray ) {
    uint16_t i;
    uint16_t size;

    uint16_t checkSum     = 0;
    uint8_t  command[ 6 ] = { 0xAA, OV528_CMD_ID_ACK, 0x00, 0x00, 0x00, 0x00 };

    if ( OV528 == NULL ) {
        return false;
    }

    command[ 4 ] = ( uint8_t )( package_ID & 0x00FF );
    command[ 5 ] = ( uint8_t )( package_ID >> 8 );

    // send data
    FIFO_Rst( OV528->fifoBuf );
    OV528->Uart->Write( command, 6 );

    if ( FIFO_WaitData( OV528->fifoBuf, 4, 250 ) ) {
        // wait for packet head data
        if ( ( FIFO_ReadData( OV528->fifoBuf, 0 ) == command[ 4 ] ) && ( FIFO_ReadData( OV528->fifoBuf, 1 ) == command[ 5 ] ) ) {  // check ID
            size = ( uint16_t )FIFO_ReadData( OV528->fifoBuf, 2 ) | ( ( uint16_t )( FIFO_ReadData( OV528->fifoBuf, 3 ) ) << 8 );
            if ( FIFO_WaitData( OV528->fifoBuf, size + 6, 3000 ) ) {
                for ( i = 0; i < size + 4; i++ )
                    checkSum += OV528->fifoBuf->buf[ i ];
                // check sum
                if ( ( ( uint8_t )( checkSum & 0x00ff ) == FIFO_ReadData( OV528->fifoBuf, size + 4 ) ) && ( ( uint8_t )( checkSum >> 8 ) == FIFO_ReadData( OV528->fifoBuf, size + 5 ) ) ) {
                    for ( i = 0; i < size; i++ )
                        outPutArray[ i ] = FIFO_ReadData( OV528->fifoBuf, i + 4 );

                    FIFO_Rst( OV528->fifoBuf );
                    OV528->Delay( 1 );
                    return size;
                }
            }
        }
    }
    FIFO_Rst( OV528->fifoBuf );
    OV528->Delay( 1 );
    return 0;
}
