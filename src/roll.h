#ifndef _ROLL_H
#define _ROLL_H

typedef enum
{
    roll_disadv = -1,
    roll_normal = 0,
    roll_adv = 1
} roll_type_t;

int roll(int num, int sides, int mod, roll_type_t typ);
void do_roll();

#endif