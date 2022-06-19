#ifndef _PRINT_H
#define _PRINT_H

typedef enum
{
    print_int,
    print_float,
    print_real,
    print_hex_arr
} print_type_t;

void print(print_type_t type, ...);

#endif