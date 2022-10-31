/*
=============================================================================

                             MODCFG STUFF

=============================================================================
*/
extern  void  LoadModConfig(void);
extern  void  SetupVswap(void);

extern  boolean modded, vswap64;
extern  int texturesize;
extern  int textureshift;
extern  int texmask;
extern  int texfromfixedshift;
extern  int spritescale;
extern  char authnote[300];
extern  char modname[40];

#define ITEMTYPE       30
#define ITEMEFFECT     6
#define STATOBJECTS    100
#define HUDELEMENTS    25
#define NUMPROJECTILES 11

enum {
    t_none,
    //chase 
    t_default,
    t_range,
    t_melee,
    t_boss,
    t_pac,
    t_flee,
    //atttack
    t_defatk,
    t_needle,
    t_fire,
    t_ffire,
    t_rocket,
    t_bite,
    t_shoot
};

// HUD
enum {
    nohud,
    statusbar,
    area,
    item,
    itemstotal,
    secrets,
    secretstotal,
    score,
    kills,
    enemies,
    face,
    lives,
    health,
    ammo_general,
    ammo_gas,
    ammo_rockets,
    ammo_bullets,
    key_gold,
    key_silver,
    timer
};

// GiveItem
enum {
    item_default,
    item_ammo,
    item_health,
    item_damage,
    item_gas,
    item_rockets,
    item_time
};

// homing
enum {
    homing_none,
    homing_default,
    homing_low,
    homing_med,
    homing_high
};

// projectiles
enum {
    noprj,
    pl_fire,
    pl_miss,
    pl_need,
    pl_ffir,
    en_fire,
    en_miss,
    en_need,
    en_ffir
};

enum {
    w_none,
    w_melee,
    w_bullets,
    w_fire,
    w_rocket,
    w_needle,
    w_fakefire
};

typedef struct
{
    short       startlives;
    short       starthealth, maxhealth;
    int         startammo, maxammo, oldmaxammo;
    int         startgas, startrockets;
    int         maxgas, maxrockets;
    int         bagammo, maxbagammo;

    short       wp_speed[NUMWEAPONS];
    short       wp_style[NUMWEAPONS];
    short       wp_damage[NUMWEAPONS];
    short       wp_start[NUMWEAPONS];
    short       wp_type[NUMWEAPONS];
    short       wp_firerate[NUMWEAPONS];
    short       wp_dist[NUMWEAPONS];
    short       wp_knockback[NUMWEAPONS];
    short       wp_ammousage[NUMWEAPONS];
    short       wp_prjamount[NUMWEAPONS];
    short       wp_prjangle[NUMWEAPONS];
    boolean     wp_silenced[NUMWEAPONS];

    short       hud_x[HUDELEMENTS];
    short       hud_y[HUDELEMENTS];
    byte        hud_align[HUDELEMENTS];

    short       item[ITEMTYPE][ITEMEFFECT];
    short       item_ignore[ITEMTYPE];

    short       hitpoints[NUMENEMIES];
    short       en_damage[NUMENEMIES + 20];   // + projectiles
    short       en_animspeed[NUMENEMIES + 20];
    byte        en_chase[NUMENEMIES];
    byte        en_attack[NUMENEMIES];
    int32_t     en_speed[NUMENEMIES];
    boolean     en_rotation;
    boolean     boss_rotation;

    int32_t     prj_speed[NUMPROJECTILES];
    int32_t     prj_lifetime[NUMPROJECTILES];
    short       prj_homingfactor[NUMPROJECTILES];
    short       prj_splashdmg[NUMPROJECTILES];
    short       prj_splashradius[NUMPROJECTILES];
    short       prj_selfdmg[NUMPROJECTILES];
    short       prj_damage[NUMPROJECTILES];
    short       prj_playerknockback[NUMPROJECTILES];
    short       player_speed;

    boolean     startfromzero;
    byte        mapTime_end;
    int32_t     mapTime;
    byte        mission;
    byte        missionitem;

    boolean     statobj_nobonus[STATOBJECTS];
    byte        statobj_solid[STATOBJECTS];
    byte        statobj_lit[STATOBJECTS];
    byte        statobj_board[STATOBJECTS];
} modfile;

extern  modfile  mod;