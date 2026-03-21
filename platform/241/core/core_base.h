#ifndef __SYSTEM_BASE_H__
#define __SYSTEM_BASE_H__

#include "riscv_encoding.h"
#include <stdint.h>

/* NMSIS compiler specific defines */
/** \brief Pass information from the compiler to the assembler. */
#define __ASM                                  __asm
/** \brief Recommend that function should be inlined by the compiler. */
#define __INLINE                               inline
/** \brief Define a static function that may be inlined by the compiler. */
#define __STATIC_INLINE                        static inline
/** \brief Define a static function that should be always inlined by the compiler. */
#define __STATIC_FORCEINLINE                   __attribute__((always_inline)) static inline
/** \brief Inform the compiler that a function does not return. */
#define __NO_RETURN                            __attribute__((__noreturn__))
/** \brief Inform that a variable shall be retained in executable image. */
#define __USED                                 __attribute__((used))
/** \brief restrict pointer qualifier to enable additional optimizations. */
#define __WEAK                                 __attribute__((weak))
/** \brief specified the vector size of the variable, measured in bytes */
#define __VECTOR_SIZE(x)                       __attribute__((vector_size(x)))
/** \brief Request smallest possible alignment. */
#define __PACKED                               __attribute__((packed, aligned(1)))
/** \brief Request smallest possible alignment for a structure. */
#define __PACKED_STRUCT                        struct __attribute__((packed, aligned(1)))
/** \brief Request smallest possible alignment for a union. */
#define __PACKED_UNION                         union __attribute__((packed, aligned(1)))
/** \brief Minimum `x` bytes alignment for a variable. */
#define __ALIGNED(x)                           __attribute__((aligned(x)))
/** \brief restrict pointer qualifier to enable additional optimizations. */
#define __RESTRICT                             __restrict
/** \brief Barrier to prevent compiler from reordering instructions. */
#define __COMPILER_BARRIER()                   __ASM volatile("":::"memory")
/** \brief provide the compiler with branch prediction information, the branch is usually true */
#define __USUALLY(exp)                         __builtin_expect((exp), 1)
/** \brief provide the compiler with branch prediction information, the branch is rarely true */
#define __RARELY(exp)                          __builtin_expect((exp), 0)
/** \brief Use this attribute to indicate that the specified function is an interrupt handler. */
#define __INTERRUPT                            __attribute__((interrupt))

/* IO definitions (access restrictions to peripheral registers) */
/** \brief Defines 'read only' permissions */
#define     __I     volatile const
/** \brief Defines 'write only' permissions */
#define     __O     volatile
/** \brief Defines 'read / write' permissions */
#define     __IO    volatile
/* following defines should be used for structure members */
/** \brief Defines 'read only' structure member permissions */
#define     __IM     volatile const
/** \brief Defines 'write only' structure member permissions */
#define     __OM     volatile
/** \brief Defines 'read/write' structure member permissions */
#define     __IOM    volatile

/**
 * \brief   Mask and shift a bit field value for use in a register bit range.
 * \details The macro \ref _VAL2FLD uses the #define's _Pos and _Msk of the related bit
 * field to shift bit-field values for assigning to a register.
 *
 * **Example**:
 * \code
 * ECLIC->CFG = _VAL2FLD(CLIC_CLICCFG_NLBIT, 3);
 * \endcode
 * \param[in] field  Name of the register bit field.
 * \param[in] value  Value of the bit field. This parameter is interpreted as an uint32_t type.
 * \return           Masked and shifted value.
 */
#define _VAL2FLD(field, value)    (((uint32_t)(value) << field ## _Pos) & field ## _Msk)

/**
 * \brief   Mask and shift a register value to extract a bit filed value.
 * \details The macro \ref _FLD2VAL uses the #define's _Pos and _Msk of the related bit
 * field to extract the value of a bit field from a register.
 *
 * **Example**:
 * \code
 * nlbits = _FLD2VAL(CLIC_CLICCFG_NLBIT, ECLIC->CFG);
 * \endcode
 * \param[in] field  Name of the register bit field.
 * \param[in] value  Value of register. This parameter is interpreted as an uint32_t type.
 * \return           Masked and shifted bit field value.
 */
#define _FLD2VAL(field, value)    (((uint32_t)(value) & field ## _Msk) >> field ## _Pos)

typedef uint32_t rv_csr_t;

/**
 * \brief CSR operation Macro for csrrw instruction.
 * \details
 * Read the content of csr register to __v,
 * then write content of val into csr register, then return __v
 * \param csr   CSR macro definition defined in
 *              \ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
 * \param val   value to store into the CSR register
 * \return the CSR register value before written
 */
#define __RV_CSR_SWAP(csr, val)                                 \
    ({                                                          \
        register rv_csr_t __v = (unsigned long)(val);           \
        __ASM volatile("csrrw %0, " STRINGIFY(csr) ", %1"       \
                     : "=r"(__v)                                \
                     : "rK"(__v)                                \
                     : "memory");                               \
        __v;                                                    \
    })

/**
 * \brief CSR operation Macro for csrr instruction.
 * \details
 * Read the content of csr register to __v and return it
 * \param csr   CSR macro definition defined in
 *              \ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
 * \return the CSR register value
 */
#define __RV_CSR_READ(csr)                                      \
    ({                                                          \
        register rv_csr_t __v;                                  \
        __ASM volatile("csrr %0, " STRINGIFY(csr)               \
                     : "=r"(__v)                                \
                     :                                          \
                     : "memory");                               \
        __v;                                                    \
    })

/**
 * \brief CSR operation Macro for csrw instruction.
 * \details
 * Write the content of val to csr register
 * \param csr   CSR macro definition defined in
 *              \ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
 * \param val   value to store into the CSR register
 */
#define __RV_CSR_WRITE(csr, val)                                \
    ({                                                          \
        register rv_csr_t __v = (rv_csr_t)(val);                \
        __ASM volatile("csrw " STRINGIFY(csr) ", %0"            \
                     :                                          \
                     : "rK"(__v)                                \
                     : "memory");                               \
    })

/**
 * \brief CSR operation Macro for csrrs instruction.
 * \details
 * Read the content of csr register to __v,
 * then set csr register to be __v | val, then return __v
 * \param csr   CSR macro definition defined in
 *              \ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
 * \param val   Mask value to be used wih csrrs instruction
 * \return the CSR register value before written
 */
#define __RV_CSR_READ_SET(csr, val)                             \
    ({                                                          \
        register rv_csr_t __v = (rv_csr_t)(val);                \
        __ASM volatile("csrrs %0, " STRINGIFY(csr) ", %1"       \
                     : "=r"(__v)                                \
                     : "rK"(__v)                                \
                     : "memory");                               \
        __v;                                                    \
    })

/**
 * \brief CSR operation Macro for csrs instruction.
 * \details
 * Set csr register to be csr_content | val
 * \param csr   CSR macro definition defined in
 *              \ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
 * \param val   Mask value to be used wih csrs instruction
 */
#define __RV_CSR_SET(csr, val)                                  \
    ({                                                          \
        register rv_csr_t __v = (rv_csr_t)(val);                \
        __ASM volatile("csrs " STRINGIFY(csr) ", %0"            \
                     :                                          \
                     : "rK"(__v)                                \
                     : "memory");                               \
    })

/**
 * \brief CSR operation Macro for csrrc instruction.
 * \details
 * Read the content of csr register to __v,
 * then set csr register to be __v & ~val, then return __v
 * \param csr   CSR macro definition defined in
 *              \ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
 * \param val   Mask value to be used wih csrrc instruction
 * \return the CSR register value before written
 */
#define __RV_CSR_READ_CLEAR(csr, val)                           \
    ({                                                          \
        register rv_csr_t __v = (rv_csr_t)(val);                \
        __ASM volatile("csrrc %0, " STRINGIFY(csr) ", %1"       \
                     : "=r"(__v)                                \
                     : "rK"(__v)                                \
                     : "memory");                               \
        __v;                                                    \
    })

/**
 * \brief CSR operation Macro for csrc instruction.
 * \details
 * Set csr register to be csr_content & ~val
 * \param csr   CSR macro definition defined in
 *              \ref NMSIS_Core_CSR_Registers, eg. \ref CSR_MSTATUS
 * \param val   Mask value to be used wih csrc instruction
 */
#define __RV_CSR_CLEAR(csr, val)                                \
    ({                                                          \
        register rv_csr_t __v = (rv_csr_t)(val);                \
        __ASM volatile("csrc " STRINGIFY(csr) ", %0"            \
                     :                                          \
                     : "rK"(__v)                                \
                     : "memory");                               \
    })


/**
 * \brief   Enable IRQ Interrupts
 * \details Enables IRQ interrupts by setting the MIE-bit in the MSTATUS Register.
 * \remarks
 *          Can only be executed in Privileged modes.
 */
__STATIC_FORCEINLINE void __enable_irq(void)
{
    __RV_CSR_SET(CSR_MSTATUS, MSTATUS_MIE);
}

/**
 * \brief   Disable IRQ Interrupts
 * \details Disables IRQ interrupts by clearing the MIE-bit in the MSTATUS Register.
 * \remarks
 *          Can only be executed in Privileged modes.
 */
__STATIC_FORCEINLINE void __disable_irq(void)
{
    __RV_CSR_CLEAR(CSR_MSTATUS, MSTATUS_MIE);
}


/**
 * \brief   NOP Instruction
 * \details
 * No Operation does nothing.
 * This instruction can be used for code alignment purposes.
 */
__STATIC_FORCEINLINE void __NOP(void)
{
    __ASM volatile("nop");
}

/**
 * \brief   Wait For Interrupt
 * \details
 * Wait For Interrupt is is executed using CSR_WFE.WFE=0 and WFI instruction.
 * It will suspends execution until interrupt, NMI or Debug happened.
 * When Core is waked up by interrupt, if
 * 1. mstatus.MIE == 1(interrupt enabled), Core will enter ISR code
 * 2. mstatus.MIE == 0(interrupt disabled), Core will resume previous execution
 */
__STATIC_FORCEINLINE void __WFI(void)
{
    __RV_CSR_CLEAR(CSR_WFE, WFE_WFE);
    __ASM volatile("wfi");
}

/**
 * \brief   Wait For Event
 * \details
 * Wait For Event is executed using CSR_WFE.WFE=1 and WFI instruction.
 * It will suspends execution until event, NMI or Debug happened.
 * When Core is waked up, Core will resume previous execution
 */
__STATIC_FORCEINLINE void __WFE(void)
{
    __RV_CSR_SET(CSR_WFE, WFE_WFE);
    __ASM volatile("wfi");
    __RV_CSR_CLEAR(CSR_WFE, WFE_WFE);
}

/**
 * \brief   Breakpoint Instruction
 * \details
 * Causes the processor to enter Debug state.
 * Debug tools can use this to investigate system state
 * when the instruction at a particular address is reached.
 */
__STATIC_FORCEINLINE void __EBREAK(void)
{
    __ASM volatile("ebreak");
}

/**
 * \brief   Environment Call Instruction
 * \details
 * The ECALL instruction is used to make a service request to
 * the execution environment.
 */
__STATIC_FORCEINLINE void __ECALL(void)
{
    __ASM volatile("ecall");
}

/**
 * \brief WFI Sleep Mode enumeration
 */
typedef enum WFI_SleepMode {
    WFI_SHALLOW_SLEEP = 0,      /*!< Shallow sleep mode, the core_clk will poweroff */
    WFI_DEEP_SLEEP = 1          /*!< Deep sleep mode, the core_clk and core_ano_clk will poweroff */
} WFI_SleepMode_Type;

/**
 * \brief   Set Sleep mode of WFI
 * \details
 * Set the SLEEPVALUE CSR register to control the
 * WFI Sleep mode.
 * \param[in] mode      The sleep mode to be set
 */
__STATIC_FORCEINLINE void __set_wfi_sleepmode(WFI_SleepMode_Type mode)
{
    __RV_CSR_WRITE(CSR_SLEEPVALUE, mode);
}

/**
 * \brief   Send TX Event
 * \details
 * Set the CSR TXEVT to control send a TX Event.
 * The Core will output signal tx_evt as output event signal.
 */
__STATIC_FORCEINLINE void __TXEVT(void)
{
    __RV_CSR_SET(CSR_TXEVT, 0x1);
}


/**
 * \brief Execute fence instruction, p -> pred, s -> succ
 * \details
 * the FENCE instruction ensures that all memory accesses from instructions preceding
 * the fence in program order (the `predecessor set`) appear earlier in the global memory order than
 * memory accesses from instructions appearing after the fence in program order (the `successor set`).
 * For details, please refer to The RISC-V Instruction Set Manual
 * \param p     predecessor set, such as iorw, rw, r, w
 * \param s     successor set, such as iorw, rw, r, w
 **/
#define __FENCE(p, s) __ASM volatile ("fence " #p "," #s : : : "memory")

/**
 * \brief   Fence.i Instruction
 * \details
 * The FENCE.I instruction is used to synchronize the instruction
 * and data streams.
 */
__STATIC_FORCEINLINE void __FENCE_I(void)
{
    __ASM volatile("fence.i");
}

/** \brief Read & Write Memory barrier */
#define __RWMB()        __FENCE(iorw,iorw)

/** \brief Read Memory barrier */
#define __RMB()         __FENCE(ir,ir)

/** \brief Write Memory barrier */
#define __WMB()         __FENCE(ow,ow)

/** \brief SMP Read & Write Memory barrier */
#define __SMP_RWMB()    __FENCE(rw,rw)

/** \brief SMP Read Memory barrier */
#define __SMP_RMB()     __FENCE(r,r)

/** \brief SMP Write Memory barrier */
#define __SMP_WMB()     __FENCE(w,w)

/** \brief CPU relax for busy loop */
#define __CPU_RELAX()   __ASM volatile ("" : : : "memory")



/* ===== Load/Store Operations ===== */
/**
 * \brief  Load 8bit value from address (8 bit)
 * \details Load 8 bit value.
 * \param [in]    addr  Address pointer to data
 * \return              value of type uint8_t at (*addr)
 */
__STATIC_FORCEINLINE uint8_t __LB(volatile void *addr)
{
    uint8_t result;

    __ASM volatile ("lb %0, 0(%1)" : "=r" (result) : "r" (addr));
    return result;
}

/**
 * \brief  Load 16bit value from address (16 bit)
 * \details Load 16 bit value.
 * \param [in]    addr  Address pointer to data
 * \return              value of type uint16_t at (*addr)
 */
__STATIC_FORCEINLINE uint16_t __LH(volatile void *addr)
{
    uint16_t result;

    __ASM volatile ("lh %0, 0(%1)" : "=r" (result) : "r" (addr));
    return result;
}

/**
 * \brief  Load 32bit value from address (32 bit)
 * \details Load 32 bit value.
 * \param [in]    addr  Address pointer to data
 * \return              value of type uint32_t at (*addr)
 */
__STATIC_FORCEINLINE uint32_t __LW(volatile void *addr)
{
    uint32_t result;

    __ASM volatile ("lw %0, 0(%1)" : "=r" (result) : "r" (addr));
    return result;
}

/**
 * \brief  Write 8bit value to address (8 bit)
 * \details Write 8 bit value.
 * \param [in]    addr  Address pointer to data
 * \param [in]    val   Value to set
 */
__STATIC_FORCEINLINE void __SB(volatile void *addr, uint8_t val)
{
    __ASM volatile ("sb %0, 0(%1)" : : "r" (val), "r" (addr));
}

/**
 * \brief  Write 16bit value to address (16 bit)
 * \details Write 16 bit value.
 * \param [in]    addr  Address pointer to data
 * \param [in]    val   Value to set
 */
__STATIC_FORCEINLINE void __SH(volatile void *addr, uint16_t val)
{
    __ASM volatile ("sh %0, 0(%1)" : : "r" (val), "r" (addr));
}

/**
 * \brief  Write 32bit value to address (32 bit)
 * \details Write 32 bit value.
 * \param [in]    addr  Address pointer to data
 * \param [in]    val   Value to set
 */
__STATIC_FORCEINLINE void __SW(volatile void *addr, uint32_t val)
{
    __ASM volatile ("sw %0, 0(%1)" : : "r" (val), "r" (addr));
}

/**
 * \brief  Compare and Swap 32bit value using LR and SC
 * \details Compare old value with memory, if identical,
 * store new value in memory. Return the initial value in memory.
 * Success is indicated by comparing return value with OLD.
 * memory address, return 0 if successful, otherwise return !0
 * \param [in]    addr      Address pointer to data, address need to be 4byte aligned
 * \param [in]    oldval    Old value of the data in address
 * \param [in]    newval    New value to be stored into the address
 * \return  return the initial value in memory
 */
__STATIC_FORCEINLINE uint32_t __CAS_W(volatile uint32_t *addr, uint32_t oldval, uint32_t newval)
{
    register uint32_t result;
    register uint32_t rc;

    __ASM volatile (                                \
            "0:     lr.w %0, %2      \n"            \
            "       bne  %0, %z3, 1f \n"            \
            "       sc.w %1, %z4, %2 \n"            \
            "       bnez %1, 0b      \n"            \
            "1:\n"                                  \
            : "=&r"(result), "=&r"(rc), "+A"(*addr) \
            : "r"(oldval), "r"(newval)              \
            : "memory");
    return result;
}


#endif /* __SYSTEM_BASE_H__ */
