#include "Nano103.h"
#include <USER_LIB.h>
#include <stdio.h>
#include <stdlib.h>

//#define W25QXX_DEBUG
w25qxx_t w25qxx;

uint8_t w25qxx_write(uint8_t data) {
    SPI_WRITE_TX0(W25QXX_SPI, data);
    SPI_TRIGGER(W25QXX_SPI);
    while(SPI_IS_BUSY(W25QXX_SPI));
    return (SPI_READ_RX0(W25QXX_SPI) & 0xFF);
}

uint8_t w25qxx_read(void) {
    SPI_TRIGGER(W25QXX_SPI);
    while(SPI_IS_BUSY(W25QXX_SPI));
    return (SPI_READ_RX0(W25QXX_SPI) & 0xFF);
}

void w25qxx_dummy(void) {
    SPI_TRIGGER(W25QXX_SPI);
    while(SPI_IS_BUSY(W25QXX_SPI));
}

uint32_t w25qxx_ReadID(void) {
    uint32_t temp0, temp1, temp2;
    SPI_SET_SS0_LOW(W25QXX_SPI);
    w25qxx_write(0x9F);
    temp0 = w25qxx_read();
    temp1 = w25qxx_read();
    temp2 = w25qxx_read();
    SPI_SET_SS0_HIGH(W25QXX_SPI);
    temp0 = ((temp0 << 16) | (temp1 << 8) | (temp2 << 0));
    return temp0;
}

void w25qxx_ReadUniqID(void) {
    uint8_t i;
    SPI_SET_SS0_LOW(W25QXX_SPI);
    w25qxx_write(0x4B);
    for( i = 0; i < 4; i++)
        w25qxx_dummy();
    for( i = 0; i < 8; i++)
        w25qxx.UniqID[i] = w25qxx_read();
    SPI_SET_SS0_HIGH(W25QXX_SPI);
}

void w25qxx_WriteEnable(void) {
    SPI_SET_SS0_LOW(W25QXX_SPI);
    w25qxx_write(0x06);
    SPI_SET_SS0_HIGH(W25QXX_SPI);
}

void w25qxx_WriteDisable(void) {
    SPI_SET_SS0_LOW(W25QXX_SPI);
    w25qxx_write(0x04);
    SPI_SET_SS0_HIGH(W25QXX_SPI);
}

void w25qxx_VolatileSRWriteEnable(void) {
    SPI_SET_SS0_LOW(W25QXX_SPI);
    w25qxx_write(0x50);
    SPI_SET_SS0_HIGH(W25QXX_SPI);
}
uint8_t w25qxx_ReadStatusRegister(uint8_t reg_1_2_3) {
    uint8_t temp;
    SPI_SET_SS0_LOW(W25QXX_SPI);
    if(reg_1_2_3 == 1) {
        w25qxx_write(0x05);
        temp = w25qxx_read();
        w25qxx.StatusRegister1 = temp;
    } else if(reg_1_2_3 == 2) {
        w25qxx_write(0x35);
        temp = w25qxx_read();
        w25qxx.StatusRegister2 = temp;
    } else if(reg_1_2_3 == 3) {
        w25qxx_write(0x15);
        temp = w25qxx_read();
        w25qxx.StatusRegister3 = temp;
    }
    SPI_SET_SS0_HIGH(W25QXX_SPI);
    return temp;
}
void w25qxx_WriteStatusRegister(uint8_t reg_1_2_3, uint8_t data) {
    SPI_SET_SS0_LOW(W25QXX_SPI);
    if(reg_1_2_3 == 1) {
        w25qxx_write(0x01);
        w25qxx.StatusRegister1 = data;
    } else if(reg_1_2_3 == 2) {
        w25qxx_write(0x31);
        w25qxx.StatusRegister2 = data;
    } else if(reg_1_2_3 == 3) {
        w25qxx_write(0x11);
        w25qxx.StatusRegister3 = data;
    }
    w25qxx_write(data);
    SPI_SET_SS0_HIGH(W25QXX_SPI);
}
void w25qxx_WaitForWriteEnd(void) {
    SPI_SET_SS0_LOW(W25QXX_SPI);
    w25qxx_write(0x05);
    do {
        w25qxx.StatusRegister1 = w25qxx_read();
    } while((w25qxx.StatusRegister1 & 0x01));
    SPI_SET_SS0_HIGH(W25QXX_SPI);
}
void w25qxx_EraseChip(void) {
    w25qxx_WriteEnable();
    SPI_SET_SS0_LOW(W25QXX_SPI);
    w25qxx_write(0xC7);
    SPI_SET_SS0_HIGH(W25QXX_SPI);
    w25qxx_WaitForWriteEnd();
}

void w25qxx_EraseSector(uint32_t Addr) {
    w25qxx_WriteEnable();
    SPI_SET_SS0_LOW(W25QXX_SPI);
    w25qxx_write(0x20);
    w25qxx_write((Addr & 0x00FF0000) >> 16);
    w25qxx_write((Addr & 0x0000FF00) >> 8);
    w25qxx_write((Addr & 0x000000FF) >> 0);
    SPI_SET_SS0_HIGH(W25QXX_SPI);
    w25qxx_WaitForWriteEnd();
}

void w25qxx_EraseHalfBlock(uint32_t Addr) {
    w25qxx_WriteEnable();
    SPI_SET_SS0_LOW(W25QXX_SPI);
    w25qxx_write(0x52);
    w25qxx_write((Addr & 0x00FF0000) >> 16);
    w25qxx_write((Addr & 0x0000FF00) >> 8);
    w25qxx_write((Addr & 0x000000FF) >> 0);
    SPI_SET_SS0_HIGH(W25QXX_SPI);
    w25qxx_WaitForWriteEnd();
}

void w25qxx_EraseFullBlock(uint32_t Addr) {
    w25qxx_WriteEnable();
    SPI_SET_SS0_LOW(W25QXX_SPI);
    w25qxx_write(0xD8);
    w25qxx_write((Addr & 0x00FF0000) >> 16);
    w25qxx_write((Addr & 0x0000FF00) >> 8);
    w25qxx_write((Addr & 0x000000FF) >> 0);
    SPI_SET_SS0_HIGH(W25QXX_SPI);
    w25qxx_WaitForWriteEnd();
}

uint8_t w25qxx_IsEmpty(uint32_t StartAddress, uint32_t Count) {
    uint32_t i;
    uint8_t temp;
    SPI_SET_SS0_LOW(W25QXX_SPI);
    w25qxx_write(0x0B);
    w25qxx_write((StartAddress & 0x00FF0000) >> 16);
    w25qxx_write((StartAddress & 0x0000FF00) >> 8);
    w25qxx_write((StartAddress & 0x000000FF) >> 0);
    w25qxx_dummy();
    for(i = StartAddress; i <= StartAddress + Count; i++) {
        temp = w25qxx_read();
        if(temp != 0xFF) {
            SPI_SET_SS0_HIGH(W25QXX_SPI);
            return 0;
        }
    }
    SPI_SET_SS0_HIGH(W25QXX_SPI);
    return 1;
}

void w25qxx_WriteData(uint8_t dataBuf[], uint32_t writeCount, uint32_t startAddr) {
    uint32_t i, wtCnt, bufSum = 0, stAddr = startAddr;
    if(writeCount == 0) return;
    do {
        wtCnt = w25qxx.PageSize - (stAddr % w25qxx.PageSize);
        wtCnt = (writeCount < wtCnt) ? writeCount : wtCnt;
#ifdef W25QXX_DEBUG
        {
            printf("Start Write \n");
            printf("Addr : 0x%08X\n", stAddr);
            printf("wtCnt : %d\n", wtCnt);
            printf("bufSum : %d\n", bufSum);
        }
#endif
        w25qxx_WriteEnable();
        SPI_SET_SS0_LOW(W25QXX_SPI);
        w25qxx_write(0x02);
        w25qxx_write((stAddr & 0x00FF0000) >> 16);
        w25qxx_write((stAddr & 0x0000FF00) >> 8);
        w25qxx_write((stAddr & 0x000000FF) >> 0);
        for(i = bufSum; i < (bufSum + wtCnt); i++)
            w25qxx_write(dataBuf[i]);
        SPI_SET_SS0_HIGH(W25QXX_SPI);
        w25qxx_WaitForWriteEnd();
        bufSum += wtCnt;
        stAddr += wtCnt;
        writeCount -= wtCnt;
    } while(writeCount > 0);
}
void w25qxx_ReadData(uint8_t dataBuf[], uint32_t readCount, uint32_t startAddr) {
    uint32_t i;
    if(readCount == 0) return;
    SPI_SET_SS0_LOW(W25QXX_SPI);
    w25qxx_write(0x0B);
    w25qxx_write((startAddr & 0x00FF0000) >> 16);
    w25qxx_write((startAddr & 0x0000FF00) >> 8);
    w25qxx_write((startAddr & 0x000000FF) >> 0);
    w25qxx_dummy();
    for(i = 0; i < readCount; i++) {
        dataBuf[i] = w25qxx_read();
    }
    SPI_SET_SS0_HIGH(W25QXX_SPI);
}
void w25qxx_CopeData(uint32_t sourceAddr, uint32_t destinationAddr, uint32_t dataCount) {
    uint8_t *dataTemp;
    uint32_t A_Addr = sourceAddr;
    uint32_t B_Addr = destinationAddr;
    uint32_t wtCnt;
    if(dataCount == 0)return;
    dataTemp = malloc(w25qxx.PageSize * sizeof(uint8_t));
    do {
        wtCnt = (dataCount > w25qxx.PageSize) ? w25qxx.PageSize : dataCount;
        wtCnt = ((w25qxx.PageSize - (B_Addr % w25qxx.PageSize)) < wtCnt) ? (w25qxx.PageSize - (B_Addr % w25qxx.PageSize)) : wtCnt;
        w25qxx_ReadData(dataTemp, wtCnt, A_Addr);
        w25qxx_WriteData(dataTemp, wtCnt, B_Addr);
        A_Addr += wtCnt;
        B_Addr += wtCnt;
        dataCount -= wtCnt;
    } while(dataCount > 0);
    free(dataTemp);
}
void w25qxx_EraseData(uint32_t startAddr, uint32_t Count) {
    uint32_t erAddr = startAddr, F_Cnt, L_Cnt, E_Cnt, base;
    do {
        if(!(erAddr % w25qxx.BlockSize) && (Count >= w25qxx.BlockSize)) {
            w25qxx_EraseFullBlock(erAddr);
#ifdef W25QXX_DEBUG
            {
                printf("Start Erase Full block\n");
                printf("erAddr : 0x%08X   Count : %d\n", erAddr, Count);
            }
#endif
            Count -= w25qxx.BlockSize;
            erAddr += w25qxx.BlockSize;
        } else if(!(erAddr % (w25qxx.BlockSize / 2)) && (Count >= (w25qxx.BlockSize / 2))) {
            w25qxx_EraseHalfBlock(erAddr);
#ifdef W25QXX_DEBUG
            {
                printf("Start Erase Half block\n");
                printf("erAddr : 0x%08X   Count : %d\n", erAddr, Count);
            }
#endif
            Count -= (w25qxx.BlockSize / 2);
            erAddr += (w25qxx.BlockSize / 2);
        } else if(!(erAddr % w25qxx.SectorSize) && (Count >= w25qxx.SectorSize)) {
            w25qxx_EraseSector(erAddr);
#ifdef W25QXX_DEBUG
            {
                printf("Start Erase Sector\n");
                printf("erAddr : 0x%08X   Count : %d\n", erAddr, Count);
            }
#endif
            Count -= w25qxx.SectorSize;
            erAddr += w25qxx.SectorSize;
        } else {
            F_Cnt = erAddr % w25qxx.SectorSize;
            E_Cnt = (Count < (w25qxx.SectorSize - F_Cnt)) ? Count : (w25qxx.SectorSize - F_Cnt);
            L_Cnt = w25qxx.SectorSize - (F_Cnt + E_Cnt);
            base = erAddr - F_Cnt;
#ifdef W25QXX_DEBUG
            {
                printf("Start Erase Half Sector\n");
                printf("erAddr : 0x%08X   Count : %d\n", erAddr, Count);
                printf("F_Cnt : %d\n", F_Cnt);
                printf("E_Cnt : %d\n", E_Cnt);
                printf("L_Cnt : %d\n", L_Cnt);
                printf("base : 0x%08X\n", base);
            }
#endif
            w25qxx_EraseSector(W25QXX_TEMP_SECTOR_ADDR);
            w25qxx_CopeData(base, W25QXX_TEMP_SECTOR_ADDR, F_Cnt);
            w25qxx_CopeData(base + F_Cnt + E_Cnt, W25QXX_TEMP_SECTOR_ADDR + F_Cnt + E_Cnt, L_Cnt);
            w25qxx_EraseSector(erAddr);
            w25qxx_CopeData(W25QXX_TEMP_SECTOR_ADDR, base, F_Cnt);
            w25qxx_CopeData(W25QXX_TEMP_SECTOR_ADDR + F_Cnt + E_Cnt, base + F_Cnt + E_Cnt, L_Cnt);
            erAddr += E_Cnt;
            Count -= E_Cnt;
        }
    } while(Count > 0);
}

uint8_t w25qxx_INIT(void) {
    uint32_t ID = w25qxx_ReadID();
    switch(ID & 0x0000FFFF) {
    case 0x401A:    //  w25q512
        w25qxx.ID = W25Q512;
        w25qxx.BlockCount = 1024;
#ifdef W25QXX_DEBUG
        printf("w25qxx Chip: w25q512\r\n");
#endif
        break;
    case 0x4019:    //  w25q256
        w25qxx.ID = W25Q256;
        w25qxx.BlockCount = 512;
#ifdef W25QXX_DEBUG
        printf("w25qxx Chip: w25q256\r\n");
#endif
        break;
    case 0x4018:    //  w25q128
        w25qxx.ID = W25Q128;
        w25qxx.BlockCount = 256;
#ifdef W25QXX_DEBUG
        printf("w25qxx Chip: w25q128\r\n");
#endif
        break;
    case 0x4017:    //  w25q64
        w25qxx.ID = W25Q64;
        w25qxx.BlockCount = 128;
#ifdef W25QXX_DEBUG
        printf("w25qxx Chip: w25q64\r\n");
#endif
        break;
    case 0x4016:    //  w25q32
        w25qxx.ID = W25Q32;
        w25qxx.BlockCount = 64;
#ifdef W25QXX_DEBUG
        printf("w25qxx Chip: w25q32\r\n");
#endif
        break;
    case 0x4015:    //  w25q16
        w25qxx.ID = W25Q16;
        w25qxx.BlockCount = 32;
#ifdef W25QXX_DEBUG
        printf("w25qxx Chip: w25q16\r\n");
#endif
        break;
    case 0x4014:    //  w25q80
        w25qxx.ID = W25Q80;
        w25qxx.BlockCount = 16;
#ifdef W25QXX_DEBUG
        printf("w25qxx Chip: w25q80\r\n");
#endif
        break;
    case 0x4013:    //  w25q40
        w25qxx.ID = W25Q40;
        w25qxx.BlockCount = 8;
#ifdef W25QXX_DEBUG
        printf("w25qxx Chip: w25q40\r\n");
#endif
        break;
    case 0x4012:    //  w25q20
        w25qxx.ID = W25Q20;
        w25qxx.BlockCount = 4;
#ifdef W25QXX_DEBUG
        printf("w25qxx Chip: w25q20\r\n");
#endif
        break;
    case 0x4011:    //  w25q10
        w25qxx.ID = W25Q10;
        w25qxx.BlockCount = 2;
#ifdef W25QXX_DEBUG
        printf("w25qxx Chip: w25q10\r\n");
#endif
        break;
    default:
#ifdef W25QXX_DEBUG
        printf("w25qxx Unknown ID\r\n");
#endif
				return 0;
    }

    w25qxx.PageSize     = 0x00000100;
    w25qxx.SectorSize = 0x00001000;
    w25qxx.BlockSize    = 0x00010000;
    w25qxx.CapacityInKiloByte = (w25qxx.BlockSize * w25qxx.BlockCount) / 1024;
    w25qxx_ReadUniqID();
    w25qxx_ReadStatusRegister(1);
    w25qxx_ReadStatusRegister(2);
    w25qxx_ReadStatusRegister(3);
#ifdef W25QXX_DEBUG
    printf("ID : %X\n", ID);
    printf("Capacity %d\n", w25qxx.CapacityInKiloByte);
#endif
    return 1;
}
