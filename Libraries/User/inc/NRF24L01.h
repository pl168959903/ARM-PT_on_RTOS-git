

#ifndef __NRF24L01_H__
#define __NRF24L01_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "bridging.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define NRF_USE_VMEMALLOC 1

#if NRF_USE_VMEMALLOC
#include "vMemAlloc.h"
#define NRF_MALLOC( size ) vMemAlloc( size )
#define NRF_FREE( ptr ) vMemFree( ptr )
#else
#define NRF_MALLOC( size ) malloc( size )
#define NRF_FREE( ptr ) free( ptr )
#endif  // NRF_USE_VMEMALLOC

extern volatile uint8_t NRF_Status_Reg;
extern volatile uint8_t NRF_Int_Flag;

//-------------------------------------------------
// NRF CMD
#define NRF_CMD_READ_REG 0x00
#define NRF_CMD_WRITE_REG 0x20
#define NRF_CMD_RX_PAYLOAD 0x61
#define NRF_CMD_TX_PATLOAD 0xA0
#define NRF_CMD_FLUSH_TX 0xE1
#define NRF_CMD_FLUSH_RX 0xE2
#define NRF_CMD_REUSE_TX 0xE3
#define NRF_CMD_NOP 0xFF
//-------------------------------------------------

//-------------------------------------------------
// NRF REG
#define NRF_REG_CONFIG 0x00

#define NRF_REG_CFG_MAKE_RX_RD_POS (6)
#define NRF_REG_CFG_MAKE_RX_RD_MSK (1U<<NRF_REG_CFG_MAKE_RX_RD_POS)
#define NRF_REG_CFG_MAKE_TX_RS_POS (5)
#define NRF_REG_CFG_MAKE_TX_RS_MSK (1U<<NRF_REG_CFG_MAKE_TX_RS_POS)
#define NRF_REG_CFG_MAKE_MAX_RT_POS (4)
#define NRF_REG_CFG_MAKE_MAX_RT_MSK (1U<<NRF_REG_CFG_MAKE_MAX_RT_POS)
#define NRF_REG_CFG_EN_CRC_POS (3)
#define NRF_REG_CFG_EN_CRC_MSK (1U<<NRF_REG_CFG_EN_CRC_POS)
#define NRF_REG_CFG_CRCO_POS (2)
#define NRF_REG_CFG_CRCO_MSK (1U<<NRF_REG_CFG_CRCO_POS)
#define NRF_REG_CFG_CRCO_POS (2)
#define NRF_REG_CFG_CRCO_MSK (1U<<NRF_REG_CFG_CRCO_POS)


#define NRF_REG_AUTO_ACK 0x01
#define NRF_REG_EN_ADDR 0x02
#define NRF_REG_SET_ADDR_WIDTH 0x03
#define NRF_REG_RESEND 0x04
#define NRF_REG_RF_CH 0x05
#define NRF_REG_RF_CONFIG 0x06
#define NRF_REG_STATUS 0x07
#define NRF_REG_TX_CHECK 0x08
#define NRF_REG_CD 0x09
#define NRF_REG_ADDR_P0 0x0A
#define NRF_REG_ADDR_P1 0x0B
#define NRF_REG_ADDR_P2 0x0C
#define NRF_REG_ADDR_P3 0x0D
#define NRF_REG_ADDR_P4 0x0E
#define NRF_REG_ADDR_P5 0x0F
#define NRF_REG_TX_ADDR 0x10
#define NRF_REG_RX_PW_P0 0x11
#define NRF_REG_RX_PW_P1 0x12
#define NRF_REG_RX_PW_P2 0x13
#define NRF_REG_RX_PW_P3 0x14
#define NRF_REG_RX_PW_P4 0x15
#define NRF_REG_RX_PW_P5 0x16
#define NRF_REG_FIFO_STATUS 0x17
//-------------------------------------------------

#define NRF_CE_PIN PA15
#define NRF_IRQ_PIN PA14
#define TX_PLOAD_WIDTH 32
#define RX_PLOAD_WIDTH 32

#define MAX_TX 0x10
#define TX_OK 0x20
#define RX_OK 0x40

#define NRF_MODE_1 0
#define NRF_MODE_2 1

typedef struct {
    uint8_t    statusReg;
    uint8_t    rxPayload;
    uint8_t    txPayload;
    SPI_Func_T *spi;
    void ( *Enable )( void );
    void ( *Disable )( void );
} NRF_T;

void NRF_Reg_Read_Byte( uint8_t, uint8_t* );
void NRF_Reg_Read_Arg( uint8_t, uint8_t, uint8_t[] );
void NRF_Reg_Write_Byte( uint8_t, uint8_t );
void NRF_Reg_Write_Arg( uint8_t, uint8_t, uint8_t[] );

uint8_t NRF_Reset_IRQ( void );
void    NRF_Clr_FiFo( void );

void    NRF_Tx_Mode( void );
void    NRF_Rx_Mode( void );
uint8_t NRF_Tx_Package( uint8_t, uint8_t*, uint8_t );
uint8_t NRF_Rx_package( uint8_t* );

#endif
