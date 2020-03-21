#include "Nano103.h"
#include "USER_LIB.h"
#include "stdio.h"
#include "vMemAlloc.h"
#include <stdlib.h>
#define SUM 10
#define ALLOCRAND 300
#define TEST_TIMES 100000

uint32_t i;

int main( void ) {
    uint32_t i;
    FIFO_T*  f1;
    uint8_t  temp;
    uint8_t  data[] = "zxcvbnm";
    SYS_UnlockReg();
    PinSetup();
    ClkSetup();
    UartSetup();
    f1 = FIFO_New( 100, NULL );
    vMemInfoPrint();
    for ( i = 0; i < 50; i++ ) {
        if ( FIFO_ByteIn( f1, ( uint8_t* )&i ) ) {
            printf( "size:%d ;head:%d ;tail:%d ;eff:%d\n", f1->size, f1->head, f1->tail, f1->effSize );
        }
        else {
            printf( "size:%d ;head:%d ;tail:%d ;eff:%d..err\n", f1->size, f1->head, f1->tail, f1->effSize );
        }
    }
    // FIFO_ByteOut(f1,&temp);
    if ( FIFO_WaitData( f1, 50, 100 ) ) {
        printf( "wait done\n" );
    }
    FIFO_Rst( f1 );
    printf( "size:%d ;head:%d ;tail:%d ;eff:%d\n", f1->size, f1->head, f1->tail, f1->effSize );
    for ( i = 0; i < 7; i++ ) {
        FIFO_ByteIn( f1, data + i );
        printf( "i:%d = %c\n", i, FIFO_ReadData( f1, i ) );
    }
    if ( FIFO_CmdCheck( f1, data, 0, 10, 100 ) ) {
        printf( "check done\n" );
    }
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
