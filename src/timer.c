#include "timer.h"
#include "print.h"
#include "keycodes.h"
#include <tice.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

static void printint(int i, int minlen)
{
    real_t r;
    int slen;
    char buf[16];

    r = os_Int24ToReal(i);
    slen = os_RealToStr(buf, &r, 0, 0, 0);
    if (slen < minlen)
    {
        memmove(buf + minlen - slen, buf, slen + 1);
        for (int i = 0; i < minlen - slen; i++)
            buf[i] = '0';
    }
    os_PutStrFull(buf);
}

void print_time(uint32_t tim, uint8_t row, uint8_t col)
{
    os_SetCursorPos(row, col);
    printint(tim / 10 / 60 / 60, 2);
    os_PutStrLine(":");
    printint(tim / 10 / 60 % 60, 2);
    os_PutStrLine(":");
    printint(tim / 10 % 60, 2);
    os_PutStrLine(".");
    printint(tim % 10, 1);
}

uint32_t time_adddigit(uint32_t secs, uint8_t val)
{
    uint8_t h, m, s, h2, m2, s2;

    s = secs % 60;      // seconds
    m = secs / 60 % 60; // minutes
    h = secs / 60 / 60; // hours
    s2 = s % 10 * 10 + val;
    m2 = s / 10 + m % 10 * 10;
    h2 = m / 10 + h * 10;
    if (s2 >= 60)
    {
        m2++;
        s2 -= 60;
        if (m2 >= 60)
        {
            h2++;
            m2 -= 60;
            if (h2 > 10)
                return secs;
        }
    }
    return (uint32_t)s2 + (uint32_t)m2 * 60 + (uint32_t)h2 * 60 * 60;
}

void do_timer()
{
    uint8_t started;
    uint8_t mode = 0;
    int8_t val;
    sk_key_t key;
    uint32_t tim;
    unsigned int r, c;

    puts("Stopwatch");
    puts("enter=Start/Stop");
    puts("vars=Reset");
    os_GetCursorPos(&r, &c);
    started = 0;
    timer_Enable(1, TIMER_32K, TIMER_NOINT, TIMER_UP);
    timer_Disable(1);
    timer_Set(1, 0);
    timer_SetReload(1, 0);
    while (true)
    {
        key = os_GetCSC();
        switch (key)
        {
        case sk_Enter:
            started = 1 - started;
            if (started)
            {
                if (mode == 0)
                    timer_Enable(1, TIMER_32K, TIMER_NOINT, TIMER_UP);
                else
                    timer_Enable(1, TIMER_32K, TIMER_0INT, TIMER_DOWN);
            }
            else
                timer_Disable(1);
            break;
        case sk_Clear:
            timer_Disable(1);
            os_NewLine();
            return;
        case sk_Vars:
            if (mode == 1)
                timer_Disable(1);
            timer_Set(1, 0);
            break;
        case sk_Mode:
            os_NewLine();
            timer_Disable(1);
            timer_Set(1, 0);
            if (mode == 0)
            {
                mode = 1;
                puts("Countdown");
                puts("enter=Start/Stop");
                puts("vars=Reset 0-9=Set");
                tim = 0;
            }
            else
            {
                mode = 0;
                puts("Stopwatch");
                puts("enter=Start/Stop");
                puts("vars=Reset");
            }
            os_GetCursorPos(&r, &c);
            break;
        case sk_Del:
            timer_Disable(1);
            timer_SetReload(1, tim = 0);
            timer_Set(1, 0);
            break;
        default:
            if (mode != 1)
                break;
            if ((val = sk2int(key)) == -1)
                break;
            timer_Disable(1);
            timer_Set(1, (tim = time_adddigit(timer_Get(1) / 32768, val)) * 32768);
            timer_SetReload(1, tim * 32768);
            break;
        }
        print_time(timer_Get(1) * 10 / 32768, r, c);
        if (timer_ChkInterrupt(1, TIMER_RELOADED))
            timer_Disable(1), timer_AckInterrupt(1, TIMER_RELOADED), started = 0, timer_Set(1, timer_GetReload(1));
    }
}
