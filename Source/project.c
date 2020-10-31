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

// 全域變數
/*---------------------------------------------------------------------------------------------------------*/
// RTC
S_RTC_TIME_DATA_T g_stRtcTime      = { 2020, 1, 1, 0, 0, 0, RTC_SUNDAY, RTC_CLOCK_24 };  // RTC
S_RTC_TIME_DATA_T g_stRtcAlarmTime = { 2020, 1, 1, 0, 0, 0, RTC_SUNDAY, RTC_CLOCK_24 };  // RTC Alarm

/*---------------------------------------------------------------------------------------------------------*/
// Camera
OV528_T *g_stOv528_s0;  // Camera module pointer

/*---------------------------------------------------------------------------------------------------------*/
// NRF
NRF_T *           g_stNrf0;                                                 // NRF module pointer
NRFP2P_CHANNEL_T *g_stNrfCh1;                                               // NRF channel pointer
FIFO_T *          g_stNrfRx;                                                // NRF RX Fifo pointer
const uint8_t     g_u8P1Address[ 5 ]   = { 0x01, 0x79, 0x02, 0x08, 0x11 };  // Channl 1 address
const uint8_t     g_u8DestAddress[ 5 ] = { 0x00, 0x79, 0x02, 0x08, 0x11 };  // Dest Address

/*---------------------------------------------------------------------------------------------------------*/
// GPS
FIFO_T *g_stGpsRx;  // GPS module pointer
uint8_t g_gpsData[ 100 ];
uint8_t g_gpsDataSize = 0;

/*---------------------------------------------------------------------------------------------------------*/
//旗標
volatile uint8_t g_u8GetPictureReadyFlag = 0;  //拍攝圖片準備旗標
volatile uint8_t g_u8GpsDataReadyFlag    = 0;  //讀取GPS準備旗標
volatile uint8_t g_u8GpsScanFlag         = 0;
volatile uint8_t g_u8ExternalTriggerFlag = 0;  //外部觸發旗標
volatile uint8_t g_u8WdtReloadFlag       = 0;

// 參數
volatile uint32_t g_u32GetPictureTime  = 0;  // 時間
volatile uint32_t g_u32GetPictureCount = 0;

//上次掉電GPIO設定參數
__IO uint32_t g_u32PinFunction[ 10 ] = { 0 };
__IO uint32_t g_u32PinPullUp[ 6 ]    = { 0 };

/*---------------------------------------------------------------------------------------------------------*/
void MCU_SavePinState( void ) {
    // pin function
    g_u32PinFunction[ 0 ] = SYS->GPA_MFPL;
    g_u32PinFunction[ 1 ] = SYS->GPA_MFPH;
    g_u32PinFunction[ 2 ] = SYS->GPB_MFPL;
    g_u32PinFunction[ 3 ] = SYS->GPB_MFPH;
    g_u32PinFunction[ 4 ] = SYS->GPC_MFPL;
    g_u32PinFunction[ 5 ] = SYS->GPC_MFPH;
    g_u32PinFunction[ 6 ] = SYS->GPD_MFPL;
    g_u32PinFunction[ 7 ] = SYS->GPD_MFPH;
    g_u32PinFunction[ 8 ] = SYS->GPE_MFPL;
    g_u32PinFunction[ 9 ] = SYS->GPF_MFPL;

    g_u32PinPullUp[ 0 ] = PA->PUEN;
    g_u32PinPullUp[ 1 ] = PB->PUEN;
    g_u32PinPullUp[ 2 ] = PC->PUEN;
    g_u32PinPullUp[ 3 ] = PD->PUEN;
    g_u32PinPullUp[ 4 ] = PE->PUEN;
    g_u32PinPullUp[ 5 ] = PF->PUEN;

    printf( "Pin setting has been saved.\n" );
    DelayUs( 100000 );
}
void MCU_PowerDown( void ) {
    MCU_SavePinState();
    SYS->GPA_MFPL = 0;
    SYS->GPA_MFPH = 0;
    SYS->GPB_MFPL = 0;
    SYS->GPB_MFPH = 0;
    SYS->GPC_MFPL = 0;
    SYS->GPC_MFPH = 0;
    SYS->GPD_MFPL = 0;
    SYS->GPD_MFPH = 0;
    SYS->GPE_MFPL = 0;
    SYS->GPF_MFPL = 0xFF0000FF;  // ICE X32

    PA->PUEN = 0x4000;
    PB->PUEN = 0x0004;
    PC->PUEN = 0;
    PD->PUEN = 0;
    PE->PUEN = 0;
    PF->PUEN = 0;

    USER_DISABLE_LED();
    SYS_UnlockReg();
    CLK->PWRCTL |= CLK_PWRCTL_PDWKIEN_Msk;
    SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;
    CLK->PWRCTL |= CLK_PWRCTL_PDEN_Msk;
    __WFI();
    USER_ENABLE_LED();
}
void MCU_PowerDownRestore( void ) {
    // pin function
    SYS->GPA_MFPL = g_u32PinFunction[ 0 ];
    SYS->GPA_MFPH = g_u32PinFunction[ 1 ];
    SYS->GPB_MFPL = g_u32PinFunction[ 2 ];
    SYS->GPB_MFPH = g_u32PinFunction[ 3 ];
    SYS->GPC_MFPL = g_u32PinFunction[ 4 ];
    SYS->GPC_MFPH = g_u32PinFunction[ 5 ];
    SYS->GPD_MFPL = g_u32PinFunction[ 6 ];
    SYS->GPD_MFPH = g_u32PinFunction[ 7 ];
    SYS->GPE_MFPL = g_u32PinFunction[ 8 ];
    SYS->GPF_MFPL = g_u32PinFunction[ 9 ];

    PA->PUEN = g_u32PinPullUp[ 0 ];
    PB->PUEN = g_u32PinPullUp[ 1 ];
    PC->PUEN = g_u32PinPullUp[ 2 ];
    PD->PUEN = g_u32PinPullUp[ 3 ];
    PE->PUEN = g_u32PinPullUp[ 4 ];
    PF->PUEN = g_u32PinPullUp[ 5 ];

    CLK->PWRCTL &= ~CLK_PWRCTL_PDEN_Msk;
    printf( "MCU wake-up!\n" );
}

/*---------------------------------------------------------------------------------------------------------*/
void MCU_WdtInit( void ) {
    // Select clock source.
    CLK_SetModuleClock( WWDT_MODULE, CLK_CLKSEL1_WWDTSEL_HCLKDIV2048, NULL );

    // Enable clock.
    CLK_EnableModuleClock( WWDT_MODULE );

    // Enable module.
    WWDT_Open( WWDT_PRESCALER_2048, 0x3E, TRUE );

    printf( "WWDT has been initialized.\n" );
}
void MCU_PinInit( void ) {
    SYS->GPA_MFPL |=
        ( SYS_GPA_MFPL_PA0MFP_GPIO |  // _LED_PA0
          SYS_GPA_MFPL_PA2MFP_GPIO |  // useless
          SYS_GPA_MFPL_PA3MFP_GPIO |  // useless
          SYS_GPA_MFPL_PA4MFP_GPIO |  // useless
          SYS_GPA_MFPL_PA5MFP_GPIO |  // useless
          SYS_GPA_MFPL_PA6MFP_GPIO    // useless
        );
    SYS->GPA_MFPH |=
        ( SYS_GPA_MFPH_PA8MFP_GPIO |   // useless
          SYS_GPA_MFPH_PA9MFP_GPIO |   // useless
          SYS_GPA_MFPH_PA14MFP_GPIO |  // _EXT_INT_PA14
          SYS_GPA_MFPH_PA15MFP_GPIO    // _NRF_CE_PA15
        );
    SYS->GPB_MFPL |=
        ( SYS_GPB_MFPL_PB0MFP_UART0_RXD |  // _GPS_UART0_RX_PB0
          SYS_GPB_MFPL_PB1MFP_UART0_TXD |  // _GPS_UART0_TX_PB1
          SYS_GPB_MFPL_PB2MFP_GPIO |       // _CAMERA_EN_PB2
          SYS_GPB_MFPL_PB3MFP_GPIO |       // _NRF_IRQ_PB3
          SYS_GPB_MFPL_PB4MFP_UART1_RXD |  // _CAMERA_UART1_RX_PB4
          SYS_GPB_MFPL_PB5MFP_UART1_TXD    // _CAMERA_UART1_TX_PB5
        );
    SYS->GPC_MFPL |=
        ( SYS_GPC_MFPL_PC0MFP_SPI0_SS0 |    // _NRF_SPI0_SS_PC0
          SYS_GPC_MFPL_PC1MFP_SPI0_CLK |    // _NRF_SPI0_SCLK_PC1
          SYS_GPC_MFPL_PC2MFP_SPI0_MISO0 |  // _NRF_SPI0_MISO_PC2
          SYS_GPC_MFPL_PC3MFP_SPI0_MOSI0    // _NRF_SPI0_MOSI_PC3
        );
    SYS->GPF_MFPL = 0;
    SYS->GPF_MFPL |=
        ( SYS_GPF_MFPL_PF0MFP_ICE_DAT |  // _ICE_DAT_PF0
          SYS_GPF_MFPL_PF1MFP_ICE_CLK |  // _ICE_CLK_PF1
          SYS_GPF_MFPL_PF2MFP_GPIO |     // useless
          SYS_GPF_MFPL_PF3MFP_GPIO |     // useless
          SYS_GPF_MFPL_PF6MFP_X32_OUT |  // _X32O_PF6
          SYS_GPF_MFPL_PF7MFP_X32_IN     // _X32I_PF7
        );
    printf( "Pin function has been initialized.\n" );
}
void MCU_ClkInit( void ) {
    /*----------------------------------------------------------------*/
    // Enable Clock
    // LXT
    CLK_EnableXtalRC( CLK_PWRCTL_LXTEN_Msk | CLK_PWRCTL_HIRC0EN_Msk );
    while ( !( CLK_WaitClockReady( CLK_STATUS_LXTSTB_Msk ) | CLK_WaitClockReady( CLK_STATUS_HIRC0STB_Msk ) ) ) {};

    // HIRC0
    CLK_EnableXtalRC( CLK_PWRCTL_HIRC0EN_Msk );
    while ( !CLK_WaitClockReady( CLK_STATUS_HIRC0STB_Msk ) ) {};

    // LIRC
    CLK_EnableXtalRC( CLK_PWRCTL_LIRCEN_Msk );
    while ( !CLK_WaitClockReady( CLK_STATUS_LIRCSTB_Msk ) ) {};

    /*----------------------------------------------------------------*/
    // Trim the clock.
    SYS->IRC0TCTL |= ( SYS_IRCTCTL_LOOP_32CLK | SYS_IRCTCTL_RETRY_512 );  // HIRC0 trim setting.
    SYS_EnableHIRC0Trim( SYS_IRC0TCTL_TRIM_12M, ( SYS_IRCTIEN_FAIL_EN | SYS_IRCTIEN_32KERR_EN ) );
    while ( !( SYS_GET_IRC0TRIM_INT_FLAG() & SYS_IRCTISTS_FREQLOCK ) ) {};  // Wait clock trim successful.

    /*----------------------------------------------------------------*/
    // Set Bus clock source
    CLK_SetHCLK( CLK_CLKSEL0_HCLKSEL_HIRC0, CLK_HCLK_CLK_DIVIDER( 1 ) );
    CLK_SetPCLK0( CLK_APB0DIV_HCLK );
    CLK_SetPCLK1( CLK_APB1DIV_HCLK );

    /*----------------------------------------------------------------*/
    // Enable clock fail interrupt
    CLK->CLKDCTL |= 0x2;  // LXT FD enable
    CLK->CLKDIE |= 0x2;   // LXT FD interrupt enable

    printf( "System clock has been initialized.\n" );
}
void MCU_GpioInit( void ) {
    /*----------------------------------------------------------------*/
    // Disable digital input
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _LED_PA0 ), _MASK( _GET_GPIO_PIN( _LED_PA0 ) ) );
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _NULL_PA2 ), _MASK( _GET_GPIO_PIN( _NULL_PA2 ) ) );
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _NULL_PA3 ), _MASK( _GET_GPIO_PIN( _NULL_PA3 ) ) );
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _NULL_PA4 ), _MASK( _GET_GPIO_PIN( _NULL_PA4 ) ) );
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _NULL_PA5 ), _MASK( _GET_GPIO_PIN( _NULL_PA5 ) ) );
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _NULL_PA6 ), _MASK( _GET_GPIO_PIN( _NULL_PA6 ) ) );
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _NULL_PA8 ), _MASK( _GET_GPIO_PIN( _NULL_PA8 ) ) );
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _NULL_PA9 ), _MASK( _GET_GPIO_PIN( _NULL_PA9 ) ) );
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _NRF_CE_PA15 ), _MASK( _GET_GPIO_PIN( _NRF_CE_PA15 ) ) );
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _GPS_UART0_TX_PB1 ), _MASK( _GET_GPIO_PIN( _GPS_UART0_TX_PB1 ) ) );
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _CAMERA_N_EN_PB2 ), _MASK( _GET_GPIO_PIN( _CAMERA_N_EN_PB2 ) ) );
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _CAMERA_UART1_TX_PB5 ), _MASK( _GET_GPIO_PIN( _CAMERA_UART1_TX_PB5 ) ) );
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _NRF_SPI0_SS_PC0 ), _MASK( _GET_GPIO_PIN( _NRF_SPI0_SS_PC0 ) ) );
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _NRF_SPI0_MOSI_PC3 ), _MASK( _GET_GPIO_PIN( _NRF_SPI0_MOSI_PC3 ) ) );
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _NULL_PF2 ), _MASK( _GET_GPIO_PIN( _NULL_PF2 ) ) );
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _NULL_PF3 ), _MASK( _GET_GPIO_PIN( _NULL_PF3 ) ) );
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _X32O_PF6 ), _MASK( _GET_GPIO_PIN( _X32O_PF6 ) ) );
    GPIO_DISABLE_DIGITAL_PATH( _GET_GPIO_PORT( _X32I_PF7 ), _MASK( _GET_GPIO_PIN( _X32I_PF7 ) ) );

    /*----------------------------------------------------------------*/
    // Pull-up
    GPIO_ENABLE_PULL_UP( _GET_GPIO_PORT( _EXT_INT_PA14 ), _MASK( _GET_GPIO_PIN( _EXT_INT_PA14 ) ) );
    GPIO_ENABLE_PULL_UP( _GET_GPIO_PORT( _NRF_IRQ_PB3 ), _MASK( _GET_GPIO_PIN( _NRF_IRQ_PB3 ) ) );

    /*----------------------------------------------------------------*/
    // GPIO mode
    GPIO_SetMode( _GET_GPIO_PORT( _LED_PA0 ), _MASK( _GET_GPIO_PIN( _LED_PA0 ) ), GPIO_PMD_OPEN_DRAIN );
    GPIO_SetMode( _GET_GPIO_PORT( _EXT_INT_PA14 ), _MASK( _GET_GPIO_PIN( _EXT_INT_PA14 ) ), GPIO_PMD_INPUT );
    GPIO_SetMode( _GET_GPIO_PORT( _NRF_CE_PA15 ), _MASK( _GET_GPIO_PIN( _NRF_CE_PA15 ) ), GPIO_PMD_OUTPUT );
    GPIO_SetMode( _GET_GPIO_PORT( _CAMERA_N_EN_PB2 ), _MASK( _GET_GPIO_PIN( _CAMERA_N_EN_PB2 ) ), GPIO_PMD_OUTPUT );
    GPIO_SetMode( _GET_GPIO_PORT( _NRF_IRQ_PB3 ), _MASK( _GET_GPIO_PIN( _NRF_IRQ_PB3 ) ), GPIO_PMD_INPUT );

    /*----------------------------------------------------------------*/
    // Enable interrupts
    GPIO_EnableInt( _GET_GPIO_PORT( _NRF_IRQ_PB3 ), _GET_GPIO_PIN( _NRF_IRQ_PB3 ), GPIO_INT_FALLING );
    GPIO_EnableInt( _GET_GPIO_PORT( _EXT_INT_PA14 ), _GET_GPIO_PIN( _EXT_INT_PA14 ), GPIO_INT_FALLING );

    // GPIO output state
    _NRF_CE_PA15     = 0;
    _CAMERA_N_EN_PB2 = 0;
    _LED_PA0         = 0;

    printf( "GPIO has been initialized.\n" );
}
void MCU_NvicInit( void ) {
    NVIC_EnableIRQ( RTC_IRQn );    // Real time control.
    NVIC_EnableIRQ( UART0_IRQn );  // GPS.
    NVIC_EnableIRQ( UART1_IRQn );  // Camera.
    NVIC_EnableIRQ( GPABC_IRQn );  // nRF IRQ.
    NVIC_EnableIRQ( CKSD_IRQn );   // Clock fail.
    NVIC_EnableIRQ( WDT_IRQn );    // Watch dog timer.
    NVIC_EnableIRQ( PDWU_IRQn );

    printf( "NVIC has been initialized.\n" );
}
void MCU_UartInit( void ) {
    // Select clock source.
    CLK_SetModuleClock( UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_UART0_CLK_DIVIDER( 1 ) );  // GPS
    CLK_SetModuleClock( UART1_MODULE, CLK_CLKSEL2_UART1SEL_HIRC, CLK_UART1_CLK_DIVIDER( 1 ) );  // CAMERA

    // Enable clock
    CLK_EnableModuleClock( UART0_MODULE );
    CLK_EnableModuleClock( UART1_MODULE );

    // Enable module.
    UART_Open( _GPS_UART, _GPS_UART_BAUDRATE );
    UART_Open( _CAMERA_UART, _CAMERA_UART_BAUDRATE );

    // FIFO interrupt trigger level
    UART_SET_RX_FIFO_INTTRGLV( _GPS_UART, UART_LINE_RFITL_14BYTES );
    UART_SET_RX_FIFO_INTTRGLV( _CAMERA_UART, UART_LINE_RFITL_14BYTES );

    // RX timeout
    UART_SetTimeoutCnt( _GPS_UART, 0xFF );
    UART_SetTimeoutCnt( _CAMERA_UART, 0xFF );

    // Enable interrupt
    UART_EnableInt( _GPS_UART, UART_INTEN_RXTOIEN_Msk | UART_INTEN_RDAIEN_Msk );
    UART_EnableInt( _CAMERA_UART, UART_INTEN_RXTOIEN_Msk | UART_INTEN_RDAIEN_Msk );

    printf( "UART has been initialized.\n" );
}
void MCU_SpiInit( void ) {
    // Select clock source.
    CLK_SetModuleClock( SPI0_MODULE, CLK_CLKSEL1_SPI0SEL_HIRC, 0 );  // NRF24L01

    // Enable clock.
    CLK_EnableModuleClock( SPI0_MODULE );

    // Enable module.
    SPI_Open( _NRF_SPI, SPI_MASTER, SPI_MODE_0, 8, _NRF_SPI_CLK );

    printf( "SPI has been initialized.\n" );
}
void MCU_RtcInit( void ) {
    // Enable clock.
    CLK_EnableModuleClock( RTC_MODULE );

    // Enable module.
    RTC_Open( NULL );
    RTC_SetDateAndTime( &g_stRtcTime );
    RTC_SetAlarmDateAndTime( NULL );

    // Enable interrupt.
    RTC_EnableInt( RTC_INTEN_ALMIEN_Msk );

    printf( "RTC has been initialized.\n" );
}
void MOD_CameraInit( void ) {
    // Instantiate
    g_stOv528_s0 = OV528_New( 0x0001, 50, &g_stUart1, CameraDelay );
    USER_DISABLE_CMAERA_POWER();

    printf( "Camera has been initialized.\n" );
}
void MOD_NrfInit( void ) {
    g_stNrfRx = FIFO_New( 1024, NULL );                             // Instantiate
    g_stNrf0  = NRF_New( &g_stSpi0, &SetCE, &ResetCE, &NrfDelay );  // Instantiate

    nrfP2P_InitNrf( g_stNrf0, 0x50 );  // Initialize

    g_stNrfCh1 = nrfP2P_NewChannel( g_stNrf0, 1, ( uint8_t * )g_u8P1Address, true, true );  // Instantiate

    nrfP2P_EnableTxDypw( g_stNrf0 );                                   // Dynamic Payload width
    nrfP2P_EnableTxAutoAck( g_stNrf0, ( uint8_t * )g_u8DestAddress );  // Auto Ack

    NRF_RxMode( g_stNrf0 );  // Setting Rx mode

    printf( "nRF has been initialized.\n" );
}
void MOD_GpsInit( void ) {
    g_stGpsRx = FIFO_New( 200, NULL );  // Instantiate

    printf( "GPS has been initialized.\n" );
}

void CameraGetImage( void ) {
    size_t  i;
    uint8_t check;
    uint8_t dataTemp[ 50 ];
    uint8_t imgConfig[ 8 ] = { 0x5E, 0xDB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A };

    // WWDT_RELOAD_COUNTER();
    // Setting nRF Tx mode.
    NRF_TxMode( g_stNrf0 );
    NRF_FlushTx( g_stNrf0 );

    // WWDT_RELOAD_COUNTER();
    // Enable Camera Power
    USER_ENABLE_CMAERA_POWER();
    g_stOv528_s0->Delay( 2000 );  // Wait camera power stable

    // WWDT_RELOAD_COUNTER();
    // Start and get pictue.
    printf( "Start Get Picture\n" );
    FIFO_Rst( g_stOv528_s0->fifoBuf );  // Clean camera fifo buffer
    OV528_SNYC( g_stOv528_s0 );
    OV528_Init( g_stOv528_s0, OV528_INIT_JPEG, OV528_INIT_PR_160_120, OV528_INIT_JPEG_640_480 );
    OV528_Snapshout( g_stOv528_s0, OV528_SNAPSHOT_COMPRESSED, 0x0000 );
    OV528_SetPacketSize( g_stOv528_s0, 32 + 6 );
    OV528_GetPictue( g_stOv528_s0, OV528_GET_PICTURE_SNAPSHOT );

    printf( "Picture size is %d\n", g_stOv528_s0->imageSize );

    // WWDT_RELOAD_COUNTER();
    // Transmit picture config
    for ( i = 0; i < 4; i++ ) imgConfig[ i + 2 ] = *( ( ( uint8_t * )&g_stOv528_s0->imageSize ) + i );    // Get picture size.
    imgConfig[ 6 ] |= ( ( ( ( g_gpsDataSize > 0 ) ? 1 : 0 ) << 0 ) | ( g_u8ExternalTriggerFlag << 1 ) );  // Get Gps data ready flag and external trigger flag.
    check                   = NrfSendData( imgConfig, 8 );                                                // Transmit
    g_u8ExternalTriggerFlag = 0;

    // Pull data and send;
    if ( check == true ) {
        // Picture data
        for ( i = 0; i < g_stOv528_s0->imagePacket; i++ ) {
            size_t size;
            // WWDT_RELOAD_COUNTER();
            size  = OV528_GetPacket( g_stOv528_s0, i, dataTemp );
            check = NrfSendData( dataTemp, size );
            if ( check == false ) { break; }
        }

        // GPS data
        if ( g_gpsDataSize > 0 ) {
            // WWDT_RELOAD_COUNTER();
            check = NrfSendData( g_gpsData, g_gpsDataSize );
        }
    }
    else {
        printf( "Send data err\n" );
    }

    // WWDT_RELOAD_COUNTER();
    // Module power down.
    USER_DISABLE_CMAERA_POWER();
    g_stOv528_s0->Delay( 2000 );  // Wait device power stable.

    // WWDT_RELOAD_COUNTER();
    // Setting nRF Rx mode.
    NRF_RxMode( g_stNrf0 );
    NRF_FlushRx( g_stNrf0 );
    FIFO_Rst( g_stNrfRx );

    g_u8GetPictureReadyFlag = 0;
}
bool NrfSendData( uint8_t *data, uint32_t length ) {
    uint32_t f = 0;
    while ( length > 0 ) {
        uint8_t c;
        uint8_t len = ( length > 32 ) ? 32 : length;
        c           = nrfP2P_SendPacket( g_stNrf0, ( uint8_t * )g_u8DestAddress, data + f, len );
        if ( c == false ) {
            uint8_t tr;
            tr = nrfP2P_TxReuse( g_stNrf0, 300 );
            if ( tr == false ) {
                NRF_RstIrq( g_stNrf0 );
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
    for ( i = 0; i < ( delayTime >> 16 ); i++ ) CLK_SysTickDelay( 0x10000 );
    if ( !( delayTime & 0xFFFF ) ) CLK_SysTickDelay( delayTime & 0xFFFF );
}
void CmdDecoder( void ) {
    if ( FIFO_IsEmpty( g_stNrfRx ) == false ) {
        uint32_t c_time = 0;
        while ( c_time++ < 500 ) {
            if ( FIFO_CmdCheck( g_stNrfRx, ( uint8_t * )"CMD:", 0, 4, 4, 0, false ) ) {
                printf( "CMD:" );
                if ( FIFO_CmdCheck( g_stNrfRx, ( uint8_t * )"GET:", 4, 8, 4, 0, false ) ) {
                    printf( "GET:" );
                    if ( FIFO_CmdCheck( g_stNrfRx, ( uint8_t * )"IMG", 8, 11, 3, 0, false ) ) {
                        printf( "IMG\n" );
                        if ( FIFO_WaitData( g_stNrfRx, 11, 0 ) ) {
                            g_u8GetPictureReadyFlag = 1;
                            FIFO_Rst( g_stNrfRx );
                        }
                    }
                    if ( FIFO_CmdCheck( g_stNrfRx, ( uint8_t * )"GPS", 8, 11, 3, 0, false ) ) {
                        printf( "GPS\n" );
                        g_u8GpsScanFlag = 1;
                        FIFO_Rst( g_stNrfRx );
                    }
                }
                if ( FIFO_CmdCheck( g_stNrfRx, ( uint8_t * )"SET:", 4, 8, 4, 0, false ) ) {
                    printf( "SET:" );
                    if ( FIFO_CmdCheck( g_stNrfRx, ( uint8_t * )"CYC:", 8, 12, 4, 0, false ) ) {
                        printf( "CYC:" );
                        if ( FIFO_WaitData( g_stNrfRx, 16, 0 ) ) {
                            g_u32GetPictureTime = ( uint32_t )( FIFO_ReadData( g_stNrfRx, 12 ) - 0x30 ) * 1000;
                            g_u32GetPictureTime += ( uint32_t )( FIFO_ReadData( g_stNrfRx, 13 ) - 0x30 ) * 100;
                            g_u32GetPictureTime += ( uint32_t )( FIFO_ReadData( g_stNrfRx, 14 ) - 0x30 ) * 10;
                            g_u32GetPictureTime += ( uint32_t )( FIFO_ReadData( g_stNrfRx, 15 ) - 0x30 ) * 1;
                            printf( "%04d\n", g_u32GetPictureTime );
                            g_u32GetPictureCount = 0;
                            FIFO_Rst( g_stNrfRx );
                            if ( g_u32GetPictureTime > 0 ) {
                                uint32_t temp = g_u32GetPictureTime;
                                uint8_t  hour, minute, second;
                                memcpy( &g_stRtcAlarmTime, &g_stRtcTime, sizeof( S_RTC_TIME_DATA_T ) );
                                hour   = temp / 3600;
                                temp   = temp % 3600;
                                minute = temp / 60;
                                temp   = temp % 60;
                                second = temp;
                                g_stRtcAlarmTime.u32Hour += hour;
                                g_stRtcAlarmTime.u32Minute += minute;
                                g_stRtcAlarmTime.u32Second += second;
                                RTC_SetDateAndTime( &g_stRtcTime );
                                RTC_SetAlarmDateAndTime( &g_stRtcAlarmTime );
                            }
                            else {
                                NRF_PowerOn( g_stNrf0 );
                                g_stNrf0->SetCE();
                            }
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
    f = nrfP2P_SendPacket( g_stNrf0, ( uint8_t * )g_u8DestAddress, g_stNrfRx->buf, length );
    NRF_RxMode( g_stNrf0 );
    FIFO_Rst( g_stNrfRx );
    printf( "SYS:" );
    if ( f == true )
        printf( "OK\n" );
    else
        printf( "TOUT\n" );
}
