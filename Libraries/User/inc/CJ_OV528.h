#ifndef __OV528_H__
#define __OV528_H__

#ifdef __cplusplus
extern "C"
{
#endif

//------------------------------------
//Command ID
#define OV528_CMD_ID_INIT               0x01
#define OV528_CMD_ID_GET_PICTURE        0x04
#define OV528_CMD_ID_SNAPSHOT           0x05
#define OV528_CMD_ID_SET_PACKAGE_SIZE   0x06
#define OV528_CMD_ID_SET_BAUD_RATE      0x07
#define OV528_CMD_ID_RESET              0x08
#define OV528_CMD_ID_POWER_DOWN         0x09
#define OV528_CMD_ID_DATA               0x0A
#define OV528_CMD_ID_SNYC               0x0D
#define OV528_CMD_ID_ACK                0x0E
#define OV528_CMD_ID_NAK                0x0F

//------------------------------------
//Command value
#define OV528_INIT_2_BIT_GRAY	      0x01
#define OV528_INIT_4_BIT_GRAY	      0x02
#define OV528_INIT_8_BIT_GRAY	      0x03
#define OV528_INIT_2_COLOR	        0x05
#define OV528_INIT_8_COLOR	        0x06
#define OV528_INIT_JPEG	            0x07
#define OV528_PR_80_60              0x01
#define OV528_PR_160_120            0x03
#define OV528_JPEG_80_64            0x01
#define OV528_JPEG_160_128          0x03
#define OV528_JPEG_320_240          0x05
#define OV528_JPEG_640_480          0x07
#define OV528_COMPRESSED_ENABLE     0x00
#define OV528_COMPRESSED_DISABLE    0x01
#define OV528_GET_SNAPSHOT	        0x01
#define OV528_GET_PREVIEW	          0x02
#define OV528_GET_JPEG_PREVIEW	    0x03

//------------------------------------
//Command error ID
#define OV528_ERR_SNYC		          0x01
#define OV528_ERR_INIT		          0x02
#define OV528_ERR_GETPICTUE		      0x03
#define OV528_ERR_SNAPSHOUT		      0x04
#define OV528_ERR_SETBAUDRATE		    0x05
#define OV528_ERR_POWNDOWN		      0x06
#define OV528_ERR_SETPACKETSZIE		  0x07
#define OV528_ERR_GETPACKET		      0x08

//------------------------------------
//Portable
#define OV528_DELAY_MS(time)		 delay_us(time*1000);

//------------------------------------
//Config
#define OV528_USE_ERR_HOOK    1

typedef struct {
    //Device Config
    uint8_t ID;

    //Image Config
    uint8_t   color;
    uint8_t   previewResolution;
    uint8_t   JPEGResolution;
    uint8_t   IsCompressed;
		uint16_t  skipFrame;
    uint16_t  packetSize;

    //UART Config
    uint32_t baudRate;

    //BUFFER
    DATA_FIFO * buf;
    uint8_t cmd[ 6 ];
    uint8_t err; 

    //Portable function
    uint32_t (*writeData)(uint8_t * arg, uint32_t sum);

    //Image
    uint8_t  imageType;
    uint32_t imageSize;
    uint32_t imagePacket;
} OV528_T;

uint8_t OV528_SNYC(OV528_T * );
uint8_t OV528_INIT(OV528_T *, uint8_t, uint8_t, uint8_t);
uint8_t OV528_GetPictue(OV528_T *, uint8_t);
uint8_t OV528_Snapshout(OV528_T *, uint8_t, uint16_t );
uint8_t OV528_SetBaudRate(OV528_T *, uint32_t );
uint8_t OV528_PowerDown(OV528_T *);
uint8_t OV528_SetPacketSize(OV528_T *, uint16_t );
uint16_t OV528_GetPacket(OV528_T *, uint16_t, uint8_t []);

void OV528_OPEN(void);

#ifdef __cplusplus
}
#endif

#endif   // __OV528_H__
