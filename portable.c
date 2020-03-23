#include "Nano103.h"
#include "USER_LIB.h"
#include <stdbool.h>

//Camera
uint32_t CameraUartWrite(uint8_t * array, uint32_t size){
    return UART_Write(UART1, array, size);
}

uint32_t CameraUartRead(uint8_t *array, uint32_t size){
    uint32_t receive;
    receive = UART_Read(UART1, array, 1);
    FIFO_ByteIn(g_stOv528_s0->fifoBuf, array);
    return receive;
}

void CameraDelay(uint32_t time){
    DelayUs(1000 * time);
}


//External Flash
uint8_t ExtFlashSpiWrite(uint8_t data) {
    SPI_WRITE_TX0(USER_CFG_W25XX_0_SPI, data);
    SPI_TRIGGER(USER_CFG_W25XX_0_SPI);
    while(SPI_IS_BUSY(USER_CFG_W25XX_0_SPI));
    return ((uint8_t)SPI_READ_RX0(USER_CFG_W25XX_0_SPI));
}

uint8_t ExtFlashSpiRead(void) {
    SPI_TRIGGER(USER_CFG_W25XX_0_SPI);
    while(SPI_IS_BUSY(USER_CFG_W25XX_0_SPI));
    return ((uint8_t)SPI_READ_RX0(USER_CFG_W25XX_0_SPI));
}

void ExtFlashSpiDummy(void) {
    SPI_TRIGGER(USER_CFG_W25XX_0_SPI);
    while(SPI_IS_BUSY(USER_CFG_W25XX_0_SPI));
}

void ExtFlashSpiSelect(void){
    SPI_SET_SS0_LOW( USER_CFG_W25XX_0_SPI );
}

void ExtFlashSpiDeselect(void){
    SPI_SET_SS0_HIGH( USER_CFG_W25XX_0_SPI );
}
