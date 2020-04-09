
#ifndef _NRF_P2P_
#define _NRF_P2P_

#ifdef __cplusplus
extern "C" {
#endif

#include "NRF24L01.h"
#include <string.h>

#define NRFP2P_USE_VMEMALLOC    1

#if NRFP2P_USE_VMEMALLOC
    #include "vMemAlloc.h"
    #define NRFP2P_MALLOC( size ) vMemAlloc( size )
    #define NRFP2P_FREE( ptr ) vMemFree( ptr )
#else
    #define NRFP2P_MALLOC( size ) malloc( size )
    #define NRFP2P_FREE( ptr ) free( ptr )
#endif  // NRFP2P_USE_VMEMALLOC


typedef struct {
    NRF_T* nrf;
    uint8_t ch;
    uint8_t *address;
    uint8_t *destAddr;
    uint8_t rxpw;
    uint8_t txpw;
    bool autoAck;
} NRFP2P_CHANNEL_T;

void nrfP2p_InitNrf( NRF_T* nrf, uint8_t rfCh );
NRFP2P_CHANNEL_T* nrfP2p_NewChannel( NRF_T* nrf, uint8_t ch, uint8_t* address, uint8_t* destAddr, uint8_t rxpw, uint8_t txpw, bool autoAck );
bool nrfP2p_SendPacket( NRFP2P_CHANNEL_T* ptr, uint8_t* array );
bool nrfP2p_ReceivePacket( NRFP2P_CHANNEL_T* ptr, uint8_t* array );

#ifdef __cplusplus
}
#endif
#endif
