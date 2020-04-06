
#include "project.h"
#define SUM 10
#define ALLOCRAND 300
#define TEST_TIMES 100000

int main( void ) {
    uint32_t i;
    
    SYS_UnlockReg();
    PinSetup();
    ClkSetup();
    UartSetup();
    GpioSetup();
    SpiSetup();
    NVIC_Init();
    CameraSetup();
	SYS_LockReg();
    USER_ENABLE_NRF_CE();
    g_stNrf0 = NRF_New(&g_stSpi0);
    for(i = 0; i < 18; i++){
        uint8_t reg;
        reg = NRF_ReadRegByte(g_stNrf0, i);
        printf( "reg %d : 0x%X\n", i, reg);
    }
    nrfP2p_InitNrf(g_stNrf0, 0x11);
     for(i = 0; i < 18; i++){
        uint8_t reg;
        reg = NRF_ReadRegByte(g_stNrf0, i);
        printf( "reg %d : 0x%X\n", i, reg);
    }
    while ( 1 ) {};
    RtcSetup();

    
    TimerSetup();
    FifoSetup();
    NVIC_Init();
    
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
