#ifndef _LSTVARS_H
#define _LSTVARS_H

#include <tice.h>
#include <stdint.h>

#ifndef MAX_PROGRAMS
#define MAX_PROGRAMS 128
#endif

#pragma pack(push, 1)
#define PROGRAM_LOCKED (1 << 0)
#define PROGRAM_HIDDEN (1 << 1)
#define PROGRAM_ARCHIVED (1 << 2)
typedef struct
{
    char name[9];
    uint8_t flags;
    var_t *ptr;
} program_entry_t;
#pragma pack(pop)

unsigned int list_programs();
void do_program_list();
const char *program_entry_gettext(const void *entry);
int program_togglelock(program_entry_t *entry);
int program_togglehide(program_entry_t *entry);
int program_togglearc(program_entry_t *entry);
int program_runprgm(const program_entry_t *entry);
int program_delete(const program_entry_t *entry);

#endif