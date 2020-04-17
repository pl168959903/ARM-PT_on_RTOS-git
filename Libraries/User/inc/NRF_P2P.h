
#ifndef _NRF_P2P_
#define _NRF_P2P_

#ifdef __cplusplus
extern "C" {
#endif

#include "NRF24L01.h"
#include <string.h>

#define nrfP2P_USE_VMEMALLOC    1

#if nrfP2P_USE_VMEMALLOC
    #include "vMemAlloc.h"
    #define nrfP2P_MALLOC( size ) vMemAlloc( size )
    #define nrfP2P_FREE( ptr ) vMemFree( ptr )
#else
    #define nrfP2P_MALLOC( size ) malloc( size )
    #define nrfP2P_FREE( ptr ) free( ptr )
#endif  // nrfP2P_USE_VMEMALLOC


typedef struct {
    NRF_T* nrf;
    uint8_t ch;
    uint8_t *address;
    uint8_t *destAddr;
    uint8_t rxpw;
    uint8_t txpw;
    bool autoAck;
} nrfP2P_CHANNEL_T;

void nrfP2P_InitNrf( NRF_T* nrf, uint8_t rfCh );
nrfP2P_CHANNEL_T* nrfP2P_NewChannel( NRF_T* nrf, uint8_t ch, uint8_t* address, uint8_t* destAddr, uint8_t rxpw, uint8_t txpw, bool autoAck );
bool nrfP2P_SendPacket( nrfP2P_CHANNEL_T* ptr, uint8_t* array );
bool nrfP2P_ReceivePacket( nrfP2P_CHANNEL_T* ptr, uint8_t* array );

#ifdef __cplusplus
}
#endif
#endif
