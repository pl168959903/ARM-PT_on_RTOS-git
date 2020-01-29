#include "USER_LIB.h"
#include "Nano103.h"
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

S_RTC_TIME_DATA_T RTC_TIME = { 2020, 1, 1, 0, 0, 0, RTC_SUNDAY, RTC_CLOCK_24 };
/*---------------------------------------------global------------------------------------------------------*/
uint8_t g_button_flag  = 0;
uint8_t g_ext_int_flag = 0;
uint8_t g_nrf_irq_flag = 0;

/*---------------------------------------------------------------------------------------------------------*/

void PIN_SETUP( void ) {
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

void CLK_SETUP( void ) {
    // SYSTEM CLOCK
    CLK_EnableXtalRC( CLK_PWRCTL_LXTEN_Msk );
    while ( !CLK_WaitClockReady( CLK_STATUS_LXTSTB_Msk ) ) {
    };
    CLK_EnableXtalRC( CLK_PWRCTL_HXTEN_Msk );
    while ( !CLK_WaitClockReady( CLK_STATUS_HXTSTB_Msk ) ) {
    };
    CLK_EnableXtalRC( CLK_PWRCTL_LIRCEN_Msk );
    while ( !CLK_WaitClockReady( CLK_STATUS_LIRCSTB_Msk ) ) {
    };
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

void GPIO_SETUP( void ) {
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

void UART_SETUP( void ) {
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

#ifdef USER_CFG_DEBUG_MODE
    printf( "CPU Freq   : %dHz\n", CLK_GetCPUFreq() );
    printf( "HCLK Freq  : %dHz\n", CLK_GetHCLKFreq() );
    printf( "HXT Freq   : %dHz\n", CLK_GetHXTFreq() );
    printf( "LXT Freq   : %dHz\n", CLK_GetLXTFreq() );
    printf( "PCLK0 Freq : %dHz\n", CLK_GetPCLK0Freq() );
    printf( "PCLK1 Freq : %dHz\n", CLK_GetPCLK1Freq() );
    printf( "PLL Freq   : %dHz\n", CLK_GetPLLClockFreq() );
#endif  // USER_CFG_DEBUG_MODE
}

void SPI_SETUP( void ) {
    CLK_EnableModuleClock( SPI0_MODULE );
    CLK_EnableModuleClock( SPI3_MODULE );

    SPI_Open( SPI0, SPI_MASTER, SPI_MODE_0, 8, 1000000 );
    SPI_Open( SPI3, SPI_MASTER, SPI_MODE_0, 8, 1000000 );
}

void TIMER_SETUP( void ) {
    CLK_EnableModuleClock( TMR0_MODULE );
    TIMER_Open( TIMER0, TIMER_PERIODIC_MODE, 8 );
    TIMER_EnableInt( TIMER0 );
}

void RTC_SETUP( void ) {
    CLK_EnableModuleClock( RTC_MODULE );
    RTC_Open( &RTC_TIME );
    RTC_SetTickPeriod( RTC_TICK_1_SEC );
}

void Array_assign( uint8_t arg[], uint32_t num, ... ) {
    uint32_t i;
    va_list  marker;
    va_start( marker, num );
    for ( i = 0; i < num; i++ ) {
        arg[ i ] = ( uint8_t )va_arg( marker, int );
    }
    va_end( marker );
}

void OV2640_OPEN( void ) {
    USER_ENABLE_CMAERA_POWER();
    printf( "Wait for Camera" );
    while ( !OV2640_SNYC() )
        printf( "." );
    delay_us( 1000000 );
    OV2640_INIT( OV2640_INIT_JPEG, OV2640_PR_160_120, OV2640_JPEG_640_480 );
    OV2640_SetBaudRate( 115200 );
    OV2640_SetPacketSize( 32 + 6 );
    printf( "down\n" );
}
void Camera_shot( uint32_t addr ) {
    OV2640_GetPictue( 0x01 );
    OV2640_SetPacketSize( 256 );
}

void delay_us( uint32_t us ) {
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
