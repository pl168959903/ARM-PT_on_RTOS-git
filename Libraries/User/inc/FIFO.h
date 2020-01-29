
#ifndef __FIFO_H__
#define __FIFO_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define FIFO_BUF_SIZE 1024
#define FIFO_timerFreq 8;

typedef volatile struct {
    uint8_t buf[FIFO_BUF_SIZE];
    uint32_t size;
    uint32_t head;
    uint32_t tail;
} DATA_FIFO;

extern DATA_FIFO FIFO_uart0_buf;
extern DATA_FIFO FIFO_uart1_buf;
//extern DATA_FIFO spi0_buf;
//extern DATA_FIFO spi3_buf;

extern uint8_t FIFO_cmd_countFlag;

void FIFO_count_start(void);
void FIFO_count_stop(void);
void FIFO_countTime(void);
void FIFO_reset_countTime(void);
void FIFO_data_byte_in(DATA_FIFO *, uint8_t *);
void FIFO_data_Byte_out(DATA_FIFO *, uint8_t *);
void FIFO_reset(DATA_FIFO *);
uint8_t FIFO_IsEmpty(DATA_FIFO *buf_st);
uint8_t FIFO_read_byte(DATA_FIFO *, int32_t );
uint8_t FIFO_waitData(DATA_FIFO *, uint32_t , uint32_t );
uint8_t FIFO_cmdCheck(DATA_FIFO *buf_st, uint8_t [], uint32_t, uint32_t );



#endif //__FIFO_H__
