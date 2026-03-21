#include <string.h>
#include <stdio.h>
#include "core_base.h"
#include "drv_base_def.h"
#include "drv_system.h"

uint32_t loop_cnt;

#define CACHE_ON {PRAM_KEY=0xACAC;SCR_CFG|=(1<<13);PRAM_KEY=0xCACA;while((SCR_CALI&0x0100)==0x0000);}
#define CACHE_OFF {PRAM_KEY=0xACAC;SCR_CFG&=~(1<<13);PRAM_KEY=0xCACA;while((SCR_CALI&0x0100)==0x0100);}



int main(void)
{
    drv_system_init();

    CACHE_OFF;

    DP0_DM2 = 0x00;
    DP0_DM1 = 0xFF;
    DP0_DM0 = 0xFF;

    DP0_GOE = 0x00;
    DP0_GIE = 0x00;

    DP1_DM2 = 0x00;
    DP1_DM1 = 0xFF;
    DP1_DM0 = 0xFF;

    DP1_GOE = 0x00;
    DP1_GIE = 0x00;

    while(1)
    {
        DP1_DR = 0xFF;
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
        DP1_DR = 0x00;
        __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
    }
}



