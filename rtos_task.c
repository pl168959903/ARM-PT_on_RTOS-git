#include "Nano103.h"
#include "USER_LIB.h"
#include <stdio.h>

// Task Handle
TaskHandle_t taskBootCreate_handler;

void taskBootCreate( void ) {
    taskENTER_CRITICAL();
    vTaskDelete( taskBootCreate_handler );
    taskEXIT_CRITICAL();
}

void taskLEDControl( void ) {
    while ( 1 ) {
        USER_TRAN_LED();
        vTaskDelay( 500 );
    }
}


void 