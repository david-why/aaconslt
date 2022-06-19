#include "main.h"
#include "print.h"
#include "stats.h"
#include "keycodes.h"
#include "prgm.h"
#include "usb.h"
#include "timer.h"
#include "roll.h"
#include "pwdctl.h"
#include "help.h"
#include "mines.h"
#include "appvar.h"
#include "flags.h"
#include "testmode.h"
#include <tice.h>
#include <string.h>
#include <stdio.h>

char buf[64];

#define _C(c, f, t) \
    case c:         \
        puts(#t);   \
        f();        \
        break

int real_main()
{
    uint16_t key;

    while (true)
    {
        os_PutStrFull(">>>");
    main_getkey:
        key = os_GetKey();
        if (key == kClear)
            break;
        switch (key)
        {
            _C(kPrgm, do_program_list, program);
            _C(kVars, do_stats, stats);
            _C(kMode, do_usb, usb);
            _C(kCapT, do_timer, timer);
            _C(kCapR, do_roll, roll);
            _C(kCapP, do_setpwd, setpwd);
            _C(kCapH, do_help, help);
            _C(kCapA, do_appvars, appvars);

            // hidden features
            _C(kCapY, do_toggle_testmodebar, ?);

            // developer features
            _C(kCapF, do_flags, flags);

        default:
            goto main_getkey;
        }
    }

    return 0;
}

int main()
{
    os_ClrHome();
    if (chkpwd())
        return real_main();
    return 1;
}
