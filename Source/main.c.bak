
#include "project.h"

int main( void ) {
    //Boot 設定
    SYS_UnlockReg();
    PinSetup();
    ClkSetup();
    UartSetup();
    RtcSetup();
    TimerSetup();
    GpioSetup();
    SpiSetup();
    NVIC_Init();
    NrfSetup();
    GpsSetup();
    CameraSetup();
    SYS_LockReg();

    while ( 1 ) {
        if ( g_u8GetPictureReadyFlag == 1 ) {
            CameraGetImage();
        }
        CmdDecoder();
        if ( g_u8RtcTickFlag == 1 ) {
            printf( "CNT:%d GPS:%d CYC:%d GPR:%d\n", g_u32GetPictureCount, g_u8GpsDataReadyFlag, g_u32GetPictureTime, g_u8GetPictureReadyFlag );
            g_u8RtcTickFlag = 0;
        }
    };
    return  0;
}


