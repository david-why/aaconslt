#include "help.h"
#include <tice.h>
#include <stdio.h>

void do_help()
{
    puts("Supported commands:\n"
         "-prgm List programs\n"
         "-vars Check stats\n"
         "-mode Get FAT vars\n"
         "-T Timer\n"
         "-R Roll dice\n"
         "-P Set password\n"
         "-H Help page (this one)\n"
         "-A List appvars");
    os_GetKey();
}
