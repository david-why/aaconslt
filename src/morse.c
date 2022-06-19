#include "morse.h"
#include "menu.h"
#include "keycodes.h"
#include <tice.h>

#define _T(c, l, b) [#c[0]] = ((0b##b) << 3) + (l)

const uint8_t morse_table[] = {
    _T(A, 2, 01),
    _T(B, 4, 1000),
    _T(C, 4, 1010),
    _T(D, 3, 100),
    _T(E, 1, 0),
    _T(F, 4, 0010),
    _T(G, 3, 110),
    _T(H, 4, 0000),
    _T(I, 2, 00),
    _T(J, 4, 0111),
    _T(K, 3, 101),
    _T(L, 4, 0100),
    _T(M, 2, 11),
    _T(N, 2, 10),
    _T(O, 3, 111),
    _T(P, 4, 0110),
    _T(Q, 4, 1101),
    _T(R, 3, 010),
    _T(S, 3, 000),
    _T(T, 1, 1),
    _T(U, 3, 001),
    _T(V, 4, 0001),
    _T(W, 3, 011),
    _T(X, 4, 1001),
    _T(Y, 4, 1011),
    _T(Z, 4, 1100),
    _T(1, 5, 01111),
    _T(2, 5, 00111),
    _T(3, 5, 00011),
    _T(4, 5, 00001),
    _T(5, 5, 00000),
    _T(6, 5, 10000),
    _T(7, 5, 11000),
    _T(8, 5, 11100),
    _T(9, 5, 11110),
    _T(0, 5, 11111),
};
const uint8_t MORSE_TABLE_LENGTH = sizeof(morse_table) / sizeof(morse_table[0]);

void morse_dispchar(char c)
{
    char buf[4] = {'<', 0, '>', 0};
    uint8_t v = MORSE_CHAR(c);
    if (!v)
    {
        buf[1] = c;
        os_PutStrFull(buf);
        return;
    }
    for (uint8_t i = 0; i < MORSE_LENGTH(v); i++)
        buf[0] = MORSE_DATA_AT(v, i) ? '-' : '.', os_PutStrFull(buf);
}

static const char *morse_choices[] = {"Encode", "Decode"};

static const char *morse_gettext(const void *data) { return data; }

static void do_morse_encode()
{
    char buf[2] = {0, 0};

    os_ClrHome();
    while (1)
    {
        uint16_t key = os_GetKey();
        if (key == kClear)
            break;
        char c = key2char(key);
        if (!c)
        {
            c = key2int(key);
            if (c < 0)
                continue;
            c += '0';
        }
        else if (c >= 'a' && c <= 'z')
            c = c - 'a' + 'A';
        buf[0] = c;
        os_PutStrFull(buf);
        os_PutStrFull(": ");
        morse_dispchar(c);
        os_NewLine();
    }
}

static void do_morse_decode()
{
}

void do_morse()
{
morse_begin:;
    int sel = select_menu("MORSE", morse_choices, 2, sizeof(const char *), morse_gettext);
    switch (sel)
    {
    case 0:
        do_morse_encode();
        goto morse_begin;
    case 1:
        do_morse_decode();
        goto morse_begin;
    case -1:
        return;
    }
}
