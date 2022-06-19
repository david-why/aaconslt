#ifndef _KEYCODES_H
#define _KEYCODES_H

#include <stdint.h>
#include <tice.h>

#define kClear 0x09
#define kPrgm 0x2D
#define kEnter 0x05
#define k0 0x8E
#define k1 0x8F
#define k2 0x90
#define k3 0x91
#define k4 0x92
#define k5 0x93
#define k6 0x94
#define k7 0x95
#define k8 0x96
#define k9 0x97
#define kRight 0x01
#define kLeft 0x02
#define kUp 0x03
#define kDown 0x04
#define kVars 0x35
#define kMode 0x45
#define kVarx 0xB4
#define kChs 0x8C
#define kDel 0x0A
#define kAdd 0x80
#define kSub 0x81
#define kMul 0x82
#define kDiv 0x83

#define kLwrA 0xFCE2
#define kLwrB 0xFCE3
#define kLwrC 0xFCE4
#define kLwrD 0xFCE5
#define kLwrE 0xFCE6
#define kLwrF 0xFCE7
#define kLwrG 0xFCE8
#define kLwrH 0xFCE9
#define kLwrI 0xFCEA
#define kLwrJ 0xFCEB
#define kLwrK 0xFCEC
#define kLwrL 0xFCED
#define kLwrM 0xFCEE
#define kLwrN 0xFCEF
#define kLwrO 0xFCF0
#define kLwrP 0xFCF1
#define kLwrQ 0xFCF2
#define kLwrR 0xFCF3
#define kLwrS 0xFCF4
#define kLwrT 0xFCF5
#define kLwrU 0xFCF6
#define kLwrV 0xFCF7
#define kLwrW 0xFCF8
#define kLwrX 0xFCF9
#define kLwrY 0xFCFA
#define kLwrZ 0xFCFB

#define kCapA 0x9A
#define kCapB 0x9B
#define kCapC 0x9C
#define kCapD 0x9D
#define kCapE 0x9E
#define kCapF 0x9F
#define kCapG 0xA0
#define kCapH 0xA1
#define kCapI 0xA2
#define kCapJ 0xA3
#define kCapK 0xA4
#define kCapL 0xA5
#define kCapM 0xA6
#define kCapN 0xA7
#define kCapO 0xA8
#define kCapP 0xA9
#define kCapQ 0xAA
#define kCapR 0xAB
#define kCapS 0xAC
#define kCapT 0xAD
#define kCapU 0xAE
#define kCapV 0xAF
#define kCapW 0xB0
#define kCapX 0xB1
#define kCapY 0xB2
#define kCapZ 0xB3

char key2char(uint16_t key);
int8_t key2int(uint16_t key);
int8_t sk2int(sk_key_t key);

#endif