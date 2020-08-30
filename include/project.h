
#ifndef __USER_LIB_H__
#define __USER_LIB_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------Config---------------------------------------------------*/
#define USER_CFG_DEBUG_MODE 1
#define USER_CFG_COMERA_EN 1
#define USER_CFG_NRF24L01_EN 1
#define USER_CFG_NRF24L01_0_SPI SPI0
#define USER_CFG_W25XX_EN 1
#define USER_CFG_W25XX_0_SPI SPI3

/*------------------------------------------------PinDefine-----------------------------------------------*/
#define USER_PIN_LED PA0
#define USER_PIN_BUTTON PA2
#define USER_PIN_EXT PA14
#define USER_PIN_NRF_CE PA15
#define USER_PIN_NRF_IRQ PB3
#define USER_PIN_CAMERA_POWER PB2

/*------------------------------------------------Function-------------------------------------------------*/
#define USER_ENABLE_CMAERA_POWER() USER_PIN_CAMERA_POWER = 0
#define USER_DISABLE_CMAERA_POWER() USER_PIN_CAMERA_POWER = 1
#define USER_ENABLE_LED() USER_PIN_LED = 0
#define USER_DISABLE_LED() USER_PIN_LED = 1
#define USER_TRAN_LED() USER_PIN_LED = USER_PIN_LED ^ 1
#define USER_ENABLE_NRF_CE() USER_PIN_NRF_CE = 1
#define USER_DISABLE_NRF_CE() USER_PIN_NRF_CE = 0

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
    extern volatile uint8_t g_u8RtcTickFlag;

    extern OV528_T *g_stOv528_s0;

    extern NRF_T *g_stNrf0;
    extern NRFP2P_CHANNEL_T *g_stNrfCh1;
    extern FIFO_T *g_stNrfRx;

    extern const uint8_t g_u8P1Address[5];
    extern const uint8_t g_u8DestAddress[5];

    extern FIFO_T *g_stGpsRx;

    extern volatile uint8_t g_u8GetPictureReadyFlag;
    extern volatile uint32_t g_u32GetPictureTime;
    extern volatile uint32_t g_u32GetPictureCount;
    extern volatile uint8_t g_u8GpsDataReadyFlag;
		extern volatile uint8_t g_u8ExternalTriggerFlag;
    extern volatile uint8_t g_u8WdtReloadEnable;

    // protable.
    extern SPI_Func_T g_stSpi0;
    extern SPI_Func_T g_stSpi3;
    extern UART_Func_T g_stUart1;

    /*---------------------------------------------------------------------------------------------------------*/
    void DebugPortEnable(void);

    void WdtSetup(void);
    void PinSetup(void);
    void ClkSetup(void);
    void GpioSetup(void);
    void UartSetup(void);
    void SpiSetup(void);
    void RtcSetup(void);

    void CameraSetup(void);
    void NrfSetup(void);
    void GpsSetup(void);

    bool NrfSendData(uint8_t *data, uint32_t length);
    void DelayUs(uint32_t);

    void SetCE(void);
    void ResetCE(void);
    void NrfDelay(uint32_t time);

    void CameraGetImage(void);

    void CmdDecoder(void);
    void CmdSend(uint8_t length);

    /*---------------------------------------------------------------------------------------------------------*/
    // portable
    void CameraDelay(uint32_t time);
    void printReg(void);

    void WdtAutoReloadDisable(uint32_t wtis);
    void WdtAutoReloadEnable(void);

#endif //__USER_LIB_H__
