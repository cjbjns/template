OUTPUT_ARCH( "riscv" )
ENTRY( _start )
EXTERN( vector_base )
/* if run code in flash*/

MEMORY
{
    data_ram0 (rwxa) : ORIGIN = 0x20000000, LENGTH = 40K
    data_ram1 (rwxa) : ORIGIN = 0x2000A000, LENGTH = 8K
    params (rxa!w) : ORIGIN = 0x00003000, LENGTH = 2K
    flash (rxa!w) : ORIGIN  = 0x00003800, LENGTH = 492K
}

/* if run code in flash*/

/* if run code in ram*/
/* MEMORY
{
    data_ram0 (rwxa) : ORIGIN = 0x2000A000, LENGTH = 8K
    data_ram1 (rwxa) : ORIGIN = 0x2000C000, LENGTH = 1K
    flash (rxa!w) : ORIGIN = 0x20000000, LENGTH = 40K
} */
/* if run code in ram*/

REGION_ALIAS("DRAM", data_ram0)
REGION_ALIAS("DRAM1", data_ram1)
REGION_ALIAS("FLASH", flash)
REGION_ALIAS("PARAMS", params)

SECTIONS
{   
    .init :
    {
        KEEP(*(.init))
        . = ALIGN(8) ;
    } >FLASH AT>FLASH

    .text : ALIGN(8)
    {
        *(.text .text.*)
        . = ALIGN(8);
        *(.rodata .rodata.*)
        . = ALIGN(8); 
    } >FLASH AT>FLASH

	PROVIDE( _text_lma = LOADADDR(.text) );
	PROVIDE( _stext = ADDR(.text) );
	PROVIDE( _etext = . );

    .data : ALIGN(8)
    {
        /* vtable text_ram */
        *(.vtable)
        . = ALIGN(8);
        *(.text_ram .text_ram.*)
        . = ALIGN(8);
        *(.data_ram .data_ram.*)
        . = ALIGN(8);

        *(.data .data.*)
        . = ALIGN(8);
        PROVIDE( __global_pointer$ = . + 0x800 );
        *(.sdata .sdata.*)
        . = ALIGN(8);
        *(.srodata .srodata.*)
        . = ALIGN(8);
        PROVIDE( __tls_base = . );
        *(.tdata .tdata.*)
        . = ALIGN(8);
    } >DRAM AT>FLASH

    .data1 : ALIGN(8)
    {
        *(.data1)
        . = ALIGN(8);
    } >DRAM1

    PROVIDE( _data_lma = LOADADDR(.data) );
    PROVIDE( _sdata = ADDR(.data) );
    PROVIDE( _edata = . );

    .bss (NOLOAD) : ALIGN(8)
    {
        *(.tbss .tbss.*)
        . = ALIGN(8);
        *(.tcommon)
        . = ALIGN(8);
        PROVIDE( __tls_end = . );
        *(.sbss .sbss.*)
        . = ALIGN(8);
        *(.bss .bss.*)
        . = ALIGN(8);
        *(COMMON)
        . = ALIGN(8);
    } >DRAM AT>FLASH

    PROVIDE( _bss_start = ADDR(.bss) );
    PROVIDE( _bss_limit = . );
    PROVIDE( end = .);

    PROVIDE(_initial_sp = 0x2000A000 - 16);

    /* Nuclei C Runtime Library requirements:
     * 1. heap need to be align at 16 bytes
     * 2. __heap_start and __heap_end symbol need to be defined
     * 3. reserved at least __HEAP_SIZE space for heap
     */

    PROVIDE(__heap_start = . );
/*
    .heap (NOLOAD) : ALIGN(16)
    {
        . = ALIGN(16);
        PROVIDE( __heap_start = . );
        . += 1024;
        . = ALIGN(16);
        PROVIDE( __heap_limit = . );
    } >DRAM AT>DRAM

    .stack ORIGIN(DRAM) + LENGTH(DRAM) - __STACK_SIZE (NOLOAD) :
    {
        . = ALIGN(16);
        PROVIDE( __heap_end = . );
        PROVIDE( __StackLimit = . );
        PROVIDE( __StackBottom = . );
        . += __STACK_SIZE;
        . = ALIGN(16);
        PROVIDE( __StackTop = . );
        PROVIDE( _initial_sp = . );
    } >DRAM AT>DRAM
*/
}
