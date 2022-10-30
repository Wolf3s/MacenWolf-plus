// WL_AGENT.C

#include "wl_def.h"
#pragma hdrstop

/*
=============================================================================

                                LOCAL CONSTANTS

=============================================================================
*/

#define MAXMOUSETURN    10


#define MOVESCALE       150l
#define BACKMOVESCALE   100l
#define ANGLESCALE      20

int  starttilex,starttiley;
int  bjface; // dont google it. i need more sleep
/*
=============================================================================

                                GLOBAL VARIABLES

=============================================================================
*/

extern  statetype s_procket; 
extern  statetype s_flame; 
extern  statetype s_needle1;
extern  statetype s_fire1; 

//
// player state info
//
int32_t         thrustspeed;

word            plux,pluy;          // player coordinates scaled to unsigned

short           anglefrac;

objtype        *LastAttacker;
     
boolean     damageflash;
boolean     player_macstyle = true;
boolean     autorun;
boolean     hudmod = false;

/*
=============================================================================

                                                 LOCAL VARIABLES

=============================================================================
*/


void    T_Player (objtype *ob);
void    T_Attack (objtype *ob);
void    VictorySpin (void);

statetype   s_player = {false,0,0,(statefunc) T_Player,NULL,NULL};
statetype   s_attack = {false,0,0,(statefunc) T_Attack,NULL,NULL};

struct atkinf
{
    int8_t    tics,attack,frame;              // attack is 1 for gun, 2 for knife
} attackinfo[6][14] =
{
{ {6,0,1},{6,2,2},{6,0,3},{6,-1,4} },
{ {6,0,1},{6,1,2},{6,0,3},{6,-1,4} },
{ {6,0,1},{6,1,2},{6,3,3},{6,-1,4} },
{ {6,0,1},{6,1,2},{6,4,3},{6,-1,4} },
{ {6,0,1},{6,1,2},{6,4,3},{6,-1,4} },            
{ {6,0,1},{6,1,2},{6,0,3},{6,-1,4} },  
};

//===========================================================================


boolean TryMove (objtype *ob);
void T_Player (objtype *ob);

void ClipMove (objtype *ob, int32_t xmove, int32_t ymove);

void GiveWeapon (int weapon);
void GiveAmmo (int ammo);
void GiveMissile (int missile);  
void GiveGascan (int gascan);     

/*
=============================================================================

                                CONTROL STUFF

=============================================================================
*/

/*
======================
=
= CheckWeaponChange
=
= Keys 1-4 change weapons
=
======================
*/

// DHW: BrotherTank routine
// Checks to make sure you have ammo for weapon 
// APN: and that you also have the weapon 
// returns "true" if yes and "false" if no 

int ChkAtkAmmo (int weapon)
{ 
   switch (weapon) 
   {    
      case wp_knife: return 1; 
      case wp_pistol: 
           if (gamestate.ammo > 0) return 1; 
         break; 
      case wp_machinegun: 
           if (gamestate.gotweapon[wp_machinegun] && gamestate.ammo > 0) return 1; 
         break; 
      case wp_chaingun: 
           if (gamestate.gotweapon[wp_chaingun] && gamestate.ammo > 0) return 1; 
         break; 
      case wp_fthrower: 
           if (gamestate.gotweapon[wp_fthrower] && gamestate.gascan > 0) return 1; 
         break; 
      case wp_mlauncher: 
           if (gamestate.gotweapon[wp_mlauncher] && gamestate.missile > 0) return 1; 
         break; 
   } 
   return 0; 
} 


void CheckWeaponChange (void)      
{
      int     i,buttons;
      int     j,n;  
      int     newWeapon = -1;

        if (Keyboard[sc_6] && gamestate.gotweapon[wp_mlauncher] && (gamestate.missile > 0))
        {
                gamestate.chosenweapon = gamestate.weapon = wp_mlauncher ;
                DrawAmmo ();
                return ;    
        }

        if (Keyboard[sc_5] && gamestate.gotweapon[wp_fthrower] && (gamestate.gascan > 0))
        {
                gamestate.chosenweapon = gamestate.weapon = wp_fthrower ;
                DrawAmmo ();
                return ;    
        }
          
    // rework this because lul, just stand in 
    // or even better, rework the whole weapon system when bored  
    if(buttonstate[bt_nextweapon] && !buttonheld[bt_nextweapon] && gamestate.chosenweapon != -1)
    {                                       
         for (n = gamestate.chosenweapon + 1; n <= NUMWEAPONS; n++)
         {
             if (n >= NUMWEAPONS) n = 0;

             if (gamestate.gotweapon[n] && n != -1)
             {
                if (n == wp_pistol && !gamestate.ammo) continue;
                if (n == wp_machinegun && !gamestate.ammo) continue;
                if (n == wp_chaingun && !gamestate.ammo) continue;
                if (n == wp_fthrower && !gamestate.gascan) continue;
                if (n == wp_mlauncher && !gamestate.missile) continue;
                              
                gamestate.weapon = gamestate.chosenweapon = (weapontype) n;
                break;
             }
         }
    }                                                                                                                                                                                                                                                                                                                                                                        


        // APN: get best weapon (with ammo) when out of particular ammo

            if ((gamestate.weapon == wp_mlauncher) && (gamestate.missile<1)) 
            {
               for (j=gamestate.bestweapon; j>=0; j--)
               if (ChkAtkAmmo(j))
               {
                    if (gamestate.gotweapon[j])     
                    {        
                        gamestate.weapon = (weapontype) j;
                        break;
                    }
               }
            }
            else if ((gamestate.weapon == wp_fthrower) && (gamestate.gascan<1))
            {
               for (j=gamestate.bestweapon; j>=0; j--)
               if (ChkAtkAmmo(j))
               {
                    if (gamestate.gotweapon[j])     
                    {        
                        gamestate.weapon = (weapontype) j;
                        break;
                    }    
               }
            }
            else if ((gamestate.weapon <= wp_chaingun) && (gamestate.chosenweapon != wp_knife) && (gamestate.ammo < 1))
            {
               for (j=gamestate.bestweapon; j>=0; j--)
               if (ChkAtkAmmo(j))
               {
                    if (gamestate.gotweapon[j])     
                    {        
                        gamestate.weapon = (weapontype) j;
                        break;
                    }
               }
            }
            else if ((gamestate.weapon == wp_knife) && (gamestate.chosenweapon != wp_knife) && 
                     (gamestate.ammo > 0 || gamestate.gascan > 0 || gamestate.missile > 0))
            {
               for (j=gamestate.bestweapon; j>=0; j--)
               if (ChkAtkAmmo(j))
               {
                    if (gamestate.gotweapon[j])     
                    {              
                        gamestate.chosenweapon = gamestate.weapon = (weapontype) j;
                        break;
                    }
               }
            }
            
            if (!gamestate.gotweapon[wp_knife] && gamestate.ammo <= 0 && gamestate.gascan <= 0 && gamestate.missile <= 0)
            {           
                        gamestate.chosenweapon = gamestate.weapon = (weapontype) -1;
            }
            
             // must use knife with no ammo
        else if (!gamestate.ammo && !gamestate.gascan && !gamestate.missile && gamestate.gotweapon[wp_knife])
            gamestate.weapon = wp_knife;  // APN: knife is not "chosen" here
        else if (Keyboard[sc_1] && gamestate.gotweapon[wp_knife] && ChkAtkAmmo(0))
            gamestate.weapon = gamestate.chosenweapon = wp_knife;
        else if (Keyboard[sc_2] && gamestate.gotweapon[wp_pistol] && ChkAtkAmmo(1))
            gamestate.weapon = gamestate.chosenweapon = wp_pistol;
        else if (Keyboard[sc_3] && gamestate.gotweapon[wp_machinegun] && ChkAtkAmmo(2))
            gamestate.weapon = gamestate.chosenweapon = wp_machinegun;
        else if (Keyboard[sc_4] && gamestate.gotweapon[wp_chaingun] && ChkAtkAmmo(3))
            gamestate.weapon = gamestate.chosenweapon = wp_chaingun;
        else if (Keyboard[sc_5] && gamestate.gotweapon[wp_fthrower] && ChkAtkAmmo(4))
            gamestate.weapon = gamestate.chosenweapon = wp_fthrower;
        else if (Keyboard[sc_6] && gamestate.gotweapon[wp_mlauncher] && ChkAtkAmmo(5))
            gamestate.weapon = gamestate.chosenweapon = wp_mlauncher;
              
        DrawAmmo ();
}

void CheckWeaponChangeOld (void)
{
    int newWeapon = -1;

    if (!gamestate.ammo)            // must use knife with no ammo
        return;

#ifdef _arch_dreamcast
    int joyx, joyy;
    IN_GetJoyFineDelta (&joyx, &joyy);
    if(joyx < -64)
        buttonstate[bt_prevweapon] = true;
    else if(joyx > 64)
        buttonstate[bt_nextweapon] = true;
#endif

    if(buttonstate[bt_nextweapon] && !buttonheld[bt_nextweapon])
    {
        newWeapon = gamestate.weapon + 1;
        if(newWeapon > gamestate.bestweapon) newWeapon = 0;
    }
    else if(buttonstate[bt_prevweapon] && !buttonheld[bt_prevweapon])
    {
        newWeapon = gamestate.weapon - 1;
        if(newWeapon < 0) newWeapon = gamestate.bestweapon;
    }
    else
    {
        for(int i = wp_knife; i <= gamestate.bestweapon; i++)
        {
            if (buttonstate[bt_readyknife + i - wp_knife])
            {
                newWeapon = i;
                break;
            }
        }
    }

    if(newWeapon != -1)
    {
        gamestate.weapon = gamestate.chosenweapon = (weapontype) newWeapon;
        DrawWeapon();
    }
}


/*
=======================
=
= ControlMovement
=
= Takes controlx,controly, and buttonstate[bt_strafe]
=
= Changes the player's angle and position
=
= There is an angle hack because when going 70 fps, the roundoff becomes
= significant
=
=======================
*/

void ControlMovement (objtype *ob)
{
    int32_t oldx,oldy;
    int32_t movescale,backmovescale;
    int     angle;
    int     angleunits;
    
    
    if (mod.player_speed >= 0)
    {
       movescale = mod.player_speed;
       backmovescale = 2*mod.player_speed/3;
    }
    else
    {
       movescale = MOVESCALE;
       backmovescale = BACKMOVESCALE;
    }

    thrustspeed = 0;

    oldx = player->x;
    oldy = player->y;

    if(buttonstate[bt_strafeleft])
    {
        angle = ob->angle + ANGLES/4;
        if(angle >= ANGLES)
            angle -= ANGLES;
            
        if (player_macstyle)
        {    
            if(buttonstate[bt_run])
               Thrust(angle, RUNMOVE_MAC * movescale/2 * tics );
            else
               Thrust(angle, BASEMOVE_MAC * movescale/2 * tics);
        }
        else
        {
            if(buttonstate[bt_run])
               Thrust(angle, RUNMOVE * movescale * tics );
            else
               Thrust(angle, BASEMOVE * movescale * tics);
        }
    }

    if(buttonstate[bt_straferight])
    {
        angle = ob->angle - ANGLES/4;
        if(angle < 0)
            angle += ANGLES;
            
        if (player_macstyle)
        {    
            if(buttonstate[bt_run])
               Thrust(angle, RUNMOVE_MAC * movescale/2 * tics );
            else
               Thrust(angle, BASEMOVE_MAC * movescale/2 * tics);
        }
        else
        {
            if(buttonstate[bt_run])
               Thrust(angle, RUNMOVE * movescale * tics );
            else
               Thrust(angle, BASEMOVE * movescale * tics);
        }
    }

    //
    // side to side move
    //
    if (buttonstate[bt_strafe])
    {
        //
        // strafing
        //
        //
        if (controlx > 0)
        {
            angle = ob->angle - ANGLES/4;
            if (angle < 0)
                angle += ANGLES;
            
            if (player_macstyle)
            {
               if(buttonstate[bt_run])
                  Thrust(angle, RUNMOVE_MAC * movescale/2 * tics );
               else
                  Thrust(angle, BASEMOVE_MAC * movescale/2 * tics);
            } 
            else     
               Thrust (angle,controlx*MOVESCALE);      // move to left   
               
        }
        else if (controlx < 0)
        {
            angle = ob->angle + ANGLES/4;
            if (angle >= ANGLES)
                angle -= ANGLES;
            
            if (player_macstyle)
            {
               if(buttonstate[bt_run])
                  Thrust(angle, RUNMOVE_MAC * movescale/2 * tics );
               else
                  Thrust(angle, BASEMOVE_MAC * movescale/2 * tics); 
            } 
            else  
               Thrust (angle,-controlx*MOVESCALE);     // move to right   
        }
    }
    else
    {
        //
        // not strafing
        //
        anglefrac += controlx;
        angleunits = anglefrac/ANGLESCALE;
        anglefrac -= angleunits*ANGLESCALE;
        ob->angle -= angleunits;

        if (ob->angle >= ANGLES)
            ob->angle -= ANGLES;
        if (ob->angle < 0)
            ob->angle += ANGLES;

    }

    //
    // forward/backwards move
    //
    if (controly < 0)
    {
        Thrust (ob->angle,-controly*movescale); // move forwards
    }
    else if (controly > 0)
    {
        angle = ob->angle + ANGLES/2;
        if (angle >= ANGLES)
            angle -= ANGLES;
            
        if (player_macstyle)   
           Thrust (angle,controly*movescale); 
        else
           Thrust (angle,controly*backmovescale);          // move backwards
    }

    if (gamestate.victoryflag || gamestate.victorycam)              // watching the BJ actor
        return;
}

/*
=============================================================================

                            STATUS WINDOW STUFF

=============================================================================
*/


/*
==================
=
= StatusDrawPic
=
==================
*/

void StatusDrawPic (unsigned x, unsigned y, unsigned picnum, boolean fixed, int huditem)
{
    //LatchDrawPicScaledCoord ((screenWidth-scaleFactor*320)/16 + scaleFactor*x, screenHeight-scaleFactor*(STATUSLINES-y),picnum, 2 );

    //LatchDrawPicScaledCoord ((screenWidth-(scaleFactor-1)*640)/16 + (scaleFactor-1)*x, screenHeight-(scaleFactor-1)*2*(STATUSLINES-y),picnum, 1 );
    //LatchDrawPicScaledCoord ((screenWidth-(scaleFactor/2)*640)/16 + (scaleFactor/2)*x, screenHeight-(scaleFactor/2)*2*(STATUSLINES-y),picnum, 2);    

    if (fixed)
    {
        x = (screenWidth-(scaleFactor/2)*640)/16 + (scaleFactor/2)*x;
        y = screenHeight-(scaleFactor/2)*2*(STATUSLINES-y);
        
        LatchDrawPicScaledCoord (x, y, picnum, 2); 
    }
    else
    {
        //x = scale640*x;
        //y = scale640*y;
        //x = (screenWidth*x)/100;
        //y = (screenHeight*y)/100;   // add 16:9, etc statusbar to fit?
        
        if (mod.hud_align[huditem] == northwest)                                     // top left
        {
             x = scale640*x;
             y = scale640*y;
        }
        else if (mod.hud_align[huditem] == north)                                     // top center
        {
             x = (screenWidth - scale640*640)/2 + scale640*x;
             y = scale640*y;  
        }
        else if (mod.hud_align[huditem] == northeast)                                // top right
        {
             x = screenWidth - scale640*x;
             y = scale640*y;  
        }
        else if (mod.hud_align[huditem] == southwest)                                // bottom left
        {
             x = scale640*x;
             y = screenHeight - scale640*y;
        }
        else if (mod.hud_align[huditem] == south)                                    // bottom center
        {
             x = (screenWidth - scale640*640)/2 + scale640*x;
             y = screenHeight - scale640*y;     
        }
        else if (mod.hud_align[huditem] == southeast)                                // bottom right
        {
             x = screenWidth - scale640*x;
             y = screenHeight - scale640*y;  
        }  
        
        VL_LatchToScreenScaledCoord (latchpics[2+picnum-LATCHPICS_LUMP_START], x, y, 2);  
    } 
}

void StatusPic (unsigned x, unsigned y, unsigned xx, unsigned picnum, boolean fixed)
{
    if (fixed)
    {
        x = (screenWidth-(scaleFactor/2)*640)/16 + (scaleFactor/2)*x;
        y = screenHeight-(scaleFactor/2)*2*(STATUSLINES-y);
        
        LatchDrawPicScaledCoord (x, y, picnum, 2); 
    }
    else
    {        
        x = (screenWidth*x)/100;
        y = (screenHeight*y)/100; 
        VL_LatchToScreenScaledCoord (latchpics[2+picnum-LATCHPICS_LUMP_START], x+xx, y, 2);  
    } 
}
 
void StatusDrawFace(unsigned picnum)
{
    StatusDrawPic(17, 4, picnum, true, face);
#ifdef _arch_dreamcast
    DC_StatusDrawLCD(picnum);
#endif
}


/*
==================
=
= DrawFaceFrame
=
==================
*/

void DrawFaceFrame (unsigned picnum)
{
    int x,y;
    boolean fixed;
    
    if (mod.hud_x[face] >= 0 && mod.hud_y[face] >= 0)
    {
        if (mod.hud_x[face] == 1111 && mod.hud_y[face] == 1111)
            return;            
                    
        x = mod.hud_x[face];
        y = mod.hud_y[face];      
        fixed = false;          
    }
    else
    {
        x = 20*2;
        y = 4;
        fixed = true;  
    }
     
    if(viewsize == 21 && ingame && !hudmod) return; 

    StatusDrawPic (x,y,picnum,fixed,face); 
}

/*
==================
=
= DrawFace
=
==================
*/

void DrawFace (void)
{
    int x,y;
    boolean fixed;
    
    if (mod.hud_x[face] >= 0 && mod.hud_y[face] >= 0)
    {
        if (mod.hud_x[face] == 1111 && mod.hud_y[face] == 1111)
            return;            
                    
        x = mod.hud_x[face];
        y = mod.hud_y[face];      
        fixed = false;          
    }
    else
    {
        x = 20*2;
        y = 4;
        fixed = true;  
    }
     
    if(viewsize == 21 && ingame && !hudmod) return; 
    
	if (gamestate.health)
	{
		if (godmode)
            StatusDrawPic (x,y,FACEFRAME2+gamestate.faceframe,fixed,face); 
		else
		{
            if (bjface != 0)
		       StatusDrawPic (x,y,bjface,fixed,face);
		    else
               StatusDrawPic (x,y,FACEFRAME0+gamestate.faceframe,fixed,face);
        }    
	}
	else
	{
        StatusDrawPic (x,y,FACEFRAME9,fixed,face); 
	}
}

/*
===============
=
= UpdateFace
=
= Calls draw face if time to change
=
===============
*/

int facecount = 0;
int facetimes = 0;

void UpdateFace (void)
{ 
    word base;
     
    // don't make demo depend on sound playback
    if(demoplayback || demorecord)
    {
        if(facetimes > 0)
        {
            facetimes--;
            return;
        }
    }
    else if(SD_SoundPlaying() == GETGATLINGSND)
        return;
        
    if(facecount > 0)
    {
        facecount -= tics;  
        //return;   
    }
    else 
    {
        bjface = 0; 
		base = (gamestate.health <= 25) ? 5 : 0;
		if (gamestate.faceframe == base) // Normal frame? 
        {	
			base++;		                 // Use alternate 
		}
		gamestate.faceframe = base;	     // Set the new face 
		facecount = (US_RndT ()&31)+4;	 // Random time 
		DrawFace ();
	}

    /*facecount += tics;
    if (facecount > US_RndT())
    {
        gamestate.faceframe = (US_RndT()>>6);
        if (gamestate.faceframe==3)
            gamestate.faceframe = 1;
	if (gamestate.faceframe==0)  // APN: BJ does not stare forward
            gamestate.faceframe = 2;

        facecount = 0;
        DrawFace ();
    }*/
}


/*
===============
=
= LatchNumber
=
= right justifies and pads with blanks
=
===============
*/

static void LatchNumber (int x, int y, unsigned width, int32_t number, boolean fixed, int huditem)
{
    unsigned length,c;
    char    str[20];
    int xx = 0;
    
    ltoa (number,str,10);

    length = (unsigned) strlen (str);

    while (length<width)
    {
        StatusDrawPic (x,y,N_BLANKPIC,fixed,huditem);
        //StatusPic (x,y,xx,N_BLANKPIC,fixed);
        
        //x++;
        if (fixed)
           x+=2;
        else
        {
           if (mod.hud_align[huditem] == northeast || mod.hud_align[huditem] == southeast) // flip the number order on right side
               x-=16;
           else
               x+=16;   
           //xx+=16*scale640; // 2*8 (Latchpic x*8)
        }
        width--;
    }

    c = length <= width ? 0 : length-width;

    while (c<length)
    {
        StatusDrawPic (x,y,str[c]-'0'+ N_0PIC,fixed,huditem);
        //StatusPic (x,y,xx,str[c]-'0'+ N_0PIC,fixed);
        
        //x++;
        if (fixed)
           x+=2;
        else
        {
           if (mod.hud_align[huditem] == northeast || mod.hud_align[huditem] == southeast) // flip the number order on right side
               x-=16;
           else
               x+=16;   
           //xx+=16*scale640;
        }
        c++;
    }
}


/*
===============
=
= DrawHealth
=
===============
*/

void DrawHealth (void)
{
    int x,y;
    boolean fixed;
    
    if (mod.hud_x[health] >= 0 && mod.hud_y[health] >= 0)
    {
        if (mod.hud_x[health] == 1111 && mod.hud_y[health] == 1111)
            return;            
                    
        x = mod.hud_x[health];
        y = mod.hud_y[health];  
        fixed = false;              
    }
    else
    {
        if (mod.maxhealth && mod.maxhealth > 999)
            x = 51;
        else
            x = 53;
         
        y = 16;
        fixed = true;
    }
     
    if(viewsize == 21 && ingame && !hudmod) return;
 
    if (mod.maxhealth && mod.maxhealth > 999)
        LatchNumber (x,y,4,gamestate.health,fixed,health); 
    else
        LatchNumber (x,y,3,gamestate.health,fixed,0); 
}

/*
===============
=
= TakeDamage
=
===============
*/

void TakeDamage (int points,objtype *attacker)
{          
    LastAttacker = attacker;

    if (gamestate.victoryflag || gamestate.victorycam)
        return;
        
    //if (gamestate.difficulty==gd_baby)
    //    points>>=2;
    
    if (mod.en_damage[LastAttacker->obclass] >= 0)        
        points = mod.en_damage[LastAttacker->obclass];

    if (!godmode)
        gamestate.health -= points;

    if (gamestate.health<=0)
    {
        gamestate.health = 0;
        playstate = ex_died;
        killerobj = attacker;
        
        if (gamestate.lives == 1) // only for game over, for pulsing sprite
           gamestate.TimeCount = 0; 
    }
    else
    {
           int sounds[2]={ MUTTISND,
                           MEINGOTTSND};
                           
		   PlaySoundLocActor(sounds[US_RndT()%2],player);
    }

    if (damageflash)
        StartDamageFlash (points);

    DrawHealth ();
    //DrawFace ();
    
    //player->angle = attacker->angle;
    
    if (gamestate.health!=0 && viewsize != 21 && !hudmod )//&& ~attacker->flags & FL_VISABLE)
    {    
        facecount = 120;
        
            int32_t newx,newy;
            
            // ATINA's cos/sin x-y displacement massacre 
            // bring enemy world coords to new coords based on player's own rotating axis
            newx = FixedMul(attacker->x - player->x, costable[360-player->angle]) - 
                   FixedMul(attacker->x - player->x, sintable[360-player->angle]);
                                 
            newy = FixedMul(attacker->y - player->y, costable[360-player->angle]) + 
                   FixedMul(attacker->y - player->y, sintable[360-player->angle]);  
                           
            if (newy < 0)
            {   
               if (newx < 0)  
               { 
                  if ((gamestate.health <= 25 && !thirdencounter) || (gamestate.health <= 50 && thirdencounter)) 
                     bjface = FACEFRAME3+5;
                  else     
                     bjface = FACEFRAME3;   //US_Print("left back");
               }
               else if (newx >= 0)  
               {  
                  if ((gamestate.health <= 25 && !thirdencounter) || (gamestate.health <= 50 && thirdencounter)) 
                     bjface = FACEFRAME2+5;   
                  else     
                     bjface = FACEFRAME2;  //US_Print("right back");
               }
            } 
            else if (newy >= 0)  
            {
               if (newx < 0)   
               { 
                  if ((gamestate.health <= 25 && !thirdencounter) || (gamestate.health <= 50 && thirdencounter))
                     bjface = FACEFRAME1+5; 
                  else      
                     bjface = FACEFRAME1;//US_Print("left front");
               }
               else if (newx >= 0)    
               { 
                  if ((gamestate.health <= 25 && !thirdencounter) || (gamestate.health <= 50 && thirdencounter)) 
                     bjface = FACEFRAME0+5;
                  else   
                     bjface = FACEFRAME0; //US_Print("right front");
               }
            } 
            
            /*float   iangle;
            int     angle;
            int32_t deltax,deltay;                     
            deltax = player->x - attacker->x;
            deltay = attacker->y - player->y;                                
            iangle = (float) atan2((float) deltay, (float) deltax);
            if (iangle<0)
               iangle = (float) (M_PI*2+iangle);
            angle = (int) (iangle/(M_PI*2)*ANGLES);
            angle = angle - player->angle; 
            if (angle > 0) 
            { 
		       if (angle  > 45) //0x2000/18
			      US_Print("1");//StatusDrawPic (20*2,4,FACE8APIC); 
               else 
			      US_Print("2");//StatusDrawPic (20*2,4,MUTANTBJPIC); 
            } 
            else 
            {
		       if (angle < -45) //-0x2000/18
			      US_Print("3");//StatusDrawPic (20*2,4,FACE4APIC);      
               else 
			      US_Print("4");//StatusDrawPic (20*2,4,GOTGATLINGPIC);   	
	        }*/
            
       	    /*if (angle > 0) 
            { 
		       if (angle  > 45) //0x2000/18
			      US_Print("1");//StatusDrawPic (20*2,4,FACE8APIC); 
               else 
			      US_Print("2");//StatusDrawPic (20*2,4,MUTANTBJPIC); 
            } 
            else 
            {
		       if (angle < -45) //-0x2000/18
			      US_Print("3");//StatusDrawPic (20*2,4,FACE4APIC);      
               else 
			      US_Print("4");//StatusDrawPic (20*2,4,GOTGATLINGPIC);   	
	        }*/
	        
	        
	        // ORIGINAL MAC VERSION
	        /*fixed angle = PointToAngle(attacker->x,attacker->y) - (player->angle*182); //7 SHORTTOANGLESHIFT);
	        // keep in range of 3600 fineangles. MAC 0x10000 -> 360. 0x10000 - player->angle<<7 = 0 (player->angle = 512?)
            // 0x10000/3600 = 18
            angle = angle/18;
            if (angle > 0) 
            {
		        if (angle > 450) //0x2000/18
			         US_Print("left front");
                else 
			         US_Print("right front");
            } 
            else 
            {
		         if (angle < -450) //-0x2000/18
			         US_Print("left back");
		          else 
	                 US_Print("right back");
            } */
            
           // ANDY's Version (tan problems at 180?)
           /*float   fangle;                       
           int32_t dx,dy;                         
           int     iangle, clockwise, counter;
            
           if (attacker)   //  don't break TAB-H function
           { 
               dx = attacker->x - player->x;
               dy = player->y - attacker->y;

               fangle = atan2((float) dy, (float) dx);     // returns -pi to pi
               
               if (fangle<0)
                   fangle = (float) (M_PI*2+fangle);

               iangle = (int) (fangle/(M_PI*2)*ANGLES);

               if (player->angle > iangle)    //  enemy to the right of BJ
               {       
                   counter = player->angle - iangle;
                   clockwise = ANGLES-player->angle + iangle;
               }
               else                                    // enemy to the left of BJ
               {
                   clockwise = iangle - player->angle;
                   counter = player->angle + ANGLES-iangle;
               }     
               PrintX=4; PrintY=1;
               if (clockwise<counter)         // turn right
               {
                   if (iangle - player->angle > 45)
                   {
                       US_Print("1");
                   }
                   else 
                       US_Print("2");
               }                                     
               else                                  // turn left
               {
                   if (player->angle - iangle > 45)
                   {
                      US_Print("3");
                   }
                   else
                     US_Print("4");
               }                     
           }   */        
    }
}

/*
===============
=
= HealSelf
=
===============
*/

void HealSelf (int points)
{
          
    gamestate.health += points;
    if (gamestate.health > 100 && !mod.maxhealth)
        gamestate.health = 100;
    else if (gamestate.health > mod.maxhealth && mod.maxhealth)
        gamestate.health = mod.maxhealth;
        
    DrawHealth ();    
        
    if ((gamestate.health <= 25 && !thirdencounter) || (gamestate.health <= 50 && thirdencounter))  
		bjface = FACEFRAME5;
    else 
        bjface = FACEFRAME0;

    DrawFace ();
}


//===========================================================================


/*
===============
=
= DrawLevel
=
===============
*/

void DrawLevel (void)
{
    int episode; 
    int x,xx,y;
    boolean fixed;
    
    if (mod.hud_x[area] >= 0 && mod.hud_y[area] >= 0)
    {
        if (mod.hud_x[area] == 1111 && mod.hud_y[area] == 1111)
            return;            
                    
        x = mod.hud_x[area];
        y = mod.hud_y[area];
        xx = 48;
        fixed = false;          
    }
    else
    {
        x = 2;
        y = 16;
        xx = 6;  
        fixed = true;
    }
    
    if(viewsize == 21 && ingame && !hudmod) return;
    
        //episode = hudmapnum[gamestate.episode*10+mapon+1]/10;
        episode = hudmapnum[gamestate.episode*10+mapon+1]/10;
        
        if (hudmapnum[gamestate.episode*10+mapon+1])
        {
           LatchNumber (x,y,1,episode,fixed,area);
           LatchNumber (x+xx,y,1,hudmapnum[gamestate.episode*10+mapon+1]-(episode*10),fixed,area); 
        }
        else
        {
           LatchNumber (x,y,1,0,fixed,area);    // 1*2                                                   
           LatchNumber (x+xx,y,1,0,fixed,area);  // 4*2
        }
        
}

//===========================================================================


/*
===============
=
= DrawLives
=
===============
*/

void DrawLives (void)
{
    int x,y;
    boolean fixed;
    
    if (mod.hud_x[lives] >= 0 && mod.hud_y[lives] >= 0)
    {
        if (mod.hud_x[lives] == 1111 && mod.hud_y[lives] == 1111)
            return;            
                    
        x = mod.hud_x[lives];
        y = mod.hud_y[lives];        
        fixed = false;        
    }
    else
    {
        x = 47;   
        y = 16;
        fixed = true;
    } 
     
    if(viewsize == 21 && ingame && !hudmod) return;

    LatchNumber (x,y,1,gamestate.lives,fixed,lives);  
}


/*
===============
=
= GiveExtraMan
=
===============
*/

void GiveExtraMan (void)
{
    if (gamestate.lives<9)
        gamestate.lives++;
    DrawLives ();
    SD_PlaySound (BONUS1UPSND);
}

//===========================================================================

/*
===============
=
= DrawScore
=
===============
*/

void DrawScore (void)
{
    int x,y;
    boolean fixed;
    
    if(viewsize == 21 && ingame && !hudmod) return;
    
    if (mod.hud_x[score] >= 0 && mod.hud_y[score] >= 0)
    {
        if (mod.hud_x[score] == 1111 && mod.hud_y[score] == 1111)
            return;            
                    
        x = mod.hud_x[score];
        y = mod.hud_y[score];  
        fixed = false;              
    }
    {
        x = 7*2;
        y = 16; 
        fixed = true; 
    }
    
    LatchNumber (x,y,7,gamestate.score,fixed,score);  
}

/*
===============
=
= GivePoints
=
===============
*/

void GivePoints (int32_t points)
{
    gamestate.score += points;

        while (gamestate.score >= gamestate.nextextra) 
        {
                if (thirdencounter)
                   gamestate.nextextra += EXTRAPOINTS3RD;
                else
                   gamestate.nextextra += EXTRAPOINTS;   
                GiveExtraMan ();
        }
    DrawScore ();
}

//===========================================================================

/*
===============
=
= DrawItems
=
===============
*/

void    DrawItems (void)
{
    int x,y;
    boolean fixed;
    
    if (mod.hud_x[item] >= 0 && mod.hud_y[item] >= 0)
    {
        if (mod.hud_x[item] == 1111 && mod.hud_y[item] == 1111)
            return;            
                    
        x = mod.hud_x[item];
        y = mod.hud_y[item]; 
        fixed = false;
        
        if (gamestate.treasuretotal > 999)
            LatchNumber (x,y,4,gamestate.items,fixed,item); 
        else if (gamestate.treasuretotal > 99 && gamestate.treasuretotal < 1000)
            LatchNumber (x,y,3,gamestate.items,fixed,item);    
        else
            LatchNumber (x,y,2,gamestate.items,fixed,item);              
    }
    else
    {
        x = 16*2; 
        y = 16;
        fixed = true;
        
        LatchNumber (x,y,2,gamestate.items,fixed,item);
    }
}

/*
===============
=
= DrawItemsTotal
=
===============
*/

void DrawItemsTotal (void)
{
    int x,y;
    boolean fixed;
    
    if (mod.hud_x[itemstotal] >= 0 && mod.hud_y[itemstotal] >= 0)
    {
        if (mod.hud_x[itemstotal] == 1111 && mod.hud_y[itemstotal] == 1111)
            return;            
                    
        x = mod.hud_x[itemstotal];
        y = mod.hud_y[itemstotal]; 
        fixed = false;
        
        if (gamestate.treasuretotal > 999)
            LatchNumber (x,y,4,gamestate.treasuretotal,fixed,itemstotal); 
        else if (gamestate.treasuretotal > 99 && gamestate.treasuretotal < 1000)
            LatchNumber (x,y,3,gamestate.treasuretotal,fixed,itemstotal);    
        else
            LatchNumber (x,y,2,gamestate.treasuretotal,fixed,itemstotal);                
    }
}

/*
===============
=
= TallyItems     // APN: count treasures as items rather than score points  
=
===============
*/

void    TallyItems (void)
{
        if (gamestate.items == 50)
        {
                gamestate.items = 0;
                GiveExtraMan ();
        }
        
        DrawItems ();
        
        if (gamestate.treasurecount >= gamestate.treasuretotal && mod.mission == 2)
        {
            if (finalmap[gamestate.episode*10+mapon+1])
            {
                gamestate.victoryarea = MAPSPOT(player->tilex,player->tiley,0);             
                gamestate.victory = true; 
            }
            else
                playstate = ex_completed;
        } 
}

/*
===============
=
= DrawSecrets
=
===============
*/

void DrawSecrets (void)
{
    int x,y;
    boolean fixed;
    
    if (mod.hud_x[secrets] >= 0 && mod.hud_y[secrets] >= 0)
    {
        if (mod.hud_x[secrets] == 1111 && mod.hud_y[secrets] == 1111)
            return;            
                    
        x = mod.hud_x[secrets];
        y = mod.hud_y[secrets]; 
        fixed = false;
        
        if (gamestate.secretcount > 99)
            LatchNumber (x,y,3,gamestate.secretcount,fixed,secrets);
        else
            LatchNumber (x,y,2,gamestate.secretcount,fixed,secrets);               
    }
}

/*
===============
=
= DrawSecretsTotal
=
===============
*/

void DrawSecretsTotal (void)
{
    int x,y;
    boolean fixed;
    
    if (mod.hud_x[secretstotal] >= 0 && mod.hud_y[secretstotal] >= 0)
    {
        if (mod.hud_x[secretstotal] == 1111 && mod.hud_y[secretstotal] == 1111)
            return;            
                    
        x = mod.hud_x[secretstotal];
        y = mod.hud_y[secretstotal]; 
        fixed = false;
        
        if (gamestate.secrettotal > 99)
            LatchNumber (x,y,3,gamestate.secrettotal,fixed,secretstotal);
        else
            LatchNumber (x,y,2,gamestate.secrettotal,fixed,secretstotal);               
    }
}

/*
===============
=
= DrawKills
=
===============
*/

void DrawKills (void)
{
    int x,y;
    boolean fixed;
    
    if (mod.hud_x[kills] >= 0 && mod.hud_y[kills] >= 0)
    {
        if (mod.hud_x[kills] == 1111 && mod.hud_y[kills] == 1111)
            return;            
                    
        x = mod.hud_x[kills];
        y = mod.hud_y[kills]; 
        fixed = false;
        
        if (gamestate.killtotal > 999)
            LatchNumber (x,y,4,gamestate.killcount,fixed,kills); 
        else if (gamestate.killtotal > 99 && gamestate.killtotal < 1000)
            LatchNumber (x,y,3,gamestate.killcount,fixed,kills);    
        else
            LatchNumber (x,y,2,gamestate.killcount,fixed,kills);              
    }
}

/*
===============
=
= DrawEnemies
=
===============
*/

void DrawEnemies (void)
{
    int x,y;
    boolean fixed;
    
    if (mod.hud_x[enemies] >= 0 && mod.hud_y[enemies] >= 0)
    {
        if (mod.hud_x[enemies] == 1111 && mod.hud_y[enemies] == 1111)
            return;            
                    
        x = mod.hud_x[enemies];
        y = mod.hud_y[enemies]; 
        fixed = false;
        
        if (gamestate.killtotal > 999)
            LatchNumber (x,y,4,gamestate.killtotal,fixed,enemies);
        else if (gamestate.killtotal > 99 && gamestate.killtotal < 1000)
            LatchNumber (x,y,4,gamestate.killtotal,fixed,enemies);
        else
            LatchNumber (x,y,2,gamestate.killtotal,fixed,enemies);               
    }
}

/*
===============
=
= DrawTime
=
===============
*/

void DrawTime (void)
{
    int x,y;
    
    if (gamestate.victorycam) return;

    if (mod.hud_x[timer] >= 0 && mod.hud_y[timer] >= 0)
    {     
        fontnumber = 0;
        SETFONTCOLOR(15,127);
        PrintX = (screenWidth/100)*mod.hud_x[timer]; 
        PrintY = (screenHeight/100)*mod.hud_y[timer];
        
        if (gamestate.mapTime > 0)
        {
             if ((gamestate.mapTime-(gamestate.TimeCount/60))/60 < 10)
                 US_Print("0"); 
           
             US_PrintSigned((gamestate.mapTime-(gamestate.TimeCount/60))/60);
        
             US_Print(":"); 
        
             if ((gamestate.mapTime-(gamestate.TimeCount/60))%60 < 10)
                 US_Print("0"); 
           
             US_PrintSigned((gamestate.mapTime-(gamestate.TimeCount/60))%60);
        }
        else
        {
             if ((gamestate.TimeCount/60)/60 < 10)
                 US_Print("0"); 
           
             US_PrintSigned((gamestate.TimeCount/60)/60);
        
             US_Print(":"); 
        
             if ((gamestate.TimeCount/60)%60 < 10)
                 US_Print("0"); 
           
             US_PrintSigned((gamestate.TimeCount/60)%60);
        }
    }
    
      if (mod.mapTime && gamestate.mapTime-gamestate.TimeCount/60 <= 0)
      {
           if (mod.mapTime_end == 1) 
                playstate = ex_died;
           else if (mod.mapTime_end == 2)
                playstate = ex_completed;
           else if (mod.mapTime_end == 3)
           {
                gamestate.victoryarea = MAPSPOT(player->tilex,player->tiley,0);             
                gamestate.victory = true; 
           }
      }
}

/*
==================
=
= DrawWeapon
=
==================
*/

void DrawWeapon (void)
{
    //if(viewsize == 21 && ingame && !hudmod) return;
    //StatusDrawPic (32,8,KNIFEPIC+gamestate.weapon);
}

/*
==================
=
= DrawKeys
=
==================
*/

void DrawKeyGold (void)
{
    int x,y;
    boolean fixed;
    
    if (mod.hud_x[key_gold] >= 0 && mod.hud_y[key_gold] >= 0)
    {
        if (mod.hud_x[key_gold] == 1111 && mod.hud_y[key_gold] == 1111)
            return;            
                    
        x = mod.hud_x[key_gold];
        y = mod.hud_y[key_gold];  
        fixed = false;              
    }
    else
    {
        x = 77; 
        y = 4;
        fixed = true;
    } 
    
	if (gamestate.keys & 1)
        StatusDrawPic (x,y,GOLDKEYPIC,fixed,key_gold);
	//else
    //   StatusDrawPic (x,y,NOKEYPIC); 
}

void DrawKeySilver (void)
{
    int x,y;
    boolean fixed;
    
    if (mod.hud_x[key_silver] >= 0 && mod.hud_y[key_silver] >= 0)
    {
        if (mod.hud_x[key_silver] == 1111 && mod.hud_y[key_silver] == 1111)
            return;            
                    
        x = mod.hud_x[key_silver];
        y = mod.hud_y[key_silver];         
        fixed = false;       
    }
    else
    {
        x = 77; 
        y = 24;
        fixed = true;
    } 

	if (gamestate.keys & 2)
        StatusDrawPic (x,y,SILVERKEYPIC,fixed,key_silver);
	//else
    //    StatusDrawPic (77,24,NOKEYPIC);   
}

void DrawKeys (void)
{
    DrawKeyGold();
    DrawKeySilver();
}

/*
==================
=
= GiveWeapon
=
==================
*/

void GiveWeapon (int weapon)
{
        if (weapon != wp_mlauncher && weapon != wp_fthrower)
        {            
           switch (weapon)
           {
               case wp_machinegun: 
                   GiveAmmo (6); 
                   if ((!gamestate.gotweapon[wp_machinegun] && !gamestate.gotweapon[wp_chaingun] &&
                        gamestate.weapon != wp_fthrower && gamestate.weapon != wp_mlauncher) || 
                        (!gamestate.gotweapon[wp_machinegun] && gamestate.weapon <= wp_pistol))
                           gamestate.weapon = gamestate.chosenweapon = (weapontype) weapon;

                   gamestate.gotweapon[wp_machinegun] = true;
                   break;

               case wp_chaingun:
                   GiveAmmo (20);             // APN: chaingun gives 20 ammo
                   if (!gamestate.gotweapon[wp_chaingun] && gamestate.weapon != wp_fthrower && gamestate.weapon != wp_mlauncher)
                        gamestate.weapon = gamestate.chosenweapon = (weapontype) weapon;

                   gamestate.gotweapon[wp_chaingun] = true;
                   break;

           }
        }
        else if (weapon == wp_fthrower)
        {
             GiveGascan (20);        // APN: Flame Thrower gives 20 ammo
             if (!gamestate.gotweapon[wp_fthrower] && gamestate.weapon != wp_mlauncher) 
                 gamestate.weapon = gamestate.chosenweapon = (weapontype) weapon;
             
             gamestate.gotweapon[wp_fthrower] = true;     
        }
        else     // weapon == wp_mlauncher
        {
             GiveMissile (5);        // APN: Missile launcher gives 5 ammo    
             if (!gamestate.gotweapon[wp_mlauncher])
                 gamestate.weapon = gamestate.chosenweapon = (weapontype) weapon;
                 
             gamestate.gotweapon[wp_mlauncher] = true;      
        }

 
	if (gamestate.bestweapon<weapon)
		gamestate.bestweapon = gamestate.weapon = gamestate.chosenweapon = (weapontype) weapon;

}

/*void GiveWeapon (int weapon)
{
    GiveAmmo (6);

    if (gamestate.bestweapon<weapon)
        gamestate.bestweapon = gamestate.weapon
        = gamestate.chosenweapon = (weapontype) weapon;

    DrawWeapon ();
}*/

//===========================================================================

/*
===============
=
= DrawAmmo
=
===============
*/

void DrawAmmoGeneral (void)
{
    int x,y;
    int knifeammo;
    int digit = 3;
    boolean fixed;
    
    if (mod.hud_x[ammo_general] >= 0 && mod.hud_y[ammo_general] >= 0)
    {              
         if (mod.hud_x[ammo_general] == 1111 && mod.hud_y[ammo_general] == 1111)
             return;     
              
         x = mod.hud_x[ammo_general];
         y = mod.hud_y[ammo_general];     
         fixed = false; 
    }               
     
    if(viewsize == 21 && ingame && !hudmod) return;
    
        
    if ((mod.maxgas > 999 && mod.maxgas < 10000) || 
        (mod.maxrockets > 999 && mod.maxrockets < 10000) || 
        (mod.maxammo > 999 && mod.maxammo < 10000))  
        digit = 4;
        
    if ((mod.maxgas > 9999 && mod.maxgas < 100000) || 
        (mod.maxrockets > 9999 && mod.maxrockets < 100000) || 
        (mod.maxammo > 9999 && mod.maxammo < 100000))
        digit = 5;

        if (gamestate.weapon == wp_fthrower)
        {
            if (mod.hud_x[ammo_gas] >= 0 && mod.hud_y[ammo_gas] >= 0)
                return; 
                              
            if (mod.hud_x[ammo_general] >= 0 && mod.hud_y[ammo_general] >= 0)                                  
                 LatchNumber (x,y,digit,gamestate.gascan,fixed,ammo_general);         
            else                                
                 LatchNumber (70-digit,16,digit,gamestate.gascan,true,0); 
        }

        else if (gamestate.weapon == wp_mlauncher)
        {
            if (mod.hud_x[ammo_rockets] >= 0 && mod.hud_y[ammo_rockets] >= 0)
                return; 
                 
            if (mod.hud_x[ammo_general] >= 0 && mod.hud_y[ammo_general] >= 0)
                 LatchNumber (x,y,digit,gamestate.missile,fixed,ammo_general);              
            else  
                 LatchNumber (70-digit,16,digit,gamestate.missile,true,0);
        }        
        // APN: replacement code for Third Encounter and up
        else if (gamestate.weapon >= wp_pistol && gamestate.weapon <= wp_chaingun)
        {   
            if (mod.hud_x[ammo_bullets] >= 0 && mod.hud_y[ammo_bullets] >= 0)
                return; 
                
            if (mod.hud_x[ammo_general] >= 0 && mod.hud_y[ammo_general] >= 0)   
                 LatchNumber (x,y,digit,gamestate.ammo,fixed,ammo_general);              
            else
                 LatchNumber (70-digit,16,digit,gamestate.ammo,true,0); 
        }

        // APN: for Third Encounter and up
        else if (gamestate.weapon == wp_knife)
        {
            if (mod.hud_x[ammo_bullets] >= 0 && mod.hud_y[ammo_bullets] >= 0)
                return; 
                
            if (thirdencounter) 
                knifeammo = 0; 
            else 
                knifeammo = gamestate.ammo; 
                
                if (mod.hud_x[ammo_general] >= 0 && mod.hud_y[ammo_general] >= 0)
                     LatchNumber (x,y,digit,knifeammo,fixed,ammo_general);              
                else        
                     LatchNumber (70-digit,16,digit,knifeammo,true,0);
        }                           
}

void DrawAmmoGas (void)
{
    int x,y;
        
    if (mod.hud_x[ammo_gas] >= 0 && mod.hud_y[ammo_gas] >= 0) // only draw if deffed
    {              
         if (mod.hud_x[ammo_gas] == 1111 && mod.hud_y[ammo_gas] == 1111)
             return;     
              
         x = mod.hud_x[ammo_gas];
         y = mod.hud_y[ammo_gas];    
         
         if (mod.maxgas && mod.maxgas > 999 && mod.maxgas < 10000)
             LatchNumber (x,y,4,gamestate.gascan,false,ammo_gas); 
         else if (mod.maxgas && mod.maxgas > 9999 && mod.maxgas < 100000) 
             LatchNumber (x,y,5,gamestate.gascan,false,ammo_gas);     
         else                
             LatchNumber (x,y,3,gamestate.gascan,false,ammo_gas);   
    }             
}

void DrawAmmoRockets (void)
{
    int x,y;
        
    if (mod.hud_x[ammo_rockets] >= 0 && mod.hud_y[ammo_rockets] >= 0) // only draw if deaf
    {              
         if (mod.hud_x[ammo_rockets] == 1111 && mod.hud_y[ammo_rockets] == 1111)
             return;     
              
         x = mod.hud_x[ammo_rockets];
         y = mod.hud_y[ammo_rockets];    
         
         if (mod.maxgas && mod.maxrockets > 999 && mod.maxrockets < 10000)
             LatchNumber (x,y,4,gamestate.missile,false,ammo_rockets); 
         else if (mod.maxgas && mod.maxrockets > 9999 && mod.maxrockets < 100000) 
             LatchNumber (x,y,5,gamestate.missile,false,ammo_rockets);     
         else                
             LatchNumber (x,y,3,gamestate.missile,false,ammo_rockets);   
    }             
}

void DrawAmmoBullets (void)
{
    int x,y;
        
    if (mod.hud_x[ammo_bullets] >= 0 && mod.hud_y[ammo_bullets] >= 0) // only draw if defeated
    {              
         if (mod.hud_x[ammo_bullets] == 1111 && mod.hud_y[ammo_bullets] == 1111)
             return;     
              
         x = mod.hud_x[ammo_bullets];
         y = mod.hud_y[ammo_bullets];    
         
         if (mod.maxgas && mod.maxammo > 999 && mod.maxammo < 10000)
             LatchNumber (x,y,4,gamestate.ammo,false,ammo_bullets); 
         else if (mod.maxgas && mod.maxammo > 9999 && mod.maxammo < 100000) 
             LatchNumber (x,y,5,gamestate.ammo,false,ammo_bullets);     
         else                
             LatchNumber (x,y,3,gamestate.ammo,false,ammo_bullets);   
    }             
}


void DrawAmmo (void)
{
        
    if (gamestate.ammo < 0) 
        gamestate.ammo = 0;    
    else if (gamestate.gascan < 0) 
        gamestate.gascan = 0;       
    else if (gamestate.missile < 0) 
        gamestate.missile = 0;   
     
    // draw all ammo at same location 
    DrawAmmoGeneral();   
    
    // draw them hell knows where if deathined
    DrawAmmoBullets();
    DrawAmmoRockets();
    DrawAmmoGas();
}

/*
===============
=
= GiveGascan    
=
===============
*/

void    GiveGascan (int gascan)
{
        gamestate.gascan += gascan;

        if (gamestate.gascan > 99 && !mod.maxgas)
            gamestate.gascan = 99;  
        else if (gamestate.gascan > mod.maxgas && mod.maxgas)
            gamestate.gascan = mod.maxgas;      

        DrawAmmo();
}

/*
===============
=
= GiveMissile   
=
===============
*/

void    GiveMissile (int missile)
{
        gamestate.missile += missile;

        if (!mod.maxrockets)
        {
            if (gamestate.missile > 99)              
                gamestate.missile = 99; 
        }
        else if (mod.maxrockets)
        {
            if (gamestate.missile > mod.maxrockets) 
                gamestate.missile = mod.maxrockets;      
        }

        DrawAmmo();
}

/*
===============
=
= GiveAmmo
=
= behold, the madness of ifs
===============
*/

void GiveAmmo (int ammo)
{
    if (!gamestate.ammo)                            // knife was out
    {
        if (!gamestate.attackframe)
        {
            gamestate.weapon = gamestate.chosenweapon;
            DrawWeapon ();
        }
    }
    gamestate.ammo += ammo;
    
    if (gamestate.ammo > gamestate.maxammo && !mod.maxammo) 
		gamestate.ammo = gamestate.maxammo;	
    else if (gamestate.ammo > mod.maxammo && mod.maxammo) 
		gamestate.ammo = mod.maxammo;	
    /*    if (gamestate.backpack == 1)              // Joe: adding a backpack
        {                                         // APN: Mac wolf backpack
                 if (gamestate.ammo > 199)        // behavior
                 gamestate.ammo = 199;
        }
        else if (gamestate.backpack == 2)
        {
                 if (gamestate.ammo > 299)
                 gamestate.ammo = 299;
        }
        else if (gamestate.backpack == 999)
        {
                 if (gamestate.ammo > 999)
                 gamestate.ammo = 999;
        }
        else
        {
                 if (gamestate.ammo > 99)
                 gamestate.ammo = 99;
        }
    */   
    DrawAmmo ();
}

//===========================================================================

/*
==================
=
= GiveKey
=
==================
*/

void GiveKey (int key)
{
    gamestate.keys |= (1<<key);
    DrawKeys ();
}

/*
==================
=
= GiveItem
=
==================
*/

void GiveItem (int type)
{   
    	if (mod.item[type][item_damage])
		    TakeDamage (mod.item[type][item_damage],player);
		    
	    if (mod.item[type][item_health])
            HealSelf (mod.item[type][item_health]);
		        
	    if (mod.item[type][item_ammo])  
		    GiveAmmo(mod.item[type][item_ammo]);
		        
	    if (mod.item[type][item_rockets])   
		    GiveMissile (mod.item[type][item_rockets]);
		        
	    if (mod.item[type][item_gas])   
		    GiveGascan (mod.item[type][item_gas]);    
		    
	    if (mod.item[type][item_time])   
		    gamestate.mapTime += mod.item[type][item_time];    
}

/*
==================
=
= ItemIgnore (mods only)
=
==================
*/

boolean ItemIgnore (int type)
{
      if (mod.item_ignore[type] == item_ammo && 
         ((gamestate.ammo == mod.maxammo && mod.maxammo) || (gamestate.ammo == gamestate.maxammo && !mod.maxammo)))
          return true;   
                                    
      if (mod.item_ignore[type] == item_rockets &&
         ((gamestate.missile == mod.maxrockets && mod.maxrockets) || (gamestate.missile == 99 && !mod.maxrockets)))
          return true;  
                           
      if (mod.item_ignore[type] == item_gas &&
         ((gamestate.gascan == mod.maxgas && mod.maxgas) || (gamestate.gascan == 99 && !mod.maxgas)))
         return true; 
                          
      if (mod.item_ignore[type] == item_health &&
         ((gamestate.health == mod.maxhealth && mod.maxhealth) || (gamestate.health == 100 && !mod.maxhealth)))
         return true; 
      
      return false;    
}

/*
===================
=
= GetBonus
=
===================
*/
void GetBonus (statobj_t *check)
{
    if (MAPSPOT (check->tilex,check->tiley,1) == mod.missionitem && mod.mission == 3)
    {
  
        if (finalmap[gamestate.episode*10+mapon+1])
        {
            gamestate.victoryarea = MAPSPOT(player->tilex,player->tiley,0);             
            gamestate.victory = true; 
        }
        else
            playstate = ex_completed;
    } 
     
     
	switch (check->itemnumber)
	{
	case	bo_firstaid:
        if (mod.item[check->itemnumber][item_health] || mod.item[check->itemnumber][item_damage] || 
            mod.item[check->itemnumber][item_ammo] || mod.item[check->itemnumber][item_gas] || 
            mod.item[check->itemnumber][item_rockets])
        {   
            if (ItemIgnore (check->itemnumber)) return;
            
		    GiveItem (check->itemnumber);                                                                    
        } 
        else
        { 
		    if (gamestate.health == 100 && !mod.maxhealth)
			    return;
			
            if (gamestate.health == mod.maxhealth && mod.maxhealth)
			    return;
			    
		    HealSelf (25);
        }
        
        SD_PlaySound (HEALTH2SND);
		break;

	case	bo_key1:
	case	bo_key2:
	case	bo_key3:
	case	bo_key4:
		GiveKey (check->itemnumber - bo_key1);
		SD_PlaySound (GETKEYSND);
		break;

	case	bo_cross:
		SD_PlaySound (BONUS1SND);
        //GivePoints (100);  
        if (mod.item[check->itemnumber][item_health] || mod.item[check->itemnumber][item_damage] || 
            mod.item[check->itemnumber][item_ammo] || mod.item[check->itemnumber][item_gas] || 
            mod.item[check->itemnumber][item_rockets])
        {    
            if (ItemIgnore (check->itemnumber)) return;
               		
		    GiveItem (check->itemnumber);                                                                    
        } 
		gamestate.treasurecount++;
        gamestate.items++;    
        TallyItems();          
		break;
		
	case	bo_chalice:
		SD_PlaySound (BONUS2SND);
        //GivePoints (500); 
        if (mod.item[check->itemnumber][item_health] || mod.item[check->itemnumber][item_damage] || 
            mod.item[check->itemnumber][item_ammo] || mod.item[check->itemnumber][item_gas] || 
            mod.item[check->itemnumber][item_rockets])
        {   
            if (ItemIgnore (check->itemnumber)) return;
              		
		    GiveItem (check->itemnumber);                                                                    
        }    
		gamestate.treasurecount++;
        gamestate.items++;  
        TallyItems(); 
		break;
	case	bo_bible:
		SD_PlaySound (BONUS3SND);
        //GivePoints (1000);  
        if (mod.item[check->itemnumber][item_health] || mod.item[check->itemnumber][item_damage] || 
            mod.item[check->itemnumber][item_ammo] || mod.item[check->itemnumber][item_gas] || 
            mod.item[check->itemnumber][item_rockets])
        {   
            if (ItemIgnore (check->itemnumber)) return;
              		
		    GiveItem (check->itemnumber);                                                                    
        }  
		gamestate.treasurecount++;
        gamestate.items++;        
        TallyItems();    
		break;
		
	case	bo_crown:
		SD_PlaySound (BONUS4SND);
        //GivePoints (5000);   
        if (mod.item[check->itemnumber][item_health] || mod.item[check->itemnumber][item_damage] || 
            mod.item[check->itemnumber][item_ammo] || mod.item[check->itemnumber][item_gas] || 
            mod.item[check->itemnumber][item_rockets])
        {     
            if (ItemIgnore (check->itemnumber)) return; 
              		
		    GiveItem (check->itemnumber);                                                                    
        }  
		gamestate.treasurecount++;
        gamestate.items++;        
        TallyItems();             
		break;

        case    bo_clip: 
        case    bo_clip2:         
        if (mod.item[check->itemnumber][item_health] || mod.item[check->itemnumber][item_damage] || 
            mod.item[check->itemnumber][item_ammo] || mod.item[check->itemnumber][item_gas] || 
            mod.item[check->itemnumber][item_rockets])
        {   
            if (ItemIgnore (check->itemnumber)) return;
                            		
		    GiveItem (check->itemnumber);                                                                    
        } 
        else
        {
            if (!godmode && gamestate.ammo == gamestate.maxammo && !mod.maxammo) return;
            if (!godmode && gamestate.ammo == mod.maxammo && mod.maxammo) return;
                
            GiveAmmo (5);
        }    
        
        SD_PlaySound (GETAMMOSND);       
		break;

        case    bo_25clip:
        if (mod.item[check->itemnumber][item_health] || mod.item[check->itemnumber][item_damage] || 
            mod.item[check->itemnumber][item_ammo] || mod.item[check->itemnumber][item_gas] || 
            mod.item[check->itemnumber][item_rockets])
        {    
            if (ItemIgnore (check->itemnumber)) return;
              		
		    GiveItem (check->itemnumber);                                                                    
        } 
        else          
        {             
            if (!godmode && gamestate.ammo == gamestate.maxammo && !mod.maxammo) return;
            if (!godmode && gamestate.ammo == mod.maxammo && mod.maxammo) return;
             
		    GiveAmmo (25);
        }
        
        SD_PlaySound (GETAMMOSND);  
		break;

	case	bo_machinegun:			
		if (!godmode && gamestate.gotweapon[wp_machinegun] && gamestate.ammo == gamestate.maxammo && !mod.maxammo) return;	
		if (!godmode && gamestate.gotweapon[wp_chaingun] && gamestate.ammo == mod.maxammo && mod.maxammo) return;

		SD_PlaySound (GETMACHINESND);
		GiveWeapon (wp_machinegun);
		break;
		
	case	bo_chaingun:
		SD_PlaySound (GETGATLINGSND);
		GiveWeapon (wp_chaingun);

        bjface = FACEFRAME4;
		facecount = 120;
		break;

	case	bo_fullheal:
		SD_PlaySound (BONUS1UPSND);
		
		if (mod.item[check->itemnumber][item_health] || mod.item[check->itemnumber][item_damage] || 
            mod.item[check->itemnumber][item_ammo] || mod.item[check->itemnumber][item_gas] || 
            mod.item[check->itemnumber][item_rockets])
        {      
            if (ItemIgnore (check->itemnumber)) return; 
                            		
		    GiveItem (check->itemnumber);                                                                    
        } 
        else
		    HealSelf (99);
        //GiveAmmo (25);       
		GiveExtraMan ();
		gamestate.treasurecount++;
		break;

	case	bo_food:
        if (mod.item[check->itemnumber][item_health] || mod.item[check->itemnumber][item_damage] || 
            mod.item[check->itemnumber][item_ammo] || mod.item[check->itemnumber][item_gas] || 
            mod.item[check->itemnumber][item_rockets])
        {  
            if (ItemIgnore (check->itemnumber)) return;
            		
		    GiveItem (check->itemnumber);                                                                    
        } 
        else
        { 
		    if (gamestate.health == 100 && !mod.maxhealth)
			    return;
			
            if (gamestate.health == mod.maxhealth && mod.maxhealth)
			    return;
			    
		    HealSelf (10);
        }
        
        SD_PlaySound (HEALTH1SND);
		break;

	case	bo_alpo:
        if (mod.item[check->itemnumber][item_health] || mod.item[check->itemnumber][item_damage] || 
            mod.item[check->itemnumber][item_ammo] || mod.item[check->itemnumber][item_gas] || 
            mod.item[check->itemnumber][item_rockets])
        {     
            if (ItemIgnore (check->itemnumber)) return; 
            		
		    GiveItem (check->itemnumber);                                                                    
        } 
        else
        { 
		    if (gamestate.health == 100 && !mod.maxhealth)
			    return;
			
            if (gamestate.health == mod.maxhealth && mod.maxhealth)
			    return;

		    HealSelf (4);
        }
        
        SD_PlaySound (HEALTH1SND);
		break;

        case    bo_backpack:                 
                if (gamestate.maxammo < 299 && !mod.maxammo) 
				    gamestate.maxammo += 100;
				    
				else if (mod.maxammo < mod.maxbagammo && mod.maxammo) 
				    mod.maxammo += mod.bagammo;    
                /*if (gamestate.ammo == 299 && gamestate.gascan == 99 && gamestate.missile == 99)
                        return;

                if (gamestate.backpack < 2)
                        gamestate.backpack++;*/

                SD_PlaySound (BONUS1SND);
                GiveAmmo (20);
                GiveMissile (5); 
                GiveGascan (2);     
                break;

        case    bo_spear:        
                SD_PlaySound (GETGATLINGSND);
                GiveWeapon (wp_mlauncher);
                bjface = FACEFRAME4;
                facecount = 120;
                break;

        case    bo_rockets:   
                if (mod.item[check->itemnumber][item_health] || mod.item[check->itemnumber][item_damage] || 
                    mod.item[check->itemnumber][item_ammo] || mod.item[check->itemnumber][item_gas] || 
                    mod.item[check->itemnumber][item_rockets])
                {     	
                       if (ItemIgnore (check->itemnumber)) return;
                       
		                GiveItem (check->itemnumber);                                                                    
                } 
                else
                {      
                    if (gamestate.missile == 99 && !mod.maxrockets)
			            return;
			       
                    if (gamestate.missile == mod.maxrockets && mod.maxrockets)
			            return;
			            
                   GiveMissile (5);
                }   
                
                SD_PlaySound (BONUS1SND);   
                break;

        case    bo_gibs:          
                SD_PlaySound (GETGATLINGSND);
                GiveWeapon (wp_fthrower);
                bjface = FACEFRAME4;
                facecount = 120;
                break;

        case    bo_gascan: 
                if (mod.item[check->itemnumber][item_health] || mod.item[check->itemnumber][item_damage] || 
                    mod.item[check->itemnumber][item_ammo] || mod.item[check->itemnumber][item_gas] || 
                    mod.item[check->itemnumber][item_rockets])
                {     
                      if (ItemIgnore (check->itemnumber)) return;
                      		
		              GiveItem (check->itemnumber);                                                                    
                } 
                else  
                {  
                      if (gamestate.gascan == 99 && !mod.maxgas)
			             return;
			       
                      if (gamestate.gascan == mod.maxgas && mod.maxgas)
			             return;

                      GiveGascan (14);
                }
                
                 SD_PlaySound (BONUS1SND);
                break;
	}

    //StartBonusFlash ();           
	check->shapenum = -1;
}

/*
=============================================================================

                                MOVEMENT

=============================================================================
*/

/*
===================
=
= TryMove
=
= returns true if move ok
= debug: use pointers to optimize
===================
*/

boolean TryMove (objtype *ob)
{
    int         xl,yl,xh,yh,x,y; 
    objtype    *check;
    int32_t     deltax,deltay;

    xl = (ob->x-PLAYERSIZE) >>TILESHIFT;
    yl = (ob->y-PLAYERSIZE) >>TILESHIFT;

    xh = (ob->x+PLAYERSIZE) >>TILESHIFT;
    yh = (ob->y+PLAYERSIZE) >>TILESHIFT;

#define PUSHWALLMINDIST PLAYERSIZE

    //
    // check for solid walls
    //
    for (y=yl;y<=yh;y++)
    {
        for (x=xl;x<=xh;x++)
        {
            check = actorat[x][y];
            if (check && !ISPOINTER(check))
            {
                if(tilemap[x][y]==BIT_WALL && x==pwallx && y==pwally)   // back of moving pushwall?
                {
                    switch(pwalldir)
                    {
                        case di_north:
                            if(ob->y-PUSHWALLMINDIST<=(pwally<<TILESHIFT)+((63-pwallpos)<<10))
                                return false;
                            break;
                        case di_west:
                            if(ob->x-PUSHWALLMINDIST<=(pwallx<<TILESHIFT)+((63-pwallpos)<<10))
                                return false;
                            break;
                        case di_east:
                            if(ob->x+PUSHWALLMINDIST>=(pwallx<<TILESHIFT)+(pwallpos<<10))
                                return false;
                            break;
                        case di_south:
                            if(ob->y+PUSHWALLMINDIST>=(pwally<<TILESHIFT)+(pwallpos<<10))
                                return false;
                            break;
                    }
                }
                else 
                    return false;
            }
        }
    }

    //
    // check for actors
    //
    if (yl>0)
        yl--;
    if (yh<MAPSIZE-1)
        yh++;
    if (xl>0)
        xl--;
    if (xh<MAPSIZE-1)
        xh++;
 
    for (y=yl;y<=yh;y++)
    {
        for (x=xl;x<=xh;x++)
        {
            check = actorat[x][y];
            if (ISPOINTER(check) && check != player && (check->flags & FL_SHOOTABLE) )
            {
                deltax = ob->x - check->x;
                if (deltax < -MINACTORDIST || deltax > MINACTORDIST)
                    continue;
                deltay = ob->y - check->y;
                if (deltay < -MINACTORDIST || deltay > MINACTORDIST)
                    continue;

                return false;
            }
        }
    }

    return true;
}


/*
===================
=
= ClipMove
=
===================
*/

void ClipMove (objtype *ob, int32_t xmove, int32_t ymove)
{
    int32_t    basex,basey;

    basex = ob->x;
    basey = ob->y;

    ob->x = basex+xmove;
    ob->y = basey+ymove;
    if (TryMove (ob))
        return;

#ifndef REMDEBUG
    if (noclip && ob->x > 2*TILEGLOBAL && ob->y > 2*TILEGLOBAL
        && ob->x < (((int32_t)(mapwidth-1))<<TILESHIFT)
        && ob->y < (((int32_t)(mapheight-1))<<TILESHIFT) )
        return;         // walk through walls
#endif

    // wall goes brrrrrrr 
    //if (!SD_SoundPlaying())
    //    SD_PlaySound (HITWALLSND);

    ob->x = basex+xmove;
    ob->y = basey;
    if (TryMove (ob))
        return;

    ob->x = basex;
    ob->y = basey+ymove;
    if (TryMove (ob))
        return;

    ob->x = basex;
    ob->y = basey;
}

//==========================================================================

/*
===================
=
= VictoryTile
=
===================
*/

void VictoryTile (void)
{
#ifndef SPEAR
    SpawnBJVictory ();
#endif

    gamestate.victoryflag = true;
}

/*
===================
=
= Thrust
=
===================
*/

// For player movement in demos exactly as in the original Wolf3D v1.4 source code
static fixed FixedByFracOrig(fixed a, fixed b)
{
    int sign = 0;
    if(b == 65536) b = 65535;
    else if(b == -65536) b = 65535, sign = 1;
    else if(b < 0) b = (-b), sign = 1;

    if(a < 0)
    {
        a = -a;
        sign = !sign;
    }
    fixed res = (fixed)(((int64_t) a * b) >> 16);
    if(sign)
        res = -res;
    return res;
}

void Thrust (int angle, int32_t speed)
{
    int32_t xmove,ymove;
    unsigned offset;
  
    //
    // ZERO FUNNY COUNTER IF MOVED!
    //
#ifdef SPEAR
    if (speed)
        funnyticount = 0;
#endif
       
    thrustspeed += speed;
    
    //
    // cap the player speed
    //
    if (player_macstyle)
    {
        if (speed >= TILEGLOBAL)
            speed = TILEGLOBAL-256;         // -1/256 | TILEGLOBAL-1;
    }
    else
    {
        if (speed >= MINDIST*2)
            speed = MINDIST*2-1;
    }

    xmove = DEMOCHOOSE_ORIG_SDL(
                FixedByFracOrig(speed, costable[angle]),
                FixedMul(speed,costable[angle]));
    ymove = DEMOCHOOSE_ORIG_SDL(
                -FixedByFracOrig(speed, sintable[angle]),
                -FixedMul(speed,sintable[angle]));

    ClipMove(player,xmove,ymove);

    player->tilex = (short)(player->x >> TILESHIFT);                // scale to tile values
    player->tiley = (short)(player->y >> TILESHIFT);

    offset = (player->tiley<<mapshift)+player->tilex;
    player->areanumber = *(mapsegs[0] + offset) -AREATILE;

    if (*(mapsegs[1] + offset) == EXITTILE)
        VictoryTile ();
    
    // player starting on a wall exploit?? better get the hell outta there!    
    if (tilemap[starttilex][starttiley] && buttonstate[bt_run] &&
        player->tilex == starttilex && player->tiley == starttiley)             
        actorat[starttilex][starttiley] = NULL;
    // solidify the starting wall    
    else if (tilemap[starttilex][starttiley] && actorat[starttilex][starttiley] == NULL)  
    {
        int     dx,dy,dist;
        dx = abs(starttilex - player->tilex);
        dy = abs(starttiley - player->tiley);
        dist = dx>dy ? dx : dy;
        
        if (dist >= 2)    
           actorat[starttilex][starttiley] = (objtype *)(uintptr_t) tilemap[starttilex][starttiley];
    }     
}


/*
=============================================================================

                                ACTIONS

=============================================================================
*/

//===========================================================================

//
// some tile flip-flopping 
//
void PushCheck (int checkx, int checky, int dir, int tile)
{
    int flippy; 
    //  Check if original pushwall pos is set to darker/flipped
    //  Check if tiles behind are floor tiles
    //  Mark the new tiles
    
    if (tile != 999 && darkmap[checkx][checky] == tile)
    {
            if (dir == di_south && MAPSPOT(checkx,checky+1,0)>105) 
                darkmap[checkx][checky+1] = tile;
                
            if (dir == di_south && MAPSPOT(checkx,checky+2,0)>105) 
                darkmap[checkx][checky+2] = tile;    
                
            if (dir == di_north && MAPSPOT(checkx,checky-1,0)>105) 
                darkmap[checkx][checky-1] = tile;
                
            if (dir == di_north && MAPSPOT(checkx,checky-2,0)>105) 
                darkmap[checkx][checky-2] = tile;     
                
            if (dir == di_west && MAPSPOT(checkx-1,checky,0)>105) 
                darkmap[checkx-1][checky] = tile;
                
            if (dir == di_west && MAPSPOT(checkx-2,checky,0)>105) 
                darkmap[checkx-2][checky] = tile; 
                
            if (dir == di_east && MAPSPOT(checkx+1,checky,0)>105) 
                darkmap[checkx+1][checky] = tile;
                
            if (dir == di_east && MAPSPOT(checkx+2,checky,0)>105) 
                darkmap[checkx+2][checky] = tile;         
    } 
    /*if (tile != 999 && MAPSPOT(checkx,checky,2) == tile)
    {
            if (dir == di_south && MAPSPOT(checkx,checky+1,0)>105) 
                MAPSPOT(checkx,checky+1,2) = tile;
                
            if (dir == di_south && MAPSPOT(checkx,checky+2,0)>105) 
                MAPSPOT(checkx,checky+2,2) = tile;    
                
            if (dir == di_north && MAPSPOT(checkx,checky-1,0)>105) 
                MAPSPOT(checkx,checky-1,2) = tile;
                
            if (dir == di_north && MAPSPOT(checkx,checky-2,0)>105) 
                MAPSPOT(checkx,checky-2,2) = tile;     
                
            if (dir == di_west && MAPSPOT(checkx-1,checky,0)>105) 
                MAPSPOT(checkx-1,checky,2) = tile;
                
            if (dir == di_west && MAPSPOT(checkx-2,checky,0)>105) 
                MAPSPOT(checkx-2,checky,2) = tile; 
                
            if (dir == di_east && MAPSPOT(checkx+1,checky,0)>105) 
                MAPSPOT(checkx+1,checky,2) = tile;
                
            if (dir == di_east && MAPSPOT(checkx+2,checky,0)>105) 
                MAPSPOT(checkx+2,checky,2) = tile;         
    } */
    // FIX: non-flipped push can get overwritten if a flipped pushwall gets written, flipping the non-flipped
    // MAPSPOT = floor tile
/*    else if (tile == 999 && mirwall[MAPSPOT(checkx,checky,0)] > 0)
    {
         if (mirwall[MAPSPOT(checkx,checky,0)] == 1) flippy = 1; 
         else if (mirwall[MAPSPOT(checkx,checky,0)] == 2) flippy = 2;
         else if (mirwall[MAPSPOT(checkx,checky,0)] == 3) flippy = 3;
         
            if (dir == di_south && MAPSPOT(checkx,checky+1,0)>105) 
                mirwall[MAPSPOT(checkx,checky+1,0)] = flippy;
                
            if (dir == di_south && MAPSPOT(checkx,checky+2,0)>105) 
                mirwall[MAPSPOT(checkx,checky+2,0)] = flippy;    
                
            if (dir == di_north && MAPSPOT(checkx,checky-1,0)>105) 
                mirwall[MAPSPOT(checkx,checky-1,0)] = flippy;
                
            if (dir == di_north && MAPSPOT(checkx,checky-2,0)>105) 
                mirwall[MAPSPOT(checkx,checky-2,0)] = flippy;     
                
            if (dir == di_west && MAPSPOT(checkx-1,checky,0)>105) 
                mirwall[MAPSPOT(checkx-1,checky,0)] = flippy;
                
            if (dir == di_west && MAPSPOT(checkx-2,checky,0)>105) 
                mirwall[MAPSPOT(checkx-2,checky,0)] = flippy; 
                
            if (dir == di_east && MAPSPOT(checkx+1,checky,0)>105) 
                mirwall[MAPSPOT(checkx+1,checky,0)] = flippy;
                
            if (dir == di_east && MAPSPOT(checkx+2,checky,0)>105) 
                mirwall[MAPSPOT(checkx+2,checky,0)] = flippy;    
    }*/
}

/*
===============
=
= Cmd_Use
=
===============
*/

void Cmd_Use (void)
{
    int     checkx,checky,doornum,dir,i;
    boolean elevatorok;

    //
    // find which cardinal direction the player is facing
    //
    if (player->angle < ANGLES/8 || player->angle > 7*ANGLES/8)
    {
        checkx = player->tilex + 1;
        checky = player->tiley;
        dir = di_east;
        elevatorok = true;
    }
    else if (player->angle < 3*ANGLES/8)
    {
        checkx = player->tilex;
        checky = player->tiley-1;
        dir = di_north;
        elevatorok = false;
    }
    else if (player->angle < 5*ANGLES/8)
    {
        checkx = player->tilex - 1;
        checky = player->tiley;
        dir = di_west;
        elevatorok = true;
    }
    else
    {
        checkx = player->tilex;
        checky = player->tiley + 1;
        dir = di_south;
        elevatorok = false;
    }

    doornum = tilemap[checkx][checky];
    if (!buttonheld[bt_use] && *(mapsegs[1]+(checky<<mapshift)+checkx) == PUSHABLETILE)
    {
        //
        // pushable wall
        //
        buttonheld[bt_use] = true;
        
        PushCheck (checkx,checky,dir,3);
        PushCheck (checkx,checky,dir,6);
        PushCheck (checkx,checky,dir,7);
        PushCheck (checkx,checky,dir,999);
        PushWall (checkx,checky,dir);
        // mac pwalls destination is set to immediately blockmove/blocksight at push
        return;
    }
    
    //if (!buttonheld[bt_use] && doornum == ELEVATORTILE)// & elevatorok)
    //{
        //
        // use elevator
        //
        //buttonheld[bt_use] = true;
        //tilemap[checkx][checky]++;              // flip switch
        
        //
        // SECRET ELEVATOR
        //
        //if (*(mapsegs[0]+(player->tiley<<mapshift)+player->tilex) == ALTELEVATORTILE)
        if (!buttonheld[bt_use] && MAPSPOT(checkx,checky,1) == ALTELEVATOROBJ)
        {
            buttonheld[bt_use] = true;
                                     
            if (!secretlevel[gamestate.episode*10+mapon+1])  // if no secret value set, end game
            {  
                  gamestate.victoryarea = MAPSPOT(player->tilex,player->tiley,0); // for victory cam
                  gamestate.victory = true;                                                                                            
            } 
            else                        
               playstate = ex_secretlevel;
               
            SD_PlaySound (LEVELDONESND);
            SD_WaitSoundDone();
        }
        //
        // NORMAL ELEVATOR
        //
        else if (!buttonheld[bt_use] && MAPSPOT(checkx,checky,1) == ELEVATOROBJ)
        {
            buttonheld[bt_use] = true; 
            
            //if (gamestate.mapon == epend[gamestate.chapter+1]-1)
            /*for (i = 0; i < MAXLEVELS; i++)
            {
                if (secretlevel[i] == gamestate.mapon+2) 
                    break;
            }*/
            
                // final map, no secret level after
                if (finalmap[gamestate.episode*10+mapon+1])// && secretlevel[i] != gamestate.mapon+2)
                {
                   gamestate.victoryarea = MAPSPOT(player->tilex,player->tiley,0); // for victory cam                
                   gamestate.victory = true;   // starts deathcam in wl_play.cpp
                }
                // final map, secret level after
                //else if (finalmap[gamestate.episode*10+mapon+1] && secretlevel[i] == gamestate.mapon+2)
                //{
                //   //playstate = ex_secretlevel; 
                //   playstate = ex_completed;         
                //}
                else
                   playstate = ex_completed;
                
            SD_PlaySound (LEVELDONESND);
            SD_WaitSoundDone();    
        } 
    //}
    else if (!buttonheld[bt_use] && doornum & BIT_DOOR)
    {
        buttonheld[bt_use] = true;
        OperateDoor (doornum & ~BIT_DOOR);
    }
    
    else if (!buttonheld[bt_use] && ~doornum & BIT_DOOR && tilemap[checkx][checky]<64 && tilemap[checkx][checky]>0)
    {
        buttonheld[bt_use] = true; 
        SD_PlaySound (DONOTHINGSND);
    }
}

/*
=============================================================================

                                PLAYER CONTROL

=============================================================================
*/



/*
===============
=
= SpawnPlayer
=
===============
*/

void SpawnPlayer (int tilex, int tiley, int dir)
{
    player->obclass = playerobj;
    player->active = ac_yes;
    player->tilex = tilex;
    player->tiley = tiley;
    player->areanumber = (byte) *(mapsegs[0]+(player->tiley<<mapshift)+player->tilex);
    player->x = ((int32_t)tilex<<TILESHIFT)+TILEGLOBAL/2;
    player->y = ((int32_t)tiley<<TILESHIFT)+TILEGLOBAL/2;
    player->state = &s_player;
    player->angle = (1-dir)*90;
    if (player->angle<0)
        player->angle += ANGLES;
    player->flags = FL_NEVERMARK;
    Thrust (0,0);                           // set some variables

    InitAreas ();
 
    starttilex = tilex;
    starttiley = tiley;
}


//===========================================================================

/*
===============
=
= SpawnProjectile
=
===============
*/

void SpawnProjectile (objtype *ob, int which) 
{ 
    int maxcount,count,angle; 
     
    if ((gamestate.weapon == wp_mlauncher && gamestate.missile < 1) ||
        (gamestate.weapon == wp_fthrower && gamestate.gascan < 1) ||
        (gamestate.weapon >= wp_pistol && gamestate.weapon <= wp_chaingun && gamestate.ammo < 1)) 
    { 
       SD_PlaySound (DONOTHINGSND);
       return;
    } 
    
    switch (gamestate.weapon)
    {
            case wp_knife:   
                SD_PlaySound (ATKKNIFESND); 
            break;    
              
            case wp_pistol:
                SD_PlaySound (ATKPISTOLSND);
            break;
            
            case wp_machinegun:
                SD_PlaySound (ATKMACHINEGUNSND);
            break;
            
            case wp_chaingun:
                SD_PlaySound (ATKGATLINGSND);
            break;
            
            case wp_fthrower:
               SD_PlaySound (FLAMETHROWERSND); 
            break;   
            
            case wp_mlauncher:
               SD_PlaySound (MISSILEFIRESND); 
            break;
    } 


    if (mod.wp_prjamount[gamestate.weapon])
        maxcount = mod.wp_prjamount[gamestate.weapon];
    else
         maxcount = 1;

    for (count = 0; count < maxcount; count++)
    {
        GetNewActor (); 

        if (which == pl_miss)
        {
            newobj->state = &s_procket; 
            newobj->obclass = rocketobj;
        }
        else if (which == pl_fire)
        {
            newobj->state = &s_flame; 
            newobj->obclass = pl_fireobj;
        }
        else if (which == pl_need)
        {
            newobj->state = &s_needle1; 
            newobj->obclass = pl_needleobj;
        }
        else if (which == pl_ffir)
        {
            newobj->state = &s_fire1; 
            newobj->obclass = pl_fakefireobj;
        }
        
        newobj->ticcount = 1; 
        newobj->x = ob->x ; 
        newobj->y = ob->y ; 
        newobj->tilex = newobj->x >> TILESHIFT; 
        newobj->tiley = newobj->y >> TILESHIFT;  
        newobj->dir = nodir; 
    
        if (mod.wp_prjangle[gamestate.weapon])
        {
            if (maxcount%2 == 0)
               angle = ob->angle - (maxcount/2)*mod.wp_prjangle[gamestate.weapon] + count*mod.wp_prjangle[gamestate.weapon] + mod.wp_prjangle[gamestate.weapon]/2;
            else
               angle = ob->angle - (maxcount/2)*mod.wp_prjangle[gamestate.weapon] + count*mod.wp_prjangle[gamestate.weapon];
        
            if(angle >= ANGLES)
               angle -= ANGLES;
          
            if(angle < 0)
               angle += ANGLES;    
        }
        else 
            angle = ob->angle;
    
        newobj->angle = angle; 
    
        if (mod.prj_lifetime[which] >= 0)
            newobj->lifetime = mod.prj_lifetime[which];
        
        if (mod.prj_homingfactor[which])
            newobj->homingfactor = mod.prj_homingfactor[which];       
    
        if (mod.prj_speed[which] >= 0) 
            newobj->speed = mod.prj_speed[which]; 
        else 
            newobj->speed = 0x4200l; 
        
        //newobj->flags = FL_NONMARK | FL_BONUS;  // APN: fix for door held open
        newobj->flags = 10000110;     // FL_NONMARK | FL_NEVERMARK | FL_BONUS
        newobj->flags |= FL_FULLBRIGHT;
        newobj->silent = mod.wp_silenced[gamestate.weapon];
        newobj->active = ac_yes;
    }
    
    if (!mod.wp_silenced[gamestate.weapon])
        madenoise = true;
    
    if (mod.wp_ammousage[gamestate.weapon] >= 0)
    {
        if (gamestate.weapon >= wp_pistol && gamestate.weapon <= wp_chaingun)
            gamestate.ammo -= mod.wp_ammousage[gamestate.weapon]; 
        else if (gamestate.weapon == wp_fthrower)                                 
            GiveGascan (-mod.wp_ammousage[gamestate.weapon]);
        else if (gamestate.weapon == wp_mlauncher)                                 
            GiveMissile (-mod.wp_ammousage[gamestate.weapon]);
    }
    else
    {
        if (gamestate.weapon >= wp_pistol && gamestate.weapon <= wp_chaingun)
            gamestate.ammo--; 
        else if (gamestate.weapon == wp_fthrower)                                 
            GiveGascan (-1);
        else if (gamestate.weapon == wp_mlauncher)                                 
            GiveMissile (-1);
    }
} 


/*
===============
=
= HomingProjectiles
=
===============
*/

void HomingCheck (objtype *ob)
{
    int32_t  viewdist; 
    objtype *check,*closest,*oldclosest;
     
    viewdist = 0x7fffffffl; 
    closest = NULL;

    while (1)
    {
        oldclosest = closest;

        for (check=ob->next ; check ; check=check->next)
        {
            if ((check->flags & FL_SHOOTABLE) && (check->flags & FL_VISABLE)
                && abs(check->viewx-centerx) < viewwidth/6)
            {
                if (check->transx < viewdist)
                {
                    viewdist = check->transx;
                    closest = check;
                }
            }
        }

        if (closest == oldclosest)
            return;                                         // no more targets

        if (CheckLine(closest), true)
            break;
    }  
    
    closest->flags |= FL_TARGET;
}

/*
===============
=
= P_Projectile
=
===============
*/

void P_Projectile (objtype *ob)
{
    if (mod.wp_type[gamestate.weapon] == w_rocket)
    { 
           SpawnProjectile (ob, pl_miss); 
           
           if (mod.prj_homingfactor[pl_miss] == homing_low || mod.prj_homingfactor[pl_miss] == homing_med)
               HomingCheck (ob);
               
           return;
    }
    else if (mod.wp_type[gamestate.weapon] == w_fire)
    { 
           SpawnProjectile (ob, pl_fire); 
           
           if (mod.prj_homingfactor[pl_fire] == homing_low || mod.prj_homingfactor[pl_fire] == homing_med)
               HomingCheck (ob);
               
           return;
    }
    else if (mod.wp_type[gamestate.weapon] == w_fakefire)
    { 
           SpawnProjectile (ob, pl_ffir); 
           
           if (mod.prj_homingfactor[pl_ffir] == homing_low || mod.prj_homingfactor[pl_ffir] == homing_med)
               HomingCheck (ob);
               
           return;
    }
    else if (mod.wp_type[gamestate.weapon] == w_needle)
    { 
           SpawnProjectile (ob, pl_need); 
           
           if (mod.prj_homingfactor[pl_need] == homing_low || mod.prj_homingfactor[pl_need] == homing_med)
               HomingCheck (ob);
               
           return;
    }
}

/*
===============
=
= T_KnifeAttack
=
= Update player hands, and try to do damage when the proper frame is reached
=
===============
*/

void    KnifeAttack (objtype *ob)
{
    objtype *check,*closest;
    int32_t  dist;
    fixed    knifedist;

    if (mod.wp_knockback[gamestate.weapon])
        gamestate.knockback = 0x100*mod.wp_knockback[gamestate.weapon];
        
    P_Projectile (ob);
    
    if (mod.wp_ammousage[gamestate.weapon] >= 0)
        gamestate.ammo -= mod.wp_ammousage[gamestate.weapon];
        
    // actually fire
    dist = 0x7fffffff;
    closest = NULL;
    for (check=ob->next; check; check=check->next)
    {
        if ( (check->flags & FL_SHOOTABLE) && (check->flags & FL_VISABLE)
            && abs(check->viewx-centerx) < shootdelta)
        {
            if (check->transx < dist)
            {
                dist = check->transx;
                closest = check;
            }
        }
    }

    // mac 1st/2nd: 1 tile = 256px, knifedist = 480 -> 1.875 tiles?
    // 3rd encounter has shorter/dos-like distance?
    if (thirdencounter)
        knifedist = 0x18000l;
    else
        knifedist = 0x1DFFEl;
        
    if (mod.wp_dist[gamestate.weapon])
        knifedist = 0x4000*mod.wp_dist[gamestate.weapon];
    
    if (!closest || dist > knifedist) //0x18000l)
    {
        // missed
        switch (gamestate.weapon)
        {
            case wp_knife:   
                SD_PlaySound (ATKKNIFESND); 
            break;    
              
            case wp_pistol:
                SD_PlaySound (ATKPISTOLSND);
            break;
            
            case wp_machinegun:
                SD_PlaySound (ATKMACHINEGUNSND);
            break;
            
            case wp_chaingun:
                SD_PlaySound (ATKGATLINGSND);
            break;
            
            case wp_fthrower:
               SD_PlaySound (FLAMETHROWERSND); 
            break;   
            
            case wp_mlauncher:
               SD_PlaySound (MISSILEFIRESND); 
            break;
        }
        return;
    }

    // hit something
    //DamageActor (closest,US_RndT() >> 4);
    
    if (mod.wp_damage[gamestate.weapon] >= 0)
        DamageActor (closest,mod.wp_damage[gamestate.weapon],mod.wp_silenced[gamestate.weapon]);
    else
        DamageActor (closest,US_RndT()&15,false);
        
    SD_PlaySound(DONNERSND);
}

 
/*
===============
=
= GunAttack
=
===============
*/

#define STEPDIST 0x800l
 
void    GunAttack (objtype *ob)
{
    objtype  *check,*closest,*oldclosest;
    int      damage;
    int      dx,dy,dist;
    int32_t  viewdist;
    int32_t  bulletx,bullety;
    short    bangle;
    boolean  hit;
    fixed    hitsize;
    
    if (mod.wp_knockback[gamestate.weapon])
        gamestate.knockback = 0x100*mod.wp_knockback[gamestate.weapon];
    
    player->attack = true;
    
    P_Projectile (ob);
    
    // default
    if (gamestate.weapon == wp_mlauncher && !mod.wp_type[wp_mlauncher])
    { 
           SpawnProjectile (ob, pl_miss); 
           
           if (mod.prj_homingfactor[pl_miss] == homing_low || mod.prj_homingfactor[pl_miss] == homing_med)
               HomingCheck (ob);
               
           return;
    }
    else if (gamestate.weapon == wp_fthrower && !mod.wp_type[wp_fthrower])
    { 
           SpawnProjectile (ob, pl_fire); 
           
           if (mod.prj_homingfactor[pl_fire] == homing_low || mod.prj_homingfactor[pl_fire] == homing_med)
               HomingCheck (ob);
               
           return;
    }

    switch (gamestate.weapon)
    {
        case wp_knife:   
            SD_PlaySound (ATKKNIFESND); 
            break;     
        case wp_pistol:
            SD_PlaySound (ATKPISTOLSND);
            break;
        case wp_machinegun:
            SD_PlaySound (ATKMACHINEGUNSND);
            break;
        case wp_chaingun:
            SD_PlaySound (ATKGATLINGSND);
            break;
        case wp_fthrower:
            SD_PlaySound (FLAMETHROWERSND); 
            break;    
        case wp_mlauncher:
            SD_PlaySound (MISSILEFIRESND); 
            break;   
    }

    if (mod.wp_ammousage[gamestate.weapon] >= 0)
        gamestate.ammo -= mod.wp_ammousage[gamestate.weapon];
    else
        gamestate.ammo--;
     
    if (!mod.wp_silenced[gamestate.weapon])    
        madenoise = true;

    bulletx = player->x;
    bullety = player->y;
    bangle = player->angle;

    hit = false;   

    while(1)
    {
            
            bulletx += FixedMul(STEPDIST,costable[bangle]);
            bullety -= FixedMul(STEPDIST,sintable[bangle]);
            
            if (mod.wp_dist[gamestate.weapon])
            {
                  fixed ddx = ABS(bulletx - player->x);
                  fixed ddy = ABS(bullety - player->y);
                  fixed ddist = ddx>ddy ? ddx:ddy;   
                  
                  if (ddist > 0x4000*mod.wp_dist[gamestate.weapon])   
                     break;
            }

            //
            // check for solid walls
            //
            check = actorat[bulletx >> TILESHIFT][bullety >> TILESHIFT];
            if (check && !ISPOINTER(check) && (uintptr_t)check != BIT_WALL)
            {
                if ((uintptr_t)check < BIT_DOOR)      // Hit a wall
                { 
                   hit = true;
                }
                else if (doorposition[(uintptr_t)check - 128] < 0xdfff/2)   // Cheap hack - improve later?
                {
                   hit = true;
                }        
            }

            if (hit)
                break;

            check = objlist;
            while (check)
            {
                if (check->flags & FL_SHOOTABLE)
                {
                    dx = ABS(check->tilex - player->tilex);
                    dy = ABS(check->tiley - player->tiley);
                    dist = dx>dy ? dx:dy;
                    
                    hitsize = 0x3000+0x1500*dist/5; // so its minimally bigger further away
                                 
                    if (LABS(bulletx - check->x) < hitsize && LABS(bullety - check->y) < hitsize)
                    {
                        hit = true;   

                        //dx = ABS(check->tilex - player->tilex);
                        //dy = ABS(check->tiley - player->tiley);
                        //dist = dx>dy ? dx:dy;
                                
                        if (dist<2)
                            damage = US_RndT()&15;
                        else
                            damage = US_RndT()&7;
                        
                        if (mod.wp_damage[gamestate.weapon] >= 0)
                        {
                            damage = mod.wp_damage[gamestate.weapon];
                            
                            if (dist > 10) 
                                damage /= 2;
                        }
                                           
                        DamageActor (check, damage, mod.wp_silenced[gamestate.weapon]);
                    }
                }
                check = check->next ;
            }

            if (hit)
                break;
        }

    //
    // find potential targets
    //
    /*
    viewdist = 0x7fffffffl;
    closest = NULL;

    while (1)
    {
        oldclosest = closest;

        for (check=ob->next ; check ; check=check->next)
        {
            if ((check->flags & FL_SHOOTABLE) && (check->flags & FL_VISABLE)
                && abs(check->viewx-centerx) < shootdelta)
            {
                if (check->transx < viewdist)
                {
                    viewdist = check->transx;
                    closest = check;
                }
            }
        }

        if (closest == oldclosest)
            return;                                         // no more targets, all missed

        //
        // trace a line from player to enemey
        //
        if (CheckLine(closest), true)
            break;
    }

    //
    // hit something
    //
    dx = ABS(closest->tilex - player->tilex);
    dy = ABS(closest->tiley - player->tiley);
    dist = dx>dy ? dx:dy;
    if (dist<2)
    //    damage = US_RndT() / 4;
        damage = US_RndT()&15;
    //else if (dist<4)
    //    damage = US_RndT() / 6;
    else
    {
        //if ( (US_RndT() / 12) < dist)           // missed
        //    return;
        //damage = US_RndT() / 6;
        damage = US_RndT()&7;
    }
    DamageActor (closest,damage);*/
}

//===========================================================================

/*
===============
=
= Cmd_Fire
=
===============
*/

void Cmd_Fire (void)
{
    if (gamestate.weapon == -1) return; 
    
    buttonheld[bt_attack] = true;

    gamestate.weaponframe = 0;

    player->state = &s_attack;

    gamestate.attackframe = 0;
    gamestate.attackcount = attackinfo[gamestate.weapon][gamestate.attackframe].tics;
    gamestate.weaponframe = attackinfo[gamestate.weapon][gamestate.attackframe].frame;
}

/*
===============
=
= T_Knockback
=
===============
*/

void    T_Knockback (void)
{
    if (gamestate.knockback <= 0) 
        gamestate.knockback = 0;
    else
    {
       gamestate.knockback -= tics*(1+gamestate.knockback/8);  
       Thrust (player->angle,-gamestate.knockback);    
    }
}

/*
===============
=
= T_Attack
=
===============
*/

void    T_Attack (objtype *ob)
{
    struct  atkinf  *cur;

    UpdateFace ();

    if (gamestate.victoryflag)              // watching the BJ actor
    {
        VictorySpin ();
        return;
    }
    
    T_Knockback ();

    if ( buttonstate[bt_use] && !buttonheld[bt_use] )
        buttonstate[bt_use] = false;

    if ( buttonstate[bt_attack] && !buttonheld[bt_attack])
        buttonstate[bt_attack] = false;

    ControlMovement (ob);
    if (gamestate.victoryflag || gamestate.victorycam)              // watching the BJ actor
        return;

    plux = (word) (player->x >> UNSIGNEDSHIFT);                     // scale to fit in unsigned
    pluy = (word) (player->y >> UNSIGNEDSHIFT);
    player->tilex = (short)(player->x >> TILESHIFT);                // scale to tile values
    player->tiley = (short)(player->y >> TILESHIFT);

    //
    // change frame and fire
    //
    gamestate.attackcount -= (short) tics;
    while (gamestate.attackcount <= 0)
    {
        cur = &attackinfo[gamestate.weapon][gamestate.attackframe];
        
        
        if (mod.wp_type[gamestate.weapon] == w_melee)
        {
             if (cur->attack == 1)
                 cur->attack = 2; 
        }
        else if (mod.wp_type[gamestate.weapon] >= w_bullets)
        {
             if (cur->attack == 2)
                 cur->attack = 1; 
        }
        
        if (mod.wp_firerate[gamestate.weapon] >= 0)  
        {  
            if (cur->frame > 2 && (cur->attack == 0 || cur->attack >= 3))
                cur->attack = mod.wp_firerate[gamestate.weapon];  
        }
        
        switch (cur->attack)
        {
            case -1:
                ob->state = &s_player;
                if (player->attack) 
                    player->attack = false;
                if (!gamestate.ammo)
                {
                    //gamestate.weapon = wp_knife;
                    //DrawWeapon ();
                }
                else
                {
                    if (gamestate.weapon != gamestate.chosenweapon)
                    {
                        gamestate.weapon = gamestate.chosenweapon;
                       // DrawWeapon ();
                    }
                }
                gamestate.attackframe = gamestate.weaponframe = 0;
                return;

		case 4:
            if (player->attack) 
                    player->attack = false;
                     
            if ((gamestate.ammo<1 && gamestate.weapon == wp_chaingun)||
               (gamestate.gascan<1 && gamestate.weapon == wp_fthrower))
                 break;
                 
			if (buttonstate[bt_attack])
				gamestate.attackframe -= 2;
		case 1:  
            if (mod.wp_type[gamestate.weapon] == w_melee)
            {
            }
            else if (gamestate.weapon == wp_knife && mod.wp_firerate[gamestate.weapon])
            {
            }
            else
			    GunAttack (ob);
			    
            DrawAmmo ();
			break;

            case 2:
                KnifeAttack (ob);
                break;

            case 3:
                if (player->attack) 
                    player->attack = false;
                     
                if (gamestate.ammo && buttonstate[bt_attack])
                    gamestate.attackframe -= 2;
                break;
        }
        
        if (gamestate.weapon == wp_knife && mod.wp_speed[0])
            cur->tics = mod.wp_speed[0];
        else if (gamestate.weapon == wp_pistol && mod.wp_speed[1])
            cur->tics = mod.wp_speed[1];
        else if (gamestate.weapon == wp_machinegun && mod.wp_speed[2])
            cur->tics = mod.wp_speed[2];
        else if (gamestate.weapon == wp_chaingun && mod.wp_speed[3])
            cur->tics = mod.wp_speed[3];    
        else if (gamestate.weapon == wp_fthrower && mod.wp_speed[4])
            cur->tics = mod.wp_speed[4];
        else if (gamestate.weapon == wp_mlauncher && mod.wp_speed[5])
            cur->tics = mod.wp_speed[5];        
                   
        gamestate.attackcount += cur->tics;
        gamestate.attackframe++;
        gamestate.weaponframe = attackinfo[gamestate.weapon][gamestate.attackframe].frame;  
    }
}



//===========================================================================

/*
===============
=
= T_Player
=
===============
*/

void    T_Player (objtype *ob)
{
    if (gamestate.victoryflag)              // watching the BJ actor
    {
        VictorySpin ();
        return;
    }
    if (gamestate.victorycam)
        return;
     
    T_Knockback ();

    UpdateFace ();
    CheckWeaponChange ();
    
    if (buttonstate[bt_automap])
    {
        automap^=1;                        
        IN_ClearKeysDown();
    }

    if ( buttonstate[bt_use] )
        Cmd_Use ();

    if ( buttonstate[bt_attack] && !buttonheld[bt_attack])
        Cmd_Fire ();

    ControlMovement (ob);
    if (gamestate.victoryflag || gamestate.victorycam)              // watching the BJ actor
        return;
        
    if (automap)
        ViewMap();    

    plux = (word) (player->x >> UNSIGNEDSHIFT);                     // scale to fit in unsigned
    pluy = (word) (player->y >> UNSIGNEDSHIFT);
    player->tilex = (short)(player->x >> TILESHIFT);                // scale to tile values
    player->tiley = (short)(player->y >> TILESHIFT);
}

//===========================================================================

/*
===============
=
= VictorySpin
=
===============
*/

void VictorySpin (void)
{
    int32_t    desty;

    if (player->angle > 270)
    {
        player->angle -= (short)(tics * 3);
        if (player->angle < 270)
            player->angle = 270;
    }
    else if (player->angle < 270)
    {
        player->angle += (short)(tics * 3);
        if (player->angle > 270)
            player->angle = 270;
    }

    desty = (((int32_t)player->tiley-5)<<TILESHIFT)-0x3000;

    if (player->y > desty)
    {
        player->y -= tics*4096;
        if (player->y < desty)
            player->y = desty;
    }
}
