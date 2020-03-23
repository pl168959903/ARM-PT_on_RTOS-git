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
    uint8_t  dataIn[] = "123456zxcvbnmABCDEFG";
    uint8_t  dataCheck[] = "zx3vbnm";
    uint8_t  temp;
    SYS_UnlockReg();
    PinSetup();
    ClkSetup();
    UartSetup();
    f1 = FIFO_New( 100, NULL );
    vMemInfoPrint();
    for ( i = 0; i < 97; i++ ) {
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
    // FIFO_Rst( f1 );
    while ( !FIFO_IsEmpty( f1 ) )
        FIFO_ByteOut( f1, &temp ); 
    printf( "size:%d ;head:%d ;tail:%d ;eff:%d\n", f1->size, f1->head, f1->tail, f1->effSize );

    for ( i = 0; i < 20; i++ ) {
        FIFO_ByteIn( f1, dataIn + i );
        printf( "i:%d = %c\n", i, FIFO_ReadData( f1, i ) );
    }
		dataCheck[2] = 0x00;
    if ( FIFO_CmdCheck( f1, dataCheck, 2, 20, 7, 0 , false) ) {
        printf( "check done\n" );
    }
    else
        printf( "err\n" );
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
