
#ifndef __USER_LIB_H__
#define __USER_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Nano103.h"

/*------------------------------------------------Typedef--------------------------------------------------*/
typedef void(FUNC_PTR)(void);

/*------------------------------------------------Config---------------------------------------------------*/
#define USER_CFG_DEBUG_MODE            1
#define USER_CFG_COMERA_EN             1
#define USER_CFG_NRF24L01_EN           1
#define USER_CFG_W25XX_EN              1

/*------------------------------------------------PinDefine-----------------------------------------------*/
#define USER_PIN_LED            PA0
#define USER_PIN_BUTTON         PA2
#define USER_PIN_EXT            PA14
#define USER_PIN_NRF_CE         PA15
#define USER_PIN_NRF_IRQ        PB3
#define USER_PIN_CAMERA_POWER   PB2

/*------------------------------------------------Function-------------------------------------------------*/
#define USER_ENABLE_CMAERA_POWER()      USER_PIN_CAMERA_POWER = 0
#define USER_DISABLE_CMAERA_POWER()     USER_PIN_CAMERA_POWER = 1
#define USER_ENABLE_LED()               USER_PIN_LED = 0 
#define USER_DISABLE_LED()              USER_PIN_LED = 1
#define USER_TRAN_LED()                 USER_PIN_LED = USER_PIN_LED ^ 1
#define USER_ENABLE_NRF_CE()            USER_PIN_NRF_CE = 1
#define USER_DISABLE_NRF_CE()           USER_PIN_NRF_CE = 0

/*------------------------------------------------Include--------------------------------------------------*/
// FreeRTOS include.
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

/*---------------------------------------------------------------------------------------------------------*/
// User.
#include "rtos_task.h"
#include "FIFO.h"
#include "Flash.h"
#include "IRQ.h"
#include "NRF24L01.h"
#include "CJ_OV528.h"
#include "W25QXX.h"


/*---------------------------------------------------------------------------------------------------------*/
extern S_RTC_TIME_DATA_T g_stRtcTime;
extern OV528_T g_stOv528_s0;
extern FIFO_T g_stUart1_buf;
extern uint8_t g_u8ButtonInterruptFlag;
extern uint8_t g_u8ExtInterruptFlag;
extern uint8_t g_u8NrfInterruptFlag;

/*---------------------------------------------------------------------------------------------------------*/

void PinSetup( void );
void ClkSetup(void);
void GpioSetup(void);
void UartSetup(void);
void SpiSetup(void);
void TimerSetup(void);
void RtcSetup(void);
void FifoSetup( void );
void CameraSetup(void);
void DelayUs(uint32_t);

/*---------------------------------------------------------------------------------------------------------*/
//portable
void CameraUartWrite(uint8_t * arr, uint32_t size);
void CameraDelay(uint32_t time);



#endif  //__USER_LIB_H__
