#include "Nano103.h"
#include "USER_LIB.h"


uint32_t CameraUartWrite(uint8_t * arr, uint32_t size){
    return UART_Write(UART1, arr, size);
}

void CameraDelay(uint32_t time){
    DelayUs(1000 * time);
}
