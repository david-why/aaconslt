#ifndef _TESTMODE_H
#define _TESTMODE_H

#include <stdint.h>

extern const uint16_t testmode_pwd[];

void fake_testmode();
void do_fake_testmode();
void toggle_testmodebar();
void do_toggle_testmodebar();

#endif