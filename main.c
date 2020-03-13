#include "Nano103.h"
#include "USER_LIB.h"
#include "stdio.h"
#include "vMemAlloc.h"
#include <stdlib.h>
#define SUM 10
#define ALLOCRAND 300
#define TEST_TIMES 100000
uint8_t* ptest[ SUM ];
size_t   xtest[ SUM ];
size_t   xtestSize[ SUM ];
size_t   c_err = 0;
uint8_t* kk;

int main( void ) {
    uint32_t i;
    SYS_UnlockReg();
    PinSetup();
    ClkSetup();
    UartSetup();

    for ( i = 0; i < 1; i++ ) {

        size_t r         = ( rand() % SUM );
        size_t  allocSize = (rand() % ALLOCRAND) + 1;
        //printf( "%04d . ", i );
        if ( xtest[ r ] == 0 ) {
            ptest[ r ] = ( uint8_t* )vMemAlloc( allocSize );
            if ( ptest[ r ] != NULL ) {
                //printf( "[%02d]Alloc : %d\n", r, allocSize );
                xtestSize[ r ] = allocSize;
                xtest[ r ]     = 1;
            }
            else {
                //printf( "[%02d]Alloc : %d ; Error\n", r, allocSize );
                c_err++;
            }
            
        }
        else {
            //printf( "[%02d]Free\n", r );
            vMemFree( ptest[ r ] );
            xtestSize[ r ] = 0;
            xtest[ r ]     = 0;
        }
         //vMemInfoPrint();
    }
    //for ( i = 0; i < SUM; i++ ) { printf( "p[%0d] Address : 0x%X ; Size : %d ; IsAlloc : %d\n", i, ( size_t )ptest[ i ], xtestSize[ i ], xtest[ i ] ); }
    printf( "Test times : %d	Error : %d(%0.1f%%)\n",TEST_TIMES, c_err, ((float)c_err / TEST_TIMES) * 100.0 );
		vMemInfoPrint();
    kk = vMemAlloc(3);
    printf("kk size : %d\n", vMemSizeOf(kk));
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
