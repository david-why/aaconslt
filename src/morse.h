#ifndef _MORSE_H
#define _MORSE_H

#include <stdint.h>

#define MORSE_LENGTH(v) ((v)&0b111)
#define MORSE_DATA_AT(v, i) (((v) >> (MORSE_LENGTH(v) - i + 2)) & 1)
#define MORSE_CHAR(c) ((c) < MORSE_TABLE_LENGTH ? morse_table[(uint8_t)(c)] : 0)

extern const uint8_t morse_table[];
extern const uint8_t MORSE_TABLE_LENGTH;

void morse_dispchar(char c);
void do_morse();

#endif