#include "Nano103.h"
#include "USER_LIB.h"
#include "stdio.h"
#include <stdlib.h>

 uint8_t temp[100];
 uint8_t ttemp[10] = "1234567890";

int main( void ) {
    uint8_t* a;
    uint8_t  i;
    SYS_UnlockReg();
    PinSetup();
    ClkSetup();
    UartSetup();

    printf( "Start!!\n" );
    a = ( uint8_t* )malloc( sizeof( uint8_t ) * 512 );
    if ( a == NULL )
        printf( "ERR A\n" );
    else {
        for ( i = 0; i < 100; i++ ) a[ i ] = i;
        for ( i = 0; i < 100; i++ ) printf( "data : %d\n", a[ i ] );
    }
		for ( i = 0; i < 100; i++ ) temp[ i ] = i;
    for ( i = 0; i < 100; i++ ) printf( "temp_data : %d\n", temp[ i ] );
		printf("%s\n",ttemp);

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
