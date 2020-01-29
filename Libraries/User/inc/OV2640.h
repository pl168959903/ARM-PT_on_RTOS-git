#ifndef __OV2640_H__
#define __OV2640_H__

#ifdef __cplusplus
extern "C"
{
#endif

//------------------------------------
#define OV2640_CMD_ID_INIT                          0x01
#define OV2640_CMD_ID_GET_PICTURE                   0x04
#define OV2640_CMD_ID_SNAPSHOT                      0x05
#define OV2640_CMD_ID_SET_PACKAGE_SIZE              0x06
#define OV2640_CMD_ID_SET_BAUD_RATE                 0x07
#define OV2640_CMD_ID_RESET                         0x08
#define OV2640_CMD_ID_POWER_DOWN                    0x09
#define OV2640_CMD_ID_DATA                          0x0A
#define OV2640_CMD_ID_SNYC                          0x0D
#define OV2640_CMD_ID_ACK                           0x0E
#define OV2640_CMD_ID_NAK                           0x0F

//------------------------------------
#define OV2640_INIT_2_BIT_GRAY	0x01
#define OV2640_INIT_4_BIT_GRAY	0x02
#define OV2640_INIT_8_BIT_GRAY	0x03
#define OV2640_INIT_2_COLOR	0x05
#define OV2640_INIT_8_COLOR	0x06
#define OV2640_INIT_JPEG	0x07
#define OV2640_PR_80_60 0x01
#define OV2640_PR_160_120 0x03
#define OV2640_JPEG_80_64 0x01
#define OV2640_JPEG_160_128 0x03
#define OV2640_JPEG_320_240 0x05
#define OV2640_JPEG_640_480 0x07
#define OV2640_COMPRESSED_ENABLE	0x00
#define OV2640_COMPRESSED_DISABLE	0x01
#define OV2640_GET_SNAPSHOT	0x01
#define OV2640_GET_PREVIEW	0x02
#define OV2640_GET_JPEG_PREVIEW	0x03
//------------------------------------
#define OV2640_ERR_SNYC		0x01
#define OV2640_ERR_INIT		0x02
#define OV2640_ERR_GETPICTUE		0x03
#define OV2640_ERR_SNAPSHOUT		0x04
#define OV2640_ERR_SETBAUDRATE		0x05
#define OV2640_ERR_POWNDOWN		0x06
#define OV2640_ERR_SETPACKETSZIE		0x07
#define OV2640_ERR_GETPACKET		0x08
//------------------------------------

#define OV2640_UART UART1
#define OV2640_UART_BUF FIFO_uart1_buf
#define OV2640_WRITE(buf,cnt)   UART_Write(OV2640_UART, buf, cnt)
#define OV2640_CMD_DELAY()		delay_us(100);

struct OV2640{
    //UART Inital
    uint32_t baudRate;

    //Image Initial
    uint8_t color;
    uint8_t previewResolution;
    uint8_t JPEGResolution;
    uint8_t IsCompressed;
		uint16_t skipFrame;
    uint16_t packetSize;

    //image
    uint8_t imageType;
    uint32_t imageSize;
    uint32_t imagePacket;
};

extern struct OV2640 OV2640;

uint8_t OV2640_SNYC(void);
uint8_t OV2640_INIT(uint8_t, uint8_t, uint8_t);

uint8_t OV2640_GetPictue(uint8_t);
uint8_t OV2640_Snapshout(uint8_t, uint16_t );
uint8_t OV2640_SetBaudRate(uint32_t );
uint8_t OV2640_PowerDown(void);
uint8_t OV2640_SetPacketSize(uint16_t );
uint16_t OV2640_GetPacket(uint16_t, uint8_t []);
void OV2640_OPEN(void);

#ifdef __cplusplus
}
#endif

#endif   // __OV2640_H__
