// Player data and functions

#define PL_DRAWLIFE  1
#define PL_DRAWARMOR 2
#define PL_DRAWWPN   4
#define PL_DRAWFRAG  8
#define PL_DRAWAIR   16
#define PL_DRAWKEYS  32
#define PL_DRAWLIVES 64

#define PLK_U  1
#define PLK_D  2
#define PLK_L  4
#define PLK_R  8
#define PLK_F  16
#define PLK_J  32
#define PLK_P  64
#define PLK_W  128
#define PLK_WL 1
#define PLK_WR 2

#define PLF_FIRE  1
#define PLF_PNSND 2
#define PLF_UP    4
#define PLF_DOWN  8

#define PL_POWERUP_TIME 546

typedef struct {
    obj_t o;
    int looky;
    int st, s;
    int life, armor, hit, hito;
    int pain, air;
    int invl, suit;
    char d;
    byte ku, kd, kl, kr, kf, kj, kwl, kwr, kp;
    int frag, ammo, shel, rock, cell, fuel, kills, secrets;
    byte fire, cwpn, csnd;
    byte amul;
    word wpns;
    char wpn;
    byte f;
    byte drawst;
    byte color;
    int id;
    byte keys;
    char lives;
} player_t;

void PL_init(void);

void PL_alloc(void);

void PL_spawn(player_t *, int, int, char);

int PL_hit(player_t *, int, int, int);

int PL_isdead(player_t *);

void PL_act(player_t *);

void PL_draw(player_t *);

void PL_drawst(player_t *);

void PL_cry(player_t *);

void PL_damage(player_t *);

int PL_give(player_t *, int);

void G_respawn_player(player_t *);

extern byte p_immortal;
extern player_t pl1, pl2;
