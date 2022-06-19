    public _toggle_testmodebar

_toggle_testmodebar:
    push hl
    push af
    ld hl,0D000C7h
    ld a,(hl)
    bit 1,a
    jr z,.set
    res 1,a
    jr .common
.set:
    set 1,a
.common:
    ld (hl),a
    pop af
    pop hl
    call 0021A3Ch
    ret
