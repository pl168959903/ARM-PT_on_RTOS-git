
#ifndef _NRF_P2P_
#define _NRF_P2P_

#ifdef __cplusplus
extern "C" {
#endif

#include "NRF24L01.h"

void nrfP2p_InitNrf( NRF_T* nrf, uint8_t rfCh );
void nrfP2p_NewChannel( NRF_T* nrf, uint8_t ch, uint8_t* addr, size_t bufSize, uint8_t rxpw, bool autoAck );
bool nrfP2p_SendPacket( NRF_T* nrf, uint8_t* addr, uint8_t* array, uint8_t size, uint8_t txpw );

#ifdef __cplusplus
}
#endif
#endif
