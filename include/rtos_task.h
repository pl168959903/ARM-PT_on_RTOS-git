#ifndef __RTOS_TASK_H__
#define __RTOS_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

extern TaskHandle_t taskBootCreate_handler;
extern TaskHandle_t taskLEDControl_handler;
extern TaskHandle_t taskOV528Open_handler;
extern TaskHandle_t taskFifoCntTrigger_handler;

void taskBootCreate( void );
void taskLEDControl( void );
void taskOV528Open(void);
void taskFifoCntTrigger(void);

#ifdef __cplusplus
}
#endif

#endif  //__RTOS_TASK_H__
