#include "mines.h"
#include "pwdctl.h"
#include "keycodes.h"
#include <string.h>
#include <tice.h>
#include <stdio.h>
#include "print.h"

static uint16_t pwd[] = {kLwrM, kLwrI, kLwrN, kLwrE, kLwrS, k0};

#define ABS(x) ((x) < 0 ? (-(x)) : (x))

static uint8_t calc_num(uint8_t x, uint8_t y, uint8_t mines[N_MINES][2])
{
    uint8_t i, ans;

    ans = 0;
    for (i = 0; i < N_MINES; i++)
        if (ABS(mines[i][0] - x) <= 1 && ABS(mines[i][1] - y) <= 1)
            ans++;
    return ans;
}

static uint8_t is_mine(uint8_t x, uint8_t y, uint8_t mines[N_MINES][2])
{
    uint8_t i;

    for (i = 0; i < N_MINES; i++)
        if (mines[i][0] == x && mines[i][1] == y)
            return 1;
    return 0;
}

static void do_reveal(uint8_t x, uint8_t y, uint8_t mines[N_MINES][2], uint32_t reveal[10], uint16_t *revealed)
{
    int8_t i, j;
    uint32_t waiting[10] = {0};
    uint16_t cnt;

    waiting[y] = (uint32_t)1 << x;
    cnt = 1;

    while (cnt)
    {
        for (x = 0; x < MINES_WIDTH; x++)
            for (y = 0; y < MINES_HEIGHT; y++)
                if (waiting[y] & ((uint32_t)1 << x))
                {
                    waiting[y] &= ~((uint32_t)1 << x);
                    cnt--;
                    if (reveal[y] & ((uint32_t)1 << x))
                        continue;
                    reveal[y] |= (uint32_t)1 << x;
                    (*revealed)++;
                    if (calc_num(x, y, mines) == 0)
                        for (i = -1; i <= 1; i++)
                            for (j = -1; j <= 1; j++)
                                if (x + i < MINES_WIDTH && x + i >= 0 &&
                                    y + j < MINES_HEIGHT && y + j >= 0 &&
                                    !(reveal[y + j] & ((uint32_t)1 << (x + i))) &&
                                    !(waiting[y + j] & ((uint32_t)1 << (x + i))))
                                    cnt++, waiting[y + j] |= ((uint32_t)1 << (x + i));
                    // do_reveal(x + i, y + j, mines, reveal, revealed);
                }
    }

    // if (reveal[y] & ((uint32_t)1 << x))
    //     return;
    // reveal[y] |= (uint32_t)1 << x;
    // (*revealed)++;
    // if (calc_num(x, y, mines) == 0)
    //     for (i = -1; i <= 1; i++)
    //         for (j = -1; j <= 1; j++)
    //             if (x + i < MINES_WIDTH && x + i >= 0 &&
    //                 y + j < MINES_HEIGHT && y + j >= 0 &&
    //                 !(reveal[y + j] & ((uint32_t)1 << (x + i))))
    //                 do_reveal(x + i, y + j, mines, reveal, revealed);
}

void do_mines()
{
    uint16_t pwdbuf[6];
    uint8_t mines[N_MINES][2];
    uint8_t i, j, x, y, running;
    uint32_t reveal[10];
    uint16_t key;
    uint16_t revealed;
    char buf[2];

    os_ClrHome();
    os_PutStrFull("Password:");
    if (getpwd(pwdbuf, 6) != 6 || memcmp(pwdbuf, pwd, sizeof(uint16_t) * 6))
        return;

    for (i = 0; i < N_MINES; i++)
    {
    new_mine:
        x = randInt(0, MINES_WIDTH - 1);
        y = randInt(0, MINES_HEIGHT - 1);
        for (j = 0; j < i; j++)
            if (mines[j][0] == x && mines[j][1] == y)
                break;
        if (j != i)
            goto new_mine;
        mines[i][0] = x;
        mines[i][1] = y;
    }

    x = y = 0;
    running = 1;
    memset(reveal, 0, sizeof(reveal));
    revealed = 0;
    buf[1] = 0;
    os_ClrHome();
    os_EnableCursor();
    while (running)
    {
        // os_ClrHome();
        os_HomeUp();
        for (j = 0; j < MINES_HEIGHT; j++)
        {
            for (i = 0; i < MINES_WIDTH; i++)
            {
                // if (x == i && y == j)
                //     invcol();
                if (!(reveal[j] & ((uint32_t)1 << i)))
                    os_PutStrLine(".");
                else if (is_mine(i, j, mines))
                    os_PutStrLine("X"), running = 0;
                else
                {
                    buf[0] = '0' + calc_num(i, j, mines);
                    if (buf[0] == '0')
                        buf[0] = ' ';
                    os_PutStrLine(buf);
                }
                // if (x == i && y == j)
                //     invcol();
            }
            if (j != MINES_HEIGHT - 1)
                os_NewLine();
        }
        os_SetCursorPos(y, x);
        if (revealed == MINES_WIDTH * MINES_HEIGHT - N_MINES)
            running = 0;

        key = os_GetKey();
        switch (key)
        {
        case kClear:
            running = 0;
            break;
        case kLeft:
            if (x == 0)
                x = MINES_WIDTH - 1;
            else
                x--;
            break;
        case kRight:
            if (x == MINES_WIDTH - 1)
                x = 0;
            else
                x++;
            break;
        case kUp:
            if (y == 0)
                y = MINES_HEIGHT - 1;
            else
                y--;
            break;
        case kDown:
            if (y == MINES_HEIGHT - 1)
                y = 0;
            else
                y++;
            break;
        case kEnter:
            // reveal[y] |= ((uint32_t)1) << x;
            do_reveal(x, y, mines, reveal, &revealed);
            // revealed++;
            break;
        }
    }
    os_DisableCursor();
}
