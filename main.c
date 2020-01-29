#include "Nano103.h"
#include "USER_LIB.h"
#include <stdio.h>

uint32_t size;
uint8_t  a;

int main(void) {
    SYS_UnlockReg();
    PIN_SETUP();
    CLK_SETUP();
    UART_SETUP();
    RTC_SETUP();
    GPIO_SETUP();
    SPI_SETUP();
    TIMER_SETUP();
    NVIC_INIT();
    SYS_LockReg();

     xTaskCreate((TaskFunction_t)taskBootCreate, (const char *)"boot", (uint16_t)128,
                (void *)NULL, (UBaseType_t)1, (TaskHandle_t *)&taskBootCreate_handler);

    vTaskStartScheduler();
}
