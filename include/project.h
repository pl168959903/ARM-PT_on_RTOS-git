
#ifndef __USER_LIB_H__
#define __USER_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif
/*---------------------------------------------------------------------------------------------------------*/
#define _GET_GPIO_PORT( pin ) ( GPIO_T * )( ( ( uint32_t )&pin ) & 0xFFFFFDC0 )
#define _GET_GPIO_PIN( pin )  ( ( ( ( uint32_t )&pin ) & 0x0000003F ) >> 2 )
#define _MASK( n )            ( 0x1u << n )

/*---------------------------------------------------------------------------------------------------------*/
// Pin define
#define _LED_PA0             PA0
#define _NULL_PA2            PA2
#define _NULL_PA3            PA3
#define _NULL_PA4            PA4
#define _NULL_PA5            PA5
#define _NULL_PA6            PA6
#define _NULL_PA8            PA8
#define _NULL_PA9            PA9
#define _EXT_INT_PA14        PA14
#define _NRF_CE_PA15         PA15
#define _GPS_UART0_RX_PB0    PB0
#define _GPS_UART0_TX_PB1    PB1
#define _CAMERA_N_EN_PB2     PB2
#define _NRF_IRQ_PB3         PB3
#define _CAMERA_UART1_RX_PB4 PB4
#define _CAMERA_UART1_TX_PB5 PB5
#define _NRF_SPI0_SS_PC0     PC0
#define _NRF_SPI0_SCLK_PC1   PC1
#define _NRF_SPI0_MISO_PC2   PC2
#define _NRF_SPI0_MOSI_PC3   PC3
#define _ICE_DAT_PF0         PF0
#define _ICE_CLK_PF1         PF1
#define _NULL_PF2            PF2
#define _NULL_PF3            PF3
#define _X32O_PF6            PF6
#define _X32I_PF7            PF7
/*---------------------------------------------------------------------------------------------------------*/
// UART
#define _DEBUG_UART UART0

#define _GPS_UART             UART0
#define _CAMERA_UART          UART1
#define _CAMERA_UART_BAUDRATE 115200
#define _GPS_UART_BAUDRATE    9600
#define _GPS_SCAN_TIMES 30;
/*---------------------------------------------------------------------------------------------------------*/
// SPI
#define _NRF_SPI     SPI0
#define _NRF_SPI_CLK 1000000
/*---------------------------------------------------------------------------------------------------------*/
void MCU_PinInit( void );
void MCU_GpioInit( void );
void MCU_ClkInit( void );
void MCU_UartInit( void );

/*------------------------------------------------Config---------------------------------------------------*/
#define USER_CFG_DEBUG_MODE     1
#define USER_CFG_COMERA_EN      1
#define USER_CFG_NRF24L01_EN    1
#define USER_CFG_NRF24L01_0_SPI SPI0
#define USER_CFG_W25XX_EN       1
#define USER_CFG_W25XX_0_SPI    SPI3

/*------------------------------------------------PinDefine-----------------------------------------------*/
#define USER_PIN_LED          PA0
#define USER_PIN_BUTTON       PA2
#define USER_PIN_EXT          PA14
#define USER_PIN_NRF_CE       PA15
#define USER_PIN_NRF_IRQ      PB3
#define USER_PIN_CAMERA_POWER PB2

/*------------------------------------------------Function-------------------------------------------------*/
#define USER_ENABLE_CMAERA_POWER()  USER_PIN_CAMERA_POWER = 0
#define USER_DISABLE_CMAERA_POWER() USER_PIN_CAMERA_POWER = 1
#define USER_ENABLE_LED()           USER_PIN_LED = 0
#define USER_DISABLE_LED()          USER_PIN_LED = 1
#define USER_TRAN_LED()             USER_PIN_LED = USER_PIN_LED ^ 1
#define USER_ENABLE_NRF_CE()        USER_PIN_NRF_CE = 1
#define USER_DISABLE_NRF_CE()       USER_PIN_NRF_CE = 0

/*------------------------------------------------Include--------------------------------------------------*/
// FreeRTOS include.

/*---------------------------------------------------------------------------------------------------------*/
// default.
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/*---------------------------------------------------------------------------------------------------------*/
// Official.
#include "Nano103.h"

/*---------------------------------------------------------------------------------------------------------*/
// User.

#include "Bridging.h"
#include "CJ_OV528.h"
#include "FIFO.h"
#include "NRF24L01.h"
#include "NRF_P2P.h"
#include "W25QXX.h"
#include "vMemAlloc.h"

/*---------------------------------------------------------------------------------------------------------*/
extern S_RTC_TIME_DATA_T g_stRtcTime;
extern volatile uint8_t  g_u8RtcTickFlag;

extern OV528_T *g_stOv528_s0;

extern NRF_T *           g_stNrf0;
extern NRFP2P_CHANNEL_T *g_stNrfCh1;
extern FIFO_T *          g_stNrfRx;

extern const uint8_t g_u8P1Address[ 5 ];
extern const uint8_t g_u8DestAddress[ 5 ];

extern FIFO_T *g_stGpsRx;
extern uint8_t g_gpsData[100];
extern uint8_t g_gpsDataSize;

extern volatile uint8_t  g_u8GetPictureReadyFlag;
extern volatile uint32_t g_u32GetPictureTime;
extern volatile uint32_t g_u32GetPictureCount;
extern volatile uint8_t  g_u8GpsDataReadyFlag;
extern volatile uint8_t  g_u8GpsScanFlag;
extern volatile uint8_t  g_u8ExternalTriggerFlag;
extern volatile uint8_t  g_u8WdtReloadFlag;

// protable.
extern SPI_Func_T  g_stSpi0;
extern SPI_Func_T  g_stSpi3;
extern UART_Func_T g_stUart1;

/*---------------------------------------------------------------------------------------------------------*/
void MCU_SavePinState( void );
void MCU_PowerDown( void );
void MCU_PowerDownRestore( void );
/*---------------------------------------------------------------------------------------------------------*/
// Initialization
void MCU_PinInit( void );
void MCU_ClkInit( void );
void MCU_GpioInit( void );
void MCU_NvicInit( void );
void MCU_UartInit( void );
void MCU_SpiInit( void );
void MCU_RtcInit( void );
void MCU_WdtInit( void );

void MOD_CameraInit( void );
void MOD_NrfInit( void );
void MOD_GpsInit( void );

/*---------------------------------------------------------------------------------------------------------*/
void CameraGetImage( void );
bool NrfSendData( uint8_t *data, uint32_t length );
void DelayUs( uint32_t );
void CmdDecoder( void );
void CmdSend( uint8_t length );

/*---------------------------------------------------------------------------------------------------------*/
// portable

void SetCE( void );
void ResetCE( void );
void NrfDelay( uint32_t time );

void CameraDelay( uint32_t time );
void printReg( void );

#endif  //__USER_LIB_H__
