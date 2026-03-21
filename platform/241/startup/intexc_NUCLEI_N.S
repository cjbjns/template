#include "riscv_encoding.h"

/**
 * 关闭全局中断使能
 */
.macro DISABLE_MIE
    csrc CSR_MSTATUS, MSTATUS_MIE
.endm

/**
 * 保存ABI调用上下文regfile到堆栈
*/
/* Save caller registers */
.macro SAVE_CONTEXT
    /* Allocate stack space for context saving */
    addi sp, sp, -20*REGBYTES

    STORE x1, 0*REGBYTES(sp)
    STORE x4, 1*REGBYTES(sp)
    STORE x5, 2*REGBYTES(sp)
    STORE x6, 3*REGBYTES(sp)
    STORE x7, 4*REGBYTES(sp)
    STORE x10, 5*REGBYTES(sp)
    STORE x11, 6*REGBYTES(sp)
    STORE x12, 7*REGBYTES(sp)
    STORE x13, 8*REGBYTES(sp)
    STORE x14, 9*REGBYTES(sp)
    STORE x15, 10*REGBYTES(sp)
    STORE x16, 14*REGBYTES(sp)
    STORE x17, 15*REGBYTES(sp)
    STORE x28, 16*REGBYTES(sp)
    STORE x29, 17*REGBYTES(sp)
    STORE x30, 18*REGBYTES(sp)
    STORE x31, 19*REGBYTES(sp)
.endm

/**
 * 从堆栈恢复ABI调用上下文到regfile
 */
/* Restore caller registers */
.macro RESTORE_CONTEXT
    LOAD x1, 0*REGBYTES(sp)
    LOAD x4, 1*REGBYTES(sp)
    LOAD x5, 2*REGBYTES(sp)
    LOAD x6, 3*REGBYTES(sp)
    LOAD x7, 4*REGBYTES(sp)
    LOAD x10, 5*REGBYTES(sp)
    LOAD x11, 6*REGBYTES(sp)
    LOAD x12, 7*REGBYTES(sp)
    LOAD x13, 8*REGBYTES(sp)
    LOAD x14, 9*REGBYTES(sp)
    LOAD x15, 10*REGBYTES(sp)
    LOAD x16, 14*REGBYTES(sp)
    LOAD x17, 15*REGBYTES(sp)
    LOAD x28, 16*REGBYTES(sp)
    LOAD x29, 17*REGBYTES(sp)
    LOAD x30, 18*REGBYTES(sp)
    LOAD x31, 19*REGBYTES(sp)

    /* De-allocate the stack space */
    addi sp, sp, 20*REGBYTES
.endm

/**
 * 保存CSR内容到堆栈（MCAUSE, MEPC, MSUBM）
 */
.macro SAVE_CSR_CONTEXT
    /* Store CSR mcause to stack using pushmcause */
    csrrwi  x0, CSR_PUSHMCAUSE, 11
    /* Store CSR mepc to stack using pushmepc */
    csrrwi  x0, CSR_PUSHMEPC, 12
    /* Store CSR msub to stack using pushmsub */
    csrrwi  x0, CSR_PUSHMSUBM, 13
.endm

/**
 * 从堆栈恢复CSR内容（MCAUSE, MEPC, MSUBM）
 */
.macro RESTORE_CSR_CONTEXT
    LOAD x5,  13*REGBYTES(sp)
    csrw CSR_MSUBM, x5
    LOAD x5,  12*REGBYTES(sp)
    csrw CSR_MEPC, x5
    LOAD x5,  11*REGBYTES(sp)
    csrw CSR_MCAUSE, x5
.endm

/**
 * Exception/NMI Entry
 */
.section .text_ram.trap
/* In CLIC mode, the exeception entry must be 64bytes aligned */
.align 6
.global exc_entry
.weak exc_entry
exc_entry:
    SAVE_CONTEXT
    SAVE_CSR_CONTEXT

    /*
     * Set the exception handler function arguments
     * argument 1: mcause value
     * argument 2: current stack point(SP) value
     */
    csrr a0, mcause
    mv a1, sp
    /*
     * TODO: Call the exception handler function
     * By default, the function template is provided in
     * system_Device.c, you can adjust it as you want
     */
    call core_exception_handler

    RESTORE_CSR_CONTEXT
    RESTORE_CONTEXT

    /* Return to regular code */
    mret

/**
 * Non-Vector Interrupt Entry
 */
.section      .text_ram.irq
/* In CLIC mode, the interrupt entry must be 4bytes aligned */
.align 2
.global irq_entry
.weak irq_entry
/* This label will be set to MTVT2 register */
irq_entry:
    SAVE_CONTEXT
    SAVE_CSR_CONTEXT

    /* This special CSR read/write operation, which is actually
     * claim the CLIC to find its pending highest ID, if the ID
     * is not 0, then automatically enable the mstatus.MIE, and
     * jump to its vector-entry-label, and update the link register
     */
    csrrw ra, CSR_JALMNXTI, ra

    DISABLE_MIE

    RESTORE_CSR_CONTEXT
    RESTORE_CONTEXT

    /* Return to regular code */
    mret

/* Default Handler for Exceptions / Interrupts */
.global default_intexc_handler
.weak default_intexc_handler
Undef_Handler:
default_intexc_handler:
1:
    j 1b
