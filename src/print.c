#include "print.h"
#include <stdarg.h>
#include <tice.h>
#include <stdio.h>

void print(print_type_t type, ...)
{
    va_list ap;
    real_t real;
    const void *ptr;
    int num;
    char buf[15];

    va_start(ap, type);
    switch (type)
    {
    case print_int:
        real = os_Int24ToReal(va_arg(ap, int));
        os_RealToStr(buf, &real, 0, 0, -1);
        os_PutStrFull(buf);
        break;
    case print_float:
        real = os_FloatToReal(va_arg(ap, double));
        os_RealToStr(buf, &real, 0, 0, -1);
        os_PutStrFull(buf);
        break;
    case print_real:
        os_RealToStr(buf, va_arg(ap, const real_t *), 0, 0, -1);
        os_PutStrFull(buf);
        break;
    case print_hex_arr:
        ptr = va_arg(ap, const void *);
        num = va_arg(ap, unsigned int);
        while (num--)
            outchar("0123456789ABCDEF"[(*(const uint8_t *)ptr >> 4) % 16]),
                outchar("0123456789ABCDEF"[*(const uint8_t *)ptr % 16]),
                ptr++;
        break;
    }
    va_end(ap);
}
