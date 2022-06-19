#ifndef _FLAGS_H
#define _FLAGS_H

#include <stdint.h>
#include <tice.h>

#define flag_get(byt, bit) (os_GetFlagByte(byt) & (1 << (bit)))
#define flag_toggle(byt, bit) (os_SetFlagByte(byt, os_GetFlagByte(byt) ^ (1 << (bit))))

void do_flags();

#endif