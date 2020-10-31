
#include "project.h"

void RstWwdt( void );
void GpsScan( void );
void GpsDataSend(void);

uint8_t gpsTestData[] = "$GPRMC,121252.000,A,3958.3032,N,11629.6046,E,15.15,359.95,070306,,,A*54\r\n";

int main( void ) {
    // Boot 設定
    SYS_UnlockReg();
    MCU_NvicInit();
    MCU_PinInit();
    MCU_ClkInit();
    MCU_WdtInit();
    MCU_RtcInit();
    MCU_GpioInit();
    MCU_UartInit();
    MCU_SpiInit();

    MOD_CameraInit();
    MOD_NrfInit();

    MOD_GpsInit();

    printf( "Start\n" );

    while ( 1 ) {
        RstWwdt();

        if ( g_u8GpsScanFlag == 1 ) { GpsScan(); }

        if ( g_u8GetPictureReadyFlag == 1 ) { CameraGetImage(); }
        CmdDecoder();
        if ( g_u32GetPictureTime > 0 ) {
            NRF_PowerDown( g_stNrf0 );
            g_stNrf0->ResetCE();
            MCU_PowerDown();
        }
    };
}
void WDT_IRQHandler( void ) {
    if ( WWDT_GET_INT_FLAG() ) {
        g_u8WdtReloadFlag = 1;
        WWDT_CLEAR_INT_FLAG();
    }
    if ( WWDT_GET_RESET_FLAG() ) {
        WWDT_RELOAD_COUNTER();
        printf( "WWDT rt\n" );
        WWDT_CLEAR_RESET_FLAG();
    }
}
void PDWU_IRQHandler( void ) {
    MCU_PowerDownRestore();
    NRF_PowerOn( g_stNrf0 );
    printf( "power on\n" );
    CLK->WKINTSTS = 0x01;
}

void GpsScan( void ) {
    uint32_t i, dotSum;
    uint32_t catchTimes = _GPS_SCAN_TIMES;

    g_u8GpsScanFlag = 0;
    g_gpsDataSize   = 0;  // clean gps data size

    // Gps power on
    USER_ENABLE_CMAERA_POWER();
    g_stOv528_s0->Delay( 3000 );  // Wait GPS power stable

    while ( catchTimes-- != 0 ) {
        i                    = 0;  // vis buf
        dotSum               = 0;  // dot counter
        g_u8GpsDataReadyFlag = 0;  // rst gps old data
        while ( g_u8GpsDataReadyFlag == 0 ) {};
        GpsDataSend();

        // view second dot data;
        while ( dotSum < 2 ) {
            if ( g_stGpsRx->buf[ i++ ] == ',' ) dotSum++;
            if ( i >= g_stGpsRx->size ) {
                printf( "GPS Scan Error.\n" );
                USER_DISABLE_CMAERA_POWER();
                return;
            }
        }

        // check data
        if ( g_stGpsRx->buf[ i ] == 'V' ) {
            printf( "GPS NULL.\n" );  //
        }
        else if ( g_stGpsRx->buf[ i ] == 'A' ) {

            //copy gps data.
            g_gpsDataSize = g_stGpsRx->size;
            memcpy( g_gpsData, g_stGpsRx->buf, g_stGpsRx->size );

            USER_DISABLE_CMAERA_POWER();
            printf( "GPS Scan successful.\n" );
            return;
        }
        else {
            printf( "GPS Scan Error.\n" );
            USER_DISABLE_CMAERA_POWER();
            return;
        }
        RstWwdt();
    }

    USER_DISABLE_CMAERA_POWER();
}

void RstWwdt( void ) {
    while ( g_u8WdtReloadFlag == 0 ) {};
    WWDT_RELOAD_COUNTER();
    printf( "wwdt reload\n" );
    g_u8WdtReloadFlag = 0;
}

void GpsDataSend(void){
    NRF_TxMode( g_stNrf0 );
    NRF_FlushTx( g_stNrf0 );
    DelayUs( 10000 );
    NrfSendData( g_stGpsRx->buf, g_stGpsRx->size );
    NRF_RxMode( g_stNrf0 );
}
