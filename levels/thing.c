#include "glob.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vga.h"
#include "error.h"
#include "files.h"
#include "memory.h"
#include "view.h"
#include "misc.h"
#include "..\map.h"

#define MANCOLOR 0xD0

extern map_block_t blk;

#define IT_TN (TH__LASTI-TH_CLIP)
#define MN_TN (TH__LASTM-TH_DEMON)

extern int curth;

static void *spr[IT_TN + 1];
static void *mon[MN_TN];
static char mond[MN_TN];
static thing_t th[MAXTH];

void TH_alloc(void) {
    int i, h;
    char nm[IT_TN + 1][4] = {
        "PLAY",
        "CLIP",
        "SHEL",
        "ROCK",
        "CELL",
        "AMMO",
        "SBOX",
        "BROK",
        "CELP",
        "STIM",
        "MEDI",
        "BPAK",
        "CSAW",
        "SHOT",
        "SGN2",
        "MGUN",
        "LAUN",
        "PLAS",
        "BFUG",
        "ARM1",
        "ARM2",
        "MEGA",
        "PINV",
        "AQUA",
        "KEYR",
        "KEYG",
        "KEYB",
        "SUIT",
        "SOUL",
        "SMRT",
        "SMGT",
        "SMBT",
        "GOR1",
        "FCAN",
        "GUN2"
    }, mn[MN_TN][4] = {
        "SARG",
        "TROO",
        "POSS",
        "SPOS",
        "CYBR",
        "CPOS",
        "BOSS",
        "BOS2",
        "HEAD",
        "SKUL",
        "PAIN",
        "SPID",
        "BSPI",
        "FATT",
        "SKEL",
        "VILE",
        "FISH",
        "BAR1",
        "ROBO",
        "PLAY"
    };

    for (i = 0; i < IT_TN + 1; ++i) {
        spr[i] = M_lock(F_getsprid(nm[i], 0, 1));
    }
    for (i = 0; i < MN_TN; ++i) {
        mon[i] = M_lock(h = F_getsprid(mn[i], 0, 1));
        mond[i] = (h & 0x8000) ? 1 : 0;
    }
}

void TH_load_old(int h) {
    int i;

    for (i = 0; i < MAXTH; ++i) {
        read(h, &th[i], sizeof(thing_t));
        if (th[i].t == 0) {
            break;
        }
    }
    SW_load_old(h);
}

int TH_load(int h) {
    int i;

    switch (blk.t) {
        case MB_THING:
            for (i = 0; i < MAXTH && blk.sz > 0; ++i, blk.sz -= sizeof(thing_t)) {
                read(h, &th[i], sizeof(thing_t));
            }
            return 1;
    }
    return 0;
}

void TH_save(void) {
    int i;

    F_start_blk(MB_THING, 0);
    for (i = 0; i < MAXTH; ++i) {
        if (th[i].t) {
            F_write_blk(&th[i], sizeof(thing_t));
        }
    }
    F_end_blk();
}

void TH_init(void) {
    int i;

    for (i = 0; i < MAXTH; ++i) {
        th[i].t = 0;
    }
}

static byte mctab[3] = { 0x70, 0x60, 0x20 };

void TH_draw(void) {
    int i, s;

    for (i = 0; i < MAXTH; ++i) {
        if (th[i].t) {
            if (th[i].t <= TH_DMSTART) {
                s = mctab[th[i].t - 1];
            } else {
                s = th[i].t - TH_CLIP + 1;
            }
            if (th[i].t >= TH_DEMON) {
                if (th[i].t < TH_MAN) {
                    Z_drawspr(th[i].x, th[i].y, mon[th[i].t - TH_DEMON], (th[i].f & THF_DIR) ^ mond[th[i].t - TH_DEMON]);
                } else {
                    Z_drawmanspr(th[i].x, th[i].y, spr[0], th[i].f & THF_DIR, MANCOLOR);
                }
            } else if (th[i].t > TH_DMSTART) {
                Z_drawspr(th[i].x, th[i].y, spr[s], 0);
            } else {
                Z_drawmanspr(th[i].x, th[i].y, spr[0], th[i].f & THF_DIR, s);
            }
            if (i == curth) {
                V_clr(th[i].x - 5 - w_x + 100, 11, th[i].y - 10 - w_y + 50, 1, 0xAD);
                V_clr(th[i].x - 5 - w_x + 100, 11, th[i].y - w_y + 50, 1, 0xAD);
                V_clr(th[i].x - 5 - w_x + 100, 1, th[i].y - 9 - w_y + 50, 9, 0xAD);
                V_clr(th[i].x + 5 - w_x + 100, 1, th[i].y - 9 - w_y + 50, 9, 0xAD);
            }
        }
    }
}

void TH_drawth(int x, int y, int t) {
    int s;

    if (t <= TH_DMSTART) {
        s = mctab[t - 1];
    } else {
        s = t - TH_CLIP + 1;
    }
    if (t >= TH_DEMON) {
        V_spr(x, y, mon[t - TH_DEMON]);
    } else if (t > TH_DMSTART) {
        V_spr(x, y, spr[s]);
    } else {
        V_manspr(x, y, spr[0], s);
    }
}

int TH_add(int x, int y, int t) {
    int i;

    for (i = 0; i < MAXTH; ++i) {
        if (!th[i].t) {
            th[i].t = t;
            th[i].x = x;
            th[i].y = y;
            th[i].f = 0;
            return i;
        }
    }
    return -1;
}

void TH_info(char *s) {
    int i, p[3], m, t;

    p[0] = p[1] = p[2] = m = t = 0;
    for (i = 0; i < MAXTH; ++i) {
        if (th[i].t) {
            if (th[i].t >= TH_DEMON) {
                ++m;
            } else if (th[i].t > TH_DMSTART) {
                ++t;
            } else {
                ++p[th[i].t - TH_PLR1];
            }
        }
    }
    sprintf(s, "1�� ��ப��: %d   2�� ��ப��: %d   �祪 DM: %d\n"
        "�।��⮢: %d   �����஢: %d\n\n", p[0], p[1], p[2], t, m);
}

int TH_isthing(int x, int y) {
    int i;

    for (i = 0; i < MAXTH; ++i) {
        if (th[i].t) {
            if (x < th[i].x - 5) {
                continue;
            }
            if (x > th[i].x + 5) {
                continue;
            }
            if (y < th[i].y - 10) {
                continue;
            }
            if (y > th[i].y) {
                continue;
            }
            return i;
        }
    }
    return -1;
}

void TH_move(int t, int x, int y) {
    th[t].x = x;
    th[t].y = y;
}

int TH_getx(int t) {
    return th[t].x;
}

int TH_gety(int t) {
    return th[t].y;
}

int TH_getf(int t) {
    return th[t].f;
}

int TH_gett(int t) {
    return th[t].t;
}

void TH_setf(int t, int f) {
    th[t].f = f;
}

void TH_sett(int t, int f) {
    th[t].t = f;
}

void TH_delete(int t) {
    th[t].t = 0;
}

int TH_nextt(int t) {
    switch (t) {
        case TH__LASTM - 1:
            return TH_PLR1;
        case TH__LASTI - 1:
            return TH_DEMON;
        case TH_DMSTART:
            return TH_CLIP;
    }
    return t + 1;
}

int TH_prevt(int t) {
    switch (t) {
        case TH_DEMON:
            return TH__LASTI - 1;
        case TH_PLR1:
            return TH__LASTM - 1;
        case TH_CLIP:
            return TH_DMSTART;
    }
    return t - 1;
}

char *TH_getname(int t) {
    switch (t) {
        case TH_PLR1:
            return "1� ��ப";
        case TH_PLR2:
            return "2�� ��ப";
        case TH_DMSTART:
            return "�窠 DM";

        case TH_CLIP:
            return "���஭�";
        case TH_SHEL:
            return "4 �����";
        case TH_ROCKET:
            return "1 ࠪ��";
        case TH_CELL:
            return "���३��";
        case TH_AMMO:
            return "�騪 ���஭��";
        case TH_SBOX:
            return "25 ����";
        case TH_RBOX:
            return "5 ࠪ��";
        case TH_CELP:
            return "�����";
        case TH_STIM:
            return "���窠";
        case TH_MEDI:
            return "�.���窠";
        case TH_BPACK:
            return "���";
        case TH_CSAW:
            return "���������";
        case TH_SGUN:
            return "����";
        case TH_SGUN2:
            return "����⢮���";
        case TH_MGUN:
            return "�㫥���";
        case TH_LAUN:
            return "ࠪ�⭨�";
        case TH_PLAS:
            return "�����.��誠";
        case TH_BFG:
            return "BFG9000";
        case TH_ARM1:
            return "����� �஭�";
        case TH_ARM2:
            return "ᨭ�� �஭�";
        case TH_MEGA:
            return "�������";
        case TH_INVL:
            return "���梨������";
        case TH_AQUA:
            return "��������";
        case TH_RKEY:
            return "���� ����";
        case TH_GKEY:
            return "������ ����";
        case TH_BKEY:
            return "ᨭ�� ����";
        case TH_SUIT:
            return "�����";
        case TH_SUPER:
            return "�ਪ 100%";
        case TH_RTORCH:
            return "���� 䠪��";
        case TH_GTORCH:
            return "������ 䠪��";
        case TH_BTORCH:
            return "ᨭ�� 䠪��";
        case TH_GOR1:
            return "�㢠�";
        case TH_FCAN:
            return "������ ��窠";
        case TH_GUN2:
            return "�㯥�㫥���";

        case TH_DEMON:
            return "�����";
        case TH_IMP:
            return "���";
        case TH_ZOMBY:
            return "�����";
        case TH_SERG:
            return "�ঠ��";
        case TH_CYBER:
            return "����थ���";
        case TH_CGUN:
            return "�㫥���稪";
        case TH_BARON:
            return "��஭ ���";
        case TH_KNIGHT:
            return "����� ���";
        case TH_CACO:
            return "���������";
        case TH_SOUL:
            return "������� �९";
        case TH_PAIN:
            return "��������";
        case TH_SPIDER:
            return "����让 ���";
        case TH_BSP:
            return "��孮�஭";
        case TH_MANCUB:
            return "�������";
        case TH_SKEL:
            return "᪥���";
        case TH_VILE:
            return "�����";
        case TH_FISH:
            return "�론";
        case TH_BARREL:
            return "��窠";
        case TH_ROBO:
            return "஡��";
        case TH_MAN:
            return "�ਪ�����";
    }
    return NULL;
}
