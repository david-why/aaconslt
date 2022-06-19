#include "prgm.h"
#include "print.h"
#include "menu.h"
#include "keycodes.h"
#include "main.h"
#include <tice.h>
#include <stdio.h>
#include <fileioc.h>
#include <string.h>

program_entry_t var_list[MAX_PROGRAMS];

static int appvar_cmp(const void *a, const void *b)
{
    return strcmp(((program_entry_t *)a)->name, ((program_entry_t *)b)->name);
}

unsigned int list_programs()
{
    void *entry;
    unsigned int found;
    uint8_t type;
    ti_var_t var;
    char *name;

    entry = NULL;
    found = 0;

    while ((name = ti_DetectAny(&entry, NULL, &type)))
    {
        if (type != 5 && type != 6)
            continue;
        if ((name[0] | 64) < 'A' || (name[0] | 64) > '[')
            continue;
        strcpy(var_list[found].name, name);
        if (!(name[0] & 64))
        {
            var_list[found].name[0] |= 64;
            var_list[found].flags |= PROGRAM_HIDDEN;
        }
        if (type == 6)
            var_list[found].flags |= PROGRAM_LOCKED;
        var = ti_OpenVar(name, "r", type);
        var_list[found].ptr = ti_GetDataPtr(var);
        if (ti_IsArchived(var))
            var_list[found].flags |= PROGRAM_ARCHIVED;
        ti_Close(var);
        if (++found > MAX_PROGRAMS)
            break;
    }

    qsort(var_list, found, sizeof(program_entry_t), appvar_cmp);

    return found;
}

static int runprgm_callback(void *data __attribute__((unused)), int retval)
{
    os_ClrHome();
    os_GetCSC();
    if (retval)
    {
        os_PutStrFull("Return code ");
        print(print_int, retval);
        os_NewLine();
        os_GetKey();
    }
    do_program_list();
    return real_main();
}

static ti_var_t prgm_open(const program_entry_t *entry, const char *mode)
{
    char buf[9];

    strcpy(buf, entry->name);
    if (entry->flags & PROGRAM_HIDDEN)
        buf[0] &= ~64;
    return ti_OpenVar(buf, mode, (entry->flags & PROGRAM_LOCKED) ? 6 : 5);
}

void do_program_list()
{
    unsigned int cnt;
    int sel;
    uint16_t key;
    char buf[32];

prgmlist_begin:
    cnt = list_programs();
    sprintf(buf, "prgm: Total %u", cnt);
    sel = select_menu(buf, var_list, cnt, sizeof(program_entry_t), program_entry_gettext);
    if (sel < 0)
        return;

    while (1)
    {
        os_PutStrFull("prgm");
        puts(var_list[sel].name);
        if (var_list[sel].flags & PROGRAM_LOCKED)
            os_PutStrLine("<LOCK>");
        if (var_list[sel].flags & PROGRAM_HIDDEN)
            os_PutStrLine("<HIDE>");
        if (var_list[sel].flags & PROGRAM_ARCHIVED)
            os_PutStrLine("<ARC>");
        os_NewLine();
        puts("vars=Lock mode=Hide X=Arc");
        puts("enter=Execute del=Delete");
        os_PutStrLine(">>>");
        key = os_GetKey();
        if (key == kClear)
        {
            puts("quit");
            break;
        }
        if (key == kEnter)
            puts("exec"), program_runprgm(var_list + sel);
        if (key == kVars)
            puts("lock"), program_togglelock(var_list + sel);
        if (key == kMode)
            puts("hide"), program_togglehide(var_list + sel);
        if (key == kVarx)
            puts("archive"), program_togglearc(var_list + sel);
        if (key == kDel)
        {
            puts("delete");
            if (program_delete(var_list + sel))
            {
                puts("Deleted!");
                os_GetKey();
                goto prgmlist_begin;
            }
            puts("Delete failed!");
            os_GetKey();
        }
    }
    goto prgmlist_begin;
}

int program_togglelock(program_entry_t *entry)
{
    ti_var_t v;
    void *dat;
    uint16_t size;
    char buf[9];

    v = prgm_open(entry, "r");
    // v = ti_OpenVar(entry->name, "r", (entry->flags & PROGRAM_LOCKED) ? 6 : 5);
    if (!v)
        return -1;
    dat = ti_GetDataPtr(v);
    size = ti_GetSize(v);
    ti_Close(v);
    // FIXME use hidden name
    strcpy(buf, entry->name);
    if (entry->flags & PROGRAM_HIDDEN)
        buf[0] &= ~64;
    v = ti_OpenVar(buf, "w", (entry->flags & PROGRAM_LOCKED) ? 5 : 6);
    if (!v)
        return -1;
    size = ti_Write(dat, size, 1, v);
    if (!size)
        return -1;
    ti_Close(v);
    entry->flags ^= PROGRAM_LOCKED;
    return entry->flags & PROGRAM_LOCKED;
}

int program_togglehide(program_entry_t *entry)
{
    char buf[9];
    int8_t ret;

    strcpy(buf, entry->name);
    buf[0] ^= 64;
    // FIXME use hidden name
    if (entry->flags & PROGRAM_HIDDEN)
        ret = ti_RenameVar(buf, entry->name, (entry->flags & PROGRAM_LOCKED) ? 6 : 5);
    else
        ret = ti_RenameVar(entry->name, buf, (entry->flags & PROGRAM_LOCKED) ? 6 : 5);
    if (ret)
        return -ret;
    // entry->name[0] ^= 64;
    entry->flags ^= PROGRAM_HIDDEN;
    return entry->flags & PROGRAM_HIDDEN;
}

int program_togglearc(program_entry_t *entry)
{
    ti_var_t var;

    // FIXME use hidden name
    var = prgm_open(entry, "r");
    // var = ti_OpenVar(entry->name, "r", (entry->flags & PROGRAM_LOCKED) ? 6 : 5);
    if (!var)
        return -1;
    ti_SetGCBehavior(NULL, (void (*)())list_programs);
    if (!ti_SetArchiveStatus(!ti_IsArchived(var), var))
        return -2;
    ti_SetGCBehavior(NULL, NULL);
    ti_Close(var);

    entry->flags ^= PROGRAM_ARCHIVED;
    return entry->flags & PROGRAM_ARCHIVED;
}

const char *program_entry_gettext(const void *entry)
{
    return ((const program_entry_t *)entry)->name;
}

int program_runprgm(const program_entry_t *entry)
{
    char buf[9];

    strcpy(buf, entry->name);
    if (entry->flags & PROGRAM_HIDDEN)
        buf[0] &= ~64;
    return os_RunPrgm(buf, NULL, 0, runprgm_callback);
}

int program_delete(const program_entry_t *entry)
{
    char buf[17];

    sprintf(buf, "Delete %s?", entry->name);
    if (areyousure_menu(buf))
    {
        strcpy(buf, entry->name);
        if (entry->flags & PROGRAM_HIDDEN)
            buf[0] &= ~64;
        return ti_DeleteVar(buf, (entry->flags & PROGRAM_LOCKED) ? 6 : 5);
    }
    return 0;
}
