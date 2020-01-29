#include "Nano103.h"
#include <USER_LIB.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


uint8_t RS232_SYNC[6];
uint8_t OV2640_ERR_CODE = 0x00;

struct OV2640 OV2640;


void OV2640_exception(uint8_t err) {
    switch(err) {
    case OV2640_ERR_SNYC:
        break;
    case OV2640_ERR_INIT:
        break;
    case OV2640_ERR_GETPICTUE:
        break;
    case OV2640_ERR_SNAPSHOUT:
        break;
    case OV2640_ERR_SETBAUDRATE:
        break;
    case OV2640_ERR_POWNDOWN:
        break;
    case OV2640_ERR_SETPACKETSZIE:
        break;
    case OV2640_ERR_GETPACKET:
        break;
    default:
        break;
    }

    if(err - 1)
        printf("Camena err!!	ERR CODE : %X\n", err);
    // while(1) {};
}

uint8_t OV2640_SNYC(void) {
    Array_assign(RS232_SYNC, 6, 0xAA, OV2640_CMD_ID_SNYC, 0x00, 0x00, 0x00, 0x00);
    FIFO_reset(&OV2640_UART_BUF);
    OV2640_WRITE(RS232_SYNC, 6);
    OV2640_WRITE(RS232_SYNC, 6);
    Array_assign(RS232_SYNC, 6, 0xAA, OV2640_CMD_ID_ACK, 0x00, 0x00, 0x00, 0x00);

    if(FIFO_cmdCheck(&OV2640_UART_BUF, RS232_SYNC, 6, 0)) goto Exit;
    else goto ERR;

ERR:
    OV2640_exception(OV2640_ERR_SNYC);
    FIFO_reset(&OV2640_UART_BUF);
    return 0;

Exit:
    FIFO_reset(&OV2640_UART_BUF);
    OV2640_CMD_DELAY();
    return 1;
}

uint8_t OV2640_INIT(uint8_t color, uint8_t PR, uint8_t JPEGR) {
    Array_assign(RS232_SYNC, 6, 0xAA, OV2640_CMD_ID_INIT, 0x00, color, PR, JPEGR);
    FIFO_reset(&OV2640_UART_BUF);
    OV2640_WRITE(RS232_SYNC, 6);
    Array_assign(RS232_SYNC, 6, 0xAA, OV2640_CMD_ID_ACK, OV2640_CMD_ID_INIT, 0x00, 0x00, 0x00);

    if(FIFO_cmdCheck(&OV2640_UART_BUF, RS232_SYNC, 6, 250)) goto Exit;
    else goto ERR;

ERR:
    OV2640_exception(OV2640_ERR_INIT);
    FIFO_reset(&OV2640_UART_BUF);
    return 0;

Exit:
    FIFO_reset(&OV2640_UART_BUF);
    OV2640.color = color;
    OV2640.previewResolution = PR;
    OV2640.JPEGResolution = JPEGR;
    OV2640_CMD_DELAY();
    return 1;
}

uint8_t OV2640_GetPictue(uint8_t imageType) {
    Array_assign(RS232_SYNC, 6, 0xAA, OV2640_CMD_ID_GET_PICTURE, imageType, 0x00, 0x00, 0x00);
    FIFO_reset(&OV2640_UART_BUF);
    OV2640_WRITE(RS232_SYNC, 6);

    Array_assign(RS232_SYNC, 6, 0xAA, OV2640_CMD_ID_ACK, OV2640_CMD_ID_GET_PICTURE, 0x00, 0x00, 0x00);
    if(!FIFO_cmdCheck(&OV2640_UART_BUF, RS232_SYNC, 6, 250)) goto ERR;

    OV2640.imageType = imageType;

    Array_assign(RS232_SYNC, 3, 0xAA, OV2640_CMD_ID_DATA, OV2640.imageType);
    if(FIFO_cmdCheck(&OV2640_UART_BUF, RS232_SYNC, 3, 1000)) {
        if(FIFO_waitData(&OV2640_UART_BUF, 12, 1)) {
            OV2640.imageSize = (uint32_t)(FIFO_read_byte(&OV2640_UART_BUF, 9) << 0)
                               | (uint32_t)(FIFO_read_byte(&OV2640_UART_BUF, 10) << 8)
                               | (uint32_t)(FIFO_read_byte(&OV2640_UART_BUF, 11) << 16);
            OV2640.imagePacket = OV2640.imageSize / (OV2640.packetSize - 6);
            if(OV2640.imageSize % (OV2640.packetSize - 6)) OV2640.imagePacket++;
            goto Exit;
        }
    }
    goto ERR;
ERR:
    OV2640_exception(OV2640_ERR_GETPICTUE);
    FIFO_reset(&OV2640_UART_BUF);
    return 0;

Exit:
    FIFO_reset(&OV2640_UART_BUF);
    OV2640_CMD_DELAY();
    return 1;
}

uint8_t OV2640_Snapshout(uint8_t Compressed, uint16_t SkipFrame) {
    Array_assign(RS232_SYNC, 6, 0xAA, OV2640_CMD_ID_SNAPSHOT, Compressed, (uint8_t)((SkipFrame & 0x00FF) >> 0), (uint8_t)((SkipFrame & 0xFF00) >> 8), 0x00);
    FIFO_reset(&OV2640_UART_BUF);
    OV2640_WRITE(RS232_SYNC, 6);
    Array_assign(RS232_SYNC, 6, 0xAA, OV2640_CMD_ID_ACK, OV2640_CMD_ID_SNAPSHOT, 0x00, 0x00, 0x00);

    if(FIFO_cmdCheck(&OV2640_UART_BUF, RS232_SYNC, 6, 250)) goto Exit;
    goto ERR;

ERR:
    OV2640_exception(OV2640_ERR_SNAPSHOUT);
    FIFO_reset(&OV2640_UART_BUF);
    return 0;

Exit:
    FIFO_reset(&OV2640_UART_BUF);
    OV2640.IsCompressed = Compressed;
    OV2640.skipFrame = SkipFrame;
    OV2640_CMD_DELAY();
    return 1;
}
uint8_t OV2640_SetBaudRate(uint32_t BAUD) {
    uint32_t div1 = (14745600 / 4 / BAUD) / 2;
    uint32_t div2 = 2;
    while(div1 > 256) {
        div1 /= 2;
        div2 *= 2;
    }
    Array_assign(RS232_SYNC, 6, 0xAA, OV2640_CMD_ID_SET_BAUD_RATE, (uint8_t)(div1 - 1), (uint8_t)(div2 - 1), 0x00, 0x00);
    FIFO_reset(&OV2640_UART_BUF);
    OV2640_WRITE(RS232_SYNC, 6);
    Array_assign(RS232_SYNC, 6, 0xAA, OV2640_CMD_ID_ACK, OV2640_CMD_ID_SET_BAUD_RATE, 0x00, 0x00, 0x00);

    if(FIFO_cmdCheck(&OV2640_UART_BUF, RS232_SYNC, 6, 250)) goto Exit;
    goto ERR;

ERR:
    OV2640_exception(OV2640_ERR_SETBAUDRATE);
    FIFO_reset(&OV2640_UART_BUF);
    return 0;

Exit:
    FIFO_reset(&OV2640_UART_BUF);
    OV2640.baudRate = 3686400 / div1 / div2;
    OV2640_CMD_DELAY();
    return 1;
}

uint8_t OV2640_PowerDown(void) {
    Array_assign(RS232_SYNC, 6, 0xAA, OV2640_CMD_ID_POWER_DOWN, 0x00, 0x00, 0x00, 0x00);
    FIFO_reset(&OV2640_UART_BUF);
    OV2640_WRITE(RS232_SYNC, 6);
    Array_assign(RS232_SYNC, 6, 0xAA, OV2640_CMD_ID_ACK, OV2640_CMD_ID_POWER_DOWN, 0x00, 0x00, 0x00);
    if(FIFO_cmdCheck(&OV2640_UART_BUF, RS232_SYNC, 6, 250)) goto Exit;
    goto ERR;

ERR:
    OV2640_exception(OV2640_ERR_POWNDOWN);
    FIFO_reset(&OV2640_UART_BUF);
    return 0;

Exit:
    FIFO_reset(&OV2640_UART_BUF);
    OV2640_CMD_DELAY();
    return 1;
}

uint8_t OV2640_SetPacketSize(uint16_t size) {
    Array_assign(RS232_SYNC, 6, 0xAA, OV2640_CMD_ID_SET_PACKAGE_SIZE, 0x08, (uint8_t)(size & 0x00FF), (uint8_t)(size >> 8), 0x00);
    FIFO_reset(&OV2640_UART_BUF);
    OV2640_WRITE(RS232_SYNC, 6);
    Array_assign(RS232_SYNC, 6, 0xAA, OV2640_CMD_ID_ACK, OV2640_CMD_ID_SET_PACKAGE_SIZE, 0x00, 0x00, 0x00);
    if(FIFO_cmdCheck(&OV2640_UART_BUF, RS232_SYNC, 6, 250)) {
        OV2640.packetSize = size;
        goto Exit;
    }
    goto ERR;

ERR:
    OV2640_exception(OV2640_ERR_SETPACKETSZIE);
    FIFO_reset(&OV2640_UART_BUF);
    return 0;

Exit:
    FIFO_reset(&OV2640_UART_BUF);
    OV2640_CMD_DELAY();
    return 1;
}

uint16_t OV2640_GetPacket(uint16_t package_ID, uint8_t package[]) {
    uint8_t ID[2];
    uint16_t i;
    uint16_t size;
    uint16_t checkSum = 0;
    ID[0] = (uint8_t)(package_ID & 0x00FF);
    ID[1] = (uint8_t)(package_ID >> 8);
    Array_assign(RS232_SYNC, 6, 0xAA, OV2640_CMD_ID_ACK, 0x00, 0x00, ID[0], ID[1]);
    FIFO_reset(&OV2640_UART_BUF);
    OV2640_WRITE(RS232_SYNC, 6);
    Array_assign(RS232_SYNC, 2, 0xAA, ID[0], ID[1]);
    if(FIFO_waitData(&OV2640_UART_BUF, 4, 250))
        if(
            (FIFO_read_byte(&OV2640_UART_BUF, 0) == ID[0]) &&
            (FIFO_read_byte(&OV2640_UART_BUF, 1) == ID[1])
        ) {
            size = (uint16_t)OV2640_UART_BUF.buf[2] | (uint16_t)(OV2640_UART_BUF.buf[3] << 8);
            if(FIFO_waitData(&OV2640_UART_BUF, size + 6, 3000)) {
                for(i = 0; i < size + 4; i++) checkSum += OV2640_UART_BUF.buf[i];
                if(
                    ((uint8_t)(checkSum & 0x00ff) == OV2640_UART_BUF.buf[size + 4]) &&
                    ((uint8_t)(checkSum >> 8) == OV2640_UART_BUF.buf[size + 5])
                ) {
                    for(i = 0; i < size; i++) package[i] = OV2640_UART_BUF.buf[i + 4];
                    goto Exit;
                }
            }
        }
    goto ERR;

ERR:
    OV2640_exception(OV2640_ERR_GETPACKET);
    FIFO_reset(&OV2640_UART_BUF);
    return 0;

Exit:
    FIFO_reset(&OV2640_UART_BUF);
    OV2640_CMD_DELAY();
    return size;
}

