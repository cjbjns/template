#ifndef __DRV_SYSTEM_H__
#define __DRV_SYSTEM_H__

#include "drv_base_def.h"
#include <stdint.h>

void drv_system_init(void);

void drv_sys_init_pclk(uint8_t clk, uint8_t src, uint8_t div);

#endif
