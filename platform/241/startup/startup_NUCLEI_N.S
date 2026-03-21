#include "riscv_encoding.h"

    .section .vtable

    .weak eclic_msip_handler
    .weak eclic_mtip_handler
    .weak QSPI_IRQHandler

    .globl vector_base
    .type vector_base, @object
vector_base:
    .word   default_intexc_handler          /* 0: Reserved */
    .word   default_intexc_handler          /* 1: Reserved */
    .word   default_intexc_handler          /* 2: Reserved */
    .word   eclic_msip_handler              /* 3: Machine software interrupt */

    .word   default_intexc_handler          /* 4: Reserved */
    .word   default_intexc_handler          /* 5: Reserved */
    .word   default_intexc_handler          /* 6: Reserved */
    .word   eclic_mtip_handler              /* 7: Machine timer interrupt */

    .word   default_intexc_handler          /* 8: Reserved */
    .word   default_intexc_handler          /* 9: Reserved */
    .word   default_intexc_handler          /* 10: Reserved */
    .word   default_intexc_handler          /* 11: Reserved */

    .word   default_intexc_handler          /* 12: Reserved */
    .word   default_intexc_handler          /* 13: Reserved */
    .word   default_intexc_handler          /* 14: Reserved */
    .word   default_intexc_handler          /* 15: Reserved */

    .word   default_intexc_handler          /* 16: Reserved */
    .word   default_intexc_handler          /* 17: Reserved */
    .word   default_intexc_handler          /* 18: Reserved */
    .word   default_intexc_handler//LVD_IRQHandler                  /* 19: Interrupt 19 */

    .word   default_intexc_handler//I3C0_IRQHandler                  /* 20: Interrupt 20 */
    .word   default_intexc_handler//SPI0_IRQHandler                 /* 21: Interrupt 21 */
    .word   default_intexc_handler//I2C0_IRQHandler                 /* 22: Interrupt 22 */
    .word   default_intexc_handler//CSX_IRQHandler                  /* 23: Interrupt 23 */

    .word   default_intexc_handler//SlpTim_IRQHandler               /* 24: Interrupt 24 */
    .word   default_intexc_handler//GPIO_IRQHandler                 /* 25: Interrupt 25 */
    .word   default_intexc_handler//VC3_IRQHandler                  /* 26: Interrupt 26 */
    .word   default_intexc_handler//DMA_IRQHandler                  /* 27: Interrupt 27 */

    .word   default_intexc_handler//UART0_IRQHandler                /* 28: Interrupt 28 */
    .word   default_intexc_handler//DSP_IRQHandler                  /* 29: Interrupt 29 */
    .word   default_intexc_handler//TIM16_IRQHandler                /* 30: Interrupt 30 */
    .word   default_intexc_handler//QSPI_IRQHandler                 /* 31: Interrupt 31 */

    .word   default_intexc_handler//APU_IRQHandler                  /* 32: Interrupt 32 */

    .section .init

    .globl _start
    .type _start, @function
    
/**
 * Reset Handler called on controller reset
 */
_start:
    /* ===== Startup Stage 1 ===== */
    /* Disable Global Interrupt */
    csrc CSR_MSTATUS, MSTATUS_MIE

    /* Initialize GP and TP */
    .option push // push pop 用于临时保存、恢复option指定的选项
    .option norelax // 禁止链接器松弛
    la gp, __global_pointer$ // 全局指针 符号名称链接器定义 relax +-2K
    la tp, __tls_base // 线程本地存储指针
    .option pop

    /* Set correct sp for current cpu */
    la sp, _initial_sp

    /* 配置NMI使用异常入口函数地址（MTVEC） 否则进入reset_vector地址
     */
    li t0, MMISC_CTL_NMI_CAUSE_FFF
    csrs CSR_MMISC_CTL, t0

    /* MTVT保存ECLIC向量模式中断源的中断向量表基地址
     * 对齐-表长： 64byte-16 128byte-32 ..
     */
    la t0, vector_base
    csrw CSR_MTVT, t0

    /* MTVT2保存ECLIC非向量模式中断源的中断入口函数地址 // 4byte对齐
     * 低1位配置非向量中断和异常为共用入口函数
     */
    la t0, irq_entry
    csrw CSR_MTVT2, t0
    csrs CSR_MTVT2, 0x1

    /* MTVEC保存异常入口函数地址 // 4byte对齐
     * 低2位控制中断模式 CLINT/ECLIC
     * CLINT模式下所有trap和interrupt都会进入MTVEC保存的函数地址
     */
    la t0, early_exc_entry
    csrw CSR_MTVEC, t0

    /* Set the interrupt processing mode to ECLIC mode */
    li t0, 0x3f
    csrc CSR_MTVEC, t0
    csrs CSR_MTVEC, 0x3

    /* ===== Startup Stage 2 ===== */

    /* Enable mcycle and minstret counter */
    //csrci CSR_MCOUNTINHIBIT, 0x5

    /* enable I-CACHE to speed up ram init */
//#ifdef CACHE_EN
//    li t0, MCACHE_CTL_IC_EN
//    csrs CSR_MCACHE_CTL, t0
//#endif

 li a0,0x40000006 
 li a1,0xACAC
 sh a1,(a0)
 li a0,0x40000000 //enable cache en
 li a1,0x2000
 sw a1,(a0)
 li a0,0x40000006 //disable pram key
 li a1,0xCACA
 sh a1,(a0)
 li a0,0x00080000
 li a1,0x40000002
 li a4,0x0000
 1:
 lw a2,(a0)
 lh a3,(a1)
 andi a5,a3,0x0100
 beq a4,a5, 1b
 2:

__init_common:
    /* ===== Startup Stage 3 ===== */
    /* Load text section */
    la a0, _text_lma
    la a1, _stext
    /* If text LMA = VMA, no need to copy */
    beq a0, a1, 2f
    la a2, _etext
    bgeu a1, a2, 2f

1:
    lw t0, (a0)
    sw t0, (a1)
    addi a0, a0, 4
    addi a1, a1, 4
    bltu a1, a2, 1b
2:
    /* Load data section */
    la a0, _data_lma
    la a1, _sdata
    /* If data LMA = VMA, no need to copy */
    beq a0, a1, 2f
    la a2, _edata
    bgeu a1, a2, 2f
1:
    lw t0, (a0)
    sw t0, (a1)
    addi a0, a0, 4
    addi a1, a1, 4
    bltu a1, a2, 1b
2:
    /* Clear bss section */
    la a0, _bss_start
    la a1, _bss_limit
    bgeu a0, a1, 2f
1:
    sw zero, (a0)
    addi a0, a0, 4
    bltu a0, a1, 1b
2:

.globl _start_premain
.type _start_premain, @function
_start_premain:
    /*
     * Call vendor defined SystemInit to
     * initialize the micro-controller system
     * SystemInit will just be called by boot cpu
     */
    call SystemInit

    ///* Call global constructors */
    //la a0, __libc_fini_array
    //call atexit
    ///* Call C/C++ constructor start up code */
    //call __libc_init_array // 全局变量和静态变量的构造函数（C++）

__skip_init:
    /* Sync all harts at this function */
    //call __sync_harts

    /* do pre-init steps before main */
    /* _premain_init will be called by each cpu
     * please make sure the implementation of __premain_int
     * considered this
     */
    //call _premain_init

    /*
     * When all initialization steps done
     * set exception entry to correct exception
     * entry and jump to main.
     * And set the interrupt processing mode to
     * ECLIC mode
     */
    la t0, exc_entry
    csrw CSR_MTVEC, t0
    li t0, 0x3f
    csrc CSR_MTVEC, t0
    csrs CSR_MTVEC, 0x3

    /* BPU cold bringup need time, so enable BPU before enter to main */
    li t0, MMISC_CTL_BPU
    csrs CSR_MMISC_CTL, t0

    /* ===== Call SMP Main Function  ===== */
    /* argc = argv = 0 */
    li a0, 0
    li a1, 0

    call main
    
    /* do post-main steps after main
     * this function will be called by each cpu */
    //call _postmain_fini

1:
    j 1b

/* Early boot exception entry before main */
.align 6
.global early_exc_entry
.type early_exc_entry, @function
early_exc_entry:
    wfi
    j early_exc_entry
p_var:
    .balign 0x200
    .word 0xCACAA540
