#include "appvar.h"
#include "keycodes.h"
#include "defs.h"
#include "menu.h"
#include <string.h>
#include <fileioc.h>

extern appvar_entry_t var_list[MAX_APPVARS];

static int appvar_cmp(const void *a, const void *b)
{
    return strcmp(((appvar_entry_t *)a)->name, ((appvar_entry_t *)b)->name);
}

#define avar_open(entry, mode) ti_Open(entry->name, mode)

unsigned int list_appvars()
{
    void *entry;
    unsigned int found;
    ti_var_t var;
    char *name;

    entry = NULL;
    found = 0;

    while ((name = ti_Detect(&entry, NULL)))
    {
        if (!strcmp(name, aaconsle_appvar_name))
            continue;
        strcpy(var_list[found].name, name);
        var = ti_Open(name, "r");
        var_list[found].ptr = ti_GetDataPtr(var);
        if (ti_IsArchived(var))
            var_list[found].flags |= APPVAR_ARCHIVED;
        ti_Close(var);
        if (++found > MAX_APPVARS)
            break;
    }

    qsort(var_list, found, sizeof(appvar_entry_t), appvar_cmp);

    return found;
}

void do_appvars()
{
    unsigned int cnt;
    int sel;
    uint16_t key;
    char buf[32];

list_begin:
    cnt = list_appvars();
    sprintf(buf, "avar: Total %u", cnt);
    sel = select_menu(buf, var_list, cnt, sizeof(appvar_entry_t), appvar_entry_gettext);
    if (sel < 0)
        return;

    while (1)
    {
        os_PutStrFull("avar");
        puts(var_list[sel].name);
        if (var_list[sel].flags & APPVAR_ARCHIVED)
            os_PutStrLine("<ARC>");
        os_NewLine();
        puts("X=Arc del=Delete");
        os_PutStrLine(">>>");
    list_getkey:
        key = os_GetKey();
        if (key == kClear)
            break;
        else if (key == kVarx)
            puts("archive"), appvar_togglearc(var_list + sel);
        else if (key == kDel)
        {
            puts("delete");
            if (appvar_delete(var_list + sel))
            {
                puts("Deleted!");
                os_GetKey();
                goto list_begin;
            }
            puts("Delete failed!");
            os_GetKey();
        }
        else
            goto list_getkey;
    }
    goto list_begin;
}

int appvar_togglearc(appvar_entry_t *entry)
{
    ti_var_t var;

    var = avar_open(entry, "r");
    if (!var)
        return -1;
    ti_SetGCBehavior(NULL, (void (*)())list_appvars);
    if (!ti_SetArchiveStatus(!ti_IsArchived(var), var))
        return -2;
    ti_SetGCBehavior(NULL, NULL);
    ti_Close(var);

    entry->flags ^= PROGRAM_ARCHIVED;
    return entry->flags & PROGRAM_ARCHIVED;
}

int appvar_delete(const appvar_entry_t *entry)
{
    char buf[17];

    sprintf(buf, "Delete %s?", entry->name);
    if (areyousure_menu(buf))
        return ti_Delete(entry->name);
    return 0;
}
