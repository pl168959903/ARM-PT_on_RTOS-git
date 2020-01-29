#include <Nano103.h>
#include <USER_LIB.h>
#include <stdio.h>


volatile uint8_t NRF_Status_Reg;
volatile uint8_t NRF_Int_Flag = 0;

uint8_t rx_p0_add[5] = {0x44, 0x43, 0x10, 0x10, 0x01};
uint8_t tx_p0_add[5] = {0x44, 0x43, 0x10, 0x10, 0x01};

//---------------------------------------------------------------
void NRF_Reg_Read_Byte(uint8_t reg, uint8_t *data) {
    SPI_SET_SS0_LOW(SPI0);

    SPI_WRITE_TX0(SPI0, reg);
    SPI_TRIGGER(SPI0);
    while(SPI_IS_BUSY(SPI0));
    NRF_Status_Reg = SPI_READ_RX0(SPI0);

    SPI_TRIGGER(SPI0);
    while(SPI_IS_BUSY(SPI0));
    *data = SPI_READ_RX0(SPI0);

    SPI_SET_SS0_HIGH(SPI0);
}
void NRF_Reg_Read_Arg(uint8_t reg, uint8_t size, uint8_t data[]) {
    uint8_t i;
    SPI_SET_SS0_LOW(SPI0);

    SPI_WRITE_TX0(SPI0, reg);
    SPI_TRIGGER(SPI0);
    while(SPI_IS_BUSY(SPI0));
    NRF_Status_Reg = SPI_READ_RX0(SPI0);
    for(i = 0; i < size; i++) {
        SPI_TRIGGER(SPI0);
        while(SPI_IS_BUSY(SPI0));
        data[i] = SPI_READ_RX0(SPI0);
    }
    SPI_SET_SS0_HIGH(SPI0);
}
void NRF_Reg_Write_Byte(uint8_t reg, uint8_t data) {
    SPI_SET_SS0_LOW(SPI0);

    SPI_WRITE_TX0(SPI0, 0x20 | reg);
    SPI_TRIGGER(SPI0);
    while(SPI_IS_BUSY(SPI0));
    NRF_Status_Reg = SPI_READ_RX0(SPI0);

    SPI_WRITE_TX0(SPI0, data);
    SPI_TRIGGER(SPI0);
    while(SPI_IS_BUSY(SPI0));

    SPI_SET_SS0_HIGH(SPI0);
}
void NRF_Reg_Write_Arg(uint8_t reg, uint8_t size, uint8_t data[]) {
    uint8_t i;
    SPI_SET_SS0_LOW(SPI0);

    SPI_WRITE_TX0(SPI0, 0x20 | reg);
    SPI_TRIGGER(SPI0);
    while(SPI_IS_BUSY(SPI0));
    NRF_Status_Reg = SPI_READ_RX0(SPI0);
    for(i = 0; i < size; i++) {
        SPI_WRITE_TX0(SPI0, data[i]);
        SPI_TRIGGER(SPI0);
        while(SPI_IS_BUSY(SPI0));
    }
    SPI_SET_SS0_HIGH(SPI0);
}

//---------------------------------------------------------------

uint8_t NRF_Reset_IRQ(void) {
    uint8_t st;
    NRF_Reg_Read_Byte(NRF_REG_STATUS, &st);
    NRF_Reg_Write_Byte(NRF_REG_STATUS, st);
    return st;
}
void NRF_Clr_FiFo(void) {
    NRF_Reg_Write_Byte(NRF_CMD_CLR_RX_FIFO, 0xFF);
    NRF_Reg_Write_Byte(NRF_CMD_CLR_TX_FIFO, 0xFF);
}

void NRF_Tx_Mode(void) {
    NRF_CE_PIN = 1;
    NRF_Reset_IRQ();
    NRF_Clr_FiFo();
    NRF_Reg_Write_Arg(NRF_REG_TX_ADDR, 5, tx_p0_add);
    NRF_Reg_Write_Arg(NRF_REG_ADDR_P0, 5, rx_p0_add);
    NRF_Reg_Write_Byte(NRF_REG_AUTO_ACK, 0x01); //0x01
    NRF_Reg_Write_Byte(NRF_REG_EN_ADDR, 0x01);
    NRF_Reg_Write_Byte(NRF_REG_RESEND, 0x1A); //0x1A
    NRF_Reg_Write_Byte(NRF_REG_RF_CH, 0x70);
    NRF_Reg_Write_Byte(NRF_REG_RF_CONFIG, 0x0F);
    NRF_Reg_Write_Byte(NRF_REG_CONFIG, 0x0E);
    NRF_CE_PIN = 0;
}
void NRF_Rx_Mode(void) {
    NRF_CE_PIN = 0;
    NRF_Reset_IRQ();
    NRF_Clr_FiFo();
    NRF_Reg_Write_Arg(NRF_REG_ADDR_P0, 5, rx_p0_add);
    NRF_Reg_Write_Byte(NRF_REG_AUTO_ACK, 0x01); //0x01
    NRF_Reg_Write_Byte(NRF_REG_EN_ADDR, 0x01);
    NRF_Reg_Write_Byte(NRF_REG_RX_PW_P0, RX_PLOAD_WIDTH);
    NRF_Reg_Write_Byte(NRF_REG_RF_CH, 0x70);
    NRF_Reg_Write_Byte(NRF_REG_RF_CONFIG, 0x0F);
    NRF_Reg_Write_Byte(NRF_REG_CONFIG, 0x0F);
    NRF_CE_PIN = 1;
}
uint8_t NRF_Tx_Package(uint8_t mode, uint8_t * buf, uint8_t data_PW) {
    volatile uint8_t st = 0;
    NRF_CE_PIN = 1;
    NRF_Int_Flag = 0;
    NRF_Reg_Write_Arg(NRF_CMD_TX_PATLOAD,  data_PW, buf);
    while(!NRF_Int_Flag) {};
    if(mode == NRF_MODE_1) {
        NRF_Reg_Write_Byte(NRF_CMD_CLR_TX_FIFO, 0xFF);
    } else if(mode == NRF_MODE_2) {
        do {
            st = NRF_Reset_IRQ();
        } while(!(st & TX_OK));
    }
		NRF_CE_PIN = 0;
    return st;
}
uint8_t NRF_Rx_package(uint8_t * buf) {
    volatile uint8_t st;
    st = NRF_Reset_IRQ();
    if(st & RX_OK) {
        NRF_Reg_Read_Arg(NRF_CMD_RX_PAYLOAD,  RX_PLOAD_WIDTH, buf);
        NRF_Reg_Write_Byte(NRF_CMD_CLR_RX_FIFO,  0xFF);
        return 1;
    }
    return 0;
}
