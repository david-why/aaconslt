#include "roll.h"
#include "print.h"
#include "keycodes.h"
#include "menu.h"
#include <tice.h>
#include <stdio.h>
#include <string.h>

int roll(int num, int sides, int res, roll_type_t typ)
{
    int ans;

    if (typ != roll_normal)
        ans = roll(num, sides, res, roll_normal);
    while (num-- > 0)
        res += randInt(1, sides);
    if (typ == roll_disadv)
        return ans < res ? ans : res;
    if (typ == roll_adv)
        return ans > res ? ans : res;
    return res;
}

static char roll_texts[5][16];
// static const char *names[] = {"Num", "Sides", "Mod"};
static const char *names[] = {"Number", "Sides", "Modifier"};

static const char *gettext(const void *ptr) { return ptr; }

void do_roll()
{
    int num, sides, mod, res, sel;
    int *const lst[] = {&num, &sides, &mod};
    roll_type_t adv;

    num = 1;
    sides = 20;
    mod = 0;
    adv = roll_normal;
    while (1)
    {
        for (sel = 0; sel < 3; sel++)
            sprintf(roll_texts[sel], "%s:%d", names[sel], *lst[sel]);
        strcpy(roll_texts[3], adv < 0 ? "Disadvantage" : (adv == 0 ? "Normal" : "Advantage"));
        sprintf(roll_texts[4], "Roll!");
        sel = select_menu("Dice rolling", roll_texts, 5, sizeof(roll_texts[0]), gettext);

        switch (sel)
        {
        case -1:
            return;
        case 0:
        case 1:
        case 2:
            edit_int(lst[sel], names[sel]);
            break;
        case 3:
            adv++;
            if (adv > roll_adv)
                adv = roll_disadv;
            break;
        case 4:
            res = roll(num, sides, mod, adv);
            os_PutStrLine("Result:");
            print(print_int, res);
            os_GetKey();
            os_NewLine();
            break;
        }
    }
}
