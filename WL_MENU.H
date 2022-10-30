//
// WL_MENU.H
//

#define BORDCOLOR       0x29
#define BORD2COLOR      0x23
#define DEACTIVE        0x27
#define BKGDCOLOR       0x10
#define STRIPE          0x2c

#define MenuFadeOut()   VL_FadeOut(0,255,43,0,0,10)

#define EREADCOLOR       0xff
#define EREADHCOLOR      0xff
#define ETEXTCOLOR       0xff
#define EHIGHLIGHT       0x00

#define READCOLOR       0x10
#define READHCOLOR      0x10
//#define VIEWCOLOR       0x7f
#define VIEWCOLOR       0x00   
#define TEXTCOLOR       0x10
#define HIGHLIGHT       0x00
#define MenuFadeIn()    VL_FadeIn(0,255,gamepal,10)


//#define MENUSONG        WONDERIN_MUS
#define MENUSONG        NAZI_NOR_MUS    // APN: seamless menu music
#define INTROSONG       TITLE_MUS

#define SENSITIVE       60
#define CENTERX         ((int) screenWidth / 2)
#define CENTERY         ((int) screenHeight / 2)

#define MENU_X  12
#define MENU_Y  2
#define MENU_W  178
#ifndef SPEAR
#ifndef GOODTIMES
#define MENU_H  13*10+6
#else
#define MENU_H  13*9+6
#endif
#else
#define MENU_H  13*9+6
#endif

#define SM_X    85*2
#define SM_W    200
#define SM_Y    55*2
#define SM_H    10*13

#define CTL_X   24
#define CTL_Y   86
#define CTL_W   284
#define CTL_H   60

#define LSM_X   190
#define LSM_Y   160
#define LSM_W   175
#define LSM_H   10*13+10

#define NM_X    50
#define NM_Y    100
#define NM_W    225
#define NM_H    13*4+15

#define NE_X    22//160
#define NE_Y    92//130
#define NE_W    320-NE_X*2
#define NE_H    200-NE_Y*2

#define CST_X           120
#define CST_Y           120
#define CST_W           310
#define CST_H           280
#define CST_START       120
#define CST_SPC         70

#define MAINMENU0    16
#define MAINMENU1    42                                              
#define MAINMENU2    78
#define MAINMENU3    116
#define MAINMENU4    177
 
#define ABOUTX  230
#define ABOUTY  120
#define FILEX   135
#define FILEY   120
#define EDITX   135
#define EDITY   90
#define OPTIX   170
#define OPTIY   182

#define VI_X    170
#define VI_W    220
#define VI_Y    60
#define VI_H    10*13

#define WHITEC  0

//
// TYPEDEFS
//
typedef struct {
                short x,y,amount,curpos,indent;
                } CP_iteminfo;

typedef struct {
                short active;
                char string[36];
                int (* routine)(int temp1);
                } CP_itemtype;

typedef struct {
                short allowed[4];
                } CustomCtrls;

extern CP_itemtype MainMenu[];
extern CP_iteminfo MainItems;

//
// FUNCTION PROTOTYPES
//

void DrawSoundVols(bool);
int AdjustVolume(int curmode); 
extern int controlmousex, controlmousey; 

void US_ControlPanel(ScanCode);

void EnableEndGameMenuItem();

void SetupControlPanel(void);
void SetupSaveGames();
void CleanupControlPanel(void);

void DrawMenu(CP_iteminfo *item_i,CP_itemtype *items);
int  HandleMenu(CP_iteminfo *item_i,
                CP_itemtype *items,
                void (*routine)(int w));
void ClearMScreen(void);
void DrawWindow(int x,int y,int w,int h,int wcolor);
void DrawOutline(int x,int y,int w,int h,int color1,int color2);
void WaitKeyUp(void);
void ReadAnyControl(ControlInfo *ci);
void TicDelay(int count);
void CacheLump(int lumpstart,int lumpend);
void UnCacheLump(int lumpstart,int lumpend);
void CacheExtraLump(int lumpstart,int lumpend);
void UnCacheExtraLump(int lumpstart,int lumpend);
int StartCPMusic(int song);
int  Confirm(const char *string);
void Message(const char *string);
void CheckPause(void);
void ShootSnd(void);
void CheckSecretMissions(void);
void BossKey(void);

void DrawGun(CP_iteminfo *item_i,CP_itemtype *items,int x,int *y,int which,int basey,void (*routine)(int w));
void DrawHalfStep(int x,int y);
void EraseGun(CP_iteminfo *item_i,CP_itemtype *items,int x,int y,int which);
void SetTextColor(CP_itemtype *items,int hlight,CP_iteminfo *iteminfo);
void DrawMenuGun(CP_iteminfo *iteminfo);
void DrawStripes(int y);
void DrawFrame (int x, int y, int w, int h, int color1, int color2, int color3, int color4);

void DefineMouseBtns(void);
void DefineJoyBtns(void);
void DefineKeyBtns(void);
void DefineKeyBtns2(void);
void DefineKeyMove(void);
void EnterCtrlData(int index,CustomCtrls *cust,void (*DrawRtn)(int),void (*PrintRtn)(int),int type);

void DrawMainMenu(void);
void DrawTimer(void);
void DrawSoundMenu(void);
void DrawLoadSaveScreen(int loadsave);
void DrawNewEpisode(void);
void DrawNewGame(void);
void DrawChangeView(int view);
void DrawViewSlider (int size);
void DrawMouseSens(void);
void DrawCtlScreen(void);
void DrawCustomScreen(void);
void DrawVideo(void);
void DrawLSAction(int which);
void DrawCustMouse(int hilight);
void DrawCustJoy(int hilight);
void DrawCustKeybd(int hilight);
void DrawCustKeybd2(int hilight);
void DrawCustKeys(int hilight);
void PrintCustMouse(int i);
void PrintCustJoy(int i);
void PrintCustKeybd(int i);
void PrintCustKeys(int i);
void PrintCustKeybd2(int i);

void PrintLSEntry(int w,int color);
void TrackWhichGame(int w);
void DrawNewGameDiff(int w);
void DrawEpisodePic (int w);
void FixupCustom(int w);

int CP_About(int);
int CP_SetVideo(int);
int CP_FileMenu(int);
int CP_Edit(int);
int CP_NewGame(int);
int CP_Sound(int);
int  CP_LoadGame(int quick);
int  CP_SaveGame(int quick);
int CP_Control(int);
int CP_ChangeView(int);
int CP_ExitOptions(int);
int CP_Quit(int);
int CP_ViewScores(int);
int  CP_EndGame(int);
int  CP_CheckQuick(ScanCode scancode);
int CustomControls(int);
int MouseSensitivity(int);

void CheckForEpisodes(void);

void FreeMusic(void);


enum {MOUSE,JOYSTICK,KEYBOARDBTNS,KEYBOARDMOVE,KEYBOARDBTNS2};        // FOR INPUT TYPES

enum menuitems
{
        newgame,
        soundmenu,
        control,
        loadgame,
        savegame,
        changeview,

#ifndef GOODTIMES
#ifndef SPEAR
        readthis,
#endif
#endif

        viewscores,
        backtodemo,
        quit
};

//
// WL_INTER
//
typedef struct {
                int kill,secret,treasure;
                int32_t time;
                } LRstruct;

extern LRstruct LevelRatios[];

void Write (int x,int y,const char *string);
void NonShareware(void);
int GetYorN(int x,int y,int pic);
