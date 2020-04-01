#ifndef _BRIDGING_H_
#define _BRIDGING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>


typedef struct SPI_Function {
    uint8_t ( *Write )( uint8_t data );
    uint8_t ( *Read )( void );
    void ( *Select )( void );
    void ( *Deselect )( void );
    void ( *Dummy )( void );
} SPI_Func_T;

typedef struct UART_Function {
    uint32_t ( *Write )( uint8_t* array, uint32_t size );
    uint32_t ( *Read )( uint8_t* array, uint32_t size );
} UART_Func_T;

#ifdef __cplusplus
}
#endif

#endif  // !_PORTABLE_
