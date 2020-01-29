
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

/*------------------------------------------------Extern---------------------------------------------------*/
extern S_RTC_TIME_DATA_T RTC_TIME;
extern uint8_t g_button_flag;
extern uint8_t g_ext_int_flag;
extern uint8_t g_nrf_irq_flag;

/*------------------------------------------------Extern---------------------------------------------------*/
void PIN_SETUP(void);
void CLK_SETUP(void);
void GPIO_SETUP(void);
void UART_SETUP(void);
void SPI_SETUP(void);
void TIMER_SETUP(void);
void RTC_SETUP(void);
void delay_us(uint32_t);
void Array_assign(uint8_t[], uint32_t, ...);
/*------------------------------------------------Include--------------------------------------------------*/
// FreeRTOS include.
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

// User.
#include "rtos_task.h"
#include "FIFO.h"
#include "Flash.h"
#include "IRQ.h"
#include "NRF24L01.h"
#include "OV2640.h"
#include "W25QXX.h"

/*---------------------------------------------------------------------------------------------------------*/


#endif  //__USER_LIB_H__
