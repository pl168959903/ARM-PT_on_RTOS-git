#include "Nano103.h"
#include "USER_LIB.h"
#include "stdio.h"
#include "vMemAlloc.h"
#include <stdlib.h>
#define SUM 10
#define ALLOCRAND 300
#define TEST_TIMES 100000

uint32_t i, j;
uint8_t  dataTemp[ 1000 ];
uint16_t size;

int main( void ) {
    uint32_t i;
    SYS_UnlockReg();
    PinSetup();
    ClkSetup();
    UartSetup();
    GpioSetup();

    g_stUart1_buf = FIFO_New( 1024, NULL );
    //vMemInfoPrint();
    g_stOv528_s0 = OV528_New( 0x0001, g_stUart1_buf, CameraUartWrite, CameraUartRead, CameraDelay );
    //vMemInfoPrint();

    USER_DISABLE_CMAERA_POWER();
    DelayUs( 1000000 );
    USER_ENABLE_CMAERA_POWER();
    DelayUs( 1000000 );
    NVIC_Init();

    if ( OV528_SNYC( g_stOv528_s0 ) ) {
        //printf( "down\n" );
    }
    if ( OV528_Init( g_stOv528_s0, OV528_INIT_JPEG, OV528_INIT_PR_160_120, OV528_INIT_JPEG_640_480 ) ) {
        //printf( "down\n" );
    }
    DelayUs( 1000000 );
    if ( OV528_Snapshout( g_stOv528_s0, OV528_SNAPSHOT_COMPRESSED, 0x0000 ) ) {
       //printf( "down\n" );
    }
    if ( OV528_SetPacketSize( g_stOv528_s0, 1000 ) ) {
        //printf( "down\n" );
    }
    if ( OV528_GetPictue( g_stOv528_s0, OV528_GET_PICTURE_SNAPSHOT ) ) {
        //printf( "down\n" );
    }

    //DelayUs( 1000000 );
    for ( i = 0; i < g_stOv528_s0->imagePacket; i++ ) {
        size = OV528_GetPacket( g_stOv528_s0, i, dataTemp );
        UART_Write( UART0, dataTemp, size );
    }
    while ( 1 ) {};
    RtcSetup();

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
