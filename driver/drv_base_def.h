#ifndef __DRV_BASE_DEF_H__
#define __DRV_BASE_DEF_H__

#include <stdint.h>

#define __TEXT_RAM__    __SECTION(.text_ram)

#define REG8(x)     (*(volatile uint8_t  *)(x))
#define REG16(x)    (*(volatile uint16_t *)(x))
#define REG32(x)    (*(volatile uint32_t *)(x))
#define REG64(x)    (*(volatile uint64_t *)(x))

#define BIT(x) (1U << (x))

#include "drv_241register.h"

#endif
