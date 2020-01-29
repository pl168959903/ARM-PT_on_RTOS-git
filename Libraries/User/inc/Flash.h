#ifndef __FLASH_H__
#define __FLASH_H__

#ifdef __cplusplus
extern "C"
{
#endif
	
#define ROM0_BASE 0x00000000
#define ROM1_BASE 0x00008000
#define REBOOT_SYS_DELAY 3
	
int8_t set_config(void);

#ifdef __cplusplus
}
#endif

#endif   // __FLASH_H__
