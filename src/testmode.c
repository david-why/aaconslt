#include "testmode.h"
#include "keycodes.h"
#include "pwdctl.h"
#include <tice.h>
#include <stdio.h>
#include <string.h>

#define textFlags (*(uint8_t *)(0xD00085))
#define trigFlags (*(uint8_t *)(0xD00080))
#define statusBarFlags (*(uint8_t *)(0xD000C7))

#define invcol() (textFlags ^= 1 << 3)

const uint16_t testmode_pwd[] = {kCapT, kCapE, kCapS, kCapT, k1};

void fake_testmode()
{
    toggle_testmodebar();
    // statusBarFlags ^= 1 << 1;
    os_ClrHome();
    os_SetCursorPos(0, 6);
    invcol();
    os_PutStrFull("RESET COMPLETE");
    invcol();
    os_SetCursorPos(2, 0);
    puts("APPS & PROGRAMS ENABLED\n"
         "Pic & Image VARS ENABLED");
    os_PutStrFull("ANGLE: ");
    puts((trigFlags & (1 << 2)) ? "DEGREE" : "RADIAN");
    puts("STAT DIAGNOSTICS: ON\n"
         "DISABLE logBASE: NO\n"
         "DISABLE \xc6(: NO\n"
         "RAM is INTACT");
    invcol();
    os_PutStrFull("Press any Key...");
    invcol();
    os_GetKey();
    toggle_testmodebar();
    // statusBarFlags ^= 1 << 1;
}

void do_fake_testmode()
{
    uint16_t buf[6];
    uint16_t cnt;

    os_PutStrFull("Password:");
    cnt = getpwd(buf, 6);
    if (cnt != 5 || memcmp(buf, testmode_pwd, sizeof(uint16_t) * 5))
        return;
    fake_testmode();
}

void do_toggle_testmodebar()
{
    uint16_t buf[6];
    uint16_t cnt;

    os_PutStrFull("Password:");
    cnt = getpwd(buf, 6);
    if (cnt != 5 || memcmp(buf, testmode_pwd, sizeof(uint16_t) * 5))
        return;
    toggle_testmodebar();
}
