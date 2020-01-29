#ifndef __IRQ_H__
#define __IRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t IRQ_timer0_flag_1;
extern uint8_t IRQ_timer0_flag_2;

void NVIC_INIT(void);


#endif //__IRQ_H__
