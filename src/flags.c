#include "flags.h"
#include "keycodes.h"
#include <stdio.h>

void do_flags()
{
    char buf[4];
    uint8_t byt, bit;
    real_t real;
    uint16_t key;

    puts("Edit flags");
    puts("*** WARNING ***");
    puts("Only do this when you knowwhat you are doing!");
    os_GetStringInput("Byte?", buf, 4);
    os_NewLine();
    real = os_StrToReal(buf, NULL);
    byt = os_RealToInt24(&real);
    os_GetStringInput("Bit?", buf, 2);
    os_NewLine();
    real = os_StrToReal(buf, NULL);
    bit = os_RealToInt24(&real);
    os_PutStrFull("Current:");
    puts(flag_get(byt, bit) ? "1" : "0");
    puts("Toggle?");
    key = os_GetKey();
    if (key == kClear)
        return;
    flag_toggle(byt, bit);
    os_PutStrFull("Now:");
    os_PutStrFull(flag_get(byt, bit) ? "1" : "0");
    os_GetKey();
}
