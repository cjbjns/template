#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "interrupt_handles.h"

#define BIT(x)  (1U << (x))

#define MAX_IRQ_CNT (14)

volatile static IrqCallBack irqCbs[MAX_IRQ_CNT];
volatile static uint8_t irqHappened[MAX_IRQ_CNT];

__SECTION(.text_ram) void core_exception_handler(uint32_t mcause, uint32_t sp)
{
    while(1);
}

void SystemInit(void) 
{
    ECLIC_SetMth(0);
    ECLIC_SetCfgNlbits(__ECLIC_INTCTLBITS);

    __enable_irq();
}

void interrupt_cfg(IRQn_Type irq, uint8_t vectorMode, uint8_t level, IrqCallBack cb)
{
    if ((irq - LVD_IRQn) > MAX_IRQ_CNT) 
        {return;}

    if(vectorMode == 0)
        ECLIC_SetShvIRQ(irq, 0);
    else
        ECLIC_SetShvIRQ(irq, 1);

    ECLIC_SetTrigIRQ(irq, ECLIC_LEVEL_TRIGGER);

    ECLIC_SetLevelIRQ(irq, level); //设置中断等级，决定中断嵌套逻辑

    ECLIC_SetPriorityIRQ(irq, 0);

    if (cb != NULL)
        ECLIC_SetVector(irq, (rv_csr_t)cb);
}

void interrupt_operate(IRQn_Type irq, ENUM_INTERRUPT_OPERATE opt)
{
    switch (opt)
    {
    case IRQ_OPT_ENABLE:
        if(irq == 0)
            __enable_irq();
        else
            ECLIC_EnableIRQ(irq);        
        break;
    case IRQ_OPT_DISABLE:
        if(irq == 0)
            __disable_irq();
        else
            ECLIC_DisableIRQ(irq);
        break;
    default:
        break;
    }
}

uint16_t interrupt_wait(uint16_t irqs, ENUM_WAIT_TYPE waitType)
{
    uint16_t irqMask = 0;

    if(waitType == WAIT_LSLEEP)
        __set_wfi_sleepmode(WFI_SHALLOW_SLEEP);
    else if(waitType == WAIT_DSLEEP)
        __set_wfi_sleepmode(WFI_DEEP_SLEEP);

    do{
        irqMask = 0x00;
        for(uint8_t i=0;i<MAX_IRQ_CNT;i++)
        {
            if(irqHappened[i])
                irqMask |= BIT(i);
        }
        
        if(irqMask & irqs)
            break;

        if (waitType != WAIT_NONE)
            __WFI();
        
    }while(1);

    return (irqMask & irqs);
}

void interrupt_recode(IRQn_Type irq)
{
    irqHappened[irq - LVD_IRQn]++; 
}

void interrupt_clear(uint16_t irqs)
{
    for(uint8_t i=0;i<MAX_IRQ_CNT;i++)
    {
        if(irqs & BIT(i))
            irqHappened[i] = 0;   
    }
}
