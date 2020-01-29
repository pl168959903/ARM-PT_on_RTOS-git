#include "Nano103.h"
#include <USER_LIB.h>
#include <stdio.h>


static uint32_t config[2] = {
    0xFFFFFFFE,
    0x00008000
};

int8_t set_config(void) {
    uint32_t config_read[2];
    int8_t i;
    FMC_ReadConfig(config_read, 2);
    if((config_read[0] == config[0]) && (config_read[1] == config[1])) return 0;
    printf("Config Changed!\n");
    FMC_ENABLE_CFG_UPDATE();
    FMC_WriteConfig(config, 2);
    FMC_ReadConfig(config_read, 2);
    if((config_read[0] != config[0]) || (config_read[1] != config[1])) {
        printf("Config set error!\n");
        return -1;
    }
    printf("Config set Successful!\n");
    printf("Config 0 : 0x%X\n", config_read[0]);
    printf("Config 1 : 0x%X\n", config_read[1]);
    printf("Flash free : %d bytes\n", ((FMC_APROM_END > config_read[1]) && !(config_read[0] & 0x1)) ? (int)(FMC_APROM_END - config_read[1]) : 0);
    printf("Reboot system after %d second.\n", REBOOT_SYS_DELAY);
    for(i = REBOOT_SYS_DELAY; i >= 0; i--) {
        printf("............%d\n", i);
        CLK_SysTickDelay(500000);
				CLK_SysTickDelay(500000);
    }
    SYS->IPRST1 = SYS_IPRST1_CHIPRST_Msk;
		return 0;
}
