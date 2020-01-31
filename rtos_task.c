#include "Nano103.h"
#include "USER_LIB.h"
#include <stdio.h>

// Task Handle
TaskHandle_t taskLEDControl_handler;
TaskHandle_t taskBootCreate_handler;

void taskBootCreate( void ) {

#ifdef USER_CFG_DEBUG_MODE
    printf( "taskBootCreate start!\n" );
#endif  // USER_CFG_DEBUG_MODE

    taskENTER_CRITICAL();
    xTaskCreate( ( TaskFunction_t )taskLEDControl, ( const char* )"LED_ctrl", ( uint16_t )50, ( void* )NULL, ( UBaseType_t )3, ( TaskHandle_t* )&taskLEDControl_handler );
    vTaskDelete( taskBootCreate_handler );
    taskEXIT_CRITICAL();
}

void taskLEDControl( void ) {

#ifdef USER_CFG_DEBUG_MODE
    printf( "taskLEDControl start!\n" );
#endif  // USER_CFG_DEBUG_MODE
    while ( 1 ) {
        USER_TRAN_LED();
        vTaskDelay( 500 );
    }
}
