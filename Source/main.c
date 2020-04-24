
#include "project.h"

void CmdDecoder( void );
void CmdSend( uint8_t length );


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
    GpsSetup();
    CameraSetup();
    SYS_LockReg();

    while ( 1 ) {
			if(g_u8GetPictureReadyFlag == 1){
				CameraGetImage();
			}
           CmdDecoder();
           if(g_u8RtcTickFlag == 1){
                printf( "CNT:%d GPS:%d CYC:%d GPR:%d\n", g_u32GetPictureCount, g_u8GpsDataReadyFlag, g_u32GetPictureTime, g_u8GetPictureReadyFlag );
                g_u8RtcTickFlag = 0;
           }
    };
}


void CmdDecoder( void ) {
    if ( FIFO_IsEmpty( g_stNrfRx ) == false ) {
        uint32_t c_time = 0;
        while ( c_time++ < 500 ) {
            if ( FIFO_CmdCheck( g_stNrfRx, ( uint8_t* )"CMD:", 0, 4, 4, 0, false ) ) {
                if ( FIFO_CmdCheck( g_stNrfRx, ( uint8_t* )"GET:", 4, 8, 4, 0, false ) ) {
                    if ( FIFO_CmdCheck( g_stNrfRx, ( uint8_t* )"IMG", 8, 11, 3, 0, false ) ) {
                        if ( FIFO_WaitData( g_stNrfRx, 11, 0 ) ) {
                            g_u8GetPictureReadyFlag = 1;
                        }
                    }
                }
                if ( FIFO_CmdCheck( g_stNrfRx, ( uint8_t* )"SET:", 4, 8, 4, 0, false ) ) {
                    if ( FIFO_CmdCheck( g_stNrfRx, ( uint8_t* )"CYC:", 8, 12, 4, 0, false ) ) {
                         if ( FIFO_WaitData( g_stNrfRx, 16, 0 ) ) {
                             g_u32GetPictureTime = (uint32_t)(FIFO_ReadData( g_stNrfRx, 12) - 0x30) * 1000;
                             g_u32GetPictureTime += (uint32_t)(FIFO_ReadData( g_stNrfRx, 13) - 0x30) * 100;
                             g_u32GetPictureTime += (uint32_t)(FIFO_ReadData( g_stNrfRx, 14) - 0x30) * 10;
                             g_u32GetPictureTime += (uint32_t)(FIFO_ReadData( g_stNrfRx, 15) - 0x30) * 1;
                             g_u32GetPictureCount = 0;
                        }
                    }
                }
            }
            DelayUs( 1000 );
        }
        FIFO_Rst( g_stNrfRx );
    }
}

void CmdSend( uint8_t length ) {
    uint8_t f;
    FIFO_WaitData( g_stNrfRx, length, 0 );
    NRF_TxMode( g_stNrf0 );
    f = nrfP2P_SendPacket( g_stNrf0, ( uint8_t* )g_u8DestAddress, g_stNrfRx->buf, length );
    NRF_RxMode( g_stNrf0 );
    FIFO_Rst( g_stNrfRx );
    printf( "SYS:" );
    if ( f == true )
        printf( "OK\n" );
    else
        printf( "TOUT\n" );
}

