/*
    __             _____   __________
   / /_  __  __   /__  /  / ____/ __ \
  / __ \/ / / /     / /  / / __/ /_/ /
 / /_/ / /_/ /     / /__/ /_/ / _, _/
/_.___/\__, /     /____/\____/_/ |_|
      /____/
*/
/*---------------------------------------------------------------------------------------------------------*/
// Include.
#include "Project.h"

/*---------------------------------------------------------------------------------------------------------*/
// 全域變數

// RTC
S_RTC_TIME_DATA_T g_stRtcTime = { 2020, 1, 1, 0, 0, 0, RTC_SUNDAY, RTC_CLOCK_24 };  // RTC

// Camera
OV528_T* g_stOv528_s0;  // 相機實體

// NRF
NRF_T*            g_stNrf0;                                                 // NRF實體
NRFP2P_CHANNEL_T* g_stNrfCh1;                                               // NRF 通道實體
FIFO_T*           g_stNrfRx;                                                // NRF RX FIFO實體
const uint8_t     g_u8P1Address[ 5 ]   = { 0x01, 0x79, 0x02, 0x08, 0x11 };  // Channel 1 地址
const uint8_t     g_u8DestAddress[ 5 ] = { 0x00, 0x79, 0x02, 0x08, 0x11 };  // 目標地址

// GPS
FIFO_T* g_stGpsRx;  // GPS 實體

//旗標
volatile uint8_t  g_u8RtcTickFlag         = 0; //RTC Tick 中斷旗標
volatile uint8_t  g_u8GetPictureReadyFlag = 0; //拍攝圖片準備旗標
volatile uint8_t  g_u8GpsDataReadyFlag    = 0; //讀取GPS準備旗標
volatile uint8_t  g_u8ExternalTriggerFlag = 0; //拍攝圖片準備旗標

// 參數
volatile uint32_t g_u32GetPictureTime     = 0; // 時間
volatile uint32_t g_u32GetPictureCount    = 0; 

volatile uint8_t g_u8WdtReloadEnable    = 1; 

/*---------------------------------------------------------------------------------------------------------*/

void DebugPortEnable(void){
    SYS->GPB_MFPL |= ( SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD ); //UART0
    CLK_EnableXtalRC(CLK_PWRCTL_HIRC0EN_Msk);
    while(!CLK_WaitClockReady(CLK_STATUS_HIRC0STB_Msk)){};
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_UART0_CLK_DIVIDER(1));
    UART_Open(UART0, 38400);
    printf("Debug Enable!\n");
    printf("HIRC : %d Hz\n", CLK_GetHCLKFreq());
}
void WdtSetup( void ){
    CLK_EnableModuleClock(WDT_MODULE);
    CLK_SetModuleClock(WDT_MODULE, CLK_CLKSEL1_WDTSEL_LXT, NULL);
    WDT_Open(WDT_TIMEOUT_2POW18, WDT_RESET_DELAY_1026CLK, TRUE, TRUE);
    WDT_EnableInt();
    NVIC_EnableIRQ(WDT_IRQn);
    printf("WDT Setting ok\n");
}
void PinSetup( void ) {
    // XTAL
    SYS->GPF_MFPL |= ( SYS_GPF_MFPL_PF7MFP_X32_IN | SYS_GPF_MFPL_PF6MFP_X32_OUT );

    // UART
    SYS->GPB_MFPL |= ( SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD );  // UART0 : GPS
    SYS->GPB_MFPL |= ( SYS_GPB_MFPL_PB4MFP_UART1_RXD | SYS_GPB_MFPL_PB5MFP_UART1_TXD );  // UART1 : CAMERA

    // SPI
    SYS->GPC_MFPL |= ( SYS_GPC_MFPL_PC0MFP_SPI0_SS0 | SYS_GPC_MFPL_PC1MFP_SPI0_CLK | SYS_GPC_MFPL_PC2MFP_SPI0_MISO0 | SYS_GPC_MFPL_PC3MFP_SPI0_MOSI0 );  // SPI0 : NRF24L01P
    SYS->GPA_MFPL |= ( SYS_GPA_MFPL_PA3MFP_SPI3_MOSI0 | SYS_GPA_MFPL_PA4MFP_SPI3_MISO0 | SYS_GPA_MFPL_PA5MFP_SPI3_CLK | SYS_GPA_MFPL_PA6MFP_SPI3_SS0 );  // SPI3 : FLASH

    // GPIO
    SYS->GPA_MFPL |= ( SYS_GPA_MFPL_PA0MFP_GPIO | SYS_GPA_MFPL_PA2MFP_GPIO );    // PA0 : LED ; PA2 : BUTTON
    SYS->GPA_MFPH |= ( SYS_GPA_MFPH_PA14MFP_GPIO | SYS_GPA_MFPH_PA15MFP_GPIO );  // PA14 : DI ; PA15 : nRF_CE
    SYS->GPB_MFPL |= ( SYS_GPB_MFPL_PB2MFP_GPIO | SYS_GPB_MFPL_PB3MFP_GPIO );    // PB2 : Camera_Power ; PB3 : nRF_IRQ

    printf("Pin setting ok!\n");
}
void ClkSetup( void ) {
    CLK_SysTickDelay(10000);
    // SYSTEM CLOCK
    CLK_EnableXtalRC( CLK_PWRCTL_LXTEN_Msk );
    while ( !CLK_WaitClockReady( CLK_STATUS_LXTSTB_Msk ) ) {};
    CLK_EnableXtalRC(CLK_PWRCTL_HIRC1EN_Msk);
    while(!CLK_WaitClockReady(CLK_STATUS_HIRC1STB_Msk)){};

    SYS->IRC1TCTL |= ( SYS_IRCTCTL_LOOP_32CLK | SYS_IRCTCTL_RETRY_512 );
    SYS_EnableHIRC1Trim( SYS_IRC1TCTL_TRIM_36M, ( SYS_IRCTIEN_FAIL_EN | SYS_IRCTIEN_32KERR_EN ) );
    while ( !( SYS_GET_IRC1TRIM_INT_FLAG() & SYS_IRCTISTS_FREQLOCK ) ){};

    CLK->CLKSEL0 |= CLK_CLKSEL0_HIRCSEL_Msk;
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC1, CLK_HCLK_CLK_DIVIDER(1));
    CLK_SetPCLK0(CLK_APB0DIV_HCLK);
    CLK_SetPCLK1(CLK_APB1DIV_HCLK);

    CLK_DisableXtalRC(CLK_PWRCTL_HIRC0EN_Msk);

    UART_Open(UART0, 38400);

    CLK->CLKDCTL |= 0x2;
    CLK->CLKDIE |= 0x2;

    NVIC_EnableIRQ( CKSD_IRQn );
    NVIC_EnableIRQ( HIRC_IRQn );

    printf( "--------------------------------\n" );
    printf( "Clock config : \n");
    printf( "HXT : %d Hz\n", CLK_GetHXTFreq() );
    printf( "LXT : %d Hz\n", CLK_GetLXTFreq() );
    printf( "HIRC0 : %d Hz\n", CLK_GetHIRC0Freq());
    printf( "HIRC1 : %d Hz\n", CLK_GetHIRC1Freq());
    printf( "HIRC : %d Hz\n", CLK_GetHIRCFreq());
    printf( "MIRC : %d Hz\n", CLK_GetMIRCFreq());
    printf( "LIRC : %d Hz\n", CLK_GetLIRCFreq());
    printf( "CPU : %d Hz\n", CLK_GetCPUFreq() );
    printf( "HCLK : %d Hz\n", CLK_GetHCLKFreq() );
    printf( "PCLK0 : %d Hz\n", CLK_GetPCLK0Freq() );
    printf( "PCLK1 : %d Hz\n", CLK_GetPCLK1Freq() );
    printf( "PLL : %d Hz\n", CLK_GetPLLClockFreq() );
}
void GpioSetup( void ) {
    // MODE.
    GPIO_SetMode( PA, BIT0, GPIO_PMD_OUTPUT ); //LED
    GPIO_SetMode( PA, BIT2, GPIO_PMD_INPUT ); //Button
    GPIO_SetMode( PA, BIT14, GPIO_PMD_INPUT ); 
    GPIO_SetMode( PA, BIT15, GPIO_PMD_OUTPUT );
    GPIO_SetMode( PB, BIT2, GPIO_PMD_OUTPUT );
    GPIO_SetMode( PB, BIT3, GPIO_PMD_INPUT );

    // PULL UP.
    GPIO_ENABLE_PULL_UP( PA, BIT2 );
    GPIO_ENABLE_PULL_UP( PA, BIT14 );
    GPIO_ENABLE_PULL_UP( PB, BIT3 );

    GPIO_ENABLE_PULL_UP( PB, BIT0 );
    GPIO_ENABLE_PULL_UP( PB, BIT1 );
    GPIO_ENABLE_PULL_UP( PB, BIT4 );
    GPIO_ENABLE_PULL_UP( PB, BIT5 );

    // Interrupt.
    GPIO_EnableInt( PA, 2, GPIO_INT_FALLING );
    GPIO_EnableInt( PA, 14, GPIO_INT_FALLING );
    GPIO_EnableInt( PB, 3, GPIO_INT_FALLING );
    NVIC_EnableIRQ( GPABC_IRQn );

    // DEBOUNCE.
    GPIO_SET_DEBOUNCE_TIME( GPIO_DBCLKSRC_HCLK, GPIO_DBCLKSEL_32768 );
    GPIO_ENABLE_DEBOUNCE( PA, BIT2 );

    // INIT pin status.
    PA0  = 1;
    PA15 = 0;
    PB2  = 1;

    printf("GPIO Setting ok\n");
}
void UartSetup( void ) {
    // UART
    CLK_SetModuleClock( UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_UART0_CLK_DIVIDER( 1 ) ); //GPS
    CLK_SetModuleClock( UART1_MODULE, CLK_CLKSEL2_UART1SEL_HIRC, CLK_UART1_CLK_DIVIDER( 1 ) ); //CAMERA
    
    CLK_EnableModuleClock( UART0_MODULE );
    CLK_EnableModuleClock( UART1_MODULE );

    UART_Open( UART0, 38400 );
    UART_Open( UART1, 115200 );

    UART_SET_RX_FIFO_INTTRGLV( UART0, UART_LINE_RFITL_14BYTES );
    UART_SET_RX_FIFO_INTTRGLV( UART1, UART_LINE_RFITL_14BYTES );

    UART_SetTimeoutCnt( UART0, 0xFF );
    UART_SetTimeoutCnt( UART1, 0xFF );

    // Interrupt
    UART_EnableInt( UART0, UART_INTEN_RXTOIEN_Msk | UART_INTEN_RDAIEN_Msk );
    UART_EnableInt( UART1, UART_INTEN_RXTOIEN_Msk | UART_INTEN_RDAIEN_Msk );
    NVIC_EnableIRQ( UART0_IRQn );
    NVIC_EnableIRQ( UART1_IRQn );
    //NVIC_SetPriority( UART1_IRQn, 0 );

    printf( "UART Setting ok\n");
}
void SpiSetup( void ) {
    // SPI
    CLK_SetModuleClock( SPI0_MODULE, CLK_CLKSEL1_SPI0SEL_HIRC, 0 ); //NRF24L01
    CLK_SetModuleClock( SPI3_MODULE, CLK_CLKSEL2_SPI3SEL_HIRC, 0 ); //W25Q128

    CLK_EnableModuleClock( SPI0_MODULE );
    CLK_EnableModuleClock( SPI3_MODULE );

    SPI_Open( SPI0, SPI_MASTER, SPI_MODE_0, 8, 1000000 );
    SPI_Open( SPI3, SPI_MASTER, SPI_MODE_0, 8, 1000000 );
    printf("SPI Setting ok\n");
}
void RtcSetup( void ) {
    CLK_EnableModuleClock( RTC_MODULE );
    RTC_Open( &g_stRtcTime );
    RTC_SetTickPeriod( RTC_TICK_1_SEC );
    RTC_EnableInt( RTC_INTEN_TICKIEN_Msk );
    NVIC_EnableIRQ( RTC_IRQn );
    printf("RTC Setting ok\n");
}
void CameraSetup( void ) {
    g_stOv528_s0 = OV528_New( 0x0001, 50, &g_stUart1, CameraDelay );
    USER_DISABLE_CMAERA_POWER();
    printf("OV528 Setting ok\n");
}
void NrfSetup( void ) {
    g_stNrfRx = FIFO_New( 1024, NULL );
    g_stNrf0  = NRF_New( &g_stSpi0, &SetCE, &ResetCE, &NrfDelay );
    nrfP2P_InitNrf( g_stNrf0, 0x50 );
    g_stNrfCh1 = nrfP2P_NewChannel( g_stNrf0, 1, ( uint8_t* )g_u8P1Address, true, true );
    nrfP2P_EnableTxDypw( g_stNrf0 );
    nrfP2P_EnableTxAutoAck( g_stNrf0, ( uint8_t* )g_u8DestAddress );
    NRF_RxMode( g_stNrf0 );
    printf("nRF24L01 Setting ok\n");
}
void GpsSetup( void ) {
    g_stGpsRx = FIFO_New( 200, NULL );
    printf("GPS Setting ok\n");
}
void CameraGetImage( void ) {
    size_t  i;
    uint8_t check;
    uint8_t dataTemp[ 50 ];
		uint8_t imgConfig[8] = {0x5E, 0xDB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A};
    // NRF TX MODE;
    NRF_TxMode( g_stNrf0 );
    NRF_FlushTx( g_stNrf0 );
    
    // Enable Camera Power
    USER_ENABLE_CMAERA_POWER();
    g_stOv528_s0->Delay( 2000 );

    FIFO_Rst( g_stOv528_s0->fifoBuf );
    OV528_SNYC( g_stOv528_s0 );
    OV528_Init( g_stOv528_s0, OV528_INIT_JPEG, OV528_INIT_PR_160_120, OV528_INIT_JPEG_640_480 );
    OV528_Snapshout( g_stOv528_s0, OV528_SNAPSHOT_COMPRESSED, 0x0000 );
    OV528_SetPacketSize( g_stOv528_s0, 32 + 6 );
    OV528_GetPictue( g_stOv528_s0, OV528_GET_PICTURE_SNAPSHOT );
		
		for(i=0;i<4;i++) imgConfig[i+2] = *(((uint8_t*)&g_stOv528_s0->imageSize) + i);
		imgConfig[6] |= ( (g_u8GpsDataReadyFlag << 0) | (g_u8ExternalTriggerFlag << 1) );
		check = NrfSendData( imgConfig, 8 );
		g_u8ExternalTriggerFlag = 0;
		
		if(check == true){
			for ( i = 0; i < g_stOv528_s0->imagePacket; i++ ) {
        size_t size;
        size = OV528_GetPacket( g_stOv528_s0, i, dataTemp );
        
        check = NrfSendData(dataTemp, size);
        if(check == false){
            break;
        }
        printf( "i = %d  %d	%d\n", i, check, size );
			}
		}
    if(check == true){
        if ( g_u8GpsDataReadyFlag == 1) {
            check = NrfSendData( g_stGpsRx->buf, g_stGpsRx->size );
            FIFO_Rst( g_stGpsRx );
            g_u8GpsDataReadyFlag = 0;
        }
    }
		/*
    if(check == true){
        check = NrfSendData(temp, 2 );
    }
		*/
    if(check == false){
        printf("Send data err\n");
    }

    // PowerDone
    USER_DISABLE_CMAERA_POWER();
    g_stOv528_s0->Delay( 2000 );

    NRF_RxMode( g_stNrf0 );
    NRF_FlushRx( g_stNrf0 );
    FIFO_Rst( g_stNrfRx );

    g_u8GetPictureReadyFlag = 0;
}
bool NrfSendData( uint8_t* data, uint32_t length ) {
    uint32_t f = 0;
    while ( length > 0 ) {
        uint8_t c;
        uint8_t len = ( length > 32 ) ? 32 : length;
        c           = nrfP2P_SendPacket( g_stNrf0, ( uint8_t* )g_u8DestAddress, data + f, len );
        if ( c == false ){
            uint8_t tr;
            tr = nrfP2P_TxReuse( g_stNrf0, 300 );
            if(tr == false){
                NRF_RstIrq(g_stNrf0);
                return false; 
            }
        }
        f += len;
        length -= len;
    }
    return true;
}
void DelayUs( uint32_t delayTime ) {
    uint32_t i;
    for(i=0; i<(delayTime>>16); i++) CLK_SysTickDelay(0x10000);
    if(!(delayTime & 0xFFFF)) CLK_SysTickDelay(delayTime & 0xFFFF);
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
                            FIFO_Rst( g_stNrfRx );
                        }
                    }
                    if ( FIFO_CmdCheck( g_stNrfRx, ( uint8_t* )"GPS", 8, 11, 3, 0, false ) ) {
                        if ( g_u8GpsDataReadyFlag == 1 ) {
                            NRF_TxMode( g_stNrf0 );
                            NRF_FlushTx( g_stNrf0 );
                            DelayUs( 10000 );
                            NrfSendData( g_stGpsRx->buf, g_stGpsRx->size );
                            NRF_RxMode( g_stNrf0 );
                            FIFO_Rst( g_stNrfRx );
                        }
                    }
                }
                if ( FIFO_CmdCheck( g_stNrfRx, ( uint8_t* )"SET:", 4, 8, 4, 0, false ) ) {
                    if ( FIFO_CmdCheck( g_stNrfRx, ( uint8_t* )"CYC:", 8, 12, 4, 0, false ) ) {
                        if ( FIFO_WaitData( g_stNrfRx, 16, 0 ) ) {
                            g_u32GetPictureTime = ( uint32_t )( FIFO_ReadData( g_stNrfRx, 12 ) - 0x30 ) * 1000;
                            g_u32GetPictureTime += ( uint32_t )( FIFO_ReadData( g_stNrfRx, 13 ) - 0x30 ) * 100;
                            g_u32GetPictureTime += ( uint32_t )( FIFO_ReadData( g_stNrfRx, 14 ) - 0x30 ) * 10;
                            g_u32GetPictureTime += ( uint32_t )( FIFO_ReadData( g_stNrfRx, 15 ) - 0x30 ) * 1;
                            g_u32GetPictureCount = 0;
                            FIFO_Rst( g_stNrfRx );
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

void WdtAutoReloadDisable( uint32_t wtis){
    g_u8WdtReloadEnable = 0;
    SYS_UnlockReg();
    WDT_RESET_COUNTER();
    WDT->CTL = (WDT->CTL & ~WDT_CTL_WTIS_Msk) | ((wtis << WDT_CTL_WTIS_Pos) & WDT_CTL_WTIS_Msk);
    SYS_LockReg();
}

void WdtAutoReloadEnable( void ){
    g_u8WdtReloadEnable = 1;
}

