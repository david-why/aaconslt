#include "menu.h"
#include "keycodes.h"
#include "print.h"
#include <stdlib.h>
#include <tice.h>
#include <stdio.h>

int select_menu(const char *title, const void *data, unsigned int nmemb, unsigned int size, const char *gettext(const void *))
{
    const unsigned int n_pages = (nmemb + MENU_ITEMS_PER_PAGE - 1) / MENU_ITEMS_PER_PAGE;
    unsigned int page, i, ind;
    uint16_t key;
    uint8_t need;
    char buf[8];

    page = 0;
    need = 1;
    while (1)
    {
        if (need)
        {
            need = 0;
            os_PutStrLine(title);
            os_NewLine();
            for (i = 0; i < MENU_ITEMS_PER_PAGE; i++)
            {
                ind = page * MENU_ITEMS_PER_PAGE + i;
                // os_SetCursorPos(i + 1, 0);
                // os_PutStrFull(sel == i ? ">" : " ");
                if (ind < nmemb)
                    sprintf((char *)&key, "%d", i),
                        os_PutStrLine((char *)&key),
                        os_PutStrLine(":"),
                        os_PutStrLine(gettext(data + size * ind)),
                        os_NewLine();
            }
            os_PutStrLine(">>?");
        }
        // os_SetCursorPos(sel + 1, 0);
        // os_PutStrFull(">");

        key = os_GetKey();
        switch (key)
        {
        case kClear:
            puts("quit");
            return -1;
        case kUp:
        case kLeft:
            i = page;
            if (page == 0)
                page = n_pages;
            page--;
            if (page != i)
                need = 1, puts("prev");
            break;
        case kDown:
        case kRight:
            i = page;
            page++;
            if (page == n_pages)
                page = 0;
            if (page != i)
                need = 1, puts("next");
            break;
        default:
            i = key2int(key);
            if (i >= 0 && page * MENU_ITEMS_PER_PAGE + i < nmemb)
            {
                sprintf(buf, "%d", i);
                puts(buf);
                return page * MENU_ITEMS_PER_PAGE + i;
            }
            break;
        }
    }
}

int areyousure_menu(const char *title)
{
    uint16_t key;

    puts(title);
    puts("Are you sure?");
    os_PutStrLine(">>?");
    key = 0;
    while (key != kClear && key != kEnter)
        key = os_GetKey();
    if (key == kEnter)
        puts("yes");
    else
        puts("no");
    return key == kEnter;
}

void edit_int(int *val, const char *title)
{
    uint16_t key;
    int x;

    puts(title);
    os_PutStrLine("Cur:  ");
    while (1)
    {
        os_PutStrLine("  ");
        print(print_int, *val);
        os_NewLine();
        os_PutStrLine(">>?");
    ed_getkey:
        key = os_GetKey();
        switch (key)
        {
        case kEnter:
            os_PutStrLine("OK   ");
            print(print_int, *val);
            os_NewLine();
            return;
        case kDel:
            os_PutStrLine("del");
            *val /= 10;
            break;
        case kClear:
            os_PutStrLine("clr");
            *val = 0;
            break;
        case kUp:
        case kAdd:
            os_PutStrLine("inc");
            ++*val;
            break;
        case kDown:
        case kSub:
            os_PutStrLine("dec");
            --*val;
            break;
        default:
            if ((x = key2int(key)) >= 0)
                os_PutStrLine("+"), sprintf((char *)&key, "%d", x),
                    os_PutStrLine((char *)&key), os_PutStrLine(" "),
                    *val = *val * 10 + x;
            else
                goto ed_getkey;
            break;
        }
    }
}
