#include "Nano103.h"

void CameraUartWrite(uint8_t * arr, uint32_t size){
    UART_Write(UART1, arr, size);
}

void CameraDelay(uint32_t time){
    vTaskDelay(time);
}
