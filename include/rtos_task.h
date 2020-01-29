#ifndef __RTOS_TASK_H__
#define __RTOS_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

extern TaskHandle_t taskBootCreate_handler;
extern TaskHandle_t taskLEDControl_handler;

void taskLEDControl( void );
void taskBootCreate( void );

#ifdef __cplusplus
}
#endif

#endif  //__RTOS_TASK_H__
