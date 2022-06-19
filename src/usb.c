#ifndef __INT24_TYPE__
#define __INT24_TYPE__ int
#define __INT24_MAX__ 0x7fffff
#define __INT24_MIN__ (~__INT24_MAX__)
typedef __INT24_TYPE__ int24_t;
#define __UINT24_TYPE__ unsigned int
#define __UINT24_MAX__ 0xffffff
typedef __UINT24_TYPE__ uint24_t;
#endif

typedef struct global global_t;
#define fat_callback_data_t msd_t

#include "usb.h"
#include "menu.h"
#include "print.h"
#include "keycodes.h"
#include <tice.h>
#include <fileioc.h>
#include <string.h>
#include <usbdrvce.h>
#include <msddrvce.h>
#include <fatdrvce.h>

#define USB_ENTRIES_PER_PAGE (MENU_ITEMS_PER_PAGE - 2)
static fat_dir_entry_t entries[USB_ENTRIES_PER_PAGE + 2];

struct global
{
    usb_device_t usb;
    msd_t msd;
};
enum
{
    USB_RETRY_INIT = USB_USER_ERROR
};
enum
{
    FAT_NOT_INITIALIZED = FAT_ERROR_INVALID_FILESYSTEM + 1
};

static usb_error_t handleUsbEvent(usb_event_t event, void *event_data, usb_callback_data_t *callback_data)
{
    global_t *global = callback_data;
    switch (event)
    {
    case USB_DEVICE_DISCONNECTED_EVENT:
        if (global->usb)
            msd_Close(&global->msd);
        global->usb = NULL;
        break;
    case USB_DEVICE_CONNECTED_EVENT:
        return usb_ResetDevice(event_data);
    case USB_DEVICE_ENABLED_EVENT:
        global->usb = event_data;
        break;
    case USB_DEVICE_DISABLED_EVENT:
        return USB_RETRY_INIT;
    default:
        break;
    }

    return USB_SUCCESS;
}

static const char *usb_gettext(const void *data)
{
    // os_PutStrFull("OK");os_GetKey();
    return ((fat_dir_entry_t *)data)->filename;
}

void do_usb()
{
    uint8_t err, num_partitions;
    uint32_t time;
    msd_info_t msdinfo;
    global_t global;
    msd_partition_t partitions[MAX_PARTITIONS];
    fat_t fat;
    size_t len;
    int sel, cnt, errno;
    unsigned int page, nblks, i, rd, slen;
    uint16_t key;
    char path[256], buf[9];
    var_t *var;
    fat_file_t file;
    void *data;
    ti_var_t tvar;

    puts("Waiting for FAT...");
    memset(&global, 0, sizeof(global_t));
    time = rtc_Time();
    do
    {
        global.usb = NULL;

        err = usb_Init(handleUsbEvent, &global, NULL, USB_DEFAULT_INIT_FLAGS);
        if (err != USB_SUCCESS)
        {
            os_PutStrFull("USB error");
            os_GetKey();
            goto usb_error;
        }

        while (err == USB_SUCCESS)
        {
            if (os_GetCSC())
                goto usb_error;
            if (rtc_Time() - time > USB_MAX_TIMEOUT)
            {
                os_PutStrFull("Timeout loading USB");
                os_GetKey();
                goto usb_error;
            }
            if (global.usb != NULL)
                break;
            err = usb_HandleEvents();
        }
    } while (err == USB_RETRY_INIT);
    if (err != USB_SUCCESS)
    {
        os_PutStrFull("USB error");
        os_GetKey();
        goto usb_error;
    }
    err = msd_Open(&global.msd, global.usb);
    if (err != MSD_SUCCESS)
    {
        os_PutStrFull("MSD error");
        os_GetKey();
        goto usb_error;
    }
    err = msd_Info(&global.msd, &msdinfo);
    if (err != MSD_SUCCESS)
    {
        os_PutStrFull("MSD error");
        os_GetKey();
        goto msd_error;
    }
    num_partitions = msd_FindPartitions(&global.msd, partitions, MAX_PARTITIONS);
    if (!num_partitions)
    {
        os_PutStrFull("No partitions");
        os_GetKey();
        goto msd_error;
    }
    fat.read = msd_Read;
    fat.write = msd_Write;
    fat.usr = &global.msd;
    for (i = 0;;)
    {
        fat.first_lba = partitions[i].first_lba;
        fat.last_lba = partitions[i].last_lba;
        err = fat_Init(&fat);
        if (err == FAT_SUCCESS)
            break;
        i++;
        if (i >= num_partitions)
        {
            os_PutStrFull("No FAT partition");
            os_GetKey();
            goto msd_error;
        }
    }

    page = 0;
    path[0] = '/';
    path[1] = 0;
    while (1)
    {
        // os_PutStrFull("OK");os_GetKey();
        cnt = fat_DirList(&fat, path, FAT_LIST_ALL, entries, USB_ENTRIES_PER_PAGE, page * USB_ENTRIES_PER_PAGE);
        // os_PutStrFull("OK2");os_GetKey();
        strcpy(entries[cnt].filename, "<NEXT PAGE>");
        strcpy(entries[cnt + 1].filename, "<PREV PAGE>");
        // os_PutStrFull("OK3");os_GetKey();
        sel = select_menu(path, entries, cnt + 2, sizeof(fat_dir_entry_t), usb_gettext);
        if (sel < 0)
            goto fat_error;
        if (sel == cnt)
        {
            if (cnt)
                page++;
        }
        else if (sel == cnt + 1)
        {
            if (page)
                page--;
        }
        else if (entries[sel].attrib & FAT_DIR)
        {
            if (!strcmp(entries[sel].filename, "."))
            {
            }
            else if (!strcmp(entries[sel].filename, ".."))
            {
                for (data = path + strlen(path) - 1; *(char *)data != '/'; data--)
                    ;
                if (data == path)
                    path[1] = 0;
                else
                    *(char *)data = 0;
            }
            else
            {
                len = strlen(path);
                if (len != 1)
                    path[len++] = '/';
                strcpy(path + len, entries[sel].filename);
            }
            page = 0;
        }
        else
        {
            puts(path);
            os_PutStrFull("Size:");
            print(print_int, entries[sel].size);
            os_NewLine();
            if (entries[sel].attrib & FAT_RDONLY)
                os_PutStrLine("<RO>");
            if (entries[sel].attrib & FAT_HIDDEN)
                os_PutStrLine("<HI>");
            os_NewLine();
            puts("enter=send");
            key = os_GetKey();
            if (key == kEnter)
            {
                nblks = entries[sel].size / FAT_BLOCK_SIZE;
                if (entries[sel].size % FAT_BLOCK_SIZE)
                    nblks++;
                // print(print_int, nblks);
                if (nblks * FAT_BLOCK_SIZE > UINT16_MAX)
                {
                    puts("Cannot send, too big");
                    os_GetKey();
                    goto sendprgm_end;
                }
                // os_PutStrFull("OK0");os_GetKey();
                var = os_CreateAppVar("_tmp", nblks * FAT_BLOCK_SIZE);
                if (var == NULL)
                {
                    puts("Cannot send, too big");
                    os_GetKey();
                    goto sendprgm_end;
                }
                len = strlen(path);
                path[len++] = '/';
                strcpy(path + len, entries[sel].filename);
                // os_PutStrFull("OK");os_GetKey();
                err = fat_Open(&file, &fat, path);
                // print(print_int, err);
                // os_GetKey();
                if (err)
                {
                    os_PutStrFull("fat_Open error ");
                    print(print_int, err);
                    os_NewLine();
                    os_GetKey();
                    goto sendprgm_end;
                }
                // print(print_int, fat_GetSize(&file));
                // print(print_int, nblks);
                // os_GetKey();
                // os_NewLine();
                errno = os_PushErrorHandler();
                if (errno)
                {
                    sprintf(path, "OS Error %d", errno);
                    puts(path);
                    os_GetKey();
                    ti_Delete("_tmp");
                    goto fat_error;
                }
                for (i = 0; i < nblks; i += 10)
                {
                    rd = i + 10 > nblks ? nblks - i : 10;
                    err = fat_Read(&file, rd, var->data + FAT_BLOCK_SIZE * i);
                    // err = fat_Read(&file, nblks, var->data);
                    // if (err != nblks)
                    if (err != rd)
                    {
                        os_PutStrFull("Read only ");
                        print(print_int, err);
                        os_NewLine();
                        os_GetKey();
                        goto sendprgm_end;
                    }
                }
                err = fat_Close(&file);
                // print(print_int, err);
                // os_GetKey();
                if (err)
                {
                    print(print_int, err);
                    os_NewLine();
                    os_GetKey();
                    goto sendprgm_end;
                }
                // os_PutStrFull("OK2");os_GetKey();
                // os_PutStrFull("OK3");os_GetKey();
                // tvar = ti_Open("_tmp", "r");
                // ti_SetArchiveStatus(1, tvar);
                // ti_Close(tvar);
                // var = os_GetAppVarData("_tmp", NULL);
                // os_PutStrFull("OK4");os_GetKey();
                // os_PutStrLine(var->data);os_GetKey();
                if (memcmp(var->data, "**TI83F*", 8))
                {
                    puts("Not TI var file");
                    os_GetKey();
                    goto sendprgm_end;
                }
                nblks = *(uint16_t *)&var->data[53];
                data = var->data + 55;
                while (nblks)
                {
                    slen = *(uint16_t *)(data + 2);
                    nblks -= slen + 17;
                    strncpy(buf, data + 5, 8);
                    buf[8] = 0;
                    tvar = ti_OpenVar(buf, "w", *(uint8_t *)(data + 4));
                    if (!tvar)
                        goto sendprgm_fail;
                    if (!ti_Write(data + 19, slen, 1, tvar))
                        goto sendprgm_fail;
                    if (*(uint8_t *)(data + 14) & 0x80)
                        ti_SetArchiveStatus(1, tvar);
                    ti_Close(tvar);
                    os_PutStrFull("Succeed for type ");
                    goto sendprgm_writenum;
                sendprgm_fail:
                    os_PutStrFull("Fail for type ");
                sendprgm_writenum:
                    print(print_int, *(uint8_t *)(data + 4));
                    os_NewLine();
                    data += slen + 17;
                    continue;
                }
                puts("Send success!");
                os_GetKey();
                os_PopErrorHandler();
            sendprgm_end:
                ti_Delete("_tmp");
                path[len - 1] = 0;
            }
        }
    }

fat_error:
    fat_Deinit(&fat);

msd_error:
    msd_Close(&global.msd);

usb_error:
    usb_Cleanup();

    return;
}
