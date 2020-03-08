#include "Nano103.h"
#include "USER_LIB.h"
#include "stdio.h"
#include "vMemAlloc.h"
#include <stdlib.h>

 uint8_t* temp[100];

int main( void ) {
    SYS_UnlockReg();
    PinSetup();
    ClkSetup();
    UartSetup();
	
		temp[0] = (uint8_t*)vMemAlloc(10);
		vMemInfoPrint();
		temp[1] = (uint8_t*)vMemAlloc(20);
		vMemInfoPrint();
		temp[2] = (uint8_t*)vMemAlloc(30);
		vMemInfoPrint();
		temp[3] = (uint8_t*)vMemAlloc(40);
		vMemInfoPrint();
		temp[4] = (uint8_t*)vMemAlloc(50);
		vMemInfoPrint();
		vMemFree(temp[1]);
		vMemInfoPrint();
		vMemFree(temp[3]);
		vMemInfoPrint();
		vMemFree(temp[2]);
		vMemInfoPrint();
    while ( 1 ) {};
    RtcSetup();
    GpioSetup();
    SpiSetup();
    TimerSetup();
    FifoSetup();
    NVIC_Init();
    SYS_LockReg();
    while ( 1 ) {
        // printf("%d\n", a++);
    };
    /*
xTaskCreate(
                ( TaskFunction_t )  taskBootCreate,
                ( const char* )     "boot",
                ( uint16_t )        128,
                ( void* )           NULL,
                ( UBaseType_t )     1,
                ( TaskHandle_t* )   &taskBootCreate_handler
            );
vTaskStartScheduler();
    */
}
