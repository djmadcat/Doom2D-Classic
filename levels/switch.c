#include "glob.h"
#include <stdio.h>
#include <string.h>
#include "vga.h"
#include "error.h"
#include "files.h"
#include "view.h"
#include "misc.h"
#include "..\map.h"

extern map_block_t blk;

static switch_t sw[MAXSW];
int cursw = -1;

void SW_load_old(int h) {
    int i;

    for (i = 0; i < MAXSW; ++i) {
        read(h, &sw[i], sizeof(old_switch_t));
        sw[i].f = SW_PL_PRESS | SW_MN_PRESS;
        if (sw[i].t == 0) {
            break;
        }
    }
}

int SW_load(int h) {
    int i;

    switch (blk.t) {
        case MB_SWITCH:
            for (i = 0; i < MAXSW && blk.sz > 0; ++i, blk.sz -= sizeof(old_switch_t)) {
                read(h, &sw[i], sizeof(old_switch_t));
                sw[i].f = SW_PL_PRESS | SW_MN_PRESS;
            }
            return 1;
        case MB_SWITCH2:
            for (i = 0; i < MAXSW && blk.sz > 0; ++i, blk.sz -= sizeof(switch_t)) {
                read(h, &sw[i], sizeof(switch_t));
            }
            return 1;
    }
    return 0;
}

void SW_save(void) {
    int i;

    F_start_blk(MB_SWITCH2, 0);
    for (i = 0; i < MAXSW; ++i) {
        if (sw[i].t) {
            F_write_blk(&sw[i], sizeof(switch_t));
        }
    }
    F_end_blk();
}

void SW_init(void) {
    int i;

    for (i = 0; i < MAXSW; ++i) {
        sw[i].t = 0;
    }
    cursw = -1;
}

void SW_info(char *s) {
    int i, n;

    for (i = n = 0; i < MAXSW; ++i) {
        if (sw[i].t) {
            ++n;
        }
    }
    sprintf(s, "��४���⥫��: %d\n\n", n);
}

void SW_draw(void) {
    int i, x, y;
    byte c;

    for (i = 0; i < MAXSW; ++i) {
        if (sw[i].t) {
            Z_dot(x = sw[i].x * 8 + 1, y = sw[i].y * 8 + 1, c = (i == cursw) ? 0xA0 : 0x70);
            Z_dot(x + 5, y, c);
            Z_dot(x + 5, y + 5, c);
            Z_dot(x, y + 5, c);
            if (i == cursw) {
                switch (sw[i].t) {
                    case SW_OPENDOOR:
                    case SW_SHUTDOOR:
                    case SW_SHUTTRAP:
                    case SW_DOOR:
                    case SW_DOOR5:
                    case SW_PRESS:
                    case SW_TELE:
                    case SW_LIFT:
                    case SW_LIFTUP:
                    case SW_LIFTDOWN:
                    case SW_TRAP:
                        x = sw[i].a * 8 + 1;
                        y = sw[i].b * 8 + 1;
                        Z_dot(x + 2, y, 0xB0);
                        Z_dot(x + 3, y + 5, 0xB0);
                        Z_dot(x, y + 3, 0xB0);
                        Z_dot(x + 5, y + 2, 0xB0);
                        break;
                }
            }
        }
    }
}

void SW_draw_mini(int x, int y) {
    if (cursw == -1) {
        return;
    }
    switch (sw[cursw].t) {
        case SW_OPENDOOR:
        case SW_SHUTDOOR:
        case SW_SHUTTRAP:
        case SW_DOOR:
        case SW_DOOR5:
        case SW_PRESS:
        case SW_TELE:
        case SW_LIFTUP:
        case SW_LIFTDOWN:
        case SW_LIFT:
        case SW_TRAP:
            V_dot(x + sw[cursw].a, y + sw[cursw].b, 0xB0);
            break;
    }
}

int SW_add(byte x, byte y) {
    int i;

    for (i = 0; i < MAXSW; ++i) {
        if (!sw[i].t) {
            if (cursw == -1) {
                sw[i].t = SW_DOOR5;
                sw[i].f = SW_PL_PRESS | SW_MN_PRESS;
                sw[i].tm = 0;
                sw[i].c = 0;
            } else {
                sw[i] = sw[cursw];
            }
            sw[i].x = x;
            sw[i].y = y;
            sw[i].a = x;
            sw[i].b = y;
            cursw = i;
            return i;
        }
    }
    return -1;
}

int SW_ishere(byte x, byte y) {
    int i;

    for (i = 0; i < MAXSW; ++i) {
        if (sw[i].t) {
            if (sw[i].x == x && sw[i].y == y) {
                return i;
            }
        }
    }
    return -1;
}

void SW_delete(int t) {
    if (t == cursw) {
        cursw = -1;
    }
    sw[t].t = 0;
}

void SW_setlink(byte x, byte y) {
    if (cursw == -1) {
        return;
    }
    switch (sw[cursw].t) {
        case SW_OPENDOOR:
        case SW_SHUTDOOR:
        case SW_SHUTTRAP:
        case SW_DOOR:
        case SW_DOOR5:
        case SW_PRESS:
        case SW_TELE:
        case SW_LIFTUP:
        case SW_LIFTDOWN:
        case SW_LIFT:
        case SW_TRAP:
            sw[cursw].a = x;
            sw[cursw].b = y;
            break;
    }
}

void SW_select(int t) {
    cursw = t;
}

void SW_setf(byte f) {
    if (cursw == -1) {
        return;
    }
    sw[cursw].f = f;
}

byte SW_getf(void) {
    if (cursw == -1) {
        return 0;
    }
    return sw[cursw].f;
}

char *SW_getname(void) {
    static char *nm[] = {
        "��室",
        "ᥪ��� ��室",
        "������ �����",
        "������� �����",
        "������� ������",
        "�����",
        "����� (5 ᥪ)",
        "����⥫�",
        "⥫�������",
        "ᥪ��",
        "���� �����",
        "���� ����",
        "����誠",
        "���� \x18/\x19"
    };

    if (cursw == -1) {
        return "";
    }
    return nm[sw[cursw].t - 1];
}

char *SW_gethelp(void) {
    static char *nm[] = {
        NULL,
        NULL,
        "������� ����� ��������� �� �������, ��।��� ����\n"
            "��頥��� �����, �஬� �窨 �������⢨�.",
        "������ ����� ��������� �� ��������, ��।��� ����\n"
            "��������� �� �窨 �������⢨�.",
        "�����, ����뢠���, 㡨���� ��, �� � ��� �����.",
        "������/������� �����.",
        "����� ����뢠���� ᠬ� �ਬ�୮ �१ 5 ᥪ㭤.",
        "���������� �� ������ ����� �窨 �������⢨�.",
        "����������� ��ப� ��� ������ � ��� �������⢨�.",
        NULL,
        "��४��砥� ���� �����.",
        "��४��砥� ���� ����.",
        "�� �ࠡ��뢠��� ����誠 ��嫮�뢠����, ��⥬\n"
            "���뢠���� � �����஥ �६� �� �ࠡ��뢠��.",
        "��४��砥� ����."
    };

    if (cursw == -1) {
        return NULL;
    }
    return nm[sw[cursw].t - 1];
}

void SW_nexttype(void) {
    if (cursw == -1) {
        return;
    }
    if (++sw[cursw].t >= SW_LAST) {
        sw[cursw].t = 1;
    }
}

void SW_prevtype(void) {
    if (cursw == -1) {
        return;
    }
    if (--sw[cursw].t == 0) {
        sw[cursw].t = SW_LAST - 1;
    }
}
