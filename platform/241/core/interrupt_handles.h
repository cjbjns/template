#ifndef __IRQ_HANDLES_H__
#define __IRQ_HANDLES_H__

#define __IREGION_BASEADDR 0x18000000
#define __IINFO_BASEADDR (__IREGION_BASEADDR+0x00000)
#define __DEBUG_BASEADDR (__IREGION_BASEADDR+0x10000)
#define __ECLIC_BASEADDR (__IREGION_BASEADDR+0x20000)
#define __TIMER_BASEADDR (__IREGION_BASEADDR+0x30000)

#include "riscv_encoding.h"
#include <stdint.h>

#include "core_base.h"
#include "core_cache.h"
#include "core_eclic.h"

#define __SECTION(x)            __attribute__((section(#x)))
#define __STATIC_FORCEINLINE    __attribute__((always_inline)) static inline

#define B_IRQ(x) (1<<(x - LVD_IRQn))

typedef enum
{
    IRQ_LEV_LVD     = 1,
    IRQ_LEV_DMA     = 6,
    IRQ_LEV_SPI     = 5,
    IRQ_LEV_I2C     = 6,
    IRQ_LEV_CSX     = 2,
    IRQ_LEV_SLP     = 2,
    IRQ_LEV_GPIO    = 3,
    IRQ_LEV_VC3     = 3,
    IRQ_LEV_NSX     = 3,
    IRQ_LEV_UART0   = 1,
    IRQ_LEV_DSP     = 1,
    IRQ_LEV_TIM16   = 2,
    IRQ_LEV_QSPI    = 1,
} ENUM_INTERRUPT_LEVEL;

#include <stdint.h>

typedef enum
{
    IRQ_OPT_ENABLE = 0,
    IRQ_OPT_DISABLE,
    IRQ_OPT_PENDING,
    IRQ_OPT_CLRPENDING,
} ENUM_INTERRUPT_OPERATE;

typedef enum
{
    WAIT_NONE = 0x20,
    WAIT_LSLEEP,
    WAIT_DSLEEP,
} ENUM_WAIT_TYPE;

typedef void (*IrqCallBack)(void);

void interrupt_cfg(IRQn_Type irq, uint8_t vectorMode, uint8_t priority, IrqCallBack cb);

void interrupt_operate(IRQn_Type irq, ENUM_INTERRUPT_OPERATE opt);

uint16_t interrupt_wait(uint16_t irqs, ENUM_WAIT_TYPE waitType);

void interrupt_recode(IRQn_Type irq);

void interrupt_clear(uint16_t irqs);

#endif
