
#include "project.h"

int main( void ) {
    SYS_UnlockReg();
    PinSetup();
    ClkSetup();
    UartSetup();

#ifdef USER_CFG_DEBUG_MODE
    printf( "CPU Freq   : %dHz\n", CLK_GetCPUFreq() );
    printf( "HCLK Freq  : %dHz\n", CLK_GetHCLKFreq() );
    printf( "HXT Freq   : %dHz\n", CLK_GetHXTFreq() );
    printf( "LXT Freq   : %dHz\n", CLK_GetLXTFreq() );
    printf( "PCLK0 Freq : %dHz\n", CLK_GetPCLK0Freq() );
    printf( "PCLK1 Freq : %dHz\n", CLK_GetPCLK1Freq() );
    printf( "PLL Freq   : %dHz\n", CLK_GetPLLClockFreq() );
#endif  // USER_CFG_DEBUG_MODE

    RtcSetup();
    TimerSetup();
    GpioSetup();
    SpiSetup();
    NVIC_Init();
    NrfSetup();
    //CameraSetup();
    SYS_LockReg();
    while ( 1 ) {};
}
