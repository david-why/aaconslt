#ifndef _APPVAR_H
#define _APPVAR_H

#include "prgm.h"

#define MAX_APPVARS MAX_PROGRAMS

#define APPVAR_ARCHIVED PROGRAM_ARCHIVED
typedef program_entry_t appvar_entry_t;

unsigned int list_appvars();
void do_appvars();
#define appvar_entry_gettext program_entry_gettext
int appvar_togglearc(appvar_entry_t *entry);
int appvar_delete(const appvar_entry_t *entry);

#endif