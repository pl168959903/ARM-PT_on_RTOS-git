#include "Nano103.h"
#include "USER_LIB.h"
#include <stdio.h>

uint32_t taskSoftCount = 0;

// Task Handle
TaskHandle_t taskBootCreate_handler;
TaskHandle_t taskLEDControl_handler;
TaskHandle_t taskOV528Open_handler;
TaskHandle_t taskSoftTimerCounter_handler;

void taskBootCreate( void ) {
    taskENTER_CRITICAL();
    xTaskCreate(    
                    ( TaskFunction_t )  taskLEDControl, 
                    ( const char* )     "task0", 
                    ( uint16_t )        128, 
                    ( void* )           NULL, 
                    ( UBaseType_t )     3, 
                    ( TaskHandle_t* )   &taskLEDControl_handler 
                );
                
    xTaskCreate(    
                    ( TaskFunction_t )  taskOV528Open, 
                    ( const char* )     "task1", 
                    ( uint16_t )        128, 
                    ( void* )           NULL, 
                    ( UBaseType_t )     2, 
                    ( TaskHandle_t* )   &taskOV528Open_handler 
                );
    
    xTaskCreate(    
                    ( TaskFunction_t )  taskSoftTimerCounter, 
                    ( const char* )     "task2", 
                    ( uint16_t )        128, 
                    ( void* )           NULL, 
                    ( UBaseType_t )     1, 
                    ( TaskHandle_t* )   &taskSoftTimerCounter_handler 
                );
    vTaskDelete( taskBootCreate_handler );
    taskEXIT_CRITICAL();
}

void taskLEDControl( void ) {
    while ( 1 ) {
        USER_TRAN_LED();
        vTaskDelay( 500 );
    }
}

void taskOV528Open( void ) {
    CameraOpen();
    vTaskDelete( taskOV528Open_handler );
}

void taskSoftTimerCounter( void ) {
    while ( 1 ) {
        printf( "count : %d\n", taskSoftCount );
        vTaskDelay( 500 );
        taskSoftCount++;
    }
}
