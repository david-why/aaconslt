#include "pwdctl.h"
#include "keycodes.h"
#include <fileioc.h>
#include <string.h>
#include <defs.h>

uint16_t getpwd(uint16_t *buf, uint16_t maxlen)
{
    uint16_t len, key;

    len = 0;
    while (len < maxlen)
    {
        key = os_GetKey();
        if (key == kEnter)
            break;
        buf[len++] = key;
        os_PutStrFull("*");
    }
    os_NewLine();
    return len;
}

uint8_t chkpwd()
{
    var_t *data;
    uint16_t buf[MAX_PWD_LEN];
    uint16_t len;

    data = os_GetAppVarData(aaconsle_appvar_name, NULL);
    if (data == NULL)
        return 1;
    os_PutStrFull("Password:");
    len = getpwd(buf, MAX_PWD_LEN);
    if (len != data->size / sizeof(uint16_t) || memcmp(data->data, buf, len * sizeof(uint16_t)))
        return 0;
    return 1;
}

void setpwd(uint16_t *pwd, uint16_t len)
{
    var_t *data;
    ti_var_t var;

    ti_Delete(aaconsle_appvar_name);
    if (!len)
        return;
    data = os_CreateAppVar(aaconsle_appvar_name, len * sizeof(uint16_t));
    memcpy(data->data, pwd, len * sizeof(uint16_t));
    var = ti_Open(aaconsle_appvar_name, "r");
    ti_SetArchiveStatus(1, var);
    ti_Close(var);
}

void do_setpwd()
{
    uint16_t len;
    uint16_t buf[MAX_PWD_LEN];

    os_ClrHome();
    os_PutStrFull("New password:");
    len = getpwd(buf, MAX_PWD_LEN);
    setpwd(buf, len);
}
