#include "keycodes.h"

char key2char(uint16_t key)
{
    if (key >= kLwrA && key <= kLwrZ)
        return key - kLwrA + 'a';
    if (key >= kCapA && key <= kCapZ)
        return key - kCapA + 'A';
    return 0;
}

int8_t key2int(uint16_t key)
{
    if (key > k9 || key < k0)
        return -1;
    return key - k0;
}

int8_t sk2int(sk_key_t key)
{
    switch (key)
    {
    case sk_0:
        return 0;
    case sk_1:
        return 1;
    case sk_2:
        return 2;
    case sk_3:
        return 3;
    case sk_4:
        return 4;
    case sk_5:
        return 5;
    case sk_6:
        return 6;
    case sk_7:
        return 7;
    case sk_8:
        return 8;
    case sk_9:
        return 9;
    }
    return -1;
}
