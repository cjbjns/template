#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#include "core_base.h"

struct stat;
// https://sourceware.org/newlib/libc.html#Stubs

/*
    需要重写一些基础函数
 */
void drv_uart_output(char data);

__WEAK __USED ssize_t _write(int fd, const void* ptr, size_t len)
{
    if (ptr != NULL) {
        const uint8_t* _buf = ptr;
        for (uint32_t i = 0; i < len; i ++) {
            drv_uart_output(_buf[i]);
        }
    }
    return len;
}

__WEAK __USED ssize_t _read(int fd, void* ptr, size_t len)
{
    return -1;
}

__WEAK __USED void* _sbrk(ptrdiff_t incr)
{
    extern char __heap_start[];
    static char* curbrk = __heap_start;

    if (curbrk + incr < __heap_start) {
        return (void*)(-1);
    }

    curbrk += incr;
    return (void*)(curbrk - incr);
}

__WEAK __USED int _isatty(int fd)
{
    return 1;
}

__WEAK __USED int _fstat(int file, struct stat* st)
{
    return -1;
}

__WEAK __USED int _lseek(int file, int offset, int whence)
{
    return 0;
}

__WEAK __USED int _close(int fd)
{
    return -1;
}
