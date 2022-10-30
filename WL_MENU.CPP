////////////////////////////////////////////////////////////////////
//
// WL_MENU.C
// by John Romero (C) 1992 Id Software, Inc.
//
////////////////////////////////////////////////////////////////////

#include <sys/stat.h>
#include <locale> 
#include <sys/types.h>
#ifdef _WIN32
    #include <io.h>
    #include <direct.h>
#else
    #include <unistd.h>
#endif

#include "wl_def.h"
#pragma hdrstop

extern int lastgamemusicoffset;
extern int numEpisodesMissing;

//
// PRIVATE PROTOTYPES
//
int CP_ReadThis (int);

#ifdef SPEAR
#define STARTITEM       newgame

#else
#ifdef GOODTIMES
#define STARTITEM       newgame

#else
#define STARTITEM       readthis
#endif
#endif

// ENDSTRx constants are defined in foreign.h
char endStrings[9][80] = {
    ENDSTR1,
    ENDSTR2,
    ENDSTR3,
    ENDSTR4,
    ENDSTR5,
    ENDSTR6,
    ENDSTR7,
    ENDSTR8,
    ENDSTR9
};

CP_itemtype MainMenu[] = {
    {1, "", CP_About},        
    {1, "File", CP_FileMenu},
    {1, "Edit", CP_Edit},
    {1, "Options", CP_Control},
    {1, "<< Game is Paused! >>", 0}
}; 

CP_itemtype FileMenu[] = {
    {1, "New Game...     N", CP_NewGame},
    {1, "Open...            O", CP_LoadGame},
    {0, "----------------", 0},
    {0, "Close", 0},
    {0, "Save              S", CP_SaveGame},
    {0, "Save As...", CP_SaveGame},
    {0, "----------------", 0},
    {1, STR_QT, CP_Quit}
};  
 
CP_itemtype OptiMenu[] = {
    {1, "Audio", CP_Sound},
    {1, "Set Screen size...", CP_SetVideo},
    {0, STR_MOUSEEN, 0},
    {0, "Mouse Y-axis", 0},
    {0, STR_SENS, MouseSensitivity},
    {0, STR_JOYEN, 0},
    {1, "Pause", 0},
    {1, "Damage Indicator", 0},
    {1, "Boss Fix", 0},
    {1, "Classic Player Set", 0},
    {1, "1994 Limits", 0},
    {1, "Autorun", 0},
    {1, STR_CUSTOM, CustomControls} 
};

CP_itemtype VidMenu[] = {
    {0, "", 0},        
    {1, "Screen size", 0},
    {0, "", 0},
    {0, "", 0},
    {0, "", 0},
    {0, "Video mode:", 0},
    {0, "", 0},
    {1, "Fullscreen", 0}, 
    {0, "", 0},
    {1, "640x400", 0},
    {1, "640x480", 0},
    {1, "720x480", 0},
    {1, "800x600", 0},
    {1, "960x600", 0},
    {1, "1024x768", 0},
    {1, "1280x720", 0},
    {1, "1280x960", 0},
    {1, "1366x768", 0},
    {1, "1920x1080", 0},
    {1, "3840x2160", 0},
};

CP_itemtype AboutMenu[] = {
    {1, "About Wolfenstein 3D...", 0},
    {1, "Speed tips", 0},
    {1, "About the shareware versions...", 0},
    {0, "-----------------------------", 0}, 
    {1, STR_VS, CP_ViewScores},
    {0, "", 0},
    {0, "-----------------------------", 0},
    {0, "M. Version 1.10", 0}
};

CP_itemtype EditMenu[] = {
    {1, "Undo               Z", 0},
    {0, "----------------", 0},
    {0, "Cut                X", 0},
    {0, "Copy.              C", 0},
    {0, "Paste.             V", 0},
    {0, "Clear", 0}
};

CP_itemtype SndMenu[] = {    
    {1, "Sound", 0},
    {1, "Sound Volume",},
    {1, "Music", 0},
    {1, "Music Volume",},
    {1, "Reverse Stereo", 0},     
};

enum { CTL_MOUSEENABLE, CTL_MOUSEYAXIS, CTL_MOUSESENS, CTL_JOYENABLE, CTL_CUSTOMIZE };

CP_itemtype NewEmenu[] = {
    {1, "Episode 1\n" "First Encounter", 0},
    {1, "Episode 2\n" "Second Encounter", 0},
    {1, "Episode 3\n" "Die, Fuhrer, Die!", 0},
    {1, "Episode 4\n" "A Dark Secret", 0},
    {1, "Episode 5\n" "Trail of the Madman", 0},
    {1, "Episode 6\n" "Confrontation", 0},
    {1, "E7", 0},
    {1, "E8", 0},
    {1, "E9", 0},
    {1, "E10", 0},
    {1, "E11", 0},
    {1, "E12", 0}
};

CP_itemtype NewMenu[] = {
    {1, "", 0},
    {1, "", 0},
    {1, "", 0},
    {1, "", 0}
};

CP_itemtype LSMenu[] = {
    {1, "", 0},
    {1, "", 0},
    {1, "", 0},
    {1, "", 0},
    {1, "", 0},
    {1, "", 0},
    {1, "", 0},
    {1, "", 0},
    {1, "", 0},
    {1, "", 0}
};

CP_itemtype CusMenu[] = {
    {1, "", 0},
    {0, "", 0},
    {0, "", 0},
    {0, "", 0},
    {1, "", 0},
    {0, "", 0},
    {0, "", 0},
    {0, "", 0},
    {1, "", 0},
    {0, "", 0},
    {0, "", 0},
    {1, "", 0}, 
    {0, "", 0},
    {0, "", 0},
    {1, "", 0}
};

// CP_iteminfo struct format: short x, y, amount, curpos, indent;
CP_iteminfo MainItems = { MENU_X, MENU_Y, lengthof(MainMenu), 1, 24 },
            SndItems  = { SM_X, SM_Y+26, lengthof(SndMenu), 0, 0 },
            LSItems   = { LSM_X, LSM_Y, lengthof(LSMenu), 0, 0 },
            CusItems  = { CST_X, CST_Y + 13 * 2, lengthof(CusMenu), 0, 0},
            NewEitems = { NE_X, NE_Y, lengthof(NewEmenu), 0, 0 },  
            AboutItems= { MAINMENU0+10, 25, lengthof(FileMenu), 0, 0},
            FileItems = { MAINMENU1+10, 25, lengthof(FileMenu), 0, 0},
            EditItems = { MAINMENU2+10, 25, lengthof(EditMenu), 0, 0},
            OptiItems = { MAINMENU3+10, 25, lengthof(OptiMenu), 0, 0},
            VidItems  = { VI_X, VI_Y, lengthof(VidMenu), 1, 0 }, 
            NewItems  = { 2, NM_Y, lengthof(NewMenu), 0, 24 };
 
int color_hlite[] = {
    DEACTIVE,
    HIGHLIGHT,
    READHCOLOR,
    0x67
};  

int color_norml[] = {
    DEACTIVE,
    TEXTCOLOR,
    READCOLOR, 
    0x6b
};
 
int ep_hlite[] = {
    DEACTIVE,
    EHIGHLIGHT,
    EREADHCOLOR,
    0xff
};   
 
int ep_norml[] = {
    DEACTIVE,
    ETEXTCOLOR,
    EREADCOLOR,
    0xff
};

int bkgdcolor = 0x10;
boolean changedres = false;

int EpisodeSelect[6] = { 1 };
 
static int SaveGamesAvail[10];
static int StartGame;
static int SoundStatus = 1;
static int pickquick;
static char SaveGameNames[10][32];
static char SaveName[13] = "savegam?.";

SDL_Surface *tempgame;  
////////////////////////////////////////////////////////////////////
//
// INPUT MANAGER SCANCODE TABLES
//
////////////////////////////////////////////////////////////////////

#if 0
static const char *ScanNames[] =      // Scan code names with single chars
{
    "?", "?", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "+", "?", "?",
    "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "|", "?", "A", "S",
    "D", "F", "G", "H", "J", "K", "L", ";", "\"", "?", "?", "?", "Z", "X", "C", "V",
    "B", "N", "M", ",", ".", "/", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?", "\xf", "?", "-", "\x15", "5", "\x11", "+", "?",
    "\x13", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?"
};                              // DEBUG - consolidate these
static ScanCode ExtScanCodes[] =        // Scan codes with >1 char names
{
    1, 0xe, 0xf, 0x1d, 0x2a, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e,
    0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x57, 0x59, 0x46, 0x1c, 0x36,
    0x37, 0x38, 0x47, 0x49, 0x4f, 0x51, 0x52, 0x53, 0x45, 0x48,
    0x50, 0x4b, 0x4d, 0x00
};
static const char *ExtScanNames[] =   // Names corresponding to ExtScanCodes
{
    "Esc", "BkSp", "Tab", "Ctrl", "LShft", "Space", "CapsLk", "F1", "F2", "F3", "F4",
    "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "ScrlLk", "Enter", "RShft",
    "PrtSc", "Alt", "Home", "PgUp", "End", "PgDn", "Ins", "Del", "NumLk", "Up",
    "Down", "Left", "Right", ""
};

/*#pragma warning 737 9
static byte
                                        *ScanNames[] =          // Scan code names with single chars
                                        {
        "?","?","1","2","3","4","5","6","7","8","9","0","-","+","?","?",
        "Q","W","E","R","T","Y","U","I","O","P","[","]","|","?","A","S",
        "D","F","G","H","J","K","L",";","\"","?","?","?","Z","X","C","V",
        "B","N","M",",",".","/","?","?","?","?","?","?","?","?","?","?",
        "?","?","?","?","?","?","?","?","\xf","?","-","\x15","5","\x11","+","?",
        "\x13","?","?","?","?","?","?","?","?","?","?","?","?","?","?","?",
        "?","?","?","?","?","?","?","?","?","?","?","?","?","?","?","?",
        "?","?","?","?","?","?","?","?","?","?","?","?","?","?","?","?"
                                        };      // DEBUG - consolidate these
static byte ExtScanCodes[] =    // Scan codes with >1 char names
                                        {
        1,0xe,0xf,0x1d,0x2a,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,
        0x3f,0x40,0x41,0x42,0x43,0x44,0x57,0x59,0x46,0x1c,0x36,
        0x37,0x38,0x47,0x49,0x4f,0x51,0x52,0x53,0x45,0x48,
        0x50,0x4b,0x4d,0x00
                                        };
static byte *ExtScanNames[] =   // Names corresponding to ExtScanCodes
                                        {
        "Esc","BkSp","Tab","Ctrl","LShft","Space","CapsLk","F1","F2","F3","F4",
        "F5","F6","F7","F8","F9","F10","F11","F12","ScrlLk","Enter","RShft",
        "PrtSc","Alt","Home","PgUp","End","PgDn","Ins","Del","NumLk","Up",
        "Down","Left","Right",""
                                        };*/

#else
static const char* const ScanNames[SDLK_LAST] =
    {
        "?","?","?","?","?","?","?","?",                                //   0
        "BkSp","Tab","?","?","?","Return","?","?",                      //   8
        "?","?","?","Pause","?","?","?","?",                            //  16
        "?","?","?","Esc","?","?","?","?",                              //  24
        "Space","!","\"","#","$","?","&","'",                           //  32
        "(",")","*","+",",","-",".","/",                                //  40
        "0","1","2","3","4","5","6","7",                                //  48
        "8","9",":",";","<","=",">","?",                                //  56
        "@","A","B","C","D","E","F","G",                                //  64
        "H","I","J","K","L","M","N","O",                                //  72
        "P","Q","R","S","T","U","V","W",                                //  80
        "X","Y","Z","[","\\","]","^","_",                               //  88
        "`","a","b","c","d","e","f","g",                                //  96
        "h","i","j","k","l","m","n","o",                                // 104
        "p","q","r","s","t","u","v","w",                                // 112
        "x","y","z","{","|","}","~","?",                                // 120
        "?","?","?","?","?","?","?","?",                                // 128
        "?","?","?","?","?","?","?","?",                                // 136
        "?","?","?","?","?","?","?","?",                                // 144
        "?","?","?","?","?","?","?","?",                                // 152
        "?","?","?","?","?","?","?","?",                                // 160
        "?","?","?","?","?","?","?","?",                                // 168
        "?","?","?","?","?","?","?","?",                                // 176
        "?","?","?","?","?","?","?","?",                                // 184
        "?","?","?","?","?","?","?","?",                                // 192
        "?","?","?","?","?","?","?","?",                                // 200
        "?","?","?","?","?","?","?","?",                                // 208
        "?","?","?","?","?","?","?","?",                                // 216
        "?","?","?","?","?","?","?","?",                                // 224
        "?","?","?","?","?","?","?","?",                                // 232
        "?","?","?","?","?","?","?","?",                                // 240
        "?","?","?","?","?","?","?","?",                                // 248
        "?","?","?","?","?","?","?","?",                                // 256
        "?","?","?","?","?","?","?","Enter",                            // 264
        "?","Up","Down","Right","Left","Ins","Home","End",              // 272
        "PgUp","PgDn","F1","F2","F3","F4","F5","F6",                    // 280
        "F7","F8","F9","F10","F11","F12","?","?",                       // 288
        "?","?","?","?","NumLk","CapsLk","ScrlLk","RShft",              // 296
        "Shift","RCtrl","Ctrl","RAlt","Alt","?","?","?",                // 304
        "?","?","?","?","PrtSc","?","?","?",                            // 312
        "?","?"                                                         // 320
    };

#endif
 
////////////////////////////////////////////////////////////////////
//
// Wolfenstein Control Panel!  Ta Da!
//
//////////////////////////////////////////////////////////////////// 
// horiz main menu bars
#define BAR_ABOUT 28 
#define BAR_FILE  36                                                   
#define BAR_EDIT  36
#define BAR_OPTI  56
#define BAR_PAUS  146
#define BAR_HIGH  18

// vert scrolling bars
#define SUB_BAR_W 114
#define SUB_BAR_H 13
 
#define SUB_ABOUT_W 210
#define SUB_LOAD_W  (LSM_W - LSItems.indent - 16)
      
void Erase (CP_iteminfo * item_i, CP_itemtype * items, int x, int y, int which)
{
    SetTextColor (items + which, 0, item_i);

    if (which == 0)
    {
        item_i->x =  MAINMENU0;
        //VWB_640Bar (item_i->x - 5, y, BAR_ABOUT, BAR_HIGH, WHITEC);  
        VWB_DrawPicScaledRes (scale640*11, scale640*0, C_BABYMODEPIC, 3);   
    }
    else if (which == 1)
    {
        item_i->x =  MAINMENU1;// + which * 13;
        VWB_640Bar (item_i->x - 5, y, BAR_FILE, BAR_HIGH, WHITEC);    
        //VWB_Bar (item_i->x/scaleFactor  - 5, y, BAR_PAUS/scaleFactor, 10, WHITEC);  
    }
    else if (which == 2)
    {
        item_i->x =  MAINMENU2;
        VWB_640Bar (item_i->x - 5, y, BAR_EDIT, BAR_HIGH, WHITEC);    
    }
    else if (which == 3)
    {
        item_i->x =  MAINMENU3;  
        VWB_640Bar (item_i->x - 5, y, BAR_OPTI, BAR_HIGH, WHITEC);    
    }
    else if (which == 4)
    {
        item_i->x = MAINMENU4;  
        VWB_640Bar (item_i->x - 5, y, BAR_PAUS, BAR_HIGH, WHITEC);    
    } 
        
    PrintX = item_i->x;// + item_i->indent;     
    PrintY = item_i->y;// + which * 13;
    US_Print640 ((items + which)->string);
    VW_UpdateScreen ();
}

//
// DRAW GUN AT NEW POSITION
//
void DrawPos (CP_iteminfo * item_i, CP_itemtype * items, int x, int y, int which, int basey, void (*routine) (int w))
{
    SetTextColor (items + which, 1, item_i); 

    if (which == 0)
    {
        item_i->x =  MAINMENU0;// + which * 13;
        //VWB_640Bar (item_i->x - 5, y, BAR_ABOUT, BAR_HIGH, BKGDCOLOR);  
        VWB_DrawPicScaledRes (scale640*11, scale640*0, C_EASYPIC, 3);  
    }
    else if (which == 1)
    {
        item_i->x =  MAINMENU1;// + which * 13;
        VWB_640Bar (item_i->x - 5, y, BAR_FILE, BAR_HIGH, BKGDCOLOR);    
    }
    else if (which == 2)
    {
        item_i->x =  MAINMENU2;
        VWB_640Bar (item_i->x - 5, y, BAR_EDIT, BAR_HIGH, BKGDCOLOR);    
    }
    else if (which == 3)
    {
        item_i->x =  MAINMENU3;  
        VWB_640Bar (item_i->x - 5, y, BAR_OPTI, BAR_HIGH, BKGDCOLOR);    
    }
    else if (which == 4)
    {
        item_i->x = MAINMENU4;  
        VWB_640Bar (item_i->x - 5, y, BAR_PAUS, BAR_HIGH, BKGDCOLOR);    
    }   
           
    PrintX = item_i->x;// + item_i->indent;          
    PrintY = item_i->y;// + which * 13;
    US_Print640 ((items + which)->string);

    //
    // CALL CUSTOM ROUTINE IF IT IS NEEDED
    //
    if (routine)
        routine (which);
    VW_UpdateScreen ();
    SD_PlaySound (MOVEGUN2SND);
}

int HandleMain (CP_iteminfo * item_i, CP_itemtype * items, void (*routine) (int w), int menujump)
{
    char key;
    static int redrawitem = 1, lastitem = -1;
    int i, x, y, basey, exit, which, shape;
    int32_t lastBlinkTime, timer;
    ControlInfo ci;

    controlmousex = 0, controlmousey = 0;
    
    which = item_i->curpos;
    
    if (which == 0)                               // x-coords for top menu
        item_i->x = MAINMENU0;// + which * 13;
    else if (which == 1)
        item_i->x = MAINMENU1;
    else if (which == 2)
        item_i->x = MAINMENU2;    
    else if (which == 3)
        item_i->x = MAINMENU3; 
    else if (which == 4)
        item_i->x = MAINMENU4; 
        
    x = item_i->x;// & -8;             
    basey = item_i->y - 2;
    y = basey;// + which * 13;

    //VWB_DrawPic (x, y, C_CURSOR1PIC);
    SetTextColor (items + which, 1, item_i);
    if (redrawitem)
    {
        PrintX = item_i->x;// + item_i->indent;// + which * 13;
        PrintY = item_i->y;// + which * 13;
        US_Print640 ((items + which)->string);
    }
    //
    // CALL CUSTOM ROUTINE IF IT IS NEEDED
    //
    if (routine)
        routine (which);
    VW_UpdateScreen ();

    shape = C_CURSOR1PIC;
    timer = 60*60;
    exit = 0;
    lastBlinkTime = GetTimeCount ();
    IN_ClearKeysDown ();

    do
    {
        //
        // CHANGE GUN SHAPE
        //
        timer--;
        
        if (timer <= 0)
        {   
            timer = 60*60;                    
            DrawTimer (); 
            VW_UpdateScreen ();
        }
        
        SDL_Delay(5);

        CheckPause ();

        //
        // SEE IF ANY KEYS ARE PRESSED FOR INITIAL CHAR FINDING
        //
        key = LastASCII;
        if (key)
        {
            int ok = 0;

            if (key >= 'a')
                key -= 'a' - 'A';

            for (i = which + 1; i < item_i->amount; i++)
                if ((items + i)->active && (items + i)->string[0] == key)
                {
                    Erase (item_i, items, x, y, which);
                    which = i;
                    DrawPos (item_i, items, x, y, which, basey, routine);
                    ok = 1;
                    IN_ClearKeysDown ();
                    break;
                }

            //
            // DIDN'T FIND A MATCH FIRST TIME THRU. CHECK AGAIN.
            //
            if (!ok)
            {
                for (i = 0; i < which; i++)
                    if ((items + i)->active && (items + i)->string[0] == key)
                    {
                        Erase (item_i, items, x, y, which);
                        which = i;
                        DrawPos (item_i, items, x, y, which, basey, routine);
                        IN_ClearKeysDown ();
                        break;
                    }
            }
        }
        
        if (menujump == 666)        // jump from sub menu directly to next main menu spot     
        {
                Erase (item_i, items, x, y, which);
                if (which && (items + which - 1)->active)
                    DrawHalfStep (x, y);

                // MOVE TO NEXT SPOT
                do
                {
                    if (which == item_i->amount - 1)
                        which = 0;
                    else
                        which++;
                }
                while (!(items + which)->active);

                DrawPos (item_i, items, x, y, which, basey, routine);
                TicDelay (20);
                exit = 2+which;      
        }
        else if (menujump == 667)       // jump from sub menu directly to next main menu spot, to the left
        {
                Erase (item_i, items, x, y, which);
                if (which && (items + which - 1)->active)
                    DrawHalfStep (x, y);

                // MOVE TO NEXT SPOT
                do
                {
                    if (!which)
                        which = item_i->amount - 1;
                    else
                        which--;
                }
                while (!(items + which)->active);

                DrawPos (item_i, items, x, y, which, basey, routine);
                TicDelay (20);
                
                if (which == 0) // jump from first to last
                  exit = 7;
                else
                  exit = which+2;      
        }

        //
        // GET INPUT
        //
        ReadAnyControl (&ci);
        switch (ci.dir)
        {
                ////////////////////////////////////////////////
                //
                // MOVE LEFT
                //
            case dir_West:

                Erase (item_i, items, x, y, which);

                //
                // ANIMATE HALF-STEP
                //
                if (which && (items + which - 1)->active)
                {
                    //y -= 6;
                    DrawHalfStep (x, y);
                }

                //
                // MOVE TO NEXT AVAILABLE SPOT
                //
                do
                {
                    if (!which)
                        which = item_i->amount - 1;
                    else
                        which--;
                }
                while (!(items + which)->active);

                DrawPos (item_i, items, x, y, which, basey, routine);
                //
                // WAIT FOR BUTTON-UP OR DELAY NEXT MOVE
                //
                TicDelay (20);
                break;

                ////////////////////////////////////////////////
                //
                // MOVE Right
                //
            case dir_East:

                Erase (item_i, items, x, y, which);
                //
                // ANIMATE HALF-STEP
                //
                if (which != item_i->amount - 1 && (items + which + 1)->active)
                {
                   // x += 6;                   
                    DrawHalfStep (x, y);
                }

                do
                {
                    if (which == item_i->amount - 1)
                        which = 0;
                    else
                        which++;
                }
                while (!(items + which)->active);

                DrawPos (item_i, items, x, y, which, basey, routine);

                //
                // WAIT FOR BUTTON-UP OR DELAY NEXT MOVE
                //
                TicDelay (20);
                break;
        }  
        
        if (ci.button0 || Keyboard[sc_Space] || Keyboard[sc_Enter] || ci.dir == dir_South)
            exit = 1;

        if (ci.button1 && !Keyboard[sc_Alt] || Keyboard[sc_Escape])
            exit = 2;

    }
    while (!exit);


    IN_ClearKeysDown ();

    //
    // ERASE EVERYTHING
    //
    if (lastitem != which)
    {       
        //VWB_Bar (x - 5, y, 25, 16, BKGDCOLOR);
        PrintX = item_i->x;// + item_i->indent;// + which * 13;
        PrintY = item_i->y;// + which * 13;
        US_Print640 ((items + which)->string);
        redrawitem = 1;
    }
    else
        redrawitem = 0;

    if (routine)
        routine (which);
    VW_UpdateScreen ();

    item_i->curpos = which;

    lastitem = which;
    
    switch (exit)
    {
        case 1:
            //
            // CALL THE ROUTINE
            //
            if ((items + which)->routine != NULL)
            {
                //ShootSnd ();
                //MenuFadeOut ();
                (items + which)->routine (0);
            }
            return which;

        case 2:
            SD_PlaySound (ESCPRESSEDSND); 
            //if (ingame) //StartGame = 1;
            //   US_ControlPanel(999); 
            return -1;
        
        case 3:    
            CP_FileMenu (0);
            return which;       
        case 4:    
            CP_Edit (0);
            return which;
        case 5:    
            CP_Control (0);
            return which;   
        case 6:   
            US_ControlPanel(0);  
            return which;   
        case 7:    
            CP_About (0);
            return which;    //-1
    }

    return 0;                   // JUST TO SHUT UP THE ERROR MESSAGES!
}


void US_ControlPanel (ScanCode scancode)
{
    int which;

#ifdef _arch_dreamcast
    DC_StatusClearLCD();
#endif

    if (ingame) 
    {
        if (CP_CheckQuick (scancode))
            return;
        lastgamemusicoffset = StartCPMusic (MENUSONG);
    }
//    else
//        StartCPMusic(MENUSONG);        // APN: seamless menu music
    SetupControlPanel ();
    
    bkgdcolor = 0x10;                   // set menu font/bar color back to normal (cause episode menu)
    //
    // F-KEYS FROM WITHIN GAME
    //
    switch (scancode)
    {
        //case sc_F1:
        //    HelpScreens ();
        //    goto finishup;

        case sc_F2:
            CP_SaveGame (0);
            goto finishup;

        case sc_F3: 
            CP_LoadGame (0);
            goto finishup;

        case sc_F4:
            CP_Sound (0);
            goto finishup;

        case sc_F5:
            CP_SetVideo (0);
            goto finishup;

        case sc_F6:
            CP_Control (0);
            goto finishup;

        finishup:   
            CleanupControlPanel ();
            
            return;
    }

    DrawMainMenu ();
    MenuFadeIn ();

    StartGame = 0;
        
    //
    // MAIN MENU LOOP
    //
    do
    {
        which = HandleMain (&MainItems, &MainMenu[0], NULL, scancode);

        switch (which)
        {        
 /*           case viewscores:
                if (MainMenu[viewscores].routine == NULL)
                {
                    if (CP_EndGame (0))
                        StartGame = 1;
                }
                else
                {
                    DrawMainMenu();
                    MenuFadeIn ();
                }
                break;

            case backtodemo:
                StartGame = 1;
                if (!ingame)
//                                        StartCPMusic(INTROSONG);  // APN: seamless music
                VL_FadeOut (0, 255, 0, 0, 0, 10);
                break;

            case -1:
            case quit:
                CP_Quit (0);
                break;
                */
            case -1:
                 if (ingame)
                     StartGame = 1;
                 if (changedres)    
                     changedres = false;    
                 break;         
                 
            default:
                if (!StartGame)
                {
                    DrawMainMenu ();
                    MenuFadeIn ();
                }
        }

        //
        // "EXIT OPTIONS" OR "NEW GAME" EXITS
        //
    }
    while (!StartGame);

    //
    // DEALLOCATE EVERYTHING
    //
    CleanupControlPanel ();

    //
    // CHANGE MAINMENU ITEM
    //
    //if (startgame || loadedgame)
    //    EnableEndGameMenuItem();

    // RETURN/START GAME EXECUTION

#ifdef SPEAR
    UnCacheLump (OPTIONS_LUMP_START, OPTIONS_LUMP_END);
#endif
}

void EnableEndGameMenuItem()
{
    MainMenu[viewscores].routine = NULL;
    strcpy (MainMenu[viewscores].string, STR_EG);
}

////////////////////////
//
// DrawTimer
//
// updates every 60*60 tics
//
 
void DrawTimer (void) 
{
         time_t now = time(0);
         char gg [80];
         struct tm * timeinfo;
         time_t rawtime;
         time (&rawtime);
         timeinfo = localtime (&rawtime);

         SETFONTCOLOR (BKGDCOLOR,0);
         PrintX = screenWidth-120;
         PrintY = 2;
         
         // customize, saveload, viewscores, ingame keys
         VWB_640Bar (PrintX - 3, 1, 60, 11, WHITEC);    
         
         // 24h TIME
         /*  
         //struct tm timeinfo = *localtime(&now);
         
         if (timeinfo.tm_hour < 10)
           US_Print640("0");   
         US_PrintUns640(timeinfo.tm_hour);
         US_Print640(":");
         
         if (timeinfo.tm_min < 10)
           US_Print640("0");
         US_PrintUns640(timeinfo.tm_min);
         */
         
         // AM/PM TIME
         strftime (gg,80,"%I:%M %p",timeinfo);
         US_Print640(gg);
         
         //US_PrintUnsigned(timeinfo.tm_mday);
         //US_Print("/");  
         //US_PrintUnsigned(timeinfo.tm_mon+1);
         //US_Print("/");  
         //US_PrintUnsigned(timeinfo.tm_year+1900);
}

void DrawMainMenu (void) 
{
    ClearMScreen ();
    fontnumber = 0;
    DrawStripes (10);
    DrawTimer (); 

    //
    // CHANGE "GAME" AND "DEMO"
    //
    /*if (ingame)
    {
        strcpy (&MainMenu[backtodemo].string[8], STR_GAME);
        MainMenu[backtodemo].active = 2;
    }
    else
    {
        strcpy (&MainMenu[backtodemo].string[8], STR_DEMO);
        MainMenu[backtodemo].active = 1;
    }*/

    //DrawMenu (&MainItems, &MainMenu[0]);
    
    CP_iteminfo * item_i;
    CP_itemtype * items;
    
    item_i = &MainItems;
    items = &MainMenu[0];
    
    int i, which = item_i->curpos;
    
    VWB_DrawPicScaledRes (scale640*11, scale640*0, C_BABYMODEPIC, 3); // draw icon when in submenu
    
    // draw black bars only for one main tab if inside another menu

    if (which == 0)     
        //VWB_Bar (MAINMENU1/scaleFactor - 5, item_i->y - 2, BAR_FILE/scaleFactor, 10, BKGDCOLOR);  
        //VWB_640Bar (MAINMENU0 - 5, item_i->y - 2, BAR_ABOUT, BAR_HIGH, BKGDCOLOR);   
        VWB_DrawPicScaledRes (scale640*11, scale640*0, C_EASYPIC, 3); 
    else if (which == 1)     
        VWB_640Bar (MAINMENU1 - 5, item_i->y - 2, BAR_FILE, BAR_HIGH, BKGDCOLOR);   
    else if (which == 2)
        VWB_640Bar (MAINMENU2 - 5, item_i->y - 2, BAR_EDIT, BAR_HIGH, BKGDCOLOR);    
    else if (which == 3)      
        VWB_640Bar (MAINMENU3 - 5, item_i->y - 2, BAR_OPTI, BAR_HIGH, BKGDCOLOR);  
    else if (which == 4)
        VWB_640Bar (MAINMENU4 - 5, item_i->y - 2, BAR_PAUS, BAR_HIGH, BKGDCOLOR);

    for (i = 0; i < item_i->amount; i++)
    {
        SetTextColor (items + i, which == i, item_i);
        
        if (i == 0)
            item_i->x = MAINMENU0;
        else if (i == 1)
            item_i->x = MAINMENU1;
        else if (i == 2)
            item_i->x = MAINMENU2;
        else if (i == 3)
            item_i->x = MAINMENU3;    
        else if (i == 4)
            item_i->x = MAINMENU4;    
        
        PrintY = item_i->y;//+ i * 13;
        PrintX = item_i->x;// + i * 13;  
           
        if ((items + i)->active) // active menu points
        {
            /*if (main == 0)
            {           
               if (i == 0)       
                  VWB_Bar (item_i->x/scaleFactor - 5, item_i->y - 2, BAR_FILE/scaleFactor, 10, BKGDCOLOR);    
               else if (which == 1)
                  VWB_Bar (item_i->x/scaleFactor - 5, item_i->y - 2, BAR_EDIT/scaleFactor, 10, BKGDCOLOR);    
               else if (which == 2)
                  VWB_Bar (item_i->x/scaleFactor - 5, item_i->y - 2, BAR_OPTI/scaleFactor, 10, BKGDCOLOR);  
               else if (which == 3)
                  VWB_Bar (item_i->x/scaleFactor - 5, item_i->y - 2, BAR_PAUS/scaleFactor, 10, BKGDCOLOR); 
            }   */  
            
            US_Print640 ((items + i)->string);
        }
        else 
        {  
            //VWB_Bar (item_i->x/scaleFactor , item_i->y, 25/scaleFactor, 10, 0); 
            SETFONTCOLOR (DEACTIVE, BKGDCOLOR);
            US_Print640 ((items + i)->string);
            SETFONTCOLOR (TEXTCOLOR, BKGDCOLOR);
        }
      //  US_Print ("\n");
    }
    
    VW_UpdateScreen ();
}

////////////////////////
//
// DrawAboutMenu
//
void DrawAboutMenu (void)
{
    int i, on;
    
    //ClearMScreen ();
    DrawMainMenu();
    fontnumber = 0;
    
    strcpy (&AboutMenu[5].string[0], authnote);
    
    DrawWindow (MAINMENU0 - 5, 20, ABOUTX, ABOUTY, 0);

    DrawMenu (&AboutItems, &AboutMenu[0]);

    DrawMenuGun (&AboutItems);
    VW_UpdateScreen ();
} 

int CP_About (int)
{
    int which;
    int exit;
    ControlInfo ci;

    DrawAboutMenu ();
    WaitKeyUp ();

    do 
    {
        which = HandleMenu (&AboutItems, &AboutMenu[0], NULL);
        //
        // HANDLE MENU CHOICES
        //     
        if (which == -1) 
        {
             US_ControlPanel(0);
             break;        
        }    
    }
    while (which<0);
        
    exit = 0;
    
    // left/right the menus and properly exit them so they wont be called in another menu
    if (which == 33) { US_ControlPanel(666); exit = 2; } //CP_FileMenu(0);
    if (which == 44) { US_ControlPanel(667); exit = 2; }
    
    do 
    {
        SDL_Delay(5);
        ReadAnyControl (&ci);  
        if (which == 0)
        {     
           VWB_DrawPicScaledRes (scale640*80, scale640*32, C_OPTIONSPIC, 3);  
           VW_UpdateScreen ();    
        } 
        else if (which == 1)
        {
           VWB_DrawPicScaledRes (scale640*80, scale640*80, C_FXTITLEPIC, 3);  
           VW_UpdateScreen ();        
        }   
        else if (which == 2)
        {
           VWB_DrawPicScaledRes (scale640*80, scale640*80, C_DIGITITLEPIC, 3);  
           VW_UpdateScreen ();        
        }  
        
        if (ci.button0 || Keyboard[sc_Space] || Keyboard[sc_Enter] || ci.button1 || Keyboard[sc_Escape])
            exit = 1;
         
    }
    while (!exit && which >= 0);    
    
    if (exit == 1)
          US_ControlPanel(0);

    return 0;
}

////////////////////////
//
// DrawFileMenu
//
void DrawFileMenu (void)
{
    int i, on;
    
    //ClearMScreen ();
    DrawMainMenu();
    fontnumber = 0;
    
    // make saving game option available
    if (ingame)
        FileMenu[4].active = 1;
    else
        FileMenu[4].active = 0;
    
    DrawWindow (MAINMENU1 - 5, 20, FILEX, FILEY, 0);

    DrawMenu (&FileItems, &FileMenu[0]);

    DrawMenuGun (&FileItems);  
    VW_UpdateScreen ();
} 

int CP_FileMenu (int)
{
    int which;

    DrawFileMenu ();
    //MenuFadeIn ();
    WaitKeyUp ();

    do
    {
        which = HandleMenu (&FileItems, &FileMenu[0], NULL);
        //
        // HANDLE MENU CHOICES
        // 
        if (which == -1) 
        {
             US_ControlPanel(0);
             break;        
        }
    }
    while (which<0);
    
    if (which == 0) CP_NewGame(0);    
    if (which == 1) CP_LoadGame(0);   
    if (which == 4) CP_SaveGame(0); 
    if (which == 7) CP_Quit(0);     
    
    // left/right the menus and properly exit them so they wont be called in another menu
    if (which == 33) US_ControlPanel(666);
    if (which == 44) US_ControlPanel(667);
    //MenuFadeOut ();

    return 0;
}

////////////////////////
//
// DrawEditMenu
//
void DrawEditMenu (void)
{    
    //ClearMScreen ();
    DrawMainMenu();
    fontnumber = 0;
    
    DrawWindow (MAINMENU2 - 5, 20, EDITX, EDITY, 0);

    DrawMenu (&EditItems, &EditMenu[0]);

    DrawMenuGun (&EditItems);
    VW_UpdateScreen ();
} 

int CP_Edit (int)
{
    int which;

    DrawEditMenu ();
    WaitKeyUp ();

    do
    {
        which = HandleMenu (&EditItems, &EditMenu[0], NULL);
        
        if (which == -1) 
        {
             US_ControlPanel(0);
             break;        
        }
    }
    while (which < 0);
    
    // left/right the menus and properly exit them so they wont be called in another menu
    if (which == 33) US_ControlPanel(666);
    if (which == 44) US_ControlPanel(667);

    return 0;
}

////////////////////////
//
// DrawOptiMenu
//
/*CP_itemtype OptiMenu[] = {
    {0, "Audio", CP_Sound},
    {1, "Set Screen size...", 0},
    {2, STR_MOUSEEN, 0},
    {3, "Mouse Y-Axis Move", 0},
    {4, STR_SENS, MouseSensitivity},
    {5, STR_JOYEN, 0},
    {6, "Pause", 0},
    {7, STR_CUSTOM, CustomControls}
};*/

void DrawOptiMenu (void)
{    
    int i; 
     
    //ClearMScreen ();
    DrawMainMenu(); 
    fontnumber = 0;
    
    DrawWindow (MAINMENU3 - 5, 20, OPTIX, OPTIY, 0);
    
    if (IN_JoyPresent())
        OptiMenu[5].active = 1;

    if (MousePresent)
    {
        OptiMenu[2].active = 1;  // m enabled
        OptiMenu[3].active = 1;  // m y-axis  
        OptiMenu[4].active = 1;  // m sens
    }

    OptiMenu[4].active = mouseenabled; // mousesens

    DrawMenu (&OptiItems, &OptiMenu[0]);
        
    if (OptiItems.curpos < 0 || !OptiMenu[OptiItems.curpos].active)
    {
        for (i = 0; i < OptiItems.amount; i++)
        {
            if (OptiMenu[i].active)
            {
                OptiItems.curpos = i;
                break;
            }
        }
    }
     
    SETFONTCOLOR (BKGDCOLOR,0);
    PrintX = MAINMENU3;
    PrintY = 25 + 25;
    if (mouseenabled)
        US_Print640 (STR_CHECK);
    else 
        VWB_640Bar (PrintX-1, 50, 9, 11, WHITEC); 
    
    PrintX = MAINMENU3;  
    PrintY = 25 + 38;  
    if (mouseyaxis) 
        US_Print640 (STR_CHECK);
    else 
        VWB_640Bar (PrintX-1, 63, 9, 11, WHITEC); 
     
    PrintX = MAINMENU3; 
    PrintY = 25 + 64;       
    if (joystickenabled)
        US_Print640 (STR_CHECK);
    else 
        VWB_640Bar (PrintX-1, 89, 9, 11, WHITEC); 
        
    PrintX = MAINMENU3; 
    PrintY = 25 + 64+26;       
    if (damageflash)
        US_Print640 (STR_CHECK);
    else 
        VWB_640Bar (PrintX-1, 89+26, 9, 11, WHITEC);   
   
    PrintX = MAINMENU3; 
    PrintY = 25 + 64+39;       
    if (bossfix)
        US_Print640 (STR_CHECK);
    else 
        VWB_640Bar (PrintX-1, 89+39, 9, 11, WHITEC);   
        
    PrintX = MAINMENU3; 
    PrintY = 141;       
    if (!player_macstyle)
        US_Print640 (STR_CHECK);
    else 
        VWB_640Bar (PrintX-1, 141, 9, 11, WHITEC);    
        
    PrintX = MAINMENU3; 
    PrintY = 154;       
    if (macactorlimit)
        US_Print640 (STR_CHECK);
    else 
        VWB_640Bar (PrintX-1, 154, 9, 11, WHITEC);      
     
    PrintX = MAINMENU3; 
    PrintY = 167;       
    if (autorun)
        US_Print640 (STR_CHECK);
    else 
        VWB_640Bar (PrintX-1, 167, 9, 11, WHITEC);  

    DrawMenuGun (&OptiItems);
    VW_UpdateScreen ();
}  

int CP_Control (int)
{
    int which;
    int exit;
      
    DrawOptiMenu ();
               
    WaitKeyUp ();

    do
    {
        which = HandleMenu (&OptiItems, &OptiMenu[0], NULL);
        switch (which)
        {
            case -1:
                US_ControlPanel(0);
                which = 0;
                break;  
                   
            case 2: //CTL_MOUSEENABLE:
                mouseenabled ^= 1;
                if(IN_IsInputGrabbed())
                    IN_CenterMouse();
                
                //if (!fullscreen) // turn on mouse in windowed
                    MouseFlip(); 
                //if (mouseenabled)
                //    OptiItems.curpos =0;  
                //DrawOptiMenu ();
                OptiMenu[4].active = mouseenabled; // mousesens
                DrawMenu (&OptiItems, &OptiMenu[0]);
                
                PrintX = MAINMENU3;
                PrintY = 25 + 25;
                if (mouseenabled)
                   US_Print640 (STR_CHECK);
                else 
                   VWB_640Bar (PrintX-1, 50, 9, 11, WHITEC); 
                   
                ShootSnd ();
                break;
                
            case 3: //CTL_MOUSEYAXIS:
                mouseyaxis ^= 1;
                if(IN_IsInputGrabbed())
                    IN_CenterMouse();
                
                DrawMenu (&OptiItems, &OptiMenu[0]);
                PrintX = MAINMENU3;
                PrintY = 25 + 38;
                if (mouseyaxis)
                   US_Print640 (STR_CHECK);
                else 
                   VWB_640Bar (PrintX-1, 63, 9, 11, WHITEC); 
                   
                ShootSnd ();
                break;  

            case 5: //CTL_JOYENABLE:
                joystickenabled ^= 1;
                
                if (joystickenabled)
                   OptiItems.curpos = 0;
                
                //DrawOptiMenu ();
                PrintX = MAINMENU3;
                if (joystickenabled)
                {
                   PrintY = 25 + 64;
                   US_Print640 (STR_CHECK);
                }
                else 
                   VWB_640Bar (PrintX-1, 89, 9, 11, WHITEC); 
                   
                ShootSnd ();
                break;
                
            case 6: // pause
                if (ingame)
                 StartGame = 1;
                 
                if (changedres)    
                   changedres = false;
                 return -1;  
                 
            case 7: // damage flash
                damageflash ^= 1;
                
                DrawMenu (&OptiItems, &OptiMenu[0]);
                PrintX = MAINMENU3;
                PrintY = 25 + 90;
                if (damageflash)
                   US_Print640 (STR_CHECK);
                else 
                   VWB_640Bar (PrintX-1, 89+26, 9, 11, WHITEC); 
                   
                ShootSnd ();
                break;           

            case 8: // fix bosses (missing uber atk frame, mecha oneliner, boss death snds)
                bossfix ^= 1;
                
                DrawMenu (&OptiItems, &OptiMenu[0]);
                PrintX = MAINMENU3;
                PrintY = 25 + 103;
                if (bossfix)
                   US_Print640 (STR_CHECK);
                else 
                   VWB_640Bar (PrintX-1, 89+39, 9, 11, WHITEC); 
                   
                ShootSnd ();
                break; 
                
            case 9:    // player style (turning, movement, fov)
                player_macstyle ^= 1;
                
                DrawMenu (&OptiItems, &OptiMenu[0]);
                PrintX = MAINMENU3;
                PrintY = 25 + 116;
                if (!player_macstyle)
                   US_Print640 (STR_CHECK);
                else 
                   VWB_640Bar (PrintX-1, 141, 9, 11, WHITEC); 
                
                CalcProjection (0x5700l);   
                ShootSnd ();
                break;   
                
            case 10:    
                macactorlimit ^= 1;
                
                DrawMenu (&OptiItems, &OptiMenu[0]);
                PrintX = MAINMENU3;
                PrintY = 154;
                if (macactorlimit)
                   US_Print640 (STR_CHECK);
                else 
                   VWB_640Bar (PrintX-1, 154, 9, 11, WHITEC); 
 
                ShootSnd ();
                break; 
                
            case 11:    
                autorun ^= 1;
                
                DrawMenu (&OptiItems, &OptiMenu[0]);
                PrintX = MAINMENU3;
                PrintY = 167;
                if (autorun)
                   US_Print640 (STR_CHECK);
                else 
                   VWB_640Bar (PrintX-1, 167, 9, 11, WHITEC); 
 
                ShootSnd ();
                break;   
        }
    }
    while (which < 0 || which == 2 || which == 3 || which == 5 || (which >= 7 && which <= 11 ));
    
    // left/right the menus and properly exit them so they wont be called in another menu
    if (which == 33) US_ControlPanel(666);
    if (which == 44) US_ControlPanel(667);

    return 0;
}

#ifndef GOODTIMES
#ifndef SPEAR
////////////////////////////////////////////////////////////////////
//
// READ THIS!
//
////////////////////////////////////////////////////////////////////
int CP_ReadThis (int)
{
    StartCPMusic (TITLE_MUS);
    HelpScreens ();
    StartCPMusic (MENUSONG);
    return true;
}
#endif
#endif

////////////////////////////////////////////////////////////////////
//
// CHECK QUICK-KEYS & QUIT (WHILE IN A GAME)
//
////////////////////////////////////////////////////////////////////
int CP_CheckQuick (ScanCode scancode)
{
    switch (scancode)
    {
        //
        // END GAME
        //
        case sc_F7:
            CA_CacheGrChunk (STARTFONT + 1);

            WindowH = 160;
            
            if (Confirm (ENDGAMESTR))
            {
                playstate = ex_died;
                killerobj = NULL;
                pickquick = gamestate.lives = 0;
            }

            WindowH = 200;
            fontnumber = 0;
            //MainMenu[savegame].active = 0;
            return 1;

        //
        // QUICKSAVE
        //
        case sc_F8:
            if (SaveGamesAvail[LSItems.curpos] && pickquick)
            {
                CA_CacheGrChunk (STARTFONT + 1);
                fontnumber = 0;
                Message (STR_SAVING "...");
                CP_SaveGame (1);
                fontnumber = 0;
            }
            else
            {
                CA_CacheGrChunk (STARTFONT + 1);
                CA_CacheGrChunk (C_BABYMODEPIC);
                CA_CacheGrChunk (C_EASYPIC);
                CA_CacheGrChunk (C_NORMALPIC);
                CA_CacheGrChunk (C_MUSICTITLEPIC);
                CA_CacheGrChunk (C_MOUSELBACKPIC);
                
                tempgame = SDL_CreateRGBSurface(SDL_SWSURFACE, screenWidth, screenHeight, 8, 0, 0, 0, 0);    
                SDL_SetPalette(tempgame, SDL_LOGPAL, gamepal, 0, 256);   
                SDL_BlitSurface(screen, NULL, tempgame, NULL);  

                //VW_FadeOut ();
                VW_InPal (gamepal); 
                if(screenHeight % 200 != 0)
                    VL_ClearScreen(0);
                
                bkgdcolor = 0x10;    
                lastgamemusicoffset = StartCPMusic (MENUSONG);
                pickquick = CP_SaveGame (0);

                SETFONTCOLOR (16, 15);
                IN_ClearKeysDown ();
                //VW_FadeOut();
                VW_InPal (gamepal); 
                if(viewsize != 21)
                    DrawPlayScreen ();

                if (!startgame && !loadedgame)
                    ContinueMusic (lastgamemusicoffset);

                if (loadedgame)
                    playstate = ex_abort;
                lasttimecount = GetTimeCount ();

                if (MousePresent && IN_IsInputGrabbed())
                    IN_CenterMouse();     // Clear accumulated mouse movement
                       
                if (tempgame != NULL)
                   SDL_FreeSurface (tempgame);     

                UNCACHEGRCHUNK (C_BABYMODEPIC);
                UNCACHEGRCHUNK (C_EASYPIC);
                UNCACHEGRCHUNK (C_NORMALPIC);
                UNCACHEGRCHUNK (C_MUSICTITLEPIC);
                UNCACHEGRCHUNK (C_MOUSELBACKPIC);
            }
            return 1;

        //
        // QUICKLOAD
        //
        case sc_F9:
            if (SaveGamesAvail[LSItems.curpos] && pickquick)
            {
                char string[100] = STR_LGC;
                CA_CacheGrChunk (STARTFONT + 1);
                fontnumber = 0;

                strcat (string, SaveGameNames[LSItems.curpos]);
                strcat (string, "\"?");

                if (Confirm (string))
                    CP_LoadGame (1);

                fontnumber = 0;
            }
            else
            {
                CA_CacheGrChunk (STARTFONT + 1);
                CA_CacheGrChunk (C_BABYMODEPIC);
                CA_CacheGrChunk (C_EASYPIC);
                CA_CacheGrChunk (C_NORMALPIC);
                CA_CacheGrChunk (C_MUSICTITLEPIC);
                CA_CacheGrChunk (C_MOUSELBACKPIC);
                
                tempgame = SDL_CreateRGBSurface(SDL_SWSURFACE, screenWidth, screenHeight, 8, 0, 0, 0, 0);    
                SDL_SetPalette(tempgame, SDL_LOGPAL, gamepal, 0, 256);   
                SDL_BlitSurface(screen, NULL, tempgame, NULL);  

                //VW_FadeOut ();
                VW_InPal (gamepal); 
                if(screenHeight % 200 != 0)
                    VL_ClearScreen(0);
                    
                bkgdcolor = 0x10;     
                lastgamemusicoffset = StartCPMusic (MENUSONG);
                pickquick = CP_LoadGame (0);    // loads lastgamemusicoffs

                SETFONTCOLOR (16, 15);
                IN_ClearKeysDown ();
                //VW_FadeOut();
                VW_InPal (gamepal); 
                if(viewsize != 21)
                    DrawPlayScreen ();

                if (!startgame && !loadedgame)
                    ContinueMusic (lastgamemusicoffset);

                if (loadedgame)
                    playstate = ex_abort;

                lasttimecount = GetTimeCount ();

                if (MousePresent && IN_IsInputGrabbed())
                    IN_CenterMouse();     // Clear accumulated mouse movement
                    
                if (tempgame != NULL)
                   SDL_FreeSurface (tempgame);     

                UNCACHEGRCHUNK (C_BABYMODEPIC);
                UNCACHEGRCHUNK (C_EASYPIC);
                UNCACHEGRCHUNK (C_NORMALPIC);
                UNCACHEGRCHUNK (C_MUSICTITLEPIC);
                UNCACHEGRCHUNK (C_MOUSELBACKPIC);
            }
            return 1;

        //
        // QUIT
        //
        case sc_F10:
            CA_CacheGrChunk (STARTFONT + 1);

            WindowX = WindowY = 0;
            WindowW = 320;
            WindowH = 160;

            if (Confirm (endStrings[US_RndT () & 0x7 + (US_RndT () & 1)]))
            {
                VW_UpdateScreen ();
                SD_MusicOff ();
                SD_StopSound ();
                //MenuFadeOut ();

                Quit (NULL);
            }

            DrawPlayBorder ();
            WindowH = 200;
            fontnumber = 0;
            return 1;
    }

    return 0;
}


////////////////////////////////////////////////////////////////////
//
// END THE CURRENT GAME
//
////////////////////////////////////////////////////////////////////
int CP_EndGame (int)
{
    int res;

    res = Confirm (ENDGAMESTR);
    DrawMainMenu();
    if(!res) return 0;

    pickquick = gamestate.lives = 0;
    playstate = ex_died;
    killerobj = NULL;

    //MainMenu[savegame].active = 0;
    //MainMenu[viewscores].routine = CP_ViewScores;
    //strcpy (MainMenu[viewscores].string, STR_VS);

    return 1;
}


////////////////////////////////////////////////////////////////////
//
// VIEW THE HIGH SCORES
//
////////////////////////////////////////////////////////////////////
int CP_ViewScores (int)
{
#ifdef SPEAR
    UnCacheLump (OPTIONS_LUMP_START, OPTIONS_LUMP_END);
    StartCPMusic (XAWARD_MUS);
#else
//    StartCPMusic (ROSTER_MUS);   // APN: seamless menu music
#endif

    DrawHighScores (true);
    VW_UpdateScreen ();
    MenuFadeIn ();

    IN_Ack ();

//    StartCPMusic(MENUSONG);    // APN: seamless menu music
//    MenuFadeOut ();

#ifdef SPEAR
    CacheLump (BACKDROP_LUMP_START, BACKDROP_LUMP_END);
    CacheLump (OPTIONS_LUMP_START, OPTIONS_LUMP_END);
#endif
    return 0;
}


////////////////////////////////////////////////////////////////////
//
// START A NEW GAME
//
////////////////////////////////////////////////////////////////////
int CP_NewGame (int)
{
    int which, episode;

  firstpart:

    bkgdcolor = 0xff;                // replace default font/bar color cause different palette   
    DrawNewEpisode ();
    do
    {
        which = HandleMenu (&NewEitems, &NewEmenu[0], DrawEpisodePic);
        switch (which)
        {
            case -1:
                //MenuFadeOut ();
                bkgdcolor = 0x10;
                
                //if (backgr != NULL)
                //  SDL_FreeSurface (backgr); 
                if (epframe != NULL)
                  SDL_FreeSurface (epframe); 
                if (epmenu != NULL)
                  SDL_FreeSurface (epmenu); 
                if (eppic[0] != NULL)
                  SDL_FreeSurface (eppic[0]); 
                if (eppic[1] != NULL)
                  SDL_FreeSurface (eppic[1]); 
                if (eppic[2] != NULL)
                  SDL_FreeSurface (eppic[2]); 
                if (eppic[3] != NULL)
                  SDL_FreeSurface (eppic[3]); 
                if (eppic[4] != NULL)
                  SDL_FreeSurface (eppic[4]); 
                if (eppic[5] != NULL)
                  SDL_FreeSurface (eppic[5]); 
                if (eppic[6] != NULL)
                  SDL_FreeSurface (eppic[6]);  
                if (eppic[7] != NULL)
                  SDL_FreeSurface (eppic[7]);
                if (eppic[8] != NULL)
                  SDL_FreeSurface (eppic[8]);
                if (eppic[9] != NULL)
                  SDL_FreeSurface (eppic[9]);
                if (eppic[10] != NULL)
                  SDL_FreeSurface (eppic[10]); 
                if (eppic[11] != NULL)
                  SDL_FreeSurface (eppic[11]);
                    
                US_ControlPanel(0);  
                return 0;

            default:
                    episode = which;
                    which = 1;
                break;
        }
    }
    while (!which);

    ShootSnd ();

    //
    // ALREADY IN A GAME?
    //
    if (ingame)
        if (!Confirm (CURGAME))
        {
            //MenuFadeOut ();
            //if (backgr != NULL)
            //      SDL_FreeSurface (backgr); 
            
            // do a loop idiot, lul i need sleep
            if (epframe != NULL)
                  SDL_FreeSurface (epframe); 
            if (epmenu != NULL)
                  SDL_FreeSurface (epmenu); 
            if (eppic[0] != NULL)
                  SDL_FreeSurface (eppic[0]); 
            if (eppic[1] != NULL)
                  SDL_FreeSurface (eppic[1]); 
            if (eppic[2] != NULL)
                  SDL_FreeSurface (eppic[2]); 
            if (eppic[3] != NULL)
                  SDL_FreeSurface (eppic[3]); 
            if (eppic[4] != NULL)
                  SDL_FreeSurface (eppic[4]); 
            if (eppic[5] != NULL)
                  SDL_FreeSurface (eppic[5]); 
            if (eppic[6] != NULL)
                  SDL_FreeSurface (eppic[6]);  
            if (eppic[7] != NULL)
                  SDL_FreeSurface (eppic[7]);
            if (eppic[8] != NULL)
                  SDL_FreeSurface (eppic[8]);
            if (eppic[9] != NULL)
                  SDL_FreeSurface (eppic[9]);
            if (eppic[10] != NULL)
                  SDL_FreeSurface (eppic[10]); 
            if (eppic[11] != NULL)
                  SDL_FreeSurface (eppic[11]);     
                  
                  US_ControlPanel(0);
            return 0;
        }

   // MenuFadeOut ();

      //if (backgr != NULL)
      //  SDL_FreeSurface (backgr); 
      
      // why do i even
      if (epframe != NULL)
        SDL_FreeSurface (epframe); 
      if (epmenu != NULL)
        SDL_FreeSurface (epmenu); 
      if (eppic[0] != NULL)
        SDL_FreeSurface (eppic[0]); 
      if (eppic[1] != NULL)
        SDL_FreeSurface (eppic[1]); 
      if (eppic[2] != NULL)
        SDL_FreeSurface (eppic[2]); 
      if (eppic[3] != NULL)
        SDL_FreeSurface (eppic[3]); 
      if (eppic[4] != NULL)
        SDL_FreeSurface (eppic[4]); 
      if (eppic[5] != NULL)
        SDL_FreeSurface (eppic[5]); 
      if (eppic[6] != NULL)
        SDL_FreeSurface (eppic[6]);  
      if (eppic[7] != NULL)
        SDL_FreeSurface (eppic[7]);
      if (eppic[8] != NULL)
        SDL_FreeSurface (eppic[8]);
      if (eppic[9] != NULL)
        SDL_FreeSurface (eppic[9]);
      if (eppic[10] != NULL)
        SDL_FreeSurface (eppic[10]); 
      if (eppic[11] != NULL)
        SDL_FreeSurface (eppic[11]);
        
                     
    DrawNewGame ();
    which = HandleMenu (&NewItems, &NewMenu[0], DrawNewGameDiff);
    if (which < 0)
    {
      //  MenuFadeOut ();
        if (backgr != NULL)
          SDL_FreeSurface (backgr); 
        if (epframe != NULL)  
          SDL_FreeSurface (epframe); 

        goto firstpart;
    }

    ShootSnd ();
    NewGame (which, episode);
    StartGame = 1;
    
    if (changedres)    
        changedres = false;
    
    if (backgr != NULL)
       SDL_FreeSurface (backgr);   // diff background
    if (epframe != NULL)  
       SDL_FreeSurface (epframe);  // diff frame
//    if (tempgame != NULL)  
//       SDL_FreeSurface (tempgame); // empty screenshotmenu background    
    // MenuFadeOut ();

    //
    // CHANGE "READ THIS!" TO NORMAL COLOR
    //
#ifndef SPEAR
#ifndef GOODTIMES
//    MainMenu[readthis].active = 1;
#endif
#endif

    pickquick = 0;
    
    return 0;
}


/////////////////////
//
// DRAW NEW EPISODE MENU
//
void DrawEpisodePic (int w)
{
    VL_LatchToScreenScaledCoord(eppic[w], 0, 0, 96, 64, (screenWidth-scaleEpMenu*369)/2 +231*scaleEpMenu, (screenHeight-scaleEpMenu*331)/2 + 17*scaleEpMenu, 3);
}

void DrawNewEpisode (void)
{
    int i;

    //ClearMScreen ();;
    VWB_Bar (0, 0,  screenWidth / scaleFactor , screenHeight / scaleFactor , 255);
    
    LoadScreens(5);
    LoadScreens(6);
    VL_LatchToScreenScaledCoord(epmenu, 0, 0, 369, 331, (screenWidth-scaleEpMenu*369)/2, (screenHeight-scaleEpMenu*331)/2, 3);
    VL_LatchToScreenScaledCoord(epframe, 0, 0, 383, 345, (screenWidth-scaleEpFrame*383)/2, (screenHeight-scaleEpFrame*345)/2, 4);
    
    LoadScreens(9);
    LoadScreens(10);
    LoadScreens(11);
    LoadScreens(12);
    LoadScreens(13);
    LoadScreens(14);
    LoadScreens(15);
    LoadScreens(16);
    LoadScreens(17);
    LoadScreens(18);
    LoadScreens(19);
    LoadScreens(20);
    
    DrawEpisodePic (NewEitems.curpos);
    //free surfaces after menu
    
    //DrawWindow (NE_X - 4, NE_Y - 4, NE_W + 8, NE_H + 8, BKGDCOLOR); 
    strcpy (&NewEmenu[0].string[0], epname[1]);
    strcpy (&NewEmenu[1].string[0], epname[2]);
    strcpy (&NewEmenu[2].string[0], epname[3]);
    strcpy (&NewEmenu[3].string[0], epname[4]);
    strcpy (&NewEmenu[4].string[0], epname[5]);
    strcpy (&NewEmenu[5].string[0], epname[6]);
    strcpy (&NewEmenu[6].string[0], epname[7]);
    strcpy (&NewEmenu[7].string[0], epname[8]);
    strcpy (&NewEmenu[8].string[0], epname[9]);
    strcpy (&NewEmenu[9].string[0], epname[10]);
    strcpy (&NewEmenu[10].string[0], epname[11]);
    strcpy (&NewEmenu[11].string[0], epname[12]);
    
    //SETFONTCOLOR (255, BKGDCOLOR);
    DrawMenu (&NewEitems, &NewEmenu[0]);
    
        
    if (epactive[1] == 0) NewEmenu[0].active = 0;
    if (epactive[2] == 0) NewEmenu[1].active = 0;  
    if (epactive[3] == 0) NewEmenu[2].active = 0;  
    if (epactive[4] == 0) NewEmenu[3].active = 0;
    if (epactive[5] == 0) NewEmenu[4].active = 0;
    if (epactive[6] == 0) NewEmenu[5].active = 0;
    if (epactive[7] == 0) NewEmenu[6].active = 0;
    if (epactive[8] == 0) NewEmenu[7].active = 0;  
    if (epactive[9] == 0) NewEmenu[8].active = 0;  
    if (epactive[10] == 0) NewEmenu[9].active = 0;
    if (epactive[11] == 0) NewEmenu[10].active = 0;
    if (epactive[12] == 0) NewEmenu[11].active = 0;
    
    //for (i = 0; i < 6; i++)
    //    VWB_DrawPic (NE_X + 32, NE_Y + i * 26, C_EPISODE1PIC + i);

    VW_UpdateScreen ();
    VW_InPal(menupal);   
    WaitKeyUp ();
}

/////////////////////
//
// DRAW NEW GAME MENU
//
void DrawNewGame (void)
{
    //ClearMScreen ();;
    VWB_Bar (0, 0,  screenWidth / scaleFactor , screenHeight / scaleFactor , 255);
    
    LoadScreens(7);
    LoadScreens(8);
    
    // scale by epframe cause bigger
    VL_LatchToScreenScaledCoord(backgr, 0, 0, 369, 331, (screenWidth-scaleEpMenu*369)/2, (screenHeight-scaleEpMenu*331)/2, 3);
    VL_LatchToScreenScaledCoord(epframe, 0, 0, 383, 345, (screenWidth-scaleEpFrame*383)/2, (screenHeight-scaleEpFrame*345)/2, 4);
    
    //DrawWindow (NM_X - 5, NM_Y - 10, NM_W, NM_H, BKGDCOLOR);  
    //DrawMenu (&NewItems, &NewMenu[0]);
    
	//DrawNewGameDiff (NewItems.curpos);
    VW_UpdateScreen ();
    VW_InPal(menupal);   
    WaitKeyUp ();
}

////////////////////////
//
// DRAW NEW GAME GRAPHIC
//
void DrawNewGameDiff (int w)
{
    int color;
     
    VL_LatchToScreenScaledCoord(backgr, 0, 0, 369, 331, (screenWidth-scaleEpMenu*369)/2, (screenHeight-scaleEpMenu*331)/2, 3);
    
    if (thirdencounter)
        color = 35;
    else 
        color = 0xff;
    
    //VL_BarScaledCoord(scaleFactor*x, scaleFactor*y, scaleFactor*width, scaleFactor*height, color);
    if (w == 0)
    { 
      VWB_EpBar ((screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + 75, (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + 65, 51, 3, color);
      VWB_EpBar ((screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + 75, (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + 65, 3, 72, color);
      VWB_EpBar ((screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + 75, (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + 134, 51, 3, color);
      VWB_EpBar ((screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + 75 + 48, (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + 65, 3, 72, color);
    }
    else if (w == 1)
    {
      VWB_EpBar ((screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + 3*77-1, (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + 65, 51, 3, color);
      VWB_EpBar ((screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + 3*77-1, (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + 65, 3, 72, color);
      VWB_EpBar ((screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + 3*77-1, (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + 134, 51, 3, color);
      VWB_EpBar ((screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + 3*77-1 + 48, (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + 65, 3, 72, color);
    }
    else if (w == 2)
    {
      VWB_EpBar ((screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + 75, (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + 167, 51, 3, color);
      VWB_EpBar ((screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + 75, (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + 167, 3, 72, color);
      VWB_EpBar ((screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + 75, (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + 167 + 69, 51, 3, color);
      VWB_EpBar ((screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + 75 + 48, (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + 167, 3, 72, color);
    }
    else if (w == 3)
    {
      VWB_EpBar ((screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + 3*77-1, (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + 167, 51, 3, color);
      VWB_EpBar ((screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + 3*77-1, (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + 167, 3, 72, color);
      VWB_EpBar ((screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + 3*77-1, (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + 167 + 69, 51, 3, color);
      VWB_EpBar ((screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + 3*77-1 + 48, (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + 167, 3, 72, color);
    }

    //DrawOutline (75/scaleEpMenu, 65/scaleEpMenu, 51/scaleEpMenu, 72/scaleEpMenu, 216, 216);
}


////////////////////////////////////////////////////////////////////
//
// HANDLE SOUND MENU
//
////////////////////////////////////////////////////////////////////
void DrawSoundVols (void)
{ 
    PrintX = 300; 
    PrintY = 148;
    SETFONTCOLOR (BKGDCOLOR, TEXTCOLOR);
    //VWB_Bar (145, 61, 16, 5, WHITEC);
    VWB_640Bar (296, 147, 30, 11, WHITEC);
    US_PrintUns640 (soundvol);
    
    PrintX = 300; 
    PrintY = 176;
    SETFONTCOLOR (BKGDCOLOR, TEXTCOLOR);
    //VWB_Bar (145, 75, 16, 5, WHITEC);
    VWB_640Bar (296, 175, 30, 11, WHITEC);
    US_PrintUns640 (musicvol);

    VW_UpdateScreen ();
}


int AdjustVolume (int curmode)
{
    ControlInfo ci;
    int exit = 0, oldSV = soundvol, oldMV = musicvol;

    DrawSoundVols ();
    MenuFadeIn ();
    WaitKeyUp();
    do
    {
        SDL_Delay(5);
        ReadAnyControl (&ci);
        switch (ci.dir)
        {
            case dir_North:
            case dir_South:
                DrawSoundVols();
                //IN_ClearKeysDown();
                exit = 1;
                break;
                
            case dir_West:
                if (curmode)
                {
                    if (musicvol > 0)
                    {
                        musicvol--;
                        DrawSoundVols();
                        SD_ChangeVolume((byte) (soundvol * 1.28), (byte) (musicvol * 1.28));
                        TicDelay(2);
                    }
                }
                else
                {
                    if (soundvol > 0)
                    {
                        soundvol--;
                        DrawSoundVols();
                        SD_ChangeVolume((byte) (soundvol * 1.28), (byte) (musicvol * 1.28));
                        TicDelay(2);
                    }
                }
                break;
            case dir_East:
                if (curmode)
                {
                    if (musicvol < 100)
                    {
                        musicvol++;
                        DrawSoundVols();
                        SD_ChangeVolume((byte) (soundvol * 1.28), (byte) (musicvol * 1.28));
                        TicDelay(2);
                    }
                }
                else
                {
                    if (soundvol < 100)
                    {
                        soundvol++;
                        DrawSoundVols();
                        SD_ChangeVolume((byte) (soundvol * 1.28), (byte) (musicvol * 1.28));
                        TicDelay(2);
                    }
                }
                break;
        }

        if (ci.button0 || Keyboard[sc_Space] || Keyboard[sc_Enter])
            exit = 1;
            
        else if (ci.button1 && !Keyboard[sc_Escape] || Keyboard[sc_Escape])
            exit = 3;

    }
    while (!exit);

    if (exit == 2)
    {
        soundvol = oldSV;
        musicvol = oldMV;
        SD_PlaySound (ESCPRESSEDSND);
    }
    else
        SD_PlaySound (SHOOTSND);
    
        
    WaitKeyUp ();
    //US_ControlPanel(0);
    //MenuFadeOut ();

    return 0;
}

int CP_Sound (int)
{
    int which;
    DrawSoundMenu ();
    //MenuFadeIn ();
    WaitKeyUp ();

    do
    {
        which = HandleMenu (&SndItems, &SndMenu[0], NULL);

        //
        // HANDLE MENU CHOICES
        //
        switch (which)
        {
                //
                // SOUND EFFECTS
                //
            case -1:  
                 which = -1;
                //US_ControlPanel(0);  
                break; 
                
            case 0:
                if (SoundMode != sdm_Off)
                {
                    SD_WaitSoundDone ();
                    SD_SetSoundMode (sdm_Off);
                }
                else if (SoundMode != sdm_AdLib)
                {
                    SD_WaitSoundDone ();
                    SD_SetSoundMode (sdm_AdLib);
                    CA_LoadAllSounds ();     
                    ShootSnd ();
                }
                DrawSoundMenu ();
                break;

                //
                // MUSIC
                //
            case 2:
                if (MusicMode != smm_Off)
                {
                    SD_SetMusicMode (smm_Off);         
                }
                else if (MusicMode != smm_AdLib)
                {
                    SD_SetMusicMode (smm_AdLib);
                    StartCPMusic (MENUSONG);
                }
                DrawSoundMenu ();                    
                ShootSnd ();
                break;

            case 4:
                reversestereo ^= 1;
                SD_Reverse(reversestereo);
                
                
                DrawSoundMenu ();    
                ShootSnd();
                break;
        }        
    }
    while (which >= 0);
    
    //MenuFadeOut ();

    return 0;
}

void DrawSoundMenu (void)
{
    int i, on;

    //
    // DRAW SOUND MENU
    // 
    //ClearMScreen (); 
    DrawMainMenu();
    DrawWindow (SM_X - 18, SM_Y - 6, SM_W, SM_H-20, WHITEC);
    DrawFrame (SM_X - 18, SM_Y - 6, SM_W, SM_H-20, BKGDCOLOR, 14, 33, 23);
    DrawMenu (&SndItems, &SndMenu[0]);
    DrawSoundVols ();
    PrintX = SM_X;
    PrintY = SM_Y;
    US_Print640 ("Audio options:");

    for (i = 0; i < SndItems.amount; i++)
        if (SndMenu[i].string[0])
        {
            //
            // DRAW SELECTED/NOT SELECTED GRAPHIC BUTTONS
            //
            on = 0;
            switch (i)
            {
                    //
                    // SOUND EFFECTS
                    //
                case 0:
                    if (SoundMode == sdm_Off)
                        on = 0;
                    else if (SoundMode == sdm_AdLib)
                        on = 1;
                    break;

                    //
                    // MUSIC
                    //
                case 2:
                    if (MusicMode == smm_Off)
                        on = 0;
                    else if (MusicMode == smm_AdLib)
                        on = 1;    
                    break;

                case 4:
                    if (reversestereo)
                        on = 1;
                    break;
            }

            if (i == 0 || i == 2 || i == 4)
            {
                if (on)
                {
                    PrintX = SM_X - 12;
                    PrintY = SM_Y + 26 + i * 13;
                    US_Print640 (STR_CHECK);
                }
                else
                   // VWB_DrawPic (SM_X, SM_Y/2 + i * 5 + 2, C_NOTSELECTEDPIC);
                    VWB_640Bar (SM_X-13, SM_Y + 26 + i * 13, 11, 11, WHITEC);
            }
        }

    DrawMenuGun (&SndItems);
    VW_UpdateScreen ();
} 

//
// DRAW LOAD/SAVE IN PROGRESS
//
void DrawLSAction (int which)
{
#define LSA_X   96
#define LSA_Y   80
#define LSA_W   130
#define LSA_H   42

    /*DrawWindow (LSA_X, LSA_Y, LSA_W, LSA_H, TEXTCOLOR);
    DrawOutline (LSA_X, LSA_Y, LSA_W, LSA_H, 0, HIGHLIGHT);
    VWB_DrawPic (LSA_X + 8, LSA_Y + 5, C_DISKLOADING1PIC);

    fontnumber = 1;
    SETFONTCOLOR (0, TEXTCOLOR);
    PrintX = LSA_X + 46;
    PrintY = LSA_Y + 13;

    if (!which)
        US_Print (STR_LOADING "...");
    else
        US_Print (STR_SAVING "...");

    VW_UpdateScreen ();*/
}


////////////////////////////////////////////////////////////////////
//
// LOAD SAVED GAMES
//
////////////////////////////////////////////////////////////////////
int CP_LoadGame (int quick)
{
    FILE *file;
    int which, exit = 0;
    char name[13];
    char loadpath[300];

    strcpy (name, SaveName);

    //
    // QUICKLOAD?
    //
    if (quick)
    {
        which = LSItems.curpos;

        if (SaveGamesAvail[which])
        {
            name[7] = which + '0';

#ifdef _arch_dreamcast
            DC_LoadFromVMU(name);
#endif

            //if(configdir[0])
            if(filedir[0])
                snprintf(loadpath, sizeof(loadpath), "%s/%s", filedir, name);
            else
                strcpy(loadpath, name);

            file = fopen (loadpath, "rb");
            fseek (file, 32, SEEK_SET);
            loadedgame = true;
            LoadTheGame (file, 0, 0);
            loadedgame = false;
            fclose (file);

            //if (tempgame != NULL)  
            //  SDL_FreeSurface (tempgame);
              
            DrawFace ();
            DrawHealth ();
            DrawLives ();
            DrawLevel ();
            DrawAmmo ();
            DrawKeys ();
            DrawWeapon ();
            DrawScore ();
            DrawItems ();
            DrawItemsTotal ();
            DrawSecrets ();
            DrawSecretsTotal ();
            DrawKills ();
            DrawEnemies ();
            ContinueMusic (lastgamemusicoffset);
            return 1;
        }
    }

    DrawLoadSaveScreen (0);

    do
    {
        which = HandleMenu (&LSItems, &LSMenu[0], TrackWhichGame);
        if (which >= 0 && SaveGamesAvail[which])
        {
            ShootSnd ();
            name[7] = which + '0';

#ifdef _arch_dreamcast
            DC_LoadFromVMU(name);
#endif

            //if(configdir[0])
            if(filedir[0])
                snprintf(loadpath, sizeof(loadpath), "%s/%s", filedir, name);
            else
                strcpy(loadpath, name);

            file = fopen (loadpath, "rb");
            fseek (file, 32, SEEK_SET);

            DrawLSAction (0);
            loadedgame = true;

            LoadTheGame (file, LSA_X + 8, LSA_Y + 5);
            fclose (file);
            
            //if (tempgame != NULL)  
            //   SDL_FreeSurface (tempgame);

            StartGame = 1;
            ShootSnd ();
            //
            // CHANGE "READ THIS!" TO NORMAL COLOR
            //

#ifndef SPEAR
#ifndef GOODTIMES
//            MainMenu[readthis].active = 1;
#endif
#endif

            exit = 1;
            break;
        }

    }
    while (which >= 0);

    //MenuFadeOut ();
    //US_ControlPanel(0);   
    return exit;
}


///////////////////////////////////
//
// HIGHLIGHT CURRENT SELECTED ENTRY
//
void TrackWhichGame (int w)
{
    static int lastgameon = 0;

    PrintLSEntry (lastgameon, TEXTCOLOR);
    PrintLSEntry (w, HIGHLIGHT);

    lastgameon = w;
}


////////////////////////////
//
// DRAW THE LOAD/SAVE SCREEN
//
void DrawLoadSaveScreen (int loadsave)
{
    int i;

    //ClearMScreen ();
    DrawMainMenu ();
    fontnumber = 0;

    if (!loadsave)
       VWB_DrawPicScaledRes (scale640*LSM_X-scale640*40, scale640*LSM_Y-scale640*60, C_MUSICTITLEPIC, 3);
    else
       VWB_DrawPicScaledRes (scale640*LSM_X-scale640*40, scale640*LSM_Y-scale640*60, C_MOUSELBACKPIC, 3); 

    //DrawWindow (LSM_X - 10, LSM_Y - 5, LSM_W, 140, WHITEC);
    VWB_640Bar (LSM_X - 10, LSM_Y - 5, LSM_W, 140, WHITEC);

    for (i = 0; i < 10; i++)
        PrintLSEntry (i, TEXTCOLOR);

    DrawMenu (&LSItems, &LSMenu[0]);
    VW_UpdateScreen ();
    MenuFadeIn ();
    WaitKeyUp ();
}


///////////////////////////////////////////
//
// PRINT LOAD/SAVE GAME ENTRY W/BOX OUTLINE
//
void PrintLSEntry (int w, int color)
{
    SETFONTCOLOR (color, BKGDCOLOR);
    //VWB_Bar (LSM_X/scaleFactor, LSM_Y/scaleFactor + w * 13, LSM_W /scaleFactor, 5, WHITEC);
    //DrawOutline (LSM_X + LSItems.indent, LSM_Y + w * 13, LSM_W - LSItems.indent - 15, 11, color, color);
    PrintX = LSM_X + LSItems.indent + 2;
    PrintY = LSM_Y + w * 13 + 1;
    fontnumber = 0;  

    if (SaveGamesAvail[w])
        US_Print640 (SaveGameNames[w]);
    else
        US_Print640 (STR_EMPTY);

    fontnumber = 0;
}


////////////////////////////////////////////////////////////////////
//
// SAVE CURRENT GAME
//
////////////////////////////////////////////////////////////////////
int CP_SaveGame (int quick)
{
    int which, exit = 0;
    FILE *file;
    char name[13];
    char savepath[300];
    char input[32];

    strcpy (name, SaveName);

    //
    // QUICKSAVE?
    //
    if (quick)
    {
        which = LSItems.curpos;

        if (SaveGamesAvail[which])
        {
            name[7] = which + '0';

            //if(configdir[0])
            if(filedir[0])
                snprintf(savepath, sizeof(savepath), "%s/%s", filedir, name);
            else
                strcpy(savepath, name);

            unlink (savepath);
            file = fopen (savepath, "wb");

            strcpy (input, &SaveGameNames[which][0]);

            fwrite (input, 1, 32, file);
            fseek (file, 32, SEEK_SET);
            SaveTheGame (file, 0, 0);
            fclose (file);

#ifdef _arch_dreamcast
            DC_SaveToVMU(name, input);
#endif

            return 1;
        }
    }


#ifdef SPEAR
    UnCacheLump (OPTIONS_LUMP_START, OPTIONS_LUMP_END);
    CacheLump (LOADSAVE_LUMP_START, LOADSAVE_LUMP_END);
#endif

    DrawLoadSaveScreen (1);

    do
    {
        which = HandleMenu (&LSItems, &LSMenu[0], TrackWhichGame);
        if (which >= 0)
        {
            //
            // OVERWRITE EXISTING SAVEGAME?
            //
            if (SaveGamesAvail[which])
            {
                if (!Confirm (GAMESVD))
                {
                    DrawLoadSaveScreen (1);
                    continue;
                }
                else
                {
                    DrawLoadSaveScreen (1);
                    VWB_640Bar (LSM_X + LSItems.indent - 2, LSM_Y + which * 13 - 2, LSM_W - LSItems.indent - 16, SUB_BAR_H, BKGDCOLOR);
                    PrintLSEntry (which, HIGHLIGHT);
                    VW_UpdateScreen ();
                }
            }

            ShootSnd ();

            strcpy (input, &SaveGameNames[which][0]);
            name[7] = which + '0';

            fontnumber = 0;
            if (!SaveGamesAvail[which])
                VWB_640Bar (LSM_X + LSItems.indent - 2, LSM_Y + which * 13 - 2, LSM_W - LSItems.indent - 16, SUB_BAR_H, BKGDCOLOR);
            VW_UpdateScreen ();

            if (US_LineInput
                (LSM_X + LSItems.indent + 2, LSM_Y + which * 13 + 1, input, input, true, 31, LSM_W - LSItems.indent - 30))
            {
                SaveGamesAvail[which] = 1;
                strcpy (&SaveGameNames[which][0], input);

                //if(configdir[0])
                if(filedir[0])
                    snprintf(savepath, sizeof(savepath), "%s/%s", filedir, name);
                else
                    strcpy(savepath, name);

                unlink (savepath);
                file = fopen (savepath, "wb");
                fwrite (input, 32, 1, file);
                fseek (file, 32, SEEK_SET);

                DrawLSAction (1);
                SaveTheGame (file, LSA_X + 8, LSA_Y + 5);

                fclose (file);

#ifdef _arch_dreamcast
                DC_SaveToVMU(name, input);
#endif

                ShootSnd ();
                exit = 1;
            }
            else
            {
               // VWB_640Bar (LSM_X + LSItems.indent + 1, LSM_Y + which * 13 + 1, LSM_W - LSItems.indent - 16, 5, 88);
                PrintLSEntry (which, HIGHLIGHT);
                VW_UpdateScreen ();
                SD_PlaySound (ESCPRESSEDSND);
                continue;
            }

            fontnumber = 1;
            break;
        }

    }
    while (which >= 0);

    //MenuFadeOut ();

#ifdef SPEAR
    UnCacheLump (LOADSAVE_LUMP_START, LOADSAVE_LUMP_END);
    CacheLump (OPTIONS_LUMP_START, OPTIONS_LUMP_END);
#endif

    return exit;
}

////////////////////////////////
//
// DRAW MOUSE SENSITIVITY SCREEN
//
void DrawMouseSens (void)
{
    DrawMainMenu();
    DrawWindow (SM_X - 8, SM_Y - 3, SM_W, SM_H/2, WHITEC);
    
    PrintX = SM_X;
    PrintY = SM_Y;
    US_Print640 ("Mouse Sensitivity");
    PrintX = SM_X;
    PrintY = SM_Y+20;
    US_Print640 ("Slow");
    PrintX = SM_X+80;
    PrintY = SM_Y+20;
    VWB_640Bar (SM_X+75, SM_Y+19, 24, 11, WHITEC);
    US_PrintUns640 (mouseadjustment);
    PrintX = SM_X+160;
    PrintY = SM_Y+20;
    US_Print640 ("Fast");
    
    //DrawMenu (&SndItems, &SndMenu[0]);
    
    VWB_640Bar (SM_X, 150, 185, 10, WHITEC);
    DrawOutline (SM_X, 150, 185, 10, BKGDCOLOR, BKGDCOLOR);
    VWB_640Bar (SM_X+1 + 5 * mouseadjustment, 151, 9, 9, BKGDCOLOR);

    VW_UpdateScreen ();
}

void DrawMouseSlider (void)
{    
    PrintX = SM_X;
    PrintY = SM_Y;
    US_Print640 ("Mouse Sensitivity");
    PrintX = SM_X;
    PrintY = SM_Y+20;
    US_Print640 ("Slow");
    PrintX = SM_X+80;
    PrintY = SM_Y+20;
    VWB_640Bar (SM_X+75, SM_Y+19, 24, 11, WHITEC);
    US_PrintUns640 (mouseadjustment);
    PrintX = SM_X+160;
    PrintY = SM_Y+20;
    US_Print640 ("Fast");
    
    VWB_640Bar (SM_X, 150, 185, 10, WHITEC);
    DrawOutline (SM_X, 150, 185, 10, BKGDCOLOR, BKGDCOLOR);
    VWB_640Bar (SM_X+1 + 5 * mouseadjustment, 151, 9, 9, BKGDCOLOR);

   VW_UpdateScreen ();
}

///////////////////////////
//
// ADJUST MOUSE SENSITIVITY
//
int MouseSensitivity (int)
{
    ControlInfo ci;
    int exit = 0, oldMA;

    controlmousex = 0, controlmousey = 0;
    
    oldMA = mouseadjustment;
    DrawMouseSens ();
    do
    {
        SDL_Delay(5);
        ReadAnyControl (&ci);
        switch (ci.dir)
        {
            case dir_West:
                if (mouseadjustment)
                {
                    mouseadjustment--;
                    DrawMouseSlider ();
                    SD_PlaySound (MOVEGUN1SND);
                    TicDelay(6);
                }
                break;

            case dir_East:
                if (mouseadjustment < 35)
                {
                    mouseadjustment++;
                    DrawMouseSlider ();
                    SD_PlaySound (MOVEGUN1SND);
                    TicDelay(6);
                }
                break;
        }

        if (ci.button0 || Keyboard[sc_Space] || Keyboard[sc_Enter] || ci.button1 || Keyboard[sc_Escape])
            exit = 1;
        //else if (ci.button1 || Keyboard[sc_Escape])
        //    exit = 2;

    }
    while (!exit);

    if (exit == 2)
    {
        mouseadjustment = oldMA;
        SD_PlaySound (ESCPRESSEDSND);
    }
    //else
    //    SD_PlaySound (SHOOTSND);

    WaitKeyUp ();
    US_ControlPanel(0);
    //MenuFadeOut ();

    return 0;
}



////////////////////////////////////////////////////////////////////
//
// CUSTOMIZE CONTROLS
//
////////////////////////////////////////////////////////////////////
enum { FIRE, STRAFE, RUN, OPEN, STRFL, STRFR, NWEP, AMAP};
char mbarray[4][3] = { "b0", "b1", "b2", "b3" };
int8_t order[4] = { RUN, OPEN, FIRE, STRAFE };
int8_t order2[4] = { NWEP, STRFL, STRFR, AMAP };

int CustomControls (int)
{
    int which;
    
    DrawCustomScreen ();
    do
    {
        which = HandleMenu (&CusItems, &CusMenu[0], NULL);

        switch (which)
        {
            case -1:
                //which = -1;
                US_ControlPanel(0);  
                break;  
        
            case 0:
                DefineMouseBtns ();
                DrawCustMouse (1);
                break;
            case 4:
                DefineJoyBtns ();
                DrawCustJoy (0);
                break;
            case 8:
                DefineKeyBtns ();
                DrawCustKeybd (0);
                break;
            case 11:
                DefineKeyMove ();
                DrawCustKeys (0);
                break;
            case 14:
                DefineKeyBtns2 ();
                DrawCustKeybd2 (0);
                break;       
        } 
    }
    while (which < 0);  

    //MenuFadeOut ();

    return 0;
}


////////////////////////
//
// DEFINE THE MOUSE BUTTONS
//
void DefineMouseBtns (void)
{
    CustomCtrls mouseallowed = { 1, 1, 1, 1 };
    EnterCtrlData (2, &mouseallowed, DrawCustMouse, PrintCustMouse, MOUSE);
}


////////////////////////
//
// DEFINE THE JOYSTICK BUTTONS
//
void DefineJoyBtns (void)
{
    CustomCtrls joyallowed = { 1, 1, 1, 1 };
    EnterCtrlData (6, &joyallowed, DrawCustJoy, PrintCustJoy, JOYSTICK);
}

////////////////////////
//
// DEFINE THE KEYBOARD BUTTONS
//
void DefineKeyBtns (void)
{
    CustomCtrls keyallowed = { 1, 1, 1, 1 };
    EnterCtrlData (10, &keyallowed, DrawCustKeybd, PrintCustKeybd, KEYBOARDBTNS);
}

////////////////////////
//
// DEFINE THE KEYBOARD BUTTONS
//
void DefineKeyMove (void)
{
    CustomCtrls keyallowed = { 1, 1, 1, 1 };
    EnterCtrlData (13, &keyallowed, DrawCustKeys, PrintCustKeys, KEYBOARDMOVE);
}

////////////////////////
//
// DEFINE THE KEYBOARD BUTTONS 2
//
void DefineKeyBtns2 (void)
{
    CustomCtrls keyallowed = { 1, 1, 1, 1 };
    EnterCtrlData (16, &keyallowed, DrawCustKeybd2, PrintCustKeybd2, KEYBOARDBTNS2);
}

////////////////////////
//
// ENTER CONTROL DATA FOR ANY TYPE OF CONTROL
//
enum { FWRD, RIGHT, BKWD, LEFT };
int moveorder[4] = { LEFT, RIGHT, FWRD, BKWD };
int exitt;

void EnterCtrlData (int index, CustomCtrls * cust, void (*DrawRtn) (int), void (*PrintRtn) (int), int type)
{
    int j, tick, redraw, which, vertwhich, x, picked, lastFlashTime;
    
    ControlInfo ci;

    controlmousex = 0, controlmousey = 0;
    
    ShootSnd ();
    PrintY = CST_Y + 13 * index;
    IN_ClearKeysDown ();
    
    vertwhich = index;
    
    //exitt = 0;
    redraw = 1;
    //
    // FIND FIRST SPOT IN ALLOWED ARRAY
    //
    if (exitt == 13) exitt = 0;
    
    if (cust->allowed[exitt])
       which = exitt;          // get the last exit spot when moving up/down in menu so it jumps to same spot in next line
    else
    {
    
       for (j = 0; j < 4; j++)
        if (cust->allowed[j])
        {
            which = j;
            break;
        }
    }
    
    exitt = 0;

    do
    {
        if (redraw)
        {                    
            x = CST_START + CST_SPC * which;
            //DrawWindow (CST_X, PrintY - 1, 50, 13, BKGDCOLOR);

            DrawRtn (1);
            DrawWindow (x - 2, PrintY-1, CST_SPC-15, 11, TEXTCOLOR);
            DrawOutline (x - 2, PrintY-1, CST_SPC-15, 11, 0, HIGHLIGHT);
            SETFONTCOLOR (0, TEXTCOLOR);
            PrintRtn (which);
            PrintX = x;
            SETFONTCOLOR (TEXTCOLOR, BKGDCOLOR);
            VW_UpdateScreen ();
            WaitKeyUp ();
            redraw = 0;
        }

        SDL_Delay(5);
        ReadAnyControl (&ci);

        if (type == MOUSE || type == JOYSTICK)
            if (IN_KeyDown (sc_Enter) || IN_KeyDown (sc_Control) || IN_KeyDown (sc_Alt))
            {
                IN_ClearKeysDown ();
                ci.button0 = ci.button1 = false;
            }

        //
        // CHANGE BUTTON VALUE?
        //
        if ((type != KEYBOARDBTNS && type != KEYBOARDBTNS2 && type != KEYBOARDMOVE) && (ci.button0 | ci.button1 | ci.button2 | ci.button3) ||
            ((type == KEYBOARDBTNS || type == KEYBOARDBTNS2 || type == KEYBOARDMOVE) && LastScan == sc_Enter))
        {
            lastFlashTime = GetTimeCount();
            tick = picked = 0;
            SETFONTCOLOR (0, TEXTCOLOR);

            if (type == KEYBOARDBTNS || type == KEYBOARDBTNS2 || type == KEYBOARDMOVE)
                IN_ClearKeysDown ();

            while(1)
            {
                int button, result = 0;

                //
                // FLASH CURSOR
                //
                if (GetTimeCount() - lastFlashTime > 10)
                {
                    switch (tick)
                    {
                        case 0:
                            VWB_640Bar (x, PrintY, CST_SPC - 15 - 2, 10, BKGDCOLOR);
                            break;
                        case 1:
                            PrintX = x;
                            US_Print640 ("?");
                            SD_PlaySound (HITWALLSND);
                    }
                    tick ^= 1;
                    lastFlashTime = GetTimeCount();
                    VW_UpdateScreen ();
                }
                else SDL_Delay(5);

                //
                // WHICH TYPE OF INPUT DO WE PROCESS?
                //
                switch (type)
                {
                    case MOUSE:
                        button = IN_MouseButtons();
                        switch (button)
                        {
                            case 1:
                                result = 1;
                                break;
                            case 2:
                                result = 2;
                                break;
                            case 4:
                                result = 3;
                                break;
                        }

                        if (result)
                        {
                            for (int z = 0; z < 4; z++)
                                if (order[which] == buttonmouse[z])
                                {
                                    buttonmouse[z] = bt_nobutton;
                                    break;
                                }

                            buttonmouse[result - 1] = order[which];
                            picked = 1;
                            SD_PlaySound (SHOOTDOORSND); 
                            
                            //clear all fields again cause different input overwrite
                            DrawWindow ((CST_START + CST_SPC * 0) - 2, PrintY-1, CST_SPC-15, 11, WHITEC);
                            DrawWindow ((CST_START + CST_SPC * 1) - 2, PrintY-1, CST_SPC-15, 11, WHITEC);
                            DrawWindow ((CST_START + CST_SPC * 2) - 2, PrintY-1, CST_SPC-15, 11, WHITEC);
                            DrawWindow ((CST_START + CST_SPC * 3) - 2, PrintY-1, CST_SPC-15, 11, WHITEC);
                            DrawCustMouse (0); 
                        }
                        break;

                    case JOYSTICK:
                        if (ci.button0)
                            result = 1;
                        else if (ci.button1)
                            result = 2;
                        else if (ci.button2)
                            result = 3;
                        else if (ci.button3)
                            result = 4;

                        if (result)
                        {
                            for (int z = 0; z < 4; z++)
                            {
                                if (order[which] == buttonjoy[z])
                                {
                                    buttonjoy[z] = bt_nobutton;
                                    break;
                                }
                            }

                            buttonjoy[result - 1] = order[which];
                            picked = 1;
                            SD_PlaySound (SHOOTDOORSND);
                            
                            //clear all fields again cause different input overwrite
                            DrawWindow ((CST_START + CST_SPC * 0) - 2, PrintY-1, CST_SPC-15, 11, WHITEC);
                            DrawWindow ((CST_START + CST_SPC * 1) - 2, PrintY-1, CST_SPC-15, 11, WHITEC);
                            DrawWindow ((CST_START + CST_SPC * 2) - 2, PrintY-1, CST_SPC-15, 11, WHITEC);
                            DrawWindow ((CST_START + CST_SPC * 3) - 2, PrintY-1, CST_SPC-15, 11, WHITEC);
                            DrawCustJoy (0); 
                        }
                        break;

                    case KEYBOARDBTNS:
                        if (LastScan && LastScan != sc_Escape)
                        {
                            buttonscan[order[which]] = LastScan;
                            picked = 1;
                            ShootSnd ();
                            IN_ClearKeysDown ();
                        }
                        break;

                    case KEYBOARDMOVE:
                        if (LastScan && LastScan != sc_Escape)
                        {
                            dirscan[moveorder[which]] = LastScan;
                            picked = 1;
                            ShootSnd ();
                            IN_ClearKeysDown ();
                        }
                        break;
                        
                    case KEYBOARDBTNS2:
                        if (LastScan && LastScan != sc_Escape)
                        {
                            buttonscan[order2[which]] = LastScan;
                            picked = 1;
                            ShootSnd ();
                            IN_ClearKeysDown ();
                        }
                        break;    
                }

                //
                // EXIT INPUT?
                //
                if (IN_KeyDown (sc_Escape) || type != JOYSTICK && ci.button1)
                {
                    picked = 1;
                    SD_PlaySound (ESCPRESSEDSND);
                }

                if(picked) break;

                ReadAnyControl (&ci);
            }

            SETFONTCOLOR (TEXTCOLOR, BKGDCOLOR);
            redraw = 1;
            WaitKeyUp ();
            continue;
        }

        if (ci.button1 && !IN_KeyDown(sc_Alt) || IN_KeyDown (sc_Escape))
            exitt = 14;       

        //
        // MOVE TO ANOTHER SPOT?
        //
        switch (ci.dir)
        {   
            case dir_West:
                do
                {
                    which--;
                    if (which < 0)
                        which = 3;
                }
                while (!cust->allowed[which]);
                
                DrawWindow (x - 2, PrintY-1, CST_SPC-15, 11, WHITEC);
                DrawOutline (x - 2, PrintY-1, CST_SPC-15, 11, 0, WHITEC);
                redraw = 1;
                SD_PlaySound (MOVEGUN1SND);
                while (ReadAnyControl (&ci), ci.dir != dir_None) SDL_Delay(5);
                IN_ClearKeysDown ();
                break;

            case dir_East:
                do
                {
                    which++;
                    if (which > 3)
                        which = 0;
                }
                while (!cust->allowed[which]);
                DrawWindow (x - 2, PrintY-1, CST_SPC-15, 11, WHITEC);
                DrawOutline (x - 2, PrintY-1, CST_SPC-15, 11, 0, WHITEC);
                redraw = 1;
                SD_PlaySound (MOVEGUN1SND);
                while (ReadAnyControl (&ci), ci.dir != dir_None) SDL_Delay(5);
                IN_ClearKeysDown ();
                break;
            case dir_North:
                { 
                   DrawWindow (x - 2, PrintY-1, CST_SPC-15, 11, WHITEC);
                   DrawOutline (x - 2, PrintY-1, CST_SPC-15, 11, 0, WHITEC);
                   
                   //if (CusMenu[index].active)
                   if (vertwhich == 2) { vertwhich = 16; DrawCustMouse (0); }
                   else if (vertwhich == 6) { vertwhich = 2; DrawCustJoy (0); }
                   else if (vertwhich == 10) { vertwhich = 6; DrawCustKeybd (0); }
                   else if (vertwhich == 13) { vertwhich = 10; DrawCustKeys (0); }
                   else if (vertwhich == 16) { vertwhich = 13; DrawCustKeybd2 (0); }
                           
                   if (which == 0)   
                     exitt = 13;
                   else               
                     exitt = which; //1;
                }
                break;
            case dir_South:
                { 
                   DrawWindow (x - 2, PrintY-1, CST_SPC-15, 11, WHITEC);
                   DrawOutline (x - 2, PrintY-1, CST_SPC-15, 11, 0, WHITEC);   

                   if (vertwhich == 2) { vertwhich = 6; DrawCustMouse (0); }
                   else if (vertwhich == 6) { vertwhich = 10; DrawCustJoy (0); }
                   else if (vertwhich == 10) { vertwhich = 13; DrawCustKeybd (0); }
                   else if (vertwhich == 13) { vertwhich = 16; DrawCustKeys (0); }
                   else if (vertwhich == 16) { vertwhich = 2; DrawCustKeybd2 (0); }
                        
                   if (which == 0)   
                     exitt = 13;
                   else               
                     exitt = which; //1;
                }
                break;
        }
    }
    while (!exitt);

    SD_PlaySound (ESCPRESSEDSND);
    WaitKeyUp ();
    
    // go up/down at same spot to reach next one below or above
    if (exitt < 14)
    {        
            if (vertwhich == 2)
            {
                DefineMouseBtns ();
                DrawCustMouse (0); 
            }
            else if (vertwhich == 6)
            {
                DefineJoyBtns  ();
                DrawCustJoy (0); 
            }
            else if (vertwhich == 10)
            {
                DefineKeyBtns ();
                DrawCustKeybd (0); 
            }
            else if (vertwhich == 13)   
            {    
                DefineKeyMove ();
                DrawCustKeys (0);  
            }  
            else if (vertwhich == 16)
            {
                DefineKeyBtns2 ();
                DrawCustKeybd2 (0); 
            }
            
    }
    
    else if (exitt == 14) 
    {
       exitt = 0;     
       US_ControlPanel (0);  
    }
    
    //DrawWindow (CST_X, PrintY - 1, 310, 13, BKGDCOLOR);
}

////////////////////////
//
// DRAW CUSTOMIZE SCREEN
//
void DrawCustomScreen (void)
{
    int i;

    //ClearMScreen ();
    DrawMainMenu();  
    DrawWindow (CST_X - 20, CST_Y - 50, CST_W, CST_H, WHITEC); 
    DrawFrame (CST_X - 20, CST_Y - 50, CST_W, CST_H, BKGDCOLOR, 14, 33, 23);

    PrintX = CST_START;
    PrintY = CST_Y-40;
    US_Print640 ("Some keys are not available, ESC is used for\n" 
                 "pause, 1-6 are used for weapon selection.");
    VWB_640Bar (CST_X - 19, PrintY+20, CST_W-1, 1, DEACTIVE); 
    
    //
    // MOUSE
    //
    SETFONTCOLOR (READCOLOR, BKGDCOLOR);

    PrintX = CST_START;
    PrintY = CST_Y+0; //-6
    US_Print640 ("Mouse:\n");
    PrintY -= 0; //+6

    SETFONTCOLOR (TEXTCOLOR, BKGDCOLOR);
    PrintY += 1;
    PrintX = CST_START;
    US_Print640 (STR_CRUN);
    PrintX = CST_START + CST_SPC * 1;
    US_Print640 (STR_COPEN); 
    PrintX = CST_START + CST_SPC * 2;
    US_Print640 (STR_CFIRE);
    PrintX = CST_START + CST_SPC * 3;
    US_Print640 (STR_CSTRAFE "\n");
    PrintY -= 1;

    //DrawWindow (CST_X, PrintY - 1, 310, 13, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 0, PrintY + 3, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 1, PrintY + 3, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 2, PrintY + 3, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 3, PrintY + 3, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawCustMouse (0);
    US_Print640 ("\n\n");

    //
    // JOYSTICK/PAD
    //
    VWB_640Bar (PrintX-25, PrintY, CST_W-1, 1, DEACTIVE); 
    SETFONTCOLOR (READCOLOR, BKGDCOLOR);
    PrintX = CST_START;
    PrintY += 6;
    US_Print640 ("Joystick/GamePad:\n");
    PrintY -= 6;
    SETFONTCOLOR (TEXTCOLOR, BKGDCOLOR);
    PrintY += 7;
    PrintX = CST_START;
    US_Print640 (STR_CRUN);
    PrintX = CST_START + CST_SPC * 1;
    US_Print640 (STR_COPEN);
    PrintX = CST_START + CST_SPC * 2;
    US_Print640 (STR_CFIRE);
    PrintX = CST_START + CST_SPC * 3;
    US_Print640 (STR_CSTRAFE "\n");
    PrintY -= 7;
    
    //DrawWindow (CST_X, PrintY - 1, 310, 13, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 0, PrintY + 9, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 1, PrintY + 9, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 2, PrintY + 9, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 3, PrintY + 9, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawCustJoy (0);
    US_Print640 ("\n\n");

    //
    // KEYBOARD
    //
    VWB_640Bar (PrintX-25, PrintY, CST_W-1, 1, DEACTIVE); 
    SETFONTCOLOR (READCOLOR, BKGDCOLOR);
    PrintX = CST_START;
    PrintY += 6;
    US_Print640 ("Keyboard:\n");
    PrintY -= 6;
    SETFONTCOLOR (TEXTCOLOR, BKGDCOLOR);

    PrintY += 7;
    PrintX = CST_START;
    US_Print640 (STR_CRUN);
    PrintX = CST_START + CST_SPC * 1;
    US_Print640 (STR_COPEN);
    PrintX = CST_START + CST_SPC * 2;
    US_Print640 (STR_CFIRE);
    PrintX = CST_START + CST_SPC * 3;
    US_Print640 (STR_CSTRAFE "\n");
    PrintY -= 7;
    //DrawWindow (CST_X, PrintY - 1, 310, 13, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 0, PrintY + 9, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 1, PrintY + 9, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 2, PrintY + 9, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 3, PrintY + 9, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR); 
    
    DrawCustKeybd (0);
    US_Print640 ("\n\n");


    //
    // KEYBOARD MOVE KEYS
    //
    SETFONTCOLOR (TEXTCOLOR, BKGDCOLOR);
    
    PrintY += 4;
    PrintX = CST_START;
    US_Print640 (STR_LEFT);
    PrintX = CST_START + CST_SPC * 1;
    US_Print640 (STR_RIGHT);
    PrintX = CST_START + CST_SPC * 2;
    US_Print640 (STR_FRWD);
    PrintX = CST_START + CST_SPC * 3;
    US_Print640 (STR_BKWD "\n");
    PrintY -= 4;
    //DrawWindow (CST_X, PrintY - 1, 310, 13, BKGDCOLOR);
    
    DrawOutline (CST_X - 4 + CST_SPC * 0, PrintY + 6, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 1, PrintY + 6, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 2, PrintY + 6, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 3, PrintY + 6, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawCustKeys (0);
    US_Print640 ("\n\n");

    //
    // KEYBOARD 2
    //
    SETFONTCOLOR (TEXTCOLOR, BKGDCOLOR);
    PrintY += 4;
    PrintX = CST_START;
    US_Print640 (STR_NWEP);
    PrintX = CST_START + CST_SPC * 1;
    US_Print640 (STR_STRFL);
    PrintX = CST_START + CST_SPC * 2;  
    US_Print640 (STR_STRFR);
    PrintX = CST_START + CST_SPC * 3;
    US_Print640 (STR_AMAP "\n");
    PrintY -= 4;

    //DrawWindow (CST_X, PrintY - 1, 310, 13, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 0, PrintY + 6, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 1, PrintY + 6, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 2, PrintY + 6, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawOutline (CST_X - 4 + CST_SPC * 3, PrintY + 6, CST_SPC-11, 15, BKGDCOLOR, BKGDCOLOR);
    DrawCustKeybd2 (0);
    
    //
    // PICK STARTING POINT IN MENU
    //
    if (CusItems.curpos < 0)
        for (i = 0; i < CusItems.amount; i++)
            if (CusMenu[i].active)
            {
                CusItems.curpos = i;
                break;
            }

    VW_UpdateScreen ();
    MenuFadeIn ();
}


void PrintCustMouse (int i)
{
    int j;

    for (j = 0; j < 4; j++)
        if (order[i] == buttonmouse[j])
        {
            PrintX = CST_START + CST_SPC * i;
            US_Print640 (mbarray[j]);
            break;
        }
}

void DrawCustMouse (int hilight)
{
    int i, color;

    color = BKGDCOLOR; 
    if (hilight)
        color = BKGDCOLOR;
    SETFONTCOLOR (color, BKGDCOLOR);

    if (!mouseenabled)
    {
        SETFONTCOLOR (DEACTIVE, BKGDCOLOR);
        CusMenu[0].active = 0;
    }
    else
        CusMenu[0].active = 1;

    PrintY = CST_Y + 13 * 2;
    for (i = 0; i < 4; i++)
        PrintCustMouse (i);
}

void PrintCustJoy (int i)
{
    for (int j = 0; j < 4; j++)
    {
        if (order[i] == buttonjoy[j])
        {
            PrintX = CST_START + CST_SPC * i;
            US_Print640 (mbarray[j]);
            break;
        }
    }
}

void DrawCustJoy (int hilight)
{
    int i, color;

    color = BKGDCOLOR;
    if (hilight)
        color = BKGDCOLOR;
    SETFONTCOLOR (color, BKGDCOLOR);

    if (!joystickenabled)
    {
        SETFONTCOLOR (DEACTIVE, BKGDCOLOR);
        CusMenu[4].active = 0;
    }
    else
        CusMenu[4].active = 1;

    PrintY = CST_Y + 13 * 6;
    for (i = 0; i < 4; i++)
        PrintCustJoy (i);
}


void PrintCustKeybd (int i)
{
    PrintX = CST_START + CST_SPC * i;
    US_Print640 ((const char *) IN_GetScanName (buttonscan[order[i]]));
}

void DrawCustKeybd (int hilight)
{
    int i, color;

    color = BKGDCOLOR;
    if (hilight)
        color = BKGDCOLOR;
    SETFONTCOLOR (color, BKGDCOLOR);

    PrintY = CST_Y + 13 * 10;
    for (i = 0; i < 4; i++)
        PrintCustKeybd (i);
}

void PrintCustKeys (int i)
{
    PrintX = CST_START + CST_SPC * i;
    US_Print640 ((const char *) IN_GetScanName (dirscan[moveorder[i]]));
}

void DrawCustKeys (int hilight)
{
    int i, color;

    color = BKGDCOLOR;
    if (hilight)
        color = BKGDCOLOR;
    SETFONTCOLOR (color, BKGDCOLOR);

    PrintY = CST_Y + 13 * 13;
    for (i = 0; i < 4; i++)
        PrintCustKeys (i);
}

void PrintCustKeybd2 (int i) 
{
    PrintX = CST_START + CST_SPC * i;
    US_Print640 ((const char *) IN_GetScanName (buttonscan[order2[i]]));
}

void DrawCustKeybd2 (int hilight)
{
    int i, color;

    color = BKGDCOLOR;
    if (hilight)
        color = BKGDCOLOR;
    SETFONTCOLOR (color, BKGDCOLOR);

    PrintY = CST_Y + 13 * 16;  
    for (i = 0; i < 4; i++)
        PrintCustKeybd2 (i);
}

////////////////////////////////////////////////////////////////////
//
// CHANGE RES
//
////////////////////////////////////////////////////////////////////
int vidresx, vidresy;

void SetResolution(boolean full) 
{
    screenWidth = vidresx;
    screenHeight = vidresy;
    
    fullscreen = full;
    
    if (screenBuffer != NULL)                           // line shouldnt be possible normally
      SDL_FreeSurface( screenBuffer );                  // dont leak ram memory like mad
  
    // free all malloc memory in VL_SetVGAPlaneMode
    free (pixelangle);                                                                   
    free (wallheight);

    VL_SetVGAPlaneMode();                               // malloc pixelangle sometimes crashes game when changing resolutions too much                                  
    
    VH_Startup ();       // calculate fizzlefade for right res again?
    
    NewViewSize (viewsize);
        
    // address the memory for statusbar and other latch images when going from full to windowed etc
    FreeLatchMem();
    LoadLatchMem();

    /*(viewwidth = screenWidth;
    viewheight = screenHeight;
    centerx = (viewwidth / 2) - 1;
    shootdelta = viewwidth/10;
    viewscreenx = viewscreeny = screenofs = 0; 

    CalcProjection(0x5700l);   */   
} 


int CP_SetVideo (int)
{
    int which,oldresx,oldresy;
    boolean oldfull;
    
    oldresx = screenWidth;
    oldresy = screenHeight;
    oldfull = fullscreen;
    
    DrawVideo ();
    DrawChangeView (viewsize);
    WaitKeyUp ();
    
    changedres = false;
    
    do
    {
        which = HandleMenu (&VidItems, &VidMenu[0], NULL);

        switch (which)
        {
                /* {7, "Fullscreen", 0},
                 {0, "", 0},
                 {9, "640x400", 0},
                 {1, "640x480", 0},
                 {1, "720x480", 0},
                 {1, "800x600", 0},
                 {1, "960x600", 0},
                 {1, "1024x768", 0},
                 {1, "1280x960", 0},
                 {1, "1366x768", 0},
                 {1, "1920x1080", 0},
                 {1, "3840x2160", 0}, */ 
            case -1:
                 which = 66;
                 WaitKeyUp ();
                 break;
               
            case 7: 
                 fullscreen ^= 1;
                 vidresx = screenWidth;
                 vidresy = screenHeight;
                 WaitKeyUp ();
                 setres = false;
                 break;
            case 9: 
                 vidresx = 640;
                 vidresy = 400;
                 setres = false;
                 break;
            case 10: 
                 vidresx = 640;
                 vidresy = 480;
                 setres = false;
                 break;  
            case 11:   
                 vidresx = 720;
                 vidresy = 480;
                 setres = false;
                 break;
            case 12:  
                 vidresx = 800;
                 vidresy = 600;
                 setres = false;
                 break;
            case 13:  
                 vidresx = 960;
                 vidresy = 600;
                 setres = false;
                 break;
            case 14:  
                 vidresx = 1024;
                 vidresy = 768;
                 setres = false;
                 break;
            case 15:  
                 vidresx = 1280;
                 vidresy = 720;
                 setres = false;
                 break;      
            case 16:  
                 vidresx = 1280;
                 vidresy = 960;
                 setres = false;
                 break;   
            case 17:  
                 vidresx = 1366;
                 vidresy = 768;
                 setres = false;
                 break;   
            case 18:  
                 vidresx = 1920;
                 vidresy = 1080;
                 setres = false;
                 break;             
            case 19:  
                 vidresx = 3840;
                 vidresy = 2160;
                 setres = false;
                 break;                      
        }
        
        if (vidresx != oldresx || vidresy != oldresy || oldfull != fullscreen)
        {
           if (!setres)  // if res is set by commandline ignore until actually changed in menu
           {         
               oldresx = screenWidth;
               oldresy = screenHeight;
               oldfull = fullscreen;
               changedres = true;        
               SetResolution (fullscreen);   
           }
                                  
           DrawVideo ();    // bombs out cause of VWB_DrawPicScaledRes in DrawMain if ingame and while which >= 0 (staying in this menu)                
           ShootSnd ();
        }
    }
    while (which < 0);
    
    if (which == 66) US_ControlPanel(0);

    return 0;
}

void DrawVideo (void)
{
    //ClearMScreen (); 
    DrawMainMenu();
    DrawWindow (VI_X - 18, VI_Y - 3, VI_W, (2*VI_H+26), WHITEC);
    DrawMenu (&VidItems, &VidMenu[0]);
    DrawChangeView (viewsize);
    DrawFrame (VI_X - 18, VI_Y - 3, VI_W, (2*VI_H+26), BKGDCOLOR, 14, 33, 23);
    
    PrintX = VI_X + 80;
    PrintY = VI_Y + 5 * 13;
    US_PrintUns640 (screenWidth);
    US_Print640 ("x");
    US_PrintUns640 (screenHeight);
    
    PrintX = VI_X;
    PrintY = VI_Y + 20 * 13;
    US_Print640 ("On most computers, a smaller\n");
    PrintX = VI_X; 
    US_Print640 ("window means faster play.");
    
    if (fullscreen)
    {
        PrintX = VI_X - 12;
        PrintY = VI_Y + 90;
        US_Print640 (STR_CHECK);
    }
    else
       VWB_640Bar (VI_X-14, VI_Y + 90, 11, 11, WHITEC);

   
    DrawMenuGun (&VidItems);
    VW_UpdateScreen ();
} 

////////////////////////////////////////////////////////////////////
//
// CHANGE SCREEN VIEWING SIZE
//
////////////////////////////////////////////////////////////////////
void DrawViewSlider (int size)
{ 
    SETFONTCOLOR(16,0); 
    PrintX = VI_X+10;
    PrintY = VI_Y+26;
    US_Print640 ("Small");
    PrintX = VI_X+70;
    PrintY = VI_Y+26;
    VWB_640Bar (VI_X+65, VI_Y+26, 24, 11, WHITEC);
    US_PrintUns640 (size);
    PrintX = VI_X+130;
    PrintY = VI_Y+26;
    US_Print640 ("Large"); 

    //VWB_640Bar (80, 77, 90, 5, WHITEC);
    //DrawOutline (80, 77, 90, 5, BKGDCOLOR, BKGDCOLOR);
    //VWB_640Bar (81 + 5 * size, 78, 4, 4, BKGDCOLOR);
    
    VWB_640Bar (200, VI_Y+45, 110, 10, WHITEC);
    DrawOutline (200, VI_Y+45, 110, 10, BKGDCOLOR, BKGDCOLOR);
    VWB_640Bar (201 + 5 * (size-1), VI_Y+46, 9, 9, BKGDCOLOR);

    VW_UpdateScreen ();
}

int CP_ChangeView (int)
{
    int exit = 0, oldview, newview;
    ControlInfo ci;

    newview = oldview = viewsize;

    //DrawVideo ();
    
    do
    {
        //CheckPause ();
        SDL_Delay(5);
        ReadAnyControl (&ci);
        switch (ci.dir)
        {
            case dir_North:
            case dir_South:
                //viewsize = newview;
                //NewViewSize (newview);
                DrawViewSlider(newview);
                exit = 1;
                break;
                          
            case dir_West:
                newview--;
                if (newview < 1)
                    newview = 1;
                    
                DrawViewSlider(newview);
                VW_UpdateScreen ();
                TicDelay (6);
                break;

            case dir_East:
                newview++;
                if (newview >= 21)
                    newview = 21;
                    
                DrawViewSlider(newview);
                VW_UpdateScreen ();
                TicDelay (6);
                break;
        }

        if (ci.button1 && !Keyboard[sc_Escape] || Keyboard[sc_Escape])
            exit = 1;
       /* else if (ci.button1 || Keyboard[sc_Escape])
        {
            //SD_PlaySound (ESCPRESSEDSND);
            //MenuFadeOut ();
            if(screenHeight % 200 != 0)
                VL_ClearScreen(0);
            return 0;
        }*/
    }
    while (!exit);

    if (oldview != newview)
    {
        //SD_PlaySound (SHOOTSND);
        //Message (STR_THINK "...");
        viewsize = newview;
        NewViewSize (newview);
    }

    //ShootSnd ();
    //MenuFadeOut ();
    //if(screenHeight % 200 != 0)k
    //    VL_ClearScreen(0);

    return 0;
}


/////////////////////////////
//
// DRAW THE CHANGEVIEW SCREEN
//

void DrawChangeView (int view)
{
    PrintX = VI_X+10;
    PrintY = VI_Y+26;
    US_Print640 ("Small");
    PrintX = VI_X+70;
    PrintY = VI_Y+26;
    VWB_640Bar (VI_X+65, VI_Y+26, 24, 11, WHITEC);
    US_PrintUns640 (view);
    PrintX = VI_X+130;
    PrintY = VI_Y+26;
    US_Print640 ("Large");
    
    //VWB_Bar (80, 77, 90, 5, WHITEC);
    //DrawOutline (80, 77, 90, 5, BKGDCOLOR, BKGDCOLOR);
    //VWB_Bar (81 + 5 * view, 78, 4, 4, BKGDCOLOR);
    
    VWB_640Bar (200, VI_Y+45, 110, 10, WHITEC);
    DrawOutline (200, VI_Y+45, 110, 10, BKGDCOLOR, BKGDCOLOR);
    VWB_640Bar (201 + 5 * (view-1), VI_Y+46, 9, 9, BKGDCOLOR);

    VW_UpdateScreen ();
}


////////////////////////////////////////////////////////////////////
//
// QUIT THIS INFERNAL GAME!
//
////////////////////////////////////////////////////////////////////
int CP_Quit (int)
{
    if (Confirm (endStrings[US_RndT () & 0x7 + (US_RndT () & 1)]))
    {
        VW_UpdateScreen ();
        SD_MusicOff ();
        SD_StopSound ();
        //MenuFadeOut ();
        Quit (NULL);
    }

    DrawMainMenu ();
    DrawFileMenu ();
    return 0;
}


////////////////////////////////////////////////////////////////////
//
// HANDLE INTRO SCREEN (SYSTEM CONFIG)
//
////////////////////////////////////////////////////////////////////
void IntroScreen (void)
{

}

////////////////////////////////////////////////////////////////////
//
// Clear Menu screens to dark red
//
////////////////////////////////////////////////////////////////////
void ClearMScreen (void)
{
#ifndef SPEAR
    //VWB_Bar (0, 0, 320, 200, BKGDCOLOR);
   if (ingame)
   {
      if (changedres)
         VWB_Bar (0, 0, screenWidth/scaleFactor, screenHeight/scaleFactor, BKGDCOLOR); 
      else 
         VL_LatchToScreenScaledCoord(tempgame, 0, 0, screenWidth, screenHeight, 0, 0, 0);
   }
   else 
      VWB_Bar (0, 0, screenWidth/scaleFactor, screenHeight/scaleFactor, BKGDCOLOR);
#else
    VWB_DrawPic (0, 0, C_BACKDROPPIC);
#endif
}


////////////////////////////////////////////////////////////////////
//
// Un/Cache a LUMP of graphics
//
////////////////////////////////////////////////////////////////////
void CacheLump (int lumpstart, int lumpend)
{
    int i;

    for (i = lumpstart; i <= lumpend; i++)
        CA_CacheGrChunk (i);
}

void UnCacheLump (int lumpstart, int lumpend)
{
    int i;

    for (i = lumpstart; i <= lumpend; i++)
        if (grsegs[i])
            UNCACHEGRCHUNK (i);
}

void CacheExtraLump (int lumpstart, int lumpend)
{
    int i;

    for (i = lumpstart; i <= lumpend; i++)
        CA_CacheGrChunk (i, 1);  
}

void UnCacheExtraLump (int lumpstart, int lumpend)
{
    int i;

    for (i = lumpstart; i <= lumpend; i++)
        if (grInterSegs[i])
            UNCACHEINTERCHUNK (i);
}

////////////////////////////////////////////////////////////////////
//
// Draw a window for a menu
//
////////////////////////////////////////////////////////////////////
void DrawWindow (int x, int y, int w, int h, int wcolor)
{
    VWB_640Bar (x, y, w, h, wcolor);
    DrawOutline (x, y, w, h, bkgdcolor, bkgdcolor);
}

void DrawOutline (int x, int y, int w, int h, int color1, int color2)
{
    VWB_640Bar (x, y, w, 1, color2);
    VWB_640Bar (x, y, 1, h, color2);
    VWB_640Bar (x, y+h, w, 1, color1);
    VWB_640Bar (x+3, y+h+1, w-2, 1, color1);
    VWB_640Bar (x+w, y, 1, h+1, color1);
    VWB_640Bar (x+w+1, y+3, 1, h-1, color1);
    
    /*
    VWB_Hlin (x, x + w, y, color2);
    VWB_Vlin (y, y + h, x, color2);
    VWB_Hlin (x, x + w, y + h, color1);
    
    VWB_Hlin (x+3, x + w+1, y + h + 1, color1);
    VWB_Vlin (y, y + h, x + w, color1);
    VWB_Vlin (y+3, y + h+1, x + w + 1, color1);
    */
}

// FRAME COLORS
// EPISODE: 255, 42, 43, 85
// MAIN: BKGDCOLOR, 14, 33, 23
// DrawFrame (VI_X - 20, VI_Y - 5, VI_W+2, (2*VI_H+13)+2, BKGDCOLOR, 14, 33, 23);
void DrawFrame (int x, int y, int w, int h, int color1, int color2, int color3, int color4)
{
    VWB_640Bar (x, y, w, 1, color1);
    VWB_640Bar (x, y+h, w, 1, color1);
    VWB_640Bar (x, y, 1, h, color1);
    VWB_640Bar (x+w, y, 1, h+1, color1);
    
    VWB_640Bar (x-1, y-1, w+3, 1, color4);
    VWB_640Bar (x-1, y-1, 1, h+3, color4);
    VWB_640Bar (x, y+h+1, w+1, 1, color2);
    VWB_640Bar (x+w+1, y, 1, h+2, color2);
    
    VWB_640Bar (x-2, y-2, w+4, 1, color3);
    VWB_640Bar (x-2, y-2, 1, h+4, color3);   
    VWB_640Bar (x-2, y+h+2, w+4, 1, color3);
    VWB_640Bar (x+w+2, y-2, 1, h+5, color3);
    
    VWB_640Bar (x-3, y-3, w+6, 1, color2);
    VWB_640Bar (x-3, y-3, 1, h+6, color2);   
    VWB_640Bar (x-3, y+h+3, w+6, 1, color4);
    VWB_640Bar (x+w+3, y-3, 1, h+7, color4);
 
}

////////////////////////////////////////////////////////////////////
//
// Setup Control Panel stuff - graphics, etc.
//
////////////////////////////////////////////////////////////////////
void SetupControlPanel (void)
{
    //
    // CACHE GRAPHICS & SOUNDS
    //
    CA_CacheGrChunk (STARTFONT + 1);
#ifndef SPEAR
    CacheLump (CONTROLS_LUMP_START, CONTROLS_LUMP_END);
#else
    CacheLump (BACKDROP_LUMP_START, BACKDROP_LUMP_END);
#endif

    SETFONTCOLOR (TEXTCOLOR, BKGDCOLOR);
    fontnumber = 1;
    WindowH = 200;
    if(screenHeight % 200 != 0)
        VL_ClearScreen(0);

    if (!ingame)
        CA_LoadAllSounds ();
    else
        MainMenu[savegame].active = 1;

    //
    // CENTER MOUSE
    //
    if(IN_IsInputGrabbed())
        IN_CenterMouse();
}

////////////////////////////////////////////////////////////////////
//
// SEE WHICH SAVE GAME FILES ARE AVAILABLE & READ STRING IN
//
////////////////////////////////////////////////////////////////////
void SetupSaveGames()
{
    char name[13];
    char savepath[300];

    strcpy(name, SaveName);
    for(int i = 0; i < 10; i++)
    {
        name[7] = '0' + i;
#ifdef _arch_dreamcast
        // Try to unpack file
        if(DC_LoadFromVMU(name))
        {
#endif
            //if(configdir[0])
            if(filedir[0])
                snprintf(savepath, sizeof(savepath), "%s/%s", filedir, name);
            else
                strcpy(savepath, name);

            const int handle = open(savepath, O_RDONLY | O_BINARY);
            if(handle >= 0)
            {
                char temp[32];

                SaveGamesAvail[i] = 1;
                read(handle, temp, 32);
                close(handle);
                strcpy(&SaveGameNames[i][0], temp);
            }
#ifdef _arch_dreamcast
            // Remove unpacked version of file
            fs_unlink(name);
        }
#endif
    }
}

////////////////////////////////////////////////////////////////////
//
// Clean up all the Control Panel stuff
//
////////////////////////////////////////////////////////////////////
void CleanupControlPanel (void)
{
#ifndef SPEAR
    UnCacheLump (CONTROLS_LUMP_START, CONTROLS_LUMP_END);
#else
    UnCacheLump (BACKDROP_LUMP_START, BACKDROP_LUMP_END);
#endif

    fontnumber = 0;
}


////////////////////////////////////////////////////////////////////
//
// Handle moving gun around a menu
//
////////////////////////////////////////////////////////////////////
int HandleMenu (CP_iteminfo * item_i, CP_itemtype * items, void (*routine) (int w))
{
    char key;
    static int redrawitem = 1, lastitem = -1;
    int i, x, y, basey, exit, which, shape;
    int32_t lastBlinkTime, timer;
    ControlInfo ci;

    controlmousex = 0, controlmousey = 0;
    
    which = item_i->curpos;
    
   if (item_i == &NewEitems)
   { 
       x = (screenWidth-scaleEpMenu*369)/(2*scaleEpMenu)  + item_i->x - 2;
       basey = (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu)  + item_i->y - 2;
       y = basey + which * 13;
   }
   else
   {
       x = item_i->x - 2;
       basey = item_i->y - 2;
       y = basey + which * 13;
   }

    //VWB_DrawPic (x, y, C_CURSOR1PIC);
    SetTextColor (items + which, 1, item_i);
    if (redrawitem)
    {
        if (item_i == &NewEitems)
        {            
           PrintX = (screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + item_i->x + item_i->indent;
           PrintY = (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + item_i->y + which * 13;
           //VWB_Bar (x/scaleFactor, y/scaleFactor, 155, 7, bkgdcolor); // first entry
           VWB_EpBar (x, y, 314, 13, bkgdcolor);
           US_PrintEp ((items + which)->string);  
        }
        else
        {            
           PrintX = item_i->x + item_i->indent;
           PrintY = item_i->y + which * 13; 
           //VWB_Bar (x/scaleFactor, y/scaleFactor, 60, 7, bkgdcolor); // first entry
           
           if (item_i == &AboutItems)
              VWB_640Bar (x, y, SUB_ABOUT_W, SUB_BAR_H, bkgdcolor); 
           else if (item_i == &LSItems)
              VWB_640Bar (x, y, SUB_LOAD_W, SUB_BAR_H, bkgdcolor);    
           else
           {
              if (item_i != &CusItems) 
                VWB_640Bar (x, y, SUB_BAR_W, SUB_BAR_H, bkgdcolor); 
           }
              
           US_Print640 ((items + which)->string);
        }           
    }
    //
    // CALL CUSTOM ROUTINE IF IT IS NEEDED
    //
    if (routine)
        routine (which);
    VW_UpdateScreen ();

    shape = C_CURSOR1PIC;
    timer = 8;
    exit = 0;
    lastBlinkTime = GetTimeCount ();
    IN_ClearKeysDown ();

    do
    {
        //
        // CHANGE GUN SHAPE
        //
        if ((int32_t)GetTimeCount () - lastBlinkTime > timer)
        {
            lastBlinkTime = GetTimeCount ();
            if (shape == C_CURSOR1PIC)
            {
                shape = C_CURSOR2PIC;
                timer = 2;
            }
            else
            {
                shape = C_CURSOR1PIC;
                timer = 4;
            }
            //VWB_DrawPic (x, y, shape);
            if (routine)
                routine (which);
            VW_UpdateScreen ();
        }
        else SDL_Delay(5);

        CheckPause ();

        //
        // SEE IF ANY KEYS ARE PRESSED FOR INITIAL CHAR FINDING
        //
        key = LastASCII;
        if (key)
        {
            int ok = 0;

            if (key >= 'a')
                key -= 'a' - 'A';

            for (i = which + 1; i < item_i->amount; i++)
                if ((items + i)->active && (items + i)->string[0] == key)
                {
                    EraseGun (item_i, items, x, y, which);
                    which = i;
                    DrawGun (item_i, items, x, &y, which, basey, routine);
                    ok = 1;
                    IN_ClearKeysDown ();
                    break;
                }

            //
            // DIDN'T FIND A MATCH FIRST TIME THRU. CHECK AGAIN.
            //
            if (!ok)
            {
                for (i = 0; i < which; i++)
                    if ((items + i)->active && (items + i)->string[0] == key)
                    {
                        EraseGun (item_i, items, x, y, which);
                        which = i;
                        DrawGun (item_i, items, x, &y, which, basey, routine);
                        IN_ClearKeysDown ();
                        break;
                    }
            }
        }

        //
        // GET INPUT
        //
        ReadAnyControl (&ci);
        switch (ci.dir)
        {
                ////////////////////////////////////////////////
                //
                // MOVE UP
                //
            case dir_North:

                EraseGun (item_i, items, x, y, which);

                //
                // ANIMATE HALF-STEP
                //
                if (which && (items + which - 1)->active)
                {
                    y -= 6;
                    DrawHalfStep (x, y);
                }

                //
                // MOVE TO NEXT AVAILABLE SPOT
                //
                do
                {
                    if (item_i == &NewItems)    // simply move up 
                    {
                       if (which == 2)
                          which = 0;
                       else if (which == 3)
                          which = 1;
                    }
                    else
                    {    
                        if (!which)
                            which = item_i->amount - 1;
                        else
                            which--;
                    }
                }
                while (!(items + which)->active);

                DrawGun (item_i, items, x, &y, which, basey, routine);
                //
                // WAIT FOR BUTTON-UP OR DELAY NEXT MOVE
                //
                TicDelay (20);
                break;

                ////////////////////////////////////////////////
                //
                // MOVE DOWN
                //
            case dir_South:

                EraseGun (item_i, items, x, y, which);
                //
                // ANIMATE HALF-STEP
                //
                if (which != item_i->amount - 1 && (items + which + 1)->active)
                {
                    y += 6;
                    DrawHalfStep (x, y);
                }

                do
                {
                    if (item_i == &NewItems)    // simply move down 
                    {
                       if (which == 0)
                          which = 2;
                       else if (which == 1)
                          which = 3;
                    }
                    else
                    { 
                    
                       if (which == item_i->amount - 1)
                           which = 0;
                       else
                           which++;     
                    }
                }
                while (!(items + which)->active);

                DrawGun (item_i, items, x, &y, which, basey, routine);

                //
                // WAIT FOR BUTTON-UP OR DELAY NEXT MOVE
                //
                TicDelay (20);
                break;        
        }
        
        // make Romero's salad custom control menu less mazey
        // let the user roam around without double entering
        // seriously why
        if (item_i == &CusItems)
        {   
            if (which == -1)   
            {    
                exit = 2; 
            } 
            else if (which == 0)   
            {    
                exit = 1;  
            }  
            else if (which == 4)   
            {    
                exit = 1; 
            }        
            else if (which == 8)   
            {    
                exit = 1; 
            }   
            else if (which == 11)   
            {    
                exit = 1; 
            }   
            else if (which == 14)   
            {    
                exit = 1; 
            }         
        }
                    
        if (ci.dir == dir_East || ci.dir == dir_West)
        {
            if (item_i == &AboutItems || item_i == &FileItems || item_i == &EditItems || item_i == &OptiItems)
            {       
               if (ci.dir == dir_East)     
                   exit = 3;
               else if (ci.dir == dir_West)     
                   exit = 4;    
            }
            
            // cheap hack for audio volume menu left/righ
            else if (item_i == &SndItems && which == 1) 
             {  AdjustVolume (0); exit = 1;  }
            else if (item_i == &SndItems && which == 1) 
             {  AdjustVolume (0); exit = 1; }
            else if (item_i == &SndItems && which == 3) 
               AdjustVolume (1);  
            else if (item_i == &SndItems && which == 3) 
               AdjustVolume (1); 
               
            else if (item_i == &VidItems && which == 1) 
               CP_ChangeView (0);  
             
            // cheap hack for difficulty screen left/right picking   
            else if (item_i == &NewItems && ci.dir == dir_West)
            {                  
                do
                {
                    if (which == 1)
                       which = 0;
                    else if (which == 3)
                       which = 2;   
                }
                while (!(items + which)->active);

                DrawGun (item_i, items, x, &y, which, basey, routine);
                TicDelay (20);
            }
            else if (item_i == &NewItems && ci.dir == dir_East)
            {                  
               do
                {
                    if (which == 0)
                       which = 1;
                    else if (which == 2)
                       which = 3;   
                }
                while (!(items + which)->active);
                DrawGun (item_i, items, x, &y, which, basey, routine);
                TicDelay (20);
            }
        }
        
        //VWB_640Bar (0,0,30,10,88);
        //PrintX = PrintY = 2;
        //US_PrintUns640(which);
        
        if (ci.button0 || Keyboard[sc_Space] || Keyboard[sc_Enter])
            exit = 1;
            
        if (ci.button1 && !Keyboard[sc_Alt] || Keyboard[sc_Escape])
            exit = 2; 
        
    }
    while (!exit);


    IN_ClearKeysDown ();

    //
    // ERASE EVERYTHING
    //
    if (lastitem != which)
    {                           
        if (item_i == &NewEitems)
        { 
           //VWB_Bar (x/scaleFactor, y/scaleFactor, 155, 7, WHITEC);        
           VWB_EpBar (x, y, 314, 13, WHITEC);    
           PrintX = (screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + item_i->x + item_i->indent;
           PrintY = (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + item_i->y + which * 13;   
           US_PrintEp ((items + which)->string);  
        }  
        else
        { 
           //VWB_Bar (x/scaleFactor, y/scaleFactor, 60, 7, WHITEC); 
           if (item_i == &AboutItems)
              VWB_640Bar (x, y, SUB_ABOUT_W, SUB_BAR_H, WHITEC); 
           else if (item_i == &LSItems)
              VWB_640Bar (x, y, SUB_LOAD_W, SUB_BAR_H, WHITEC);    
           else 
           {
              if (item_i != &CusItems) 
              {
                 if (item_i == &NewItems)          
                    VWB_640Bar (x, y, SUB_BAR_W, SUB_BAR_H, bkgdcolor);     
                 else    
                    VWB_640Bar (x, y, SUB_BAR_W, SUB_BAR_H, WHITEC); 
              }
           }
              
           PrintX = item_i->x + item_i->indent;
           PrintY = item_i->y + which * 13;
           US_Print640 ((items + which)->string);
        }
           
        redrawitem = 1;
    }
    else
        redrawitem = 0;

    if (routine)
        routine (which);
    VW_UpdateScreen ();

    item_i->curpos = which;

    lastitem = which;
    switch (exit)
    {  
        case 1:
            //
            // CALL THE ROUTINE (if CP_ != NULL)
            //
            if (item_i == &FileItems)
               return which;     
            else if (item_i == &NewItems) 
            {
               //  if (!StartGame)  
               return which;  
            }
            else
            {
               if ((items + which)->routine != NULL)// && item_i != &NewItems) // or else crashng on difficulty selection if diff = x picked
               {   
                  ShootSnd ();
                  //MenuFadeOut ();
                (items + which)->routine (0); // routine = cp_
               }            
               // if (!StartGame) // so the friggin loop escapes from the submenu into a new game. WTF ROMERO WITH THIS MENU MAZE. I'M GONNA TRIPLE IT UP
               return which;
            }

        case 2:    // going one back in menu
            SD_PlaySound (ESCPRESSEDSND);
            
            //if (StartGame)
            //US_ControlPanel(999); 
            return -1;
            
        // left/right the menus and properly exit them so they wont be called in another menu    
        case 3:  
            //US_ControlPanel(666);   
            return 33;
        case 4:  
            //US_ControlPanel(667);   
            return 44;   
    }

    return 0;                   // JUST TO SHUT UP THE ERROR MESSAGES!
}


//
// ERASE GUN & DE-HIGHLIGHT STRING
//
void EraseGun (CP_iteminfo * item_i, CP_itemtype * items, int x, int y, int which)
{
    if (item_i == &NewItems) return;
    
    SetTextColor (items + which, 0, item_i);        
        
    if (item_i == &NewEitems)
    { 
           //VWB_Bar (x/scaleFactor, y/scaleFactor, 155, 7, WHITEC);     
           VWB_EpBar (x, y, 314, 13, WHITEC);    
           PrintX = (screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + item_i->x + item_i->indent;
           PrintY = (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + item_i->y + which * 13;  
           US_PrintEp ((items + which)->string);  
    }  
    else
    { 
           if (item_i == &AboutItems)
              VWB_640Bar (x, y, SUB_ABOUT_W, SUB_BAR_H, WHITEC); 
           else if (item_i == &LSItems)
              VWB_640Bar (x, y, SUB_LOAD_W, SUB_BAR_H, WHITEC);    
           else
           {
              if (item_i != &CusItems) 
                VWB_640Bar (x, y, SUB_BAR_W, SUB_BAR_H, WHITEC); 
           }   
           
           PrintX = item_i->x + item_i->indent;
           PrintY = item_i->y + which * 13;
           US_Print640 ((items + which)->string);
    }
       
    VW_UpdateScreen ();
}


//
// DRAW HALF STEP OF GUN TO NEXT POSITION
//
void DrawHalfStep (int x, int y)
{
    //VWB_DrawPic (x, y, C_CURSOR1PIC);
    //VW_UpdateScreen ();
    //SD_PlaySound (MOVEGUN1SND);
    SDL_Delay (8 * 100 / 7);
}


//
// DRAW GUN AT NEW POSITION
//
void DrawGun (CP_iteminfo * item_i, CP_itemtype * items, int x, int *y, int which, int basey,
         void (*routine) (int w))
{
    *y = basey + which * 13;
      
    SetTextColor (items + which, 1, item_i);

    if (item_i == &NewEitems)
    { 
           //VWB_Bar (x/scaleFactor, *y/scaleFactor, 155, 7, bkgdcolor);   
           VWB_EpBar (x, *y, 314, 13, bkgdcolor);      
           PrintX = (screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + item_i->x + item_i->indent;
           PrintY = (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + item_i->y + which * 13;   
           US_PrintEp ((items + which)->string); 
    }  
    else
    { 
           if (item_i == &AboutItems)
              VWB_640Bar (x, *y, SUB_ABOUT_W, SUB_BAR_H, bkgdcolor);
           else if (item_i == &LSItems)
              VWB_640Bar (x, *y, SUB_LOAD_W, SUB_BAR_H, bkgdcolor);        
           else
           {
              if (item_i != &CusItems) 
                VWB_640Bar (x, *y, SUB_BAR_W, SUB_BAR_H, bkgdcolor); 
           } 
              
           PrintX = item_i->x + item_i->indent;
           PrintY = item_i->y + which * 13;
           US_Print640 ((items + which)->string);
    }

    //
    // CALL CUSTOM ROUTINE IF IT IS NEEDED
    //
    if (routine)
        routine (which);
    VW_UpdateScreen ();
    //SD_PlaySound (MOVEGUN2SND);
}

////////////////////////////////////////////////////////////////////
//
// DELAY FOR AN AMOUNT OF TICS OR UNTIL CONTROLS ARE INACTIVE
//
////////////////////////////////////////////////////////////////////
void TicDelay (int count)
{
    ControlInfo ci;
    int32_t startTime = GetTimeCount ();
    
    controlmousex = 0, controlmousey = 0;
    do
    {
        SDL_Delay(5);
        ReadAnyControl (&ci);
    }
    while ((int32_t) GetTimeCount () - startTime < count && ci.dir != dir_None);
}
   

////////////////////////////////////////////////////////////////////
//
// Draw a menu
//
////////////////////////////////////////////////////////////////////
void DrawMenu (CP_iteminfo * item_i, CP_itemtype * items)
{
    int i, which = item_i->curpos;

  if (item_i == &NewEitems)
  {      
     WindowX = PrintX = (screenWidth-scaleEpMenu*369)/(2*scaleEpMenu) + item_i->x + item_i->indent;
     WindowY = PrintY = (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + item_i->y;
     WindowW = 369;
     WindowH = 331;     
  }
  else
  {
     WindowX = PrintX = item_i->x + item_i->indent;
     WindowY = PrintY = item_i->y;
     WindowW = 320;
     WindowH = 200;
  }

    // in case jumping from sub menu to next mainmenu -> first spot needs background
    if (item_i->curpos == 0)
    {
        if (item_i == &NewEitems)              
            VWB_EpBar (WindowX -2, WindowY -2, 314, SUB_BAR_H, bkgdcolor);               
        else if (item_i != &LSItems)  
        {  
            if (item_i == &AboutItems)
              VWB_640Bar (PrintX -2, PrintY -2, SUB_ABOUT_W, SUB_BAR_H, bkgdcolor);
            else  
              VWB_640Bar (PrintX -2, PrintY -2, SUB_BAR_W, SUB_BAR_H, bkgdcolor);  
        }          
          //VWB_Bar ((PrintX-1)/scaleFactor, PrintY/scaleFactor -1, 60, 7, bkgdcolor);
    }

    //else if (item_i == &FileItems && item_i->curpos == 7)                         // fix: quit message: blank white issue
    //    VWB_Bar (PrintX/scaleFactor - 2, (5*PrintY)/scaleFactor -1, 60, 7, 88);
    
    for (i = 0; i < item_i->amount; i++)
    {
        SetTextColor (items + i, which == i, item_i);
        
        if (item_i == &NewEitems)
          PrintY = (screenHeight-scaleEpMenu*331)/(2*scaleEpMenu) + item_i->y + i * 13;   
        else
          PrintY = item_i->y + i * 13;
        
        if ((items + i)->active)
        {
            if (which == i)    
            {   
               //if (item_i == &NewEitems) // longer episode bars
               //  VWB_EpBar (WindowX - 2, WindowY -2, 314, SUB_BAR_H, bkgdcolor);
              //VWB_Bar ((PrintX-1)/scaleFactor, PrintY/scaleFactor -1, 155, 7, bkgdcolor); 
              // else    
              // {   
                   if (item_i == &AboutItems)
                      VWB_640Bar (PrintX -2, PrintY -2, SUB_ABOUT_W, SUB_BAR_H, bkgdcolor);
                   else    
                   {    
                      if (item_i != &NewEitems)         
                        VWB_640Bar (PrintX -2, PrintY -2, SUB_BAR_W, SUB_BAR_H, bkgdcolor);  // first entry when swapping back and forth in main menus      
                   }
              // }
            }
            
            if (item_i == &NewEitems)
               US_PrintEp ((items + i)->string);
            else 
               US_Print640 ((items + i)->string);
        }
        else
        {
            SETFONTCOLOR (DEACTIVE, bkgdcolor);
            
            if (item_i == &NewEitems)
               US_PrintEp ((items + i)->string);
            else   
               US_Print640 ((items + i)->string);
            SETFONTCOLOR (TEXTCOLOR, bkgdcolor);
        }

        US_Print ("\n");
    }
}


////////////////////////////////////////////////////////////////////
//
// SET TEXT COLOR (HIGHLIGHT OR NO)
//
////////////////////////////////////////////////////////////////////
void SetTextColor (CP_itemtype * items, int hlight, CP_iteminfo * item_i)
{     
    if (hlight)
    {
       if (item_i == &NewEitems)
       {
         SETFONTCOLOR (ep_hlite[items->active], 255);
       }
       else 
       {
         SETFONTCOLOR (color_hlite[items->active], 255);
       }
    }
    else
    {
        if (item_i == &NewEitems)
        {
         SETFONTCOLOR (ep_norml[items->active], 255);
        }
       else 
       {
         SETFONTCOLOR (color_norml[items->active], 255);
       }
    }
}


////////////////////////////////////////////////////////////////////
//
// WAIT FOR CTRLKEY-UP OR BUTTON-UP
//
////////////////////////////////////////////////////////////////////
void WaitKeyUp (void)
{
    ControlInfo ci;
    while (ReadAnyControl (&ci), ci.button0 |
           ci.button1 |
           ci.button2 | ci.button3 | Keyboard[sc_Space] | Keyboard[sc_Enter] | Keyboard[sc_Escape])
    {
        IN_WaitAndProcessEvents();
    }
}


////////////////////////////////////////////////////////////////////
//
// READ KEYBOARD, JOYSTICK AND MOUSE FOR INPUT
//
////////////////////////////////////////////////////////////////////
int controlmousex, controlmousey; 

void ReadAnyControl (ControlInfo * ci)
{
    int mouseactive = 0;

    IN_ReadControl (0, ci);

    if (mouseenabled && IN_IsInputGrabbed())
    {
        int mousex, mousey, buttons;
        
        buttons = SDL_GetRelativeMouseState(&mousex, &mousey);
        int middlePressed = buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE);
        int rightPressed = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
        buttons &= ~(SDL_BUTTON(SDL_BUTTON_MIDDLE) | SDL_BUTTON(SDL_BUTTON_RIGHT));
        if(middlePressed) buttons |= 1 << 2;
        if(rightPressed) buttons |= 1 << 1;

        controlmousex += mousex, controlmousey += mousey;

        if(controlmousey < -SENSITIVE)
        {
            controlmousey = 0;
            ci->dir = dir_North;
            mouseactive = 1;
        }
        else if(controlmousey > SENSITIVE)
        {
            controlmousey = 0;
            ci->dir = dir_South;
            mouseactive = 1;
        }

        if(controlmousex < -SENSITIVE)
        {
            controlmousex = 0;
            ci->dir = dir_West;
            mouseactive = 1;
        }
        else if(controlmousex > SENSITIVE)
        {
            controlmousex = 0;
            ci->dir = dir_East;
            mouseactive = 1;
        }  
      /*  buttons = SDL_GetRelativeMouseState(&mousex, &mousey);
        int middlePressed = buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE);
        int rightPressed = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
        buttons &= ~(SDL_BUTTON(SDL_BUTTON_MIDDLE) | SDL_BUTTON(SDL_BUTTON_RIGHT));
        if(middlePressed) buttons |= 1 << 2;
        if(rightPressed) buttons |= 1 << 1;

        if(mousey - CENTERY < -SENSITIVE)
        {
            ci->dir = dir_North;
            mouseactive = 1;
        }
        else if(mousey - CENTERY > SENSITIVE)
        {
            ci->dir = dir_South;
            mouseactive = 1;
        }

        if(mousex - CENTERX < -SENSITIVE)
        {
            ci->dir = dir_West;
            mouseactive = 1;
        }
        else if(mousex - CENTERX > SENSITIVE)
        {
            ci->dir = dir_East;
            mouseactive = 1;
        }

        if(mouseactive)
            IN_CenterMouse();*/

        if (buttons)
        {
            ci->button0 = buttons & 1;
            ci->button1 = buttons & 2;
            ci->button2 = buttons & 4;
            ci->button3 = false;
            mouseactive = 1;
        }
    }

    if (joystickenabled && !mouseactive)
    {
        int jx, jy, jb;

        IN_GetJoyDelta (&jx, &jy);
        if (jy < -SENSITIVE)
            ci->dir = dir_North;
        else if (jy > SENSITIVE)
            ci->dir = dir_South;

        if (jx < -SENSITIVE)
            ci->dir = dir_West;
        else if (jx > SENSITIVE)
            ci->dir = dir_East;

        jb = IN_JoyButtons ();
        if (jb)
        {
            ci->button0 = jb & 1;
            ci->button1 = jb & 2;
            ci->button2 = jb & 4;
            ci->button3 = jb & 8;
        }
    }
}


////////////////////////////////////////////////////////////////////
//
// DRAW DIALOG AND CONFIRM YES OR NO TO QUESTION
//
////////////////////////////////////////////////////////////////////
int Confirm (const char *string)
{
    int xit = 0, x, y, tick = 0, lastBlinkTime;
    int whichsnd[2] = { ESCPRESSEDSND, SHOOTSND };
    ControlInfo ci;

    controlmousex = 0, controlmousey = 0;
    
    Message (string);
    IN_ClearKeysDown ();
    WaitKeyUp ();

    //
    // BLINK CURSOR
    //
    x = PrintX;
    y = PrintY;
    lastBlinkTime = GetTimeCount();

    do
    {
        ReadAnyControl(&ci);

        if (GetTimeCount() - lastBlinkTime >= 10)
        {
            switch (tick)
            {
                case 0:
                    VWB_640Bar (x, y, 8, 13, WHITEC);
                    break;
                case 1:
                    PrintX = x;
                    PrintY = y;
                    US_Print640 ("_");
            }
            VW_UpdateScreen ();
            tick ^= 1;
            lastBlinkTime = GetTimeCount();
        }
        else SDL_Delay(5);

    }
    while (!Keyboard[sc_Y] && !Keyboard[sc_N] && !Keyboard[sc_Escape] && !ci.button0 && !ci.button1);

    if (Keyboard[sc_Y] || ci.button0)
    {
        xit = 1;
        ShootSnd ();
    }

    IN_ClearKeysDown ();
    WaitKeyUp ();

    SD_PlaySound ((soundnames) whichsnd[xit]);

    return xit;
}

////////////////////////////////////////////////////////////////////
//
// PRINT A MESSAGE IN A WINDOW
//
////////////////////////////////////////////////////////////////////
void Message (const char *string)
{
    int h = 0, w = 0, mw = 0, i, len = (int) strlen(string);
    fontstruct *font;

    CA_CacheGrChunk (STARTFONT + 1);
    fontnumber = 0;
    font = (fontstruct *) grsegs[STARTFONT + fontnumber];
    h = font->height;
    for (i = 0; i < len; i++)
    {
        if (string[i] == '\n')
        {
            if (w > mw)
                mw = w;
            w = 0;
            h += font->height;
        }
        else
            w += font->width[string[i]];
    }

    if (w + 10 > mw)
        mw = w + 10;

    PrintY = (WindowH / 2) - h / 2;
    PrintX = WindowX = viewwidth/4 - mw / 2;

    DrawWindow (WindowX - 5, PrintY - 5, mw + 10, h + 10, WHITEC);
    DrawOutline (WindowX - 5, PrintY - 5, mw + 10, h + 10, bkgdcolor, bkgdcolor);
    SETFONTCOLOR (bkgdcolor, TEXTCOLOR);
    US_Print640 (string);
    VW_UpdateScreen ();
}

////////////////////////////////////////////////////////////////////
//
// THIS MAY BE FIXED A LITTLE LATER...
//
////////////////////////////////////////////////////////////////////
static int lastmusic;

int StartCPMusic (int song)
{
    int lastoffs;

    lastmusic = song;
    lastoffs = SD_MusicOff ();
    //UNCACHEAUDIOCHUNK (STARTMUSIC + lastmusic);

    SD_StartMusic(STARTMUSIC + song);
    return lastoffs;
}

void FreeMusic (void)
{
    //UNCACHEAUDIOCHUNK (STARTMUSIC + lastmusic);
}


///////////////////////////////////////////////////////////////////////////
//
//      IN_GetScanName() - Returns a string containing the name of the
//              specified scan code
//
///////////////////////////////////////////////////////////////////////////
const char * IN_GetScanName (ScanCode scan)
{
/*    const char **p;
    ScanCode *s;

    for (s = ExtScanCodes, p = ExtScanNames; *s; p++, s++)
        if (*s == scan)
            return (*p);*/

    return (ScanNames[scan]);
}


///////////////////////////////////////////////////////////////////////////
//
// CHECK FOR PAUSE KEY (FOR MUSIC ONLY)
//
///////////////////////////////////////////////////////////////////////////
void CheckPause (void)
{
    if (Paused)
    {
        switch (SoundStatus)
        {
            case 0:
                SD_MusicOn ();
                break;
            case 1:
                SD_MusicOff ();
                break;
        }

        SoundStatus ^= 1;
        VW_WaitVBL (3);
        IN_ClearKeysDown ();
        Paused = false;
    }
}

///////////////////////////////////////////////////////////////////////////
//
// DRAW GUN CURSOR AT CORRECT POSITION IN MENU
//
///////////////////////////////////////////////////////////////////////////
void DrawMenuGun (CP_iteminfo * iteminfo)
{
    int x, y;

    //x = iteminfo->x;
    //y = iteminfo->y + iteminfo->curpos * 13 - 2;
    //VWB_DrawPic (x, y, C_CURSOR1PIC);
}


///////////////////////////////////////////////////////////////////////////
//
// DRAW SCREEN TITLE STRIPES
//
///////////////////////////////////////////////////////////////////////////
void Plot (int x, int y, int color)
{
    /*if(scaleFactor <= 2)
        VW_Plot(x,y,color);
    else
        VW_Bar(x, y, 1, 1, color);*/
        
    VW_640Bar(x, y, 1, 1, color);    
}


void DrawStripes (int y)
{
    int x; 
     
    //VWB_Bar (0, 0, screenWidth/scaleFactor, 10, 0);
    VWB_640Bar (0, 0, screenWidth/scale640, BAR_HIGH, WHITEC);
    
    Plot (0,0,16);
    Plot (1,0,16);
    Plot (2,0,16);
    Plot (3,0,16);
    Plot (4,0,16);
    Plot (0,1,16);
    Plot (0,2,16);
    Plot (0,3,16);
    Plot (0,4,16);
    Plot (1,1,16);
    Plot (2,1,16);
    Plot (1,2,16);
    
    //if(scaleFactor == 2)
    //   x = screenWidth;
    //else
    //   x = screenWidth/scaleFactor;
    x = screenWidth/scale640;
    
    Plot (x-1,0,16);
    Plot (x-2,0,16);
    Plot (x-3,0,16);
    Plot (x-4,0,16);
    Plot (x-5,0,16);
    Plot (x-1,1,16);
    Plot (x-1,2,16);
    Plot (x-1,3,16);
    Plot (x-1,4,16);
    Plot (x-2,1,16);
    Plot (x-3,1,16);
    Plot (x-2,2,16);
    
    VW_Hlin(0,screenWidth-1,18*scale640,BKGDCOLOR);
    
    VWB_DrawPicScaledRes (scale640*(screenWidth-60), scale640*1, C_NORMALPIC, 3); 
}

void ShootSnd (void)
{
    SD_PlaySound (SHOOTSND);
}


///////////////////////////////////////////////////////////////////////////
//
// CHECK FOR EPISODES
//
///////////////////////////////////////////////////////////////////////////
void CheckForEpisodes (void)
{
    struct stat statbuf;

    // On Linux like systems, the configdir defaults to $HOME/.wolf4sdl
#if !defined(_WIN32) && !defined(_arch_dreamcast)
    if(configdir[0] == 0)
    {
        // Set config location to home directory for multi-user support
        char *homedir = getenv("HOME");
        if(homedir == NULL)
        {
            Quit("Your $HOME directory is not defined. You must set this before playing.");
        }
        #define WOLFDIR "/.wolf4sdl"
        if(strlen(homedir) + sizeof(WOLFDIR) > sizeof(configdir))
        {
            Quit("Your $HOME directory path is too long. It cannot be used for saving games.");
        }
        snprintf(configdir, sizeof(configdir), "%s" WOLFDIR, homedir);
    }
#endif

    if(configdir[0] != 0)
    {
        // Ensure config directory exists and create if necessary
        if(stat(configdir, &statbuf) != 0)
        {
#ifdef _WIN32
            if(_mkdir(configdir) != 0)
#else
            if(mkdir(configdir, 0755) != 0)
#endif
            {
                Quit("The configuration directory \"%s\" could not be created.", configdir);
            }
        }
    }


    /*if(!stat("vswap.wl6", &statbuf))
    {
        strcpy (extension, "wl6");
        NewEmenu[2].active =
            NewEmenu[4].active =
            NewEmenu[6].active =
            NewEmenu[8].active =
            NewEmenu[10].active =
            EpisodeSelect[1] =
            EpisodeSelect[2] = EpisodeSelect[3] = EpisodeSelect[4] = EpisodeSelect[5] = 1;
    }
    else
    {
        if(!stat("vswap.wl3", &statbuf))
        {
            strcpy (extension, "wl3");
            numEpisodesMissing = 3;
            NewEmenu[2].active = NewEmenu[4].active = EpisodeSelect[1] = EpisodeSelect[2] = 1;
        }
        else
        {
            if(!stat("vswap.wl1", &statbuf))
            {
                strcpy (extension, "wl1");
                numEpisodesMissing = 5;
            }
            else
                Quit ("NO WOLF 3-D DATA to be found!");
        }
    }*/
    NewEmenu[2].active = NewEmenu[4].active = NewEmenu[6].active = NewEmenu[8].active = NewEmenu[10].active =
    EpisodeSelect[1] = EpisodeSelect[2] = EpisodeSelect[3] = EpisodeSelect[4] = EpisodeSelect[5] = 1;
    strcpy (extension, "wl6");


    strcpy (graphext, extension);
    //strcpy (audioext, extension);
    strcat (configname, extension);
    strcat (SaveName, extension);
    strcat (demoname, extension);

//#ifndef SPEAR
//#ifndef GOODTIMES
//    strcat (helpfilename, extension);
//#endif
//    strcat (endfilename, extension);
//#endif
}
