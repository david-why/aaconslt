#include "stats.h"
#include "menu.h"
#include <tice.h>
#include <stdio.h>
#include <fileioc.h>

static char var_list[6][27];

static const char *gettext(const void *dat) { return dat; }

void do_stats()
{
    int sel;
    void *data;
    const system_info_t *sysinfo = os_GetSystemInfo();

    while (1)
    {
        sprintf(var_list[0], "lwrCaseActive: %s", (os_GetFlagByte(0x24) & (1 << 3)) ? "ON" : "OFF");
        sprintf(var_list[1], "apdAble: %s", (os_GetFlagByte(0x08) & (1 << 2)) ? "ON" : "OFF");
        sprintf(var_list[2], "mode: %s", (os_GetFlagByte(0x44) & (1 << 5)) ? "MP" : "CL");
        sprintf(var_list[3], "bootCodeVer: %u.%u.%u", sysinfo->bootMajorVersion, sysinfo->bootMinorVersion, sysinfo->bootRevisionVersion);
        sprintf(var_list[4], "memFree: %u", os_MemChk(&data));
        os_ArcChk();
        sprintf(var_list[5], "arcFree: %u", os_TempFreeArc);
        sel = select_menu("vars", var_list, 6, sizeof(var_list[0]), gettext);
        if (sel == -1)
            break;
        if (sel == 0)
            vars_togglelwrcase();
        if (sel == 1)
            vars_toggleapd();
        if (sel == 2)
            vars_togglemp();
    }
}

void vars_togglelwrcase()
{
    os_SetFlagByte(0x24, os_GetFlagByte(0x24) ^ (1 << 3));
}

void vars_toggleapd()
{
    if (os_GetFlagByte(0x08) & (1 << 2))
        os_DisableAPD();
    else
        os_EnableAPD();
}

void vars_togglemp()
{
    os_SetFlagByte(0x44, os_GetFlagByte(0x44) ^ (1 << 5));
    os_DrawStatusBar();
}
