/*
    __             _____   __________
   / /_  __  __   /__  /  / ____/ __ \
  / __ \/ / / /     / /  / / __/ /_/ /
 / /_/ / /_/ /     / /__/ /_/ / _, _/
/_.___/\__, /     /____/\____/_/ |_|
      /____/
*/

#include "Project.h"

// Stuct Value
S_RTC_TIME_DATA_T g_stRtcTime = { 2020, 1, 1, 0, 0, 0, RTC_SUNDAY, RTC_CLOCK_24 };

// Camera
OV528_T* g_stOv528_s0;  // 相機實體

// NRF
NRF_T*            g_stNrf0;    // NRF實體
nrfP2P_CHANNEL_T* g_stNrfCh1;  // NRF 通道實體
FIFO_T*           g_stNrfRx;
const uint8_t     g_u8P1Address[ 5 ]   = { 0x01, 0x79, 0x02, 0x08, 0x11 };
const uint8_t     g_u8DestAddress[ 5 ] = { 0x00, 0x79, 0x02, 0x08, 0x11 };
uint8_t           temp[ 32 ]           = { 0x03, 0xBB, 0xCC, 0xDD };

/*---------------------------------------------------------------------------------------------------------*/

void PinSetup( void ) {
    // XTAL
    SYS->GPF_MFPL |= ( SYS_GPF_MFPL_PF7MFP_X32_IN | SYS_GPF_MFPL_PF6MFP_X32_OUT | SYS_GPF_MFPL_PF3MFP_XT1_IN | SYS_GPF_MFPL_PF2MFP_XT1_OUT );

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
}
void ClkSetup( void ) {
    // SYSTEM CLOCK
    CLK_EnableXtalRC( CLK_PWRCTL_LXTEN_Msk );
    while ( !CLK_WaitClockReady( CLK_STATUS_LXTSTB_Msk ) ) {};
    CLK_EnableXtalRC( CLK_PWRCTL_HXTEN_Msk );
    while ( !CLK_WaitClockReady( CLK_STATUS_HXTSTB_Msk ) ) {};
    CLK_EnableXtalRC( CLK_PWRCTL_LIRCEN_Msk );
    while ( !CLK_WaitClockReady( CLK_STATUS_LIRCSTB_Msk ) ) {};
    CLK_SetHCLK( CLK_CLKSEL0_HCLKSEL_HXT, CLK_HCLK_CLK_DIVIDER( 1 ) );

    // UART
    CLK_SetModuleClock( UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT, CLK_UART0_CLK_DIVIDER( 1 ) );
    CLK_SetModuleClock( UART1_MODULE, CLK_CLKSEL2_UART1SEL_HXT, CLK_UART1_CLK_DIVIDER( 1 ) );

    // SPI
    CLK_SetModuleClock( SPI0_MODULE, CLK_CLKSEL1_SPI0SEL_HXT, 0 );
    CLK_SetModuleClock( SPI3_MODULE, CLK_CLKSEL2_SPI3SEL_HXT, 0 );

    // TMR
    CLK_SetModuleClock( TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_LXT, CLK_TMR0_CLK_DIVIDER( 16 ) );
}
void GpioSetup( void ) {
    // MODE.
    GPIO_SetMode( PA, BIT0, GPIO_PMD_OUTPUT );
    GPIO_SetMode( PA, BIT2, GPIO_PMD_INPUT );
    GPIO_SetMode( PA, BIT14, GPIO_PMD_INPUT );
    GPIO_SetMode( PA, BIT15, GPIO_PMD_OUTPUT );
    GPIO_SetMode( PB, BIT2, GPIO_PMD_OUTPUT );
    GPIO_SetMode( PB, BIT3, GPIO_PMD_INPUT );

    // PULL UP.
    GPIO_ENABLE_PULL_UP( PA, BIT2 );
    GPIO_ENABLE_PULL_UP( PA, BIT14 );
    GPIO_ENABLE_PULL_UP( PB, BIT3 );

    // Interrupt.
    GPIO_EnableInt( PA, 2, GPIO_INT_FALLING );
    GPIO_EnableInt( PA, 14, GPIO_INT_FALLING );
    GPIO_EnableInt( PB, 3, GPIO_INT_FALLING );

    // DEBOUNCE.
    GPIO_SET_DEBOUNCE_TIME( GPIO_DBCLKSRC_IRC10K, GPIO_DBCLKSEL_8 );
    GPIO_ENABLE_DEBOUNCE( PA, BIT2 );

    // INIT pin status.
    PA0  = 1;
    PA15 = 0;
    PB2  = 1;
}
void UartSetup( void ) {
    CLK_EnableModuleClock( UART0_MODULE );
    CLK_EnableModuleClock( UART1_MODULE );

    UART_Open( UART0, 115200 );
    UART_Open( UART1, 115200 );

    UART_SET_RX_FIFO_INTTRGLV( UART0, UART_LINE_RFITL_14BYTES );
    UART_SET_RX_FIFO_INTTRGLV( UART1, UART_LINE_RFITL_14BYTES );

    UART_SetTimeoutCnt( UART0, 0xFF );
    UART_SetTimeoutCnt( UART1, 0xFF );

    // Interrupt
    UART_EnableInt( UART1, UART_INTEN_RXTOIEN_Msk | UART_INTEN_RDAIEN_Msk );
}
void SpiSetup( void ) {
    CLK_EnableModuleClock( SPI0_MODULE );
    CLK_EnableModuleClock( SPI3_MODULE );

    SPI_Open( SPI0, SPI_MASTER, SPI_MODE_0, 8, 1000000 );
    SPI_Open( SPI3, SPI_MASTER, SPI_MODE_0, 8, 1000000 );
}
void TimerSetup( void ) {
    CLK_EnableModuleClock( TMR0_MODULE );
    TIMER_Open( TIMER0, TIMER_PERIODIC_MODE, 8 );
    TIMER_EnableInt( TIMER0 );
}
void RtcSetup( void ) {
    CLK_EnableModuleClock( RTC_MODULE );
    RTC_Open( &g_stRtcTime );
    RTC_SetTickPeriod( RTC_TICK_1_SEC );
}
void NVIC_Init( void ) {
    NVIC_EnableIRQ( GPABC_IRQn );
    // NVIC_EnableIRQ( TMR0_IRQn );
    NVIC_EnableIRQ( UART1_IRQn );
}
void CameraSetup( void ) {
    size_t  i, size;
    uint8_t dataTemp[ 100 ];
    g_stOv528_s0 = OV528_New( 0x0001, 1024, &g_stUart1, CameraDelay );
    USER_DISABLE_CMAERA_POWER();
    g_stOv528_s0->Delay( 1000 );
    USER_ENABLE_CMAERA_POWER();
    g_stOv528_s0->Delay( 1000 );
    OV528_SNYC( g_stOv528_s0 );
    OV528_Init( g_stOv528_s0, OV528_INIT_JPEG, OV528_INIT_PR_160_120, OV528_INIT_JPEG_640_480 );
    OV528_Snapshout( g_stOv528_s0, OV528_SNAPSHOT_COMPRESSED, 0x0000 );
    OV528_SetPacketSize( g_stOv528_s0, 100 );
    OV528_GetPictue( g_stOv528_s0, OV528_GET_PICTURE_SNAPSHOT );
    for ( i = 0; i < g_stOv528_s0->imagePacket; i++ ) {
        size = OV528_GetPacket( g_stOv528_s0, i, dataTemp );
        UART_Write( UART0, dataTemp, size );
    }
}
void NrfSetup( void ) {
    bool a;
    g_stNrf0 = NRF_New( &g_stSpi0, &SetCE, &ResetCE );
    nrfP2P_InitNrf( g_stNrf0, 0x50 );
    g_stNrfCh1 = nrfP2P_NewChannel( g_stNrf0, 1, ( uint8_t* )g_u8P1Address, ( uint8_t* )g_u8DestAddress, 32, 32, true );
    NRF_RegPrintf(g_stNrf0);
    while(1){};
    NRF_FlushRx( g_stNrf0 );
    NRF_FlushTx( g_stNrf0 );
    NRF_TxMode( g_stNrf0 );
    while ( 1 ) {
        a = nrfP2P_SendPacket( g_stNrfCh1, temp );
        // NRF_RxMode( g_stNrf0 );
        if ( a == true ) {
            printf( "T\n" );
        }
        else {
            printf( "F\n" );
        }
    }
}

void DelayUs( uint32_t us ) {
    uint32_t delayMaxTime = 16777216 / ( CLK_GetCPUFreq() / 1000000 );
    do {
        if ( us > delayMaxTime ) {
            CLK_SysTickDelay( delayMaxTime );
            us -= delayMaxTime;
        }
        else {
            CLK_SysTickDelay( us );
            us = 0;
        }
    } while ( us > 0 );
}

