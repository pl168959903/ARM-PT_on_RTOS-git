#ifndef __OV528_H__
#define __OV528_H__

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------
// Command ID
#define OV528_CMD_ID_INIT 0x01
#define OV528_CMD_ID_GET_PICTURE 0x04
#define OV528_CMD_ID_SNAPSHOT 0x05
#define OV528_CMD_ID_SET_PACKAGE_SIZE 0x06
#define OV528_CMD_ID_SET_BAUD_RATE 0x07
#define OV528_CMD_ID_RESET 0x08
#define OV528_CMD_ID_POWER_DOWN 0x09
#define OV528_CMD_ID_DATA 0x0A
#define OV528_CMD_ID_SNYC 0x0D
#define OV528_CMD_ID_ACK 0x0E
#define OV528_CMD_ID_NAK 0x0F
//------------------------------------

// Command value
#define OV528_INIT_4_BIT_GRAY 0x02
#define OV528_INIT_2_BIT_GRAY 0x01
#define OV528_INIT_8_BIT_GRAY 0x03
#define OV528_INIT_2_COLOR 0x05
#define OV528_INIT_8_COLOR 0x06
#define OV528_INIT_JPEG 0x07
#define OV528_INIT_PR_80_60 0x01
#define OV528_INIT_PR_160_120 0x03
#define OV528_INIT_JPEG_80_64 0x01
#define OV528_INIT_JPEG_160_128 0x03
#define OV528_INIT_JPEG_320_240 0x05
#define OV528_INIT_JPEG_640_480 0x07
#define OV528_GET_PICTURE_SNAPSHOT 0x01
#define OV528_GET_PICTURE_PR 0x02
#define OV528_GET_PICTURE_JPEG_PR 0x03
#define OV528_SNAPSHOT_COMPRESSED 0x00
#define OV528_SNAPSHOT_UNCOMPRESSED 0x01

//------------------------------------
// Command error ID
#define OV528_ERR_SNYC 0x01
#define OV528_ERR_INIT 0x02
#define OV528_ERR_GETPICTUE 0x03
#define OV528_ERR_SNAPSHOUT 0x04
#define OV528_ERR_SETBAUDRATE 0x05
#define OV528_ERR_POWNDOWN 0x06
#define OV528_ERR_SETPACKETSZIE 0x07
#define OV528_ERR_GETPACKET 0x08

//------------------------------------
// Config
#define OV528_USE_ERR_HOOK 1

//------------------------------------
// include
#include <FIFO.h>

//------------------------------------
// Struct
typedef struct {
    // Device Config
    uint8_t ID;

    // Image Config
    uint8_t  color;
    uint8_t  previewResolution;
    uint8_t  JPEGResolution;
    uint8_t  IsCompressed;
    uint16_t skipFrame;
    uint16_t packetSize;

    // UART Config
    uint32_t baudRate;

    // BUFFER
    FIFO_T* buf;
    uint8_t cmd[ 6 ];
    uint8_t err;

    // Image
    uint8_t  imageType;
    uint32_t imageSize;
    uint32_t imagePacket;

    // WriteData function
    // arr : Array ; size : writeSize
    uint32_t ( *WriteData )( uint8_t* arr, uint32_t size );
    void ( *Delay )( uint32_t time );

} OV528_T;

typedef size_t ( *funcWriteData )( uint8_t*, uint32_t );
typedef void ( *funcDelay )( uint32_t );
//------------------------------------
// Function
OV528_T* OV528_New( uint8_t ID, FIFO_T* FIFO_st, uint32_t ( *WriteFunction )( uint8_t*, uint32_t ), void ( *DelayFunction )( uint32_t ) );
uint8_t  OV528_SNYC( OV528_T* OV528 );
uint8_t  OV528_Init( OV528_T* OV528, uint8_t color, uint8_t PR, uint8_t JPEGR );
uint8_t  OV528_GetPictue( OV528_T* OV528, uint8_t imageType );
uint8_t  OV528_Snapshout( OV528_T* OV528, uint8_t Compressed, uint16_t SkipFrame );
uint8_t  OV528_SetBaudRate( OV528_T* OV528, uint32_t BAUD );
uint8_t  OV528_PowerDown( OV528_T* OV528 );
uint8_t  OV528_SetPacketSize( OV528_T* OV528, uint16_t size );
uint16_t OV528_GetPacket( OV528_T*, uint16_t, uint8_t[] );

#ifdef __cplusplus
}
#endif

#endif  // __OV528_H__
