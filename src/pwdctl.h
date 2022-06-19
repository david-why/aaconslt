#ifndef _PWDCTL_H
#define _PWDCTL_H

#include <stdint.h>

#define MAX_PWD_LEN 8

uint16_t getpwd(uint16_t *buf, uint16_t maxlen);
uint8_t chkpwd();
void setpwd(uint16_t *pwd, uint16_t len);
void do_setpwd();

#endif