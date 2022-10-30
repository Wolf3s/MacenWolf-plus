// WL_ACT2.C

#include <stdio.h>
#include <math.h>
#include "wl_def.h"
#pragma hdrstop

/*
=============================================================================

                               LOCAL CONSTANTS

=============================================================================
*/

#define PROJECTILESIZE  0x7800l //0xc000l
#define BJROCKETSIZE    0x7800l //0x6000l  

#define BJRUNSPEED      2048
#define BJJUMPSPEED     680


/*
=============================================================================

                              GLOBAL VARIABLES

=============================================================================
*/



/*
=============================================================================

                              LOCAL VARIABLES

=============================================================================
*/

// chase0 so enemies go immediately to shooting again when hugging the player for some love
// alternatively change DoActor to execute action at start of new state instead of at the end and
// that they should still inch around like worms

dirtype dirtable[9] = {northwest,north,northeast,west,nodir,east,
    southwest,south,southeast};

short starthitpoints[4][NUMENEMIES] = {
	 //
	 // BABY MODE
	 //
    {
	      6,	// guards
          12,   // officer                                             
          25,   // SS                            
	      1,	// dogs

          250,  // Hans                        
          350,  // Schabbs                
	      400,	// fake hitler
          250,  // mecha hitler                 
          18,   // mutants                  
	      25,	// ghosts
	      25,	// ghosts
	      25,	// ghosts
	      25,	// ghosts

          300,  // Gretel = Mac Trans           
          450,  // Gift = Mac Death          
          400,  // Fat = Mac Uber                
	      15,	// en_spectre,
	      1650,	// en_angel,
	      1050,	// en_trans,
	      1250,	// en_uber,
	      1150,	// en_will,
	      1450,	// en_death  
	      1450	// en_what
    },
    //
    // DON'T HURT ME MODE
    //
    {
	      6,	// guards
          12,   // officer                                             
          25,   // SS                            
	      1,	// dogs

          250,  // Hans                     
          350,  // Schabbs                       
	      400,	// fake hitler
          250,  // mecha hitler               
          18,   // mutants                  
	      25,	// ghosts
	      25,	// ghosts
	      25,	// ghosts
	      25,	// ghosts

          300,  // Gretel = Mac Trans           
          450,  // Gift = Mac Death     
          400,  // Fat = Mac Uber        
	      15,	// en_spectre,
	      1650,	// en_angel,
	      1050,	// en_trans,
	      1250,	// en_uber,
	      1150,	// en_will,
	      1450,	// en_death  
	      1450	// en_what
    },
    //
    // BRING 'EM ON MODE
    //
    {
	      6,	// guards
          12,   // officer                                             
          25,   // SS                            
	      1,	// dogs

          250,  // Hans                       
          350,  // Schabbs                    
	      400,	// fake hitler
          250,  // mecha hitler                
          18,   // mutants                    
	      25,	// ghosts
	      25,	// ghosts
	      25,	// ghosts
	      25,	// ghosts

          300,  // Gretel = Mac Trans          
          450,  // Gift = Mac Death             
          400,  // Fat = Mac Uber            
	      15,	// en_spectre,
	      1650,	// en_angel,
	      1050,	// en_trans,
	      1250,	// en_uber,
	      1150,	// en_will,
	      1450,	// en_death  
	      1450	// en_what
    },
    //
    // DEATH INCARNATE MODE
    //
    {
	      6,	// guards
          12,   // officer                                             
          25,   // SS                            
	      1,	// dogs

          250,  // Hans                         
          350,  // Schabbs                   
	      400,	// fake hitler
          250,  // mecha hitler                 
          18,   // mutants                    
	      25,	// ghosts
	      25,	// ghosts
	      25,	// ghosts
	      25,	// ghosts

          300,  // Gretel = Mac Trans            
          450,  // Gift = Mac Death              
          400,  // Fat = Mac Uber                
	      15,	// en_spectre,
	      1650,	// en_angel,
	      1050,	// en_trans,
	      1250,	// en_uber,
	      1150,	// en_will,
	      1450,	// en_death  
	      1450	// en_what
	  }
};

void    A_StartDeathCam (void);


void    T_Path (objtype *ob);
void    T_Shoot (objtype *ob);
void    T_Bite (objtype *ob);
void    T_DogChase (objtype *ob);
void    T_Chase (objtype *ob);
void    T_BossChase (objtype *ob);
void    T_Projectile (objtype *ob);
void    T_Stand (objtype *ob);
void    T_Target (objtype *ob);
void    T_Attacking (objtype *ob);
void    T_AltAttack (objtype *ob);
void    T_Rocket (objtype *ob);
void    T_Boom (objtype *ob);

void A_DeathScream (objtype *ob);

extern  statetype s_rocket;
extern  statetype s_smoke1;
extern  statetype s_smoke2;
extern  statetype s_smoke3;
extern  statetype s_smoke4;
extern  statetype s_boom2;
extern  statetype s_boom3;

void A_Smoke (objtype *ob);

statetype s_procket             = {false,SPR_PLAYER_ROCKET,3,(statefunc)T_Projectile,NULL,&s_procket};
statetype s_rocket              = {false,SPR_ENEMY_ROCKET,3,(statefunc)T_Projectile,NULL,&s_rocket};
statetype s_smoke1              = {false,SPR_PLAYER_BOOM,3,NULL,NULL,NULL};
//statetype s_smoke1              = {false,SPR_SMOKE_1,3,NULL,NULL,&s_smoke2};
//statetype s_smoke2              = {false,SPR_SMOKE_2,3,NULL,NULL,&s_smoke3};
//statetype s_smoke3              = {false,SPR_SMOKE_3,3,NULL,NULL,&s_smoke4};
//statetype s_smoke4              = {false,SPR_SMOKE_4,3,NULL,NULL,NULL};

statetype s_boom1	 	= {false,SPR_PLAYER_BOOM,7,NULL,NULL,&s_boom2};
statetype s_boom2       = {false,SPR_PLAYER_BOOM,7,NULL,(statefunc)T_Boom,&s_boom3};
statetype s_boom3       = {false,SPR_PLAYER_BOOM,2,NULL,NULL,NULL};

extern  statetype s_explosion1;
extern  statetype s_explosion2;
extern  statetype s_explosion3;

statetype s_explosion1	 	 = {false,SPR_EXPLOSION,3,NULL,NULL,&s_explosion2};
statetype s_explosion2       = {false,SPR_EXPLOSION,3,NULL,(statefunc)T_Boom,&s_explosion3};
statetype s_explosion3       = {false,SPR_EXPLOSION,2,NULL,NULL,NULL};

extern  statetype d_drocket;
 
statetype s_drocket     = {false,SPR_ENEMY_ROCKET,3,(statefunc)T_Projectile,NULL,&s_drocket};

extern  statetype s_flame;
extern  statetype s_hsmoke1;
extern  statetype s_hsmoke2;
extern  statetype s_hsmoke3;
extern  statetype s_hsmoke4;
extern  statetype s_hboom1;
extern  statetype s_hboom2;
extern  statetype s_hboom3;

void A_Smoke (objtype *ob);

//statetype s_hrocket             = {true,SPR_HROCKET_1,3,(statefunc)T_Projectile,(statefunc)A_Smoke,&s_hrocket};
//statetype s_hsmoke1             = {false,SPR_HSMOKE_1,3,NULL,NULL,&s_hsmoke2};
//statetype s_hsmoke2             = {false,SPR_HSMOKE_2,3,NULL,NULL,&s_hsmoke3};
//statetype s_hsmoke3             = {false,SPR_HSMOKE_3,3,NULL,NULL,&s_hsmoke4};
//statetype s_hsmoke4             = {false,SPR_HSMOKE_4,3,NULL,NULL,NULL};

//statetype s_hboom1              = {false,SPR_HBOOM_1,6,NULL,NULL,&s_hboom2};
//statetype s_hboom2              = {false,SPR_HBOOM_2,6,NULL,NULL,&s_hboom3};
//statetype s_hboom3              = {false,SPR_HBOOM_3,6,NULL,NULL,NULL};

statetype s_flame             = {false,SPR_FIRE,3,(statefunc)T_Projectile,NULL,&s_flame};
// statetype s_hsmoke2             = {false,SPR_SMOKE_2,3,NULL,NULL,&s_hsmoke3};
// statetype s_hsmoke3             = {false,SPR_SMOKE_3,3,NULL,NULL,&s_hsmoke4};
// statetype s_hsmoke4             = {false,SPR_SMOKE_4,3,NULL,NULL,NULL};

statetype s_hboom1              = {false,SPR_FIRE2,6,NULL,NULL,&s_hboom2};
statetype s_hboom2              = {false,SPR_FIRE2,6,NULL,NULL,&s_hboom3};
statetype s_hboom3              = {false,SPR_FIRE2,6,NULL,NULL,NULL};

void    T_SchabbThrow (objtype *ob);
void    T_Fake (objtype *ob);
void    T_FakeFire (objtype *ob);
void    T_Ghosts (objtype *ob);

void A_Slurpie (objtype *ob);
void A_HitlerMorph (objtype *ob);
void A_MechaSound (objtype *ob);

/*
=================
=
= A_Smoke
=
=================
*/

void A_Smoke (objtype *ob)
{
    GetNewActor ();
#ifdef SPEAR
    if (ob->obclass == pl_fireobj)
        newobj->state = &s_hsmoke1;
    else
#endif
        newobj->state = &s_smoke1;
    newobj->ticcount = 6;

    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->obclass = inertobj;
    newobj->active = ac_yes;

    newobj->flags = FL_NEVERMARK;
}

/*
===================
=
= Body Hits the Ground
=
===================
*/

void A_Thud (objtype *ob)
{
	SD_PlaySound(ROSESND);  
}


/*
===================
=
= T_Bright
=
= illuminate when shooting
=
===================
*/

void T_Bright (objtype *ob)
{ 
    if (mod.en_attack[ob->obclass] == t_bite) 
        return;
    
	ob->attack = true;
}

void T_Dark (objtype *ob)
{
	ob->attack = false;
}

/*
===================
=
= ProjectileTryMove
=
= returns true if move ok
===================
*/

#define PROJSIZE        0x1
     
boolean ProjectileTryMove (objtype *ob)
{ 
    int      xl,yl,xh,yh,x,y; 
    objtype  *solid;
    byte     check;
    int32_t     deltax,deltay;

    xl = (ob->x-PROJSIZE) >> TILESHIFT;
    yl = (ob->y-PROJSIZE) >> TILESHIFT;

    xh = (ob->x+PROJSIZE) >> TILESHIFT;
    yh = (ob->y+PROJSIZE) >> TILESHIFT;
    
    //
    // check for solid walls
    //
    for (y=yl;y<=yh;y++)
        for (x=xl;x<=xh;x++)
        {
            solid = actorat[x][y];
           
            if (thirdencounter && (MAPSPOT(x,y,1) == 30 || MAPSPOT(x,y,1) == 40)) //check for column or hanging cage     
                return false;  

            if (!thirdencounter && solid && !ISPOINTER(solid) && MAPSPOT (x,y,1))
                return false;

             check = tilemap[x][y];
             if (check)                
             { 
                if (check & 0x80)                                       // doors
                { 
                   if ((doorobjlist[check & ~0x80].action == dr_closed)||  // door is closed 
                       (doorobjlist[check & ~0x80].action == dr_opening)|| 
                       (doorobjlist[check & ~0x80].action == dr_closing))   // door is closing
                   return false; 
                } 
                else 
                {
                    ob->x -= FixedMul((int32_t)ob->speed*tics,costable[ob->angle]);	// undo the last motion to have explosion on the wall
            	    ob->y -= -FixedMul((int32_t)ob->speed*tics ,sintable[ob->angle]);  
            	    
                    return false;                                         // solid wall 
                }
             } 
        }

        return true;
}

/*
=================
=
= T_Homing
=
=================
*/

void T_Homing (objtype *ob)
{
    int32_t move;
    int     dx,dy,dist;
    
    if (ob->obclass == drocketobj || ob->obclass == needleobj || ob->obclass == fireobj || ob->obclass == en_fireobj)
    {
        if (LABS(ob->x - player->x) < PROJECTILESIZE && LABS(ob->y - player->y) < PROJECTILESIZE) 
        {  
            if (ob->obclass == drocketobj)  
            { 
               ob->state = &s_boom1;
            }
            else 
            {
               ob->state = &s_explosion1; 
            }
            
            if (ob->homingtime)
                ob->homingtime = 0;
                
            TakeDamage ((US_RndT() >>3) + 30,ob);
                
            return;
        }
    } 
    
    if (ob->dir == nodir)
    {
        if (ob->obclass == rocketobj || ob->obclass == pl_fireobj || 
           ob->obclass == pl_fakefireobj || ob->obclass == pl_needleobj) // player projectiles     
            SelectHomingDir (ob);
        else
            SelectChaseDir (ob);  
        
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        if (ob->obclass == rocketobj || ob->obclass == pl_fireobj ||
            ob->obclass == pl_fakefireobj || ob->obclass == pl_needleobj) // player projectiles     
            SelectHomingDir (ob);
        else
            SelectChaseDir (ob);  

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}

int A_HomingAngle (objtype *ob, boolean targetplayer)
{
    float   angle;
    int     iangle;       
    int32_t deltax, deltay; 
    objtype *closest,*oldclosest;
    objtype *check;
    
    if (targetplayer)
    {
        deltax = player->x - ob->x;
        deltay = ob->y - player->y;   
    }
    else
    {
    
        closest = NULL;
        check = objlist; 
        while (check) 
        { 
            oldclosest = closest;  
            if (check->flags & FL_TARGET) 
            { 
                 if (check->areanumber >= NUMAREAS || areabyplayer[check->areanumber]) 
                 {
                    closest = check;
                    break;
                 }
            }    
                      
            check = check->next;
        } 
    
        if (closest == oldclosest)
            return -1;
            
        if (closest->flags & FL_TARGET && ob->homingfactor == homing_low && ob->homingtime > 6)
            closest->flags  &= ~FL_TARGET;
                                   
        deltax = check->x - ob->x;
        deltay = ob->y - check->y;
    }
    
    angle = (float) atan2((float) deltay, (float) deltax);

    if (angle<0)
        angle = (float)(M_PI*2+angle);
        
    iangle = (int) (angle/(M_PI*2)*ANGLES);
        
    return iangle;  
}

/*
=================
=
= T_En_SplashDmg
=
=================
*/

void T_En_SplashDmg (objtype* ob, int which)
{
  int     deltax,deltay;
  int     dist,boomdist;
  int     damage;
  objtype *check;
 
  check = objlist;

   while (check)
   {
       if (check != ob && (check->flags & FL_SHOOTABLE))
       {
	        deltax = abs(ob->tilex - check->tilex);
	        deltay = abs(ob->tiley - check->tiley);
	        dist = deltax>deltay ? deltax : deltay;
   	
            boomdist = mod.prj_splashradius[which];
	        damage = mod.prj_splashdmg[which];

	        if (dist <= boomdist && areabyplayer[check->areanumber])
	        {
                DamageActor (check, damage/(1+dist), ob->silent);
                
                if (check->flags & FL_TARGET)   
                    check->flags &= ~FL_TARGET; 
            }
       }

       check = check->next;
   }
}

/*
=================
=
= T_Pl_SplashDmg
=
=================
*/
void T_Pl_SplashDmg (objtype* ob, int which)
{
    int     deltax,deltay;
    int     dist,boomdist;
    int     damage;

	deltax = abs(ob->tilex - player->tilex);
	deltay = abs(ob->tiley - player->tiley);
	dist = deltax>deltay ? deltax : deltay;
   	
    boomdist = mod.prj_splashradius[which];
	damage = mod.prj_splashdmg[which];

	if (dist <= boomdist && CheckLine (ob))
	{
        TakeDamage (damage/(1+dist), ob);
        
        if (mod.prj_playerknockback[which])
            gamestate.knockback = 0x100*mod.prj_playerknockback[which]/(1+dist); 
    }   
}

/*
=================
=
= T_Boom
=
=================
*/

// next time mod.prj_selfdmg[ob->oblcass] and reduce it to few lines
void T_Boom (objtype* ob)
{
        if (ob->obclass == rocketobj)
        {
            if (mod.prj_selfdmg[pl_miss] == 1 || mod.prj_selfdmg[pl_miss] == 3)            
                T_Pl_SplashDmg (ob, pl_miss);
            
            if (mod.prj_selfdmg[pl_miss] >= 2)
                T_En_SplashDmg (ob, pl_miss);
        }
        else if (ob->obclass == drocketobj) 
        {
            if (mod.prj_selfdmg[en_miss] == 1 || mod.prj_selfdmg[en_miss] == 3)  
                T_Pl_SplashDmg (ob, en_miss); 
            
            if (mod.prj_selfdmg[en_miss] >= 2)
                T_En_SplashDmg (ob, en_miss);
        }
        else if (ob->obclass == pl_fireobj)
        {
            if (mod.prj_selfdmg[pl_fire] == 1 || mod.prj_selfdmg[pl_fire] == 3)  
                T_Pl_SplashDmg (ob, pl_fire);
            
            if (mod.prj_selfdmg[pl_fire] >= 2)
                T_En_SplashDmg (ob, pl_fire);
        }
        
        else if (ob->obclass == pl_fakefireobj)
        {
            if (mod.prj_selfdmg[pl_ffir] == 1 || mod.prj_selfdmg[pl_ffir] == 3)  
                T_Pl_SplashDmg (ob, pl_ffir);
            
            if (mod.prj_selfdmg[pl_ffir] >= 2)
                T_En_SplashDmg (ob, pl_ffir);
        }
        else if (ob->obclass == pl_needleobj)
        {
            if (mod.prj_selfdmg[pl_need] == 1 || mod.prj_selfdmg[pl_need] == 3)  
                T_Pl_SplashDmg (ob, pl_need);
            
            if (mod.prj_selfdmg[pl_need] >= 2)
                T_En_SplashDmg (ob, pl_need);
        }
        
        else if (ob->obclass == fireobj)
        {
            if (mod.prj_selfdmg[en_ffir] == 1 || mod.prj_selfdmg[en_ffir] == 3)  
                T_Pl_SplashDmg (ob, en_ffir); 
            
            if (mod.prj_selfdmg[en_ffir] >= 2)
                T_En_SplashDmg (ob, en_ffir);
        }
        else if (ob->obclass == en_fireobj)
        {
            if (mod.prj_selfdmg[en_fire] == 1 || mod.prj_selfdmg[en_fire] == 3)  
                T_Pl_SplashDmg (ob, en_fire);
            
            if (mod.prj_selfdmg[en_fire] >= 2)  
                T_En_SplashDmg (ob, en_fire);    
        }
        else if (ob->obclass == needleobj)
        {
            if (mod.prj_selfdmg[en_need] == 1 || mod.prj_selfdmg[en_need] == 3)  
                T_Pl_SplashDmg (ob, en_need); 
            
            if (mod.prj_selfdmg[en_need] >= 2)
                T_En_SplashDmg (ob, en_need); 
        }
}

/*
=================
=
= T_Projectile
=
=================
*/

void T_Projectile (objtype *ob)
{
    int32_t deltax,deltay;
    int     damage;
    int32_t speed;
    objtype *check;
    boolean targetplayer;
    
        
    if (ob->obclass == needleobj || ob->obclass == pl_needleobj)
    {
    }
    else
        ob->attack = true;

    if (ob->lifetime > 0) 
    {
        ob->lifetime -= tics;
        
        if (!objfreelist)         // terminate them if no space left
        {
            ob->state = NULL; 
            
            ob->lifetime = 0;
            
            if (ob->homingtime)
                ob->homingtime = 0;
        }
    
        if (ob->lifetime <= 0)
        {
            if (ob->obclass == rocketobj || ob->obclass == drocketobj)   
               ob->state = &s_boom1;
            else 
               ob->state = &s_explosion1;  

            ob->lifetime = 0;
            
            if (ob->homingtime)
                ob->homingtime = 0;
               
            return;   
        }
    }
    
    // no homi            
        if (ob->homingfactor >= homing_low)
        {                     
            ob->homingtime += tics; 
            
            if (ob->obclass == rocketobj || ob->obclass == pl_fireobj || ob->obclass == pl_fakefireobj || ob->obclass == pl_needleobj)
                targetplayer = false;
            else
                targetplayer = true; 

            if (!ProjectileTryMove (ob))
            {
                 if (ob->obclass == rocketobj)                  // player rocket
                 {
                      PlaySoundLocActor(MISSILEHITSND,ob);
                      ob->state = &s_boom1;
                 }
                 else if (ob->obclass == drocketobj)            // enemy rocket
                 {
                      PlaySoundLocActor(MISSILEHITSND,ob);
                      ob->state = &s_boom1;
                 }
                 else 
                 {
                      PlaySoundLocActor(MISSILEHITSND,ob);
                      ob->state = &s_explosion1;   
                 }
                 
                 return;
            }
        
            if (ob->homingfactor == homing_low)
            {                                                               
                 if (ob->homingtime > 4 && ob->homingtime < 6)     
                 {   
                     if (A_HomingAngle(ob,targetplayer) >= 0)                 
                         ob->angle = A_HomingAngle(ob,targetplayer);      
                 }                               
            }
            else if (ob->homingfactor == homing_med)
            {
                 if (A_HomingAngle(ob,targetplayer) >= 0)                 
                     ob->angle = A_HomingAngle(ob,targetplayer);                                        
            }
            else if (ob->homingfactor >= homing_high)
            {
                 if (ob->homingtime > 10) // go straight first. start aiming after a 1/6 sec
                 {                                       
                    T_Homing (ob); 
                    return;
                 }
            }
        }
    
    speed = (int32_t)ob->speed*tics;

    deltax = FixedMul(speed,costable[ob->angle]);
    deltay = -FixedMul(speed,sintable[ob->angle]);
    
    // THE LIMIT IS THE SKY (and the out of bounds huh)    
    //if (deltax>0x10000l)
    //    deltax = 0x10000l;
    //if (deltay>0x10000l)
    //    deltay = 0x10000l;

    ob->x += deltax;
    ob->y += deltay;

    deltax = LABS(ob->x - player->x);
    deltay = LABS(ob->y - player->y);

    if (!ProjectileTryMove (ob))
    {                       
            check = objlist; 
            while (check) 
            { 
                // enemy inside a wall, trigger them  
                if (check->flags & FL_SHOOTABLE && ((MAPSPOT(check->tilex,check->tiley,0) < AREATILE) || tilemap[check->tilex][check->tiley]==64)) 
                { 
                    deltax = LABS(ob->x - check->x); 
                    deltay = LABS(ob->y - check->y); 

                    if (deltax < 2*BJROCKETSIZE && deltay < 2*BJROCKETSIZE) 
                    { 
                        if (ob->obclass == pl_fireobj)       
                           DamageActor (check, (US_RndT()&0x0f)+1, ob->silent);  
                        else      
                           DamageActor (check, (US_RndT()&0x30)+16, ob->silent); 
                    } 
                } 
                check = check->next ; 
            }  
      
        //ob->x -= FixedMul(speed,costable[ob->angle]);	// undo the last motion to have explosion on the wall
	    //ob->y -= -FixedMul(speed,sintable[ob->angle]);    

        if (ob->obclass == rocketobj) // player rocket
        {
            PlaySoundLocActor(MISSILEHITSND,ob);
            ob->state = &s_boom1;
        }
        else if (ob->obclass == drocketobj) // enemy rocket
        {
            PlaySoundLocActor(MISSILEHITSND,ob);
            ob->state = &s_boom1;
        }
        else 
        {
            PlaySoundLocActor(MISSILEHITSND,ob);
            ob->state = &s_explosion1;
        }
        //else
        //    ob->state = NULL;               // mark for removal

        return;
    }

        // player projectiles
        if (ob->flags & FL_NONMARK && ob->flags & FL_BONUS) 
        {
            check = objlist; 
            while (check) 
            { 
                if (check->flags & FL_SHOOTABLE) 
                { 
                    deltax = LABS(ob->x - check->x); 
                    deltay = LABS(ob->y - check->y); 

                    if (deltax < BJROCKETSIZE && deltay < BJROCKETSIZE) 
                    { 
                        if (check->obclass == bossobj ||   // APN: keep going, except for Bosses
                            check->obclass == gretelobj ||
                            check->obclass == schabbobj ||
                            check->obclass == giftobj ||
                            check->obclass == fatobj ||
                            check->obclass == mechahitlerobj ||
                            check->obclass == realhitlerobj)
                          {
                              PlaySoundLocActor(MISSILEHITSND,ob); 
                              
                              if (ob->obclass == rocketobj)
                                  ob->state = &s_boom1; 
                              else   
                                  ob->state = &s_explosion1;   
                          }
                         
                        if (ob->obclass == rocketobj)
                        {  
                            if (mod.wp_damage[pl_miss] >= 0)
                                DamageActor (check, mod.prj_damage[pl_miss], ob->silent); 
                            else    
                                DamageActor (check, (US_RndT()&0x30)+16, ob->silent); 
                        }
                        else if (ob->obclass == pl_fireobj)
                        {
                            if (mod.wp_damage[pl_fire] >= 0)
                                DamageActor (check, mod.prj_damage[pl_fire], ob->silent); 
                            else
                                DamageActor (check, (US_RndT()&0x0f)+1, ob->silent); 
                        }  
                        else if (ob->obclass == pl_fakefireobj)
                        {
                            if (mod.wp_damage[pl_ffir] >= 0)
                                DamageActor (check, mod.prj_damage[pl_ffir], ob->silent); 
                        } 
                        else if (ob->obclass == pl_needleobj)
                        {
                            if (mod.wp_damage[pl_need] >= 0)
                                DamageActor (check, mod.prj_damage[pl_need], ob->silent); 
                        } 
                         
                    } 
                } 
                check = check->next ; 
            } 
        } 
    else // Check if player is hit by anything
    { 

        deltax = LABS(ob->x - player->x); 
        deltay = LABS(ob->y - player->y); 

        if (deltax < BJROCKETSIZE && deltay < BJROCKETSIZE)
        {     
            switch (ob->obclass)
            {
               case needleobj:
                    damage = (US_RndT() >>3) + 20;
               break;
               case en_fireobj:
	           case drocketobj:  
               case sparkobj:
                    damage = (US_RndT() >>3) + 30;
               break;
               case fireobj:
                    damage = (US_RndT() >>3);
               break;
            }

            TakeDamage (damage,ob);
            //if (ob->obclass==needleobj || ob->obclass==drocketobj)
            //{
                ob->state = &s_explosion1; 
                PlaySoundLocActor(MISSILEHITSND,ob); 
            //}
            //else
            //  ob->state = NULL;               // mark for removal
          
            return;
        }
    }
    ob->tilex = (short)(ob->x >> TILESHIFT);
    ob->tiley = (short)(ob->y >> TILESHIFT);
}


/*
=============================================================================

GUARD

=============================================================================
*/

//
// guards
//

extern  statetype s_grdstand;

extern  statetype s_grdpath1;
extern  statetype s_grdpath1s;
extern  statetype s_grdpath2;
extern  statetype s_grdpath3;
extern  statetype s_grdpath3s;
extern  statetype s_grdpath4;

extern  statetype s_grdpain;
extern  statetype s_grdpain1;

extern  statetype s_grdgiveup;

extern  statetype s_grdshoot1;
extern  statetype s_grdshoot2;
extern  statetype s_grdshoot3;
extern  statetype s_grdshoot4;

extern  statetype s_grdchase0;
extern  statetype s_grdchase1;
extern  statetype s_grdchase1s;
extern  statetype s_grdchase2;
extern  statetype s_grdchase3;
extern  statetype s_grdchase3s;
extern  statetype s_grdchase4;

extern  statetype s_grddie1;
extern  statetype s_grddie1d;
extern  statetype s_grddie2;
extern  statetype s_grddie3;
extern  statetype s_grddie4;

statetype s_grdstand            = {false,SPR_GRD_W4_1,0,(statefunc)T_Stand,NULL,&s_grdstand};

statetype s_grdpath1            = {false,SPR_GRD_W1_1,20,(statefunc)T_Path,NULL,&s_grdpath1s};
statetype s_grdpath1s           = {false,SPR_GRD_W1_1,5,NULL,NULL,&s_grdpath2};
statetype s_grdpath2            = {false,SPR_GRD_W2_1,15,(statefunc)T_Path,NULL,&s_grdpath3};
statetype s_grdpath3            = {false,SPR_GRD_W3_1,20,(statefunc)T_Path,NULL,&s_grdpath3s};
statetype s_grdpath3s           = {false,SPR_GRD_W3_1,5,NULL,NULL,&s_grdpath4};
statetype s_grdpath4            = {false,SPR_GRD_W4_1,15,(statefunc)T_Path,NULL,&s_grdpath1};

statetype s_grdpain             = {2,SPR_GRD_PAIN_1,12,NULL,NULL,&s_grdchase1};
statetype s_grdpain1            = {2,SPR_GRD_PAIN_1,12,NULL,NULL,&s_grdchase1};

statetype s_grdchase0           = {false,SPR_GRD_W1_1,1,(statefunc)T_Chase,(statefunc)T_Target,&s_grdchase1};

statetype s_grdshoot1           = {false,SPR_GRD_SHOOT1,16,NULL,NULL,&s_grdshoot2};
statetype s_grdshoot2           = {false,SPR_GRD_SHOOT2,16,NULL,NULL,&s_grdshoot3};
statetype s_grdshoot3           = {false,SPR_GRD_SHOOT3,16,(statefunc)T_Bright,(statefunc)T_Attacking,&s_grdchase0};

statetype s_grdchase1           = {false,SPR_GRD_W1_1,12,(statefunc)T_Chase,(statefunc)T_Target,&s_grdchase2};
statetype s_grdchase2           = {false,SPR_GRD_W2_1,12,(statefunc)T_Chase,(statefunc)T_Target,&s_grdchase3};
statetype s_grdchase3           = {false,SPR_GRD_W3_1,12,(statefunc)T_Chase,(statefunc)T_Target,&s_grdchase4};
statetype s_grdchase4           = {false,SPR_GRD_W4_1,12,(statefunc)T_Chase,(statefunc)T_Target,&s_grdchase1};

statetype s_grddie1             = {false,SPR_GRD_PAIN_1,8,NULL,NULL,&s_grddie2};
statetype s_grddie2             = {false,SPR_GRD_DIE_1,8,NULL,(statefunc)A_DeathScream,&s_grddie3};
statetype s_grddie3             = {false,SPR_GRD_DIE_2,8,NULL,(statefunc)A_Thud,&s_grddie4};
statetype s_grddie4             = {false,SPR_GRD_DIE_3,0,NULL,NULL,&s_grddie4};


#ifndef SPEAR
//
// ghosts
//
extern  statetype s_blinkychase1;
extern  statetype s_blinkychase2;
extern  statetype s_inkychase1;
extern  statetype s_inkychase2;
extern  statetype s_pinkychase1;
extern  statetype s_pinkychase2;
extern  statetype s_clydechase1;
extern  statetype s_clydechase2;

void T_Hurt (objtype *ob)
{    
    if (LABS (player->x - ob->x) < 0x17000 && LABS(player->y - ob->y) < 0x17000) 
        TakeDamage (15,ob);   
}

statetype s_blinkychase1        = {false,SPR_BLINKY_W1,10,(statefunc)T_Ghosts,(statefunc)T_Hurt,&s_blinkychase2};
statetype s_blinkychase2        = {false,SPR_BLINKY_W1,10,(statefunc)T_Ghosts,NULL,&s_blinkychase1};

statetype s_inkychase1          = {false,SPR_INKY_W1,10,(statefunc)T_Ghosts,(statefunc)T_Hurt,&s_inkychase2};
statetype s_inkychase2          = {false,SPR_INKY_W1,10,(statefunc)T_Ghosts,NULL,&s_inkychase1};

statetype s_pinkychase1         = {false,SPR_PINKY_W1,10,(statefunc)T_Ghosts,(statefunc)T_Hurt,&s_pinkychase2};
statetype s_pinkychase2         = {false,SPR_PINKY_W1,10,(statefunc)T_Ghosts,NULL,&s_pinkychase1};

statetype s_clydechase1         = {false,SPR_CLYDE_W1,10,(statefunc)T_Ghosts,(statefunc)T_Hurt,&s_clydechase2};
statetype s_clydechase2         = {false,SPR_CLYDE_W1,10,(statefunc)T_Ghosts,NULL,&s_clydechase1};
#endif

//
// dogs
//

extern  statetype s_dogstand;     // MCS: stationary dogs

extern  statetype s_dogpath1;
extern  statetype s_dogpath1s;
extern  statetype s_dogpath2;
extern  statetype s_dogpath3;
extern  statetype s_dogpath3s;
extern  statetype s_dogpath4;

extern  statetype s_dogjump1;
extern  statetype s_dogjump2;
extern  statetype s_dogjump3;
extern  statetype s_dogjump4;
extern  statetype s_dogjump5;

extern  statetype s_dogchase0;
extern  statetype s_dogchase1;
extern  statetype s_dogchase1s;
extern  statetype s_dogchase2;
extern  statetype s_dogchase3;
extern  statetype s_dogchase3s;
extern  statetype s_dogchase4;

extern  statetype s_dogdie1;
extern  statetype s_dogdie1d;
extern  statetype s_dogdie2;
extern  statetype s_dogdie3;
extern  statetype s_dogdead;

statetype s_dogstand    	= {false,SPR_DOG_W4_1,0,(statefunc)T_Stand,NULL,&s_dogstand};  // MCS: stationary dogs

statetype s_dogpath1            = {false,SPR_DOG_W1_1,20,(statefunc)T_Path,NULL,&s_dogpath1s};
statetype s_dogpath1s           = {false,SPR_DOG_W1_1,5,NULL,NULL,&s_dogpath2};
statetype s_dogpath2            = {false,SPR_DOG_W2_1,15,(statefunc)T_Path,NULL,&s_dogpath3};
statetype s_dogpath3            = {false,SPR_DOG_W3_1,20,(statefunc)T_Path,NULL,&s_dogpath3s};
statetype s_dogpath3s           = {false,SPR_DOG_W3_1,5,NULL,NULL,&s_dogpath4};
statetype s_dogpath4            = {false,SPR_DOG_W4_1,15,(statefunc)T_Path,NULL,&s_dogpath1};

statetype s_dogjump1            = {false,SPR_DOG_JUMP1,8,NULL,NULL,&s_dogjump2};
statetype s_dogjump2            = {false,SPR_DOG_JUMP2,8,NULL,NULL,&s_dogjump3};
statetype s_dogjump3            = {false,SPR_DOG_JUMP3,20,NULL,(statefunc)T_Attacking,&s_dogjump4};
statetype s_dogjump4            = {false,SPR_DOG_W1_1,8,NULL,NULL,&s_dogchase0};

statetype s_dogchase0           = {false,SPR_DOG_W1_1,1,(statefunc)T_DogChase,(statefunc)T_Target,&s_dogchase1};
statetype s_dogchase1           = {false,SPR_DOG_W1_1,8,(statefunc)T_DogChase,(statefunc)T_Target,&s_dogchase2};
statetype s_dogchase2           = {false,SPR_DOG_W2_1,8,(statefunc)T_DogChase,(statefunc)T_Target,&s_dogchase3};
statetype s_dogchase3           = {false,SPR_DOG_W3_1,8,(statefunc)T_DogChase,(statefunc)T_Target,&s_dogchase4};
statetype s_dogchase4           = {false,SPR_DOG_W4_1,8,(statefunc)T_DogChase,(statefunc)T_Target,&s_dogchase1};

statetype s_dogdie1             = {false,SPR_DOG_DIE_1,8,NULL,NULL,&s_dogdie2};
statetype s_dogdie2             = {false,SPR_DOG_DIE_2,8,NULL,(statefunc)A_DeathScream,&s_dogdie3};
statetype s_dogdie3             = {false,SPR_DOG_DIE_3,8,NULL,(statefunc)A_Thud,&s_dogdead};
statetype s_dogdead             = {false,SPR_DOG_DIE_3,0,NULL,NULL,&s_dogdead};


//
// officers
//

extern  statetype s_ofcstand;

extern  statetype s_ofcpath1;
extern  statetype s_ofcpath1s;
extern  statetype s_ofcpath2;
extern  statetype s_ofcpath3;
extern  statetype s_ofcpath3s;
extern  statetype s_ofcpath4;

extern  statetype s_ofcpain;
extern  statetype s_ofcpain1;

extern  statetype s_ofcshoot1;
extern  statetype s_ofcshoot2;
extern  statetype s_ofcshoot3;
extern  statetype s_ofcshoot4;

extern  statetype s_ofcchase0;
extern  statetype s_ofcchase1;
extern  statetype s_ofcchase1s;
extern  statetype s_ofcchase2;
extern  statetype s_ofcchase3;
extern  statetype s_ofcchase3s;
extern  statetype s_ofcchase4;

extern  statetype s_ofcdie1;
extern  statetype s_ofcdie2;
extern  statetype s_ofcdie3;
extern  statetype s_ofcdie4;
extern  statetype s_ofcdie5;

statetype s_ofcstand            = {false,SPR_OFC_W4_1,0,(statefunc)T_Stand,NULL,&s_ofcstand};

statetype s_ofcpath1            = {false,SPR_OFC_W1_1,20,(statefunc)T_Path,NULL,&s_ofcpath1s};
statetype s_ofcpath1s           = {false,SPR_OFC_W1_1,5,NULL,NULL,&s_ofcpath2};
statetype s_ofcpath2            = {false,SPR_OFC_W2_1,15,(statefunc)T_Path,NULL,&s_ofcpath3};
statetype s_ofcpath3            = {false,SPR_OFC_W3_1,20,(statefunc)T_Path,NULL,&s_ofcpath3s};
statetype s_ofcpath3s           = {false,SPR_OFC_W3_1,5,NULL,NULL,&s_ofcpath4};
statetype s_ofcpath4            = {false,SPR_OFC_W4_1,15,(statefunc)T_Path,NULL,&s_ofcpath1};

statetype s_ofcpain             = {2,SPR_OFC_PAIN_1,8,NULL,NULL,&s_ofcchase1};
statetype s_ofcpain1            = {2,SPR_OFC_PAIN_1,8,NULL,NULL,&s_ofcchase1};

statetype s_ofcshoot1           = {false,SPR_OFC_SHOOT1,8,NULL,NULL,&s_ofcshoot2};
statetype s_ofcshoot2           = {false,SPR_OFC_SHOOT2,8,NULL,NULL,&s_ofcshoot3};
statetype s_ofcshoot3           = {false,SPR_OFC_SHOOT3,16,(statefunc)T_Bright,(statefunc)T_Attacking,&s_ofcchase0};

statetype s_ofcchase0           = {false,SPR_OFC_W1_1,1,(statefunc)T_Chase,(statefunc)T_Target,&s_ofcchase1};
statetype s_ofcchase1           = {false,SPR_OFC_W1_1,12,(statefunc)T_Chase,(statefunc)T_Target,&s_ofcchase2};
statetype s_ofcchase2           = {false,SPR_OFC_W2_1,8,(statefunc)T_Chase,(statefunc)T_Target,&s_ofcchase3};
statetype s_ofcchase3           = {false,SPR_OFC_W3_1,12,(statefunc)T_Chase,(statefunc)T_Target,&s_ofcchase4};
statetype s_ofcchase4           = {false,SPR_OFC_W4_1,8,(statefunc)T_Chase,(statefunc)T_Target,&s_ofcchase1};

statetype s_ofcdie1             = {false,SPR_OFC_PAIN_1,8,NULL,NULL,&s_ofcdie2};
statetype s_ofcdie2             = {false,SPR_OFC_DIE_2,8,NULL,(statefunc)A_DeathScream,&s_ofcdie3};
statetype s_ofcdie3             = {false,SPR_OFC_DIE_3,8,NULL,(statefunc)A_Thud,&s_ofcdie4};
statetype s_ofcdie4             = {false,SPR_OFC_DIE_3,0,NULL,NULL,&s_ofcdie4};


//
// mutant
//

extern  statetype s_mutstand;

extern  statetype s_mutpath1;
extern  statetype s_mutpath1s;
extern  statetype s_mutpath2;
extern  statetype s_mutpath3;
extern  statetype s_mutpath3s;
extern  statetype s_mutpath4;

extern  statetype s_mutpain;
extern  statetype s_mutpain1;

extern  statetype s_mutgiveup;

extern  statetype s_mutshoot1;
extern  statetype s_mutshoot2;
extern  statetype s_mutshoot3;
extern  statetype s_mutshoot4;

extern  statetype s_mutchase0;
extern  statetype s_mutchase1;
extern  statetype s_mutchase1s;
extern  statetype s_mutchase2;
extern  statetype s_mutchase3;
extern  statetype s_mutchase3s;
extern  statetype s_mutchase4;

extern  statetype s_mutdie1;
extern  statetype s_mutdie2;
extern  statetype s_mutdie3;
extern  statetype s_mutdie4;
extern  statetype s_mutdie5;

statetype s_mutstand            = {false,SPR_MUT_W4_1,0,(statefunc)T_Stand,NULL,&s_mutstand};

statetype s_mutpath1            = {false,SPR_MUT_W1_1,20,(statefunc)T_Path,NULL,&s_mutpath1s};
statetype s_mutpath1s           = {false,SPR_MUT_W1_1,5,NULL,NULL,&s_mutpath2};
statetype s_mutpath2            = {false,SPR_MUT_W2_1,15,(statefunc)T_Path,NULL,&s_mutpath3};
statetype s_mutpath3            = {false,SPR_MUT_W3_1,20,(statefunc)T_Path,NULL,&s_mutpath3s};
statetype s_mutpath3s           = {false,SPR_MUT_W3_1,5,NULL,NULL,&s_mutpath4};
statetype s_mutpath4            = {false,SPR_MUT_W4_1,15,(statefunc)T_Path,NULL,&s_mutpath1};

statetype s_mutpain             = {2,SPR_MUT_PAIN_1,8,NULL,NULL,&s_mutchase1};
statetype s_mutpain1            = {2,SPR_MUT_PAIN_1,8,NULL,NULL,&s_mutchase1};

statetype s_mutshoot1           = {false,SPR_MUT_SHOOT1,8,NULL,NULL,&s_mutshoot2};
statetype s_mutshoot2           = {false,SPR_MUT_SHOOT2,20,(statefunc)T_Bright,(statefunc)T_Attacking,&s_mutshoot3};
statetype s_mutshoot3           = {false,SPR_MUT_SHOOT1,8,(statefunc)T_Dark,NULL,&s_mutshoot4};
statetype s_mutshoot4           = {false,SPR_MUT_SHOOT3,20,(statefunc)T_Bright,(statefunc)T_Attacking,&s_mutchase0};

statetype s_mutchase0           = {false,SPR_MUT_W1_1,1,(statefunc)T_Chase,(statefunc)T_Target,&s_mutchase1};
statetype s_mutchase1           = {false,SPR_MUT_W1_1,16,(statefunc)T_Chase,(statefunc)T_Target,&s_mutchase2};
statetype s_mutchase2           = {false,SPR_MUT_W2_1,16,(statefunc)T_Chase,(statefunc)T_Target,&s_mutchase3};
statetype s_mutchase3           = {false,SPR_MUT_W3_1,16,(statefunc)T_Chase,(statefunc)T_Target,&s_mutchase2};
statetype s_mutchase4           = {false,SPR_MUT_W4_1,16,(statefunc)T_Chase,(statefunc)T_Target,&s_mutchase1};

statetype s_mutdie1             = {false,SPR_MUT_PAIN_1,8,NULL,NULL,&s_mutdie2};
statetype s_mutdie2             = {false,SPR_MUT_DIE_1,8,NULL,(statefunc)A_DeathScream,&s_mutdie3};
statetype s_mutdie3             = {false,SPR_MUT_DIE_2,8,NULL,(statefunc)A_Thud,&s_mutdie4};
statetype s_mutdie4             = {false,SPR_MUT_DIE_3,0,NULL,NULL,&s_mutdie4};


//
// SS
//

extern  statetype s_ssstand;

extern  statetype s_sspath1;
extern  statetype s_sspath1s;
extern  statetype s_sspath2;
extern  statetype s_sspath3;
extern  statetype s_sspath3s;
extern  statetype s_sspath4;

extern  statetype s_sspain;
extern  statetype s_sspain1;

extern  statetype s_ssshoot1;
extern  statetype s_ssshoot2;
extern  statetype s_ssshoot3;
extern  statetype s_ssshoot4;
extern  statetype s_ssshoot5;
extern  statetype s_ssshoot6;
extern  statetype s_ssshoot7;
extern  statetype s_ssshoot8;
extern  statetype s_ssshoot9;

extern  statetype s_sschase0;
extern  statetype s_sschase1;
extern  statetype s_sschase1s;
extern  statetype s_sschase2;
extern  statetype s_sschase3;
extern  statetype s_sschase3s;
extern  statetype s_sschase4;

extern  statetype s_ssdie1;
extern  statetype s_ssdie2;
extern  statetype s_ssdie3;
extern  statetype s_ssdie4;

statetype s_ssstand             = {false,SPR_SS_W4_1,0,(statefunc)T_Stand,NULL,&s_ssstand};

statetype s_sspath1             = {false,SPR_SS_W1_1,20,(statefunc)T_Path,NULL,&s_sspath1s};
statetype s_sspath1s            = {false,SPR_SS_W1_1,5,NULL,NULL,&s_sspath2};
statetype s_sspath2             = {false,SPR_SS_W2_1,15,(statefunc)T_Path,NULL,&s_sspath3};
statetype s_sspath3             = {false,SPR_SS_W3_1,20,(statefunc)T_Path,NULL,&s_sspath3s};
statetype s_sspath3s            = {false,SPR_SS_W3_1,5,NULL,NULL,&s_sspath4};
statetype s_sspath4             = {false,SPR_SS_W4_1,15,(statefunc)T_Path,NULL,&s_sspath1};

statetype s_sspain              = {2,SPR_SS_PAIN_1,8,NULL,NULL,&s_sschase1};
statetype s_sspain1             = {2,SPR_SS_PAIN_1,8,NULL,NULL,&s_sschase1};

statetype s_ssshoot1            = {false,SPR_SS_SHOOT1,20,NULL,NULL,&s_ssshoot2};
statetype s_ssshoot2            = {false,SPR_SS_SHOOT2,20,NULL,NULL,&s_ssshoot3};
statetype s_ssshoot3            = {false,SPR_SS_SHOOT3,8,(statefunc)T_Bright,(statefunc)T_Attacking,&s_ssshoot4};
statetype s_ssshoot4            = {false,SPR_SS_SHOOT2,8,(statefunc)T_Dark,NULL,&s_ssshoot5};
statetype s_ssshoot5            = {false,SPR_SS_SHOOT3,8,(statefunc)T_Bright,(statefunc)T_Attacking,&s_ssshoot6};
statetype s_ssshoot6            = {false,SPR_SS_SHOOT2,8,(statefunc)T_Dark,NULL,&s_ssshoot7};
statetype s_ssshoot7            = {false,SPR_SS_SHOOT3,8,(statefunc)T_Bright,(statefunc)T_Attacking,&s_ssshoot8};
statetype s_ssshoot8            = {false,SPR_SS_SHOOT2,8,(statefunc)T_Dark,NULL,&s_ssshoot9};
statetype s_ssshoot9            = {false,SPR_SS_SHOOT3,8,(statefunc)T_Bright,(statefunc)T_Attacking,&s_sschase0};

statetype s_sschase0            = {false,SPR_SS_W1_1,1,(statefunc)T_Chase,(statefunc)T_Target,&s_sschase1};
statetype s_sschase1            = {false,SPR_SS_W1_1,20,(statefunc)T_Chase,(statefunc)T_Target,&s_sschase2};
statetype s_sschase2            = {false,SPR_SS_W2_1,20,(statefunc)T_Chase,(statefunc)T_Target,&s_sschase3};
statetype s_sschase3            = {false,SPR_SS_W3_1,20,(statefunc)T_Chase,(statefunc)T_Target,&s_sschase4};
statetype s_sschase4            = {false,SPR_SS_W4_1,20,(statefunc)T_Chase,(statefunc)T_Target,&s_sschase1};

statetype s_ssdie1              = {false,SPR_SS_PAIN_1,8,NULL,NULL,&s_ssdie2};
statetype s_ssdie2              = {false,SPR_SS_DIE_1,8,NULL,(statefunc)A_DeathScream,&s_ssdie3};
statetype s_ssdie3              = {false,SPR_SS_DIE_2,8,NULL,(statefunc)A_Thud,&s_ssdie4};
statetype s_ssdie4              = {false,SPR_SS_DIE_3,0,NULL,NULL,&s_ssdie4};


#ifndef SPEAR
//
// hans
//
extern  statetype s_bossstand;

extern  statetype s_bosschase0;
extern  statetype s_bosschase1;
extern  statetype s_bosschase1s;
extern  statetype s_bosschase2;
extern  statetype s_bosschase3;
extern  statetype s_bosschase3s;
extern  statetype s_bosschase4;

extern  statetype s_bossdie1;
extern  statetype s_bossdie2;
extern  statetype s_bossdie3;
extern  statetype s_bossdie4;

extern  statetype s_bossshoot1;
extern  statetype s_bossshoot2;
extern  statetype s_bossshoot3;
extern  statetype s_bossshoot4;
extern  statetype s_bossshoot5;
extern  statetype s_bossshoot6;
extern  statetype s_bossshoot7;
extern  statetype s_bossshoot8;


statetype s_bossstand           = {false,SPR_BOSS_W4,0,(statefunc)T_Stand,NULL,&s_bossstand};

statetype s_bosschase0          = {false,SPR_BOSS_W1,1,(statefunc)T_Chase,(statefunc)T_Target,&s_bosschase1};
statetype s_bosschase1          = {false,SPR_BOSS_W1,20,(statefunc)T_Chase,(statefunc)T_Target,&s_bosschase2};
statetype s_bosschase2          = {false,SPR_BOSS_W2,20,(statefunc)T_Chase,(statefunc)T_Target,&s_bosschase3};
statetype s_bosschase3          = {false,SPR_BOSS_W3,20,(statefunc)T_Chase,(statefunc)T_Target,&s_bosschase4};
statetype s_bosschase4          = {false,SPR_BOSS_W4,20,(statefunc)T_Chase,(statefunc)T_Target,&s_bosschase1};

statetype s_bossdie1            = {false,SPR_BOSS_DIE1,16,NULL,(statefunc)A_DeathScream,&s_bossdie2};
statetype s_bossdie2            = {false,SPR_BOSS_DIE2,16,NULL,(statefunc)A_Thud,&s_bossdie3};
statetype s_bossdie3            = {false,SPR_BOSS_DIE3,0,NULL,NULL,&s_bossdie4};
statetype s_bossdie4            = {false,SPR_BOSS_DIE3,0,NULL,NULL,&s_bossdie4};

statetype s_bossshoot1          = {false,SPR_BOSS_SHOOT1,40,NULL,NULL,&s_bossshoot2};
statetype s_bossshoot2          = {false,SPR_BOSS_SHOOT2,12,(statefunc)T_Bright,(statefunc)T_Attacking,&s_bossshoot3};
statetype s_bossshoot3          = {false,SPR_BOSS_SHOOT3,12,(statefunc)T_Bright,(statefunc)T_Attacking,&s_bossshoot4};
statetype s_bossshoot4          = {false,SPR_BOSS_SHOOT2,12,(statefunc)T_Bright,(statefunc)T_Attacking,&s_bossshoot5};
statetype s_bossshoot5          = {false,SPR_BOSS_SHOOT3,12,(statefunc)T_Bright,(statefunc)T_Attacking,&s_bossshoot6};
statetype s_bossshoot6          = {false,SPR_BOSS_SHOOT2,12,(statefunc)T_Bright,(statefunc)T_Attacking,&s_bossshoot7};
statetype s_bossshoot7          = {false,SPR_BOSS_SHOOT1,12,(statefunc)T_Bright,(statefunc)T_Attacking,&s_bosschase0};


//
// gretel
//
extern  statetype s_gretelstand;

extern  statetype s_gretelchase0;
extern  statetype s_gretelchase1;
extern  statetype s_gretelchase1s;
extern  statetype s_gretelchase2;
extern  statetype s_gretelchase3;
extern  statetype s_gretelchase3s;
extern  statetype s_gretelchase4;

extern  statetype s_greteldie1;
extern  statetype s_greteldie2;
extern  statetype s_greteldie3;
extern  statetype s_greteldie4;

extern  statetype s_gretelshoot1;
extern  statetype s_gretelshoot2;
extern  statetype s_gretelshoot3;
extern  statetype s_gretelshoot4;
extern  statetype s_gretelshoot5;
extern  statetype s_gretelshoot6;
extern  statetype s_gretelshoot7;
extern  statetype s_gretelshoot8;


statetype s_gretelstand         = {false,SPR_GRETEL_W4,0,(statefunc)T_Stand,NULL,&s_gretelstand};

statetype s_gretelchase0        = {false,SPR_GRETEL_W1,1,(statefunc)T_Chase,(statefunc)T_Target,&s_gretelchase1};
statetype s_gretelchase1        = {false,SPR_GRETEL_W1,20,(statefunc)T_Chase,(statefunc)T_Target,&s_gretelchase2};
statetype s_gretelchase2        = {false,SPR_GRETEL_W2,20,(statefunc)T_Chase,(statefunc)T_Target,&s_gretelchase3};
statetype s_gretelchase3        = {false,SPR_GRETEL_W3,20,(statefunc)T_Chase,(statefunc)T_Target,&s_gretelchase4};
statetype s_gretelchase4        = {false,SPR_GRETEL_W4,20,(statefunc)T_Chase,(statefunc)T_Target,&s_gretelchase1};

statetype s_greteldie1          = {false,SPR_GRETEL_DIE1,16,NULL,(statefunc)A_DeathScream,&s_greteldie2};
statetype s_greteldie2          = {false,SPR_GRETEL_DIE2,16,NULL,(statefunc)A_Thud,&s_greteldie3};
statetype s_greteldie3          = {false,SPR_GRETEL_DIE3,0,NULL,NULL,&s_greteldie4};
statetype s_greteldie4          = {false,SPR_GRETEL_DIE3,0,NULL,NULL,&s_greteldie4};

statetype s_gretelshoot1        = {false,SPR_GRETEL_SHOOT1,48,NULL,NULL,&s_gretelshoot2};
statetype s_gretelshoot2        = {false,SPR_GRETEL_SHOOT2,12,(statefunc)T_Bright,(statefunc)T_Attacking,&s_gretelshoot3};
statetype s_gretelshoot3        = {false,SPR_GRETEL_SHOOT3,12,(statefunc)T_Bright,(statefunc)T_Attacking,&s_gretelshoot4};
statetype s_gretelshoot4        = {false,SPR_GRETEL_SHOOT2,12,(statefunc)T_Bright,(statefunc)T_Attacking,&s_gretelshoot5};
statetype s_gretelshoot5        = {false,SPR_GRETEL_SHOOT3,12,(statefunc)T_Bright,(statefunc)T_Attacking,&s_gretelshoot6};
statetype s_gretelshoot6        = {false,SPR_GRETEL_SHOOT2,12,(statefunc)T_Bright,(statefunc)T_Attacking,&s_gretelshoot7};
statetype s_gretelshoot7        = {false,SPR_GRETEL_SHOOT3,12,(statefunc)T_Bright,(statefunc)T_Attacking,&s_gretelchase0};
#endif


/*
===============
=
= SpawnStand
=
===============
*/

void SpawnStand (enemy_t which, int tilex, int tiley, int dir, boolean ambush)
{
    word *map;
    word tile;

    switch (which)
    {
        case en_guard:
            SpawnNewObj (tilex,tiley,&s_grdstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_officer:
            SpawnNewObj (tilex,tiley,&s_ofcstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_mutant:
            SpawnNewObj (tilex,tiley,&s_mutstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_ss:
            SpawnNewObj (tilex,tiley,&s_ssstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;
            
        case en_dog:
            //SpawnNewObj (tilex,tiley,&s_dogpath1);
	        SpawnNewObj (tilex,tiley,&s_dogstand);
            newobj->speed = SPDDOG;
            if (!loadedgame)
                gamestate.killtotal++;
            break;   
    }


    map = mapsegs[0]+(tiley<<mapshift)+tilex;
    tile = *map;
    if (tile == AMBUSHTILE)
    {
        tilemap[tilex][tiley] = 0;

        if (*(map+1) >= AREATILE)
            tile = *(map+1);
        if (*(map-mapwidth) >= AREATILE)
            tile = *(map-mapwidth);
        if (*(map+mapwidth) >= AREATILE)
            tile = *(map+mapwidth);
        if ( *(map-1) >= AREATILE)
            tile = *(map-1);

        *map = tile;
        newobj->areanumber = tile-AREATILE;

        newobj->flags |= FL_AMBUSH;
    }

    newobj->obclass = (classtype)(guardobj + which);
    
    if (mod.hitpoints[which])
        newobj->hitpoints = mod.hitpoints[which];
    else
        newobj->hitpoints = starthitpoints[gamestate.difficulty][which];
    
    if (mod.en_rotation)    
        newobj->dir = (dirtype)(dir * 2);
    else
        newobj->dir = nodir;	// APN: make actors see in any direction
        
    newobj->flags |= FL_SHOOTABLE;
    newobj->attack = false;
    
    if (ambush)
       newobj->flags |= FL_AMBUSH;
       
    if (MAPSPOT(tilex,tiley,0) < AREATILE)  // make deaf if inside wall
        newobj->flags |= FL_AMBUSH;    
}



/*
===============
=
= SpawnDeadGuard
=
===============
*/

void SpawnDeadGuard (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_grddie4);
    DEMOIF_SDL
    {
        newobj->flags |= FL_NONMARK;    // walk through moving enemy fix
    }
    newobj->obclass = inertobj;
}



#ifndef SPEAR
/*
===============
=
= SpawnBoss
=
===============
*/

void SpawnBoss (int tilex, int tiley, boolean ambush)
{
    SpawnNewObj (tilex,tiley,&s_bossstand);
    newobj->speed = SPDPATROL;

    newobj->obclass = bossobj;
    
    if (mod.hitpoints[en_boss])
        newobj->hitpoints = mod.hitpoints[en_boss];
    else
        newobj->hitpoints = starthitpoints[gamestate.difficulty][en_boss];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE;//|FL_AMBUSH;
    newobj->attack = false;
    
    if (ambush)
       newobj->flags |= FL_AMBUSH;
       
    if (MAPSPOT(tilex,tiley,0) < AREATILE)  // make deaf if inside wall
        newobj->flags |= FL_AMBUSH; 
       
    if (!loadedgame)
        gamestate.killtotal++;
}

/*
===============
=
= SpawnGretel
=
===============
*/

void SpawnGretel (int tilex, int tiley, boolean ambush)
{
    SpawnNewObj (tilex,tiley,&s_gretelstand);
    newobj->speed = SPDPATROL;

    newobj->obclass = gretelobj;
    
    if (mod.hitpoints[en_gretel])
        newobj->hitpoints = mod.hitpoints[en_gretel];
    else
        newobj->hitpoints = starthitpoints[gamestate.difficulty][en_gretel];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE;//|FL_AMBUSH;
    newobj->attack = false;
    
    if (ambush)
       newobj->flags |= FL_AMBUSH;
       
    if (MAPSPOT(tilex,tiley,0) < AREATILE)  // make deaf if inside wall
        newobj->flags |= FL_AMBUSH;    
       
    if (!loadedgame)
        gamestate.killtotal++;
}
#endif

/*
===============
=
= SpawnPatrol
=
===============
*/

void SpawnPatrol (enemy_t which, int tilex, int tiley, int dir, boolean ambush)
{
    switch (which)
    {
        case en_guard:
            SpawnNewObj (tilex,tiley,&s_grdpath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_officer:
            SpawnNewObj (tilex,tiley,&s_ofcpath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_ss:
            SpawnNewObj (tilex,tiley,&s_sspath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_mutant:
            SpawnNewObj (tilex,tiley,&s_mutpath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_dog:
            //SpawnNewObj (tilex,tiley,&s_dogpath1);
	        SpawnNewObj (tilex,tiley,&s_dogstand);
            newobj->speed = SPDDOG;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

    }

    newobj->obclass = (classtype)(guardobj+which);
    //newobj->dir = (dirtype)(dir*2);
    newobj->dir = nodir;	
    
    if (mod.hitpoints[which])
        newobj->hitpoints = mod.hitpoints[which];
    else
        newobj->hitpoints = starthitpoints[gamestate.difficulty][which];
        
    newobj->distance = TILEGLOBAL;
    newobj->flags |= FL_SHOOTABLE;
    newobj->attack = false;
    
    if (ambush)
       newobj->flags |= FL_AMBUSH;
       
    newobj->active = ac_yes;

    actorat[newobj->tilex][newobj->tiley] = NULL;           // don't use original spot

    switch (dir)
    {
        case 0:
            newobj->tilex++;
            break;
        case 1:
            newobj->tiley--;
            break;
        case 2:
            newobj->tilex--;
            break;
        case 3:
            newobj->tiley++;
            break;
    }

    actorat[newobj->tilex][newobj->tiley] = newobj;
}



/*
==================
=
= A_DeathScream
=
==================
*/


void A_DeathScream (objtype *ob)
{
	switch (ob->obclass)
	{
    case mutantobj:         
    case guardobj:        
	case officerobj:
	case ssobj:
		{
           int sounds[2]={ DEATHSCREAM7SND,
                           DEATHSCREAM8SND};
		   PlaySoundLocActor(sounds[US_RndT()%2],ob);
		}
		break;

	case bossobj:
	case schabbobj:
	case realhitlerobj:
	case gretelobj:
	case giftobj: 
	case fatobj:
        if (bossfix) 
        {
           int sounds[2]={ DEATHSCREAM7SND,
                           DEATHSCREAM8SND};
		   PlaySoundLocActor(sounds[US_RndT()%2],ob);
		} 
		break;	
		
	case dogobj:
		SD_PlaySound(DOGDEATHSND);
		break;		
		
	case mechahitlerobj:
        SD_PlaySound(DIESND);    // APN: Hitler repeats himself
		break;	
	case fakeobj:
		SD_PlaySound(HITLERHASND);
		break;	
    }
}


/*
=============================================================================

                                SPEAR ACTORS

=============================================================================
*/

#ifdef SPEAR

void T_Launch (objtype *ob);
void T_Will (objtype *ob);

extern  statetype s_angelshoot1;
extern  statetype s_deathshoot1;
extern  statetype s_spark1;

//
// trans
//
extern  statetype s_transstand;

extern  statetype s_transchase1;
extern  statetype s_transchase1s;
extern  statetype s_transchase2;
extern  statetype s_transchase3;
extern  statetype s_transchase3s;
extern  statetype s_transchase4;

extern  statetype s_transdie0;
extern  statetype s_transdie01;
extern  statetype s_transdie1;
extern  statetype s_transdie2;
extern  statetype s_transdie3;
extern  statetype s_transdie4;

extern  statetype s_transshoot1;
extern  statetype s_transshoot2;
extern  statetype s_transshoot3;
extern  statetype s_transshoot4;
extern  statetype s_transshoot5;
extern  statetype s_transshoot6;
extern  statetype s_transshoot7;
extern  statetype s_transshoot8;


statetype s_transstand          = {false,SPR_TRANS_W1,0,(statefunc)T_Stand,NULL,&s_transstand};

statetype s_transchase1         = {false,SPR_TRANS_W1,10,(statefunc)T_Chase,NULL,&s_transchase1s};
statetype s_transchase1s        = {false,SPR_TRANS_W1,3,NULL,NULL,&s_transchase2};
statetype s_transchase2         = {false,SPR_TRANS_W2,8,(statefunc)T_Chase,NULL,&s_transchase3};
statetype s_transchase3         = {false,SPR_TRANS_W3,10,(statefunc)T_Chase,NULL,&s_transchase3s};
statetype s_transchase3s        = {false,SPR_TRANS_W3,3,NULL,NULL,&s_transchase4};
statetype s_transchase4         = {false,SPR_TRANS_W4,8,(statefunc)T_Chase,NULL,&s_transchase1};

statetype s_transdie0           = {false,SPR_TRANS_W1,1,NULL,(statefunc)A_DeathScream,&s_transdie01};
statetype s_transdie01          = {false,SPR_TRANS_W1,1,NULL,NULL,&s_transdie1};
statetype s_transdie1           = {false,SPR_TRANS_DIE1,15,NULL,NULL,&s_transdie2};
statetype s_transdie2           = {false,SPR_TRANS_DIE2,15,NULL,NULL,&s_transdie3};
statetype s_transdie3           = {false,SPR_TRANS_DIE3,15,NULL,NULL,&s_transdie4};
statetype s_transdie4           = {false,SPR_TRANS_DEAD,0,NULL,NULL,&s_transdie4};

statetype s_transshoot1         = {false,SPR_TRANS_SHOOT1,30,NULL,NULL,&s_transshoot2};
statetype s_transshoot2         = {false,SPR_TRANS_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_transshoot3};
statetype s_transshoot3         = {false,SPR_TRANS_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_transshoot4};
statetype s_transshoot4         = {false,SPR_TRANS_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_transshoot5};
statetype s_transshoot5         = {false,SPR_TRANS_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_transshoot6};
statetype s_transshoot6         = {false,SPR_TRANS_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_transshoot7};
statetype s_transshoot7         = {false,SPR_TRANS_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_transshoot8};
statetype s_transshoot8         = {false,SPR_TRANS_SHOOT1,10,NULL,NULL,&s_transchase1};


/*
===============
=
= SpawnTrans
=
===============
*/

void SpawnTrans (int tilex, int tiley)
{
    //        word *map;
    //        word tile;

    if (SoundBlasterPresent && DigiMode != sds_Off)
        s_transdie01.tictime = 105;

    SpawnNewObj (tilex,tiley,&s_transstand);
    newobj->obclass = transobj;
    
    if (mod.hitpoints[en_trans])
        newobj->hitpoints = mod.hitpoints[en_trans];
    else
        newobj->hitpoints = starthitpoints[gamestate.difficulty][en_trans];
        
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}


//
// uber
//
void T_UShoot (objtype *ob);

extern  statetype s_uberstand;

extern  statetype s_uberchase1;
extern  statetype s_uberchase1s;
extern  statetype s_uberchase2;
extern  statetype s_uberchase3;
extern  statetype s_uberchase3s;
extern  statetype s_uberchase4;

extern  statetype s_uberdie0;
extern  statetype s_uberdie01;
extern  statetype s_uberdie1;
extern  statetype s_uberdie2;
extern  statetype s_uberdie3;
extern  statetype s_uberdie4;
extern  statetype s_uberdie5;

extern  statetype s_ubershoot1;
extern  statetype s_ubershoot2;
extern  statetype s_ubershoot3;
extern  statetype s_ubershoot4;
extern  statetype s_ubershoot5;
extern  statetype s_ubershoot6;
extern  statetype s_ubershoot7;


statetype s_uberstand           = {false,SPR_UBER_W1,0,(statefunc)T_Stand,NULL,&s_uberstand};

statetype s_uberchase1          = {false,SPR_UBER_W1,10,(statefunc)T_Chase,NULL,&s_uberchase1s};
statetype s_uberchase1s         = {false,SPR_UBER_W1,3,NULL,NULL,&s_uberchase2};
statetype s_uberchase2          = {false,SPR_UBER_W2,8,(statefunc)T_Chase,NULL,&s_uberchase3};
statetype s_uberchase3          = {false,SPR_UBER_W3,10,(statefunc)T_Chase,NULL,&s_uberchase3s};
statetype s_uberchase3s         = {false,SPR_UBER_W3,3,NULL,NULL,&s_uberchase4};
statetype s_uberchase4          = {false,SPR_UBER_W4,8,(statefunc)T_Chase,NULL,&s_uberchase1};

statetype s_uberdie0            = {false,SPR_UBER_W1,1,NULL,(statefunc)A_DeathScream,&s_uberdie01};
statetype s_uberdie01           = {false,SPR_UBER_W1,1,NULL,NULL,&s_uberdie1};
statetype s_uberdie1            = {false,SPR_UBER_DIE1,15,NULL,NULL,&s_uberdie2};
statetype s_uberdie2            = {false,SPR_UBER_DIE2,15,NULL,NULL,&s_uberdie3};
statetype s_uberdie3            = {false,SPR_UBER_DIE3,15,NULL,NULL,&s_uberdie4};
statetype s_uberdie4            = {false,SPR_UBER_DIE4,15,NULL,NULL,&s_uberdie5};
statetype s_uberdie5            = {false,SPR_UBER_DEAD,0,NULL,NULL,&s_uberdie5};

statetype s_ubershoot1          = {false,SPR_UBER_SHOOT1,30,NULL,NULL,&s_ubershoot2};
statetype s_ubershoot2          = {false,SPR_UBER_SHOOT2,12,NULL,(statefunc)T_UShoot,&s_ubershoot3};
statetype s_ubershoot3          = {false,SPR_UBER_SHOOT3,12,NULL,(statefunc)T_UShoot,&s_ubershoot4};
statetype s_ubershoot4          = {false,SPR_UBER_SHOOT4,12,NULL,(statefunc)T_UShoot,&s_ubershoot5};
statetype s_ubershoot5          = {false,SPR_UBER_SHOOT3,12,NULL,(statefunc)T_UShoot,&s_ubershoot6};
statetype s_ubershoot6          = {false,SPR_UBER_SHOOT2,12,NULL,(statefunc)T_UShoot,&s_ubershoot7};
statetype s_ubershoot7          = {false,SPR_UBER_SHOOT1,12,NULL,NULL,&s_uberchase1};


/*
===============
=
= SpawnUber
=
===============
*/

void SpawnUber (int tilex, int tiley)
{
    if (SoundBlasterPresent && DigiMode != sds_Off)
        s_uberdie01.tictime = 70;

    SpawnNewObj (tilex,tiley,&s_uberstand);
    newobj->obclass = uberobj;
    
    if (mod.hitpoints[en_uber])
        newobj->hitpoints = mod.hitpoints[en_uber];
    else
        newobj->hitpoints = starthitpoints[gamestate.difficulty][en_uber];
        
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
===============
=
= T_UShoot
=
===============
*/

void T_UShoot (objtype *ob)
{
    int     dx,dy,dist;

    T_Shoot (ob);

    dx = abs(ob->tilex - player->tilex);
    dy = abs(ob->tiley - player->tiley);
    dist = dx>dy ? dx : dy;
    if (dist <= 1)
        TakeDamage (10,ob);
}


//
// will
//
extern  statetype s_willstand;

extern  statetype s_willchase1;
extern  statetype s_willchase1s;
extern  statetype s_willchase2;
extern  statetype s_willchase3;
extern  statetype s_willchase3s;
extern  statetype s_willchase4;

extern  statetype s_willdie1;
extern  statetype s_willdie2;
extern  statetype s_willdie3;
extern  statetype s_willdie4;
extern  statetype s_willdie5;
extern  statetype s_willdie6;

extern  statetype s_willshoot1;
extern  statetype s_willshoot2;
extern  statetype s_willshoot3;
extern  statetype s_willshoot4;
extern  statetype s_willshoot5;
extern  statetype s_willshoot6;


statetype s_willstand           = {false,SPR_WILL_W1,0,(statefunc)T_Stand,NULL,&s_willstand};

statetype s_willchase1          = {false,SPR_WILL_W1,10,(statefunc)T_Will,NULL,&s_willchase1s};
statetype s_willchase1s         = {false,SPR_WILL_W1,3,NULL,NULL,&s_willchase2};
statetype s_willchase2          = {false,SPR_WILL_W2,8,(statefunc)T_Will,NULL,&s_willchase3};
statetype s_willchase3          = {false,SPR_WILL_W3,10,(statefunc)T_Will,NULL,&s_willchase3s};
statetype s_willchase3s         = {false,SPR_WILL_W3,3,NULL,NULL,&s_willchase4};
statetype s_willchase4          = {false,SPR_WILL_W4,8,(statefunc)T_Will,NULL,&s_willchase1};

statetype s_willdeathcam        = {false,SPR_WILL_W1,1,NULL,NULL,&s_willdie1};

statetype s_willdie1            = {false,SPR_WILL_W1,1,NULL,(statefunc)A_DeathScream,&s_willdie2};
statetype s_willdie2            = {false,SPR_WILL_W1,10,NULL,NULL,&s_willdie3};
statetype s_willdie3            = {false,SPR_WILL_DIE1,10,NULL,NULL,&s_willdie4};
statetype s_willdie4            = {false,SPR_WILL_DIE2,10,NULL,NULL,&s_willdie5};
statetype s_willdie5            = {false,SPR_WILL_DIE3,10,NULL,NULL,&s_willdie6};
statetype s_willdie6            = {false,SPR_WILL_DEAD,20,NULL,NULL,&s_willdie6};

statetype s_willshoot1          = {false,SPR_WILL_SHOOT1,30,NULL,NULL,&s_willshoot2};
statetype s_willshoot2          = {false,SPR_WILL_SHOOT2,10,NULL,(statefunc)T_Launch,&s_willshoot3};
statetype s_willshoot3          = {false,SPR_WILL_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_willshoot4};
statetype s_willshoot4          = {false,SPR_WILL_SHOOT4,10,NULL,(statefunc)T_Shoot,&s_willshoot5};
statetype s_willshoot5          = {false,SPR_WILL_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_willshoot6};
statetype s_willshoot6          = {false,SPR_WILL_SHOOT4,10,NULL,(statefunc)T_Shoot,&s_willchase1};


/*
===============
=
= SpawnWill
=
===============
*/

void SpawnWill (int tilex, int tiley)
{
    if (SoundBlasterPresent && DigiMode != sds_Off)
        s_willdie2.tictime = 70;

    SpawnNewObj (tilex,tiley,&s_willstand);
    newobj->obclass = willobj;
    
    if (mod.hitpoints[en_will])
        newobj->hitpoints = mod.hitpoints[en_will];
    else
        newobj->hitpoints = starthitpoints[gamestate.difficulty][en_will];
        
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
================
=
= T_Will
=
================
*/

void T_Will (objtype *ob)
{
    int32_t move;
    int     dx,dy,dist;
    boolean dodge;

    dodge = false;
    dx = abs(ob->tilex - player->tilex);
    dy = abs(ob->tiley - player->tiley);
    dist = dx>dy ? dx : dy;

    if (CheckLine(ob))                                              // got a shot at player?
    {
        ob->hidden = false;
        if ( (unsigned) US_RndT() < (tics<<3) && objfreelist)
        {
            //
            // go into attack frame
            //
            if (ob->obclass == willobj)
                NewState (ob,&s_willshoot1);
            else if (ob->obclass == angelobj)
                NewState (ob,&s_angelshoot1);
            else
                NewState (ob,&s_deathshoot1);
            return;
        }
        dodge = true;
    }
    else
        ob->hidden = true;

    if (ob->dir == nodir)
    {
        if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            TryWalk(ob);
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        if (dist <4)
            SelectRunDir (ob);
        else if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

}


//
// death
//
extern  statetype s_deathstand;

extern  statetype s_deathchase1;
extern  statetype s_deathchase1s;
extern  statetype s_deathchase2;
extern  statetype s_deathchase3;
extern  statetype s_deathchase3s;
extern  statetype s_deathchase4;

extern  statetype s_deathdie1;
extern  statetype s_deathdie2;
extern  statetype s_deathdie3;
extern  statetype s_deathdie4;
extern  statetype s_deathdie5;
extern  statetype s_deathdie6;
extern  statetype s_deathdie7;
extern  statetype s_deathdie8;
extern  statetype s_deathdie9;

extern  statetype s_deathshoot1;
extern  statetype s_deathshoot2;
extern  statetype s_deathshoot3;
extern  statetype s_deathshoot4;
extern  statetype s_deathshoot5;


statetype s_deathstand          = {false,SPR_DEATH_W1,0,(statefunc)T_Stand,NULL,&s_deathstand};

statetype s_deathchase1         = {false,SPR_DEATH_W1,10,(statefunc)T_Will,NULL,&s_deathchase1s};
statetype s_deathchase1s        = {false,SPR_DEATH_W1,3,NULL,NULL,&s_deathchase2};
statetype s_deathchase2         = {false,SPR_DEATH_W2,8,(statefunc)T_Will,NULL,&s_deathchase3};
statetype s_deathchase3         = {false,SPR_DEATH_W3,10,(statefunc)T_Will,NULL,&s_deathchase3s};
statetype s_deathchase3s        = {false,SPR_DEATH_W3,3,NULL,NULL,&s_deathchase4};
statetype s_deathchase4         = {false,SPR_DEATH_W4,8,(statefunc)T_Will,NULL,&s_deathchase1};

statetype s_deathdeathcam       = {false,SPR_DEATH_W1,1,NULL,NULL,&s_deathdie1};

statetype s_deathdie1           = {false,SPR_DEATH_W1,1,NULL,(statefunc)A_DeathScream,&s_deathdie2};
statetype s_deathdie2           = {false,SPR_DEATH_W1,10,NULL,NULL,&s_deathdie3};
statetype s_deathdie3           = {false,SPR_DEATH_DIE1,10,NULL,NULL,&s_deathdie4};
statetype s_deathdie4           = {false,SPR_DEATH_DIE2,10,NULL,NULL,&s_deathdie5};
statetype s_deathdie5           = {false,SPR_DEATH_DIE3,10,NULL,NULL,&s_deathdie6};
statetype s_deathdie6           = {false,SPR_DEATH_DIE4,10,NULL,NULL,&s_deathdie7};
statetype s_deathdie7           = {false,SPR_DEATH_DIE5,10,NULL,NULL,&s_deathdie8};
statetype s_deathdie8           = {false,SPR_DEATH_DIE6,10,NULL,NULL,&s_deathdie9};
statetype s_deathdie9           = {false,SPR_DEATH_DEAD,0,NULL,NULL,&s_deathdie9};

statetype s_deathshoot1         = {false,SPR_DEATH_SHOOT1,30,NULL,NULL,&s_deathshoot2};
statetype s_deathshoot2         = {false,SPR_DEATH_SHOOT2,10,NULL,(statefunc)T_Launch,&s_deathshoot3};
statetype s_deathshoot3         = {false,SPR_DEATH_SHOOT4,10,NULL,(statefunc)T_Shoot,&s_deathshoot4};
statetype s_deathshoot4         = {false,SPR_DEATH_SHOOT3,10,NULL,(statefunc)T_Launch,&s_deathshoot5};
statetype s_deathshoot5         = {false,SPR_DEATH_SHOOT4,10,NULL,(statefunc)T_Shoot,&s_deathchase1};


/*
===============
=
= SpawnDeath
=
===============
*/

void SpawnDeath (int tilex, int tiley)
{
    if (SoundBlasterPresent && DigiMode != sds_Off)
        s_deathdie2.tictime = 105;

    SpawnNewObj (tilex,tiley,&s_deathstand);
    newobj->obclass = deathobj;
    
    if (mod.hitpoints[en_death])
        newobj->hitpoints = mod.hitpoints[en_death];
    else
        newobj->hitpoints = starthitpoints[gamestate.difficulty][en_death];
        
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}

/*
===============
=
= T_Launch
=
===============
*/

void T_Launch (objtype *ob)
{
    int32_t deltax,deltay;
    float   angle;
    int     iangle;

    deltax = player->x - ob->x;
    deltay = ob->y - player->y;
    angle = (float) atan2 ((float) deltay, (float) deltax);
    if (angle<0)
        angle = (float) (M_PI*2+angle);
    iangle = (int) (angle/(M_PI*2)*ANGLES);
    if (ob->obclass == deathobj)
    {
        T_Shoot (ob);
        if (ob->state == &s_deathshoot2)
        {
            iangle-=4;
            if (iangle<0)
                iangle+=ANGLES;
        }
        else
        {
            iangle+=4;
            if (iangle>=ANGLES)
                iangle-=ANGLES;
        }
    }

    GetNewActor ();
    newobj->state = &s_rocket;
    newobj->ticcount = 1;

    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->obclass = rocketobj;
    switch(ob->obclass)
    {
        case deathobj:
            newobj->state = &s_flame;
            newobj->obclass = en_fireobj;
            PlaySoundLocActor (KNIGHTMISSILESND,newobj);
            break;
        case angelobj:
            newobj->state = &s_spark1;
            newobj->obclass = sparkobj;
            PlaySoundLocActor (ANGELFIRESND,newobj);
            break;
        default:
            PlaySoundLocActor (MISSILEFIRESND,newobj);
    }

    newobj->dir = nodir;
    newobj->angle = iangle;
    newobj->speed = 0x2000l;
    newobj->flags = FL_NEVERMARK;
    newobj->active = ac_yes;
}



//
// angel
//
void A_Relaunch (objtype *ob);
void A_Victory (objtype *ob);
void A_StartAttack (objtype *ob);
void A_Breathing (objtype *ob);

extern  statetype s_angelstand;

extern  statetype s_angelchase1;
extern  statetype s_angelchase1s;
extern  statetype s_angelchase2;
extern  statetype s_angelchase3;
extern  statetype s_angelchase3s;
extern  statetype s_angelchase4;

extern  statetype s_angeldie1;
extern  statetype s_angeldie11;
extern  statetype s_angeldie2;
extern  statetype s_angeldie3;
extern  statetype s_angeldie4;
extern  statetype s_angeldie5;
extern  statetype s_angeldie6;
extern  statetype s_angeldie7;
extern  statetype s_angeldie8;
extern  statetype s_angeldie9;

extern  statetype s_angelshoot1;
extern  statetype s_angelshoot2;
extern  statetype s_angelshoot3;
extern  statetype s_angelshoot4;
extern  statetype s_angelshoot5;
extern  statetype s_angelshoot6;

extern  statetype s_angeltired;
extern  statetype s_angeltired2;
extern  statetype s_angeltired3;
extern  statetype s_angeltired4;
extern  statetype s_angeltired5;
extern  statetype s_angeltired6;
extern  statetype s_angeltired7;

extern  statetype s_spark1;
extern  statetype s_spark2;
extern  statetype s_spark3;
extern  statetype s_spark4;


statetype s_angelstand          = {false,SPR_ANGEL_W1,0,(statefunc)T_Stand,NULL,&s_angelstand};

statetype s_angelchase1         = {false,SPR_ANGEL_W1,10,(statefunc)T_Will,NULL,&s_angelchase1s};
statetype s_angelchase1s        = {false,SPR_ANGEL_W1,3,NULL,NULL,&s_angelchase2};
statetype s_angelchase2         = {false,SPR_ANGEL_W2,8,(statefunc)T_Will,NULL,&s_angelchase3};
statetype s_angelchase3         = {false,SPR_ANGEL_W3,10,(statefunc)T_Will,NULL,&s_angelchase3s};
statetype s_angelchase3s        = {false,SPR_ANGEL_W3,3,NULL,NULL,&s_angelchase4};
statetype s_angelchase4         = {false,SPR_ANGEL_W4,8,(statefunc)T_Will,NULL,&s_angelchase1};

statetype s_angeldie1           = {false,SPR_ANGEL_W1,1,NULL,(statefunc)A_DeathScream,&s_angeldie11};
statetype s_angeldie11          = {false,SPR_ANGEL_W1,1,NULL,NULL,&s_angeldie2};
statetype s_angeldie2           = {false,SPR_ANGEL_DIE1,10,NULL,(statefunc)A_Slurpie,&s_angeldie3};
statetype s_angeldie3           = {false,SPR_ANGEL_DIE2,10,NULL,NULL,&s_angeldie4};
statetype s_angeldie4           = {false,SPR_ANGEL_DIE3,10,NULL,NULL,&s_angeldie5};
statetype s_angeldie5           = {false,SPR_ANGEL_DIE4,10,NULL,NULL,&s_angeldie6};
statetype s_angeldie6           = {false,SPR_ANGEL_DIE5,10,NULL,NULL,&s_angeldie7};
statetype s_angeldie7           = {false,SPR_ANGEL_DIE6,10,NULL,NULL,&s_angeldie8};
statetype s_angeldie8           = {false,SPR_ANGEL_DIE7,10,NULL,NULL,&s_angeldie9};
statetype s_angeldie9           = {false,SPR_ANGEL_DEAD,130,NULL,(statefunc)A_Victory,&s_angeldie9};

statetype s_angelshoot1         = {false,SPR_ANGEL_SHOOT1,10,NULL,(statefunc)A_StartAttack,&s_angelshoot2};
statetype s_angelshoot2         = {false,SPR_ANGEL_SHOOT2,20,NULL,(statefunc)T_Launch,&s_angelshoot3};
statetype s_angelshoot3         = {false,SPR_ANGEL_SHOOT1,10,NULL,(statefunc)A_Relaunch,&s_angelshoot2};

statetype s_angeltired          = {false,SPR_ANGEL_TIRED1,40,NULL,(statefunc)A_Breathing,&s_angeltired2};
statetype s_angeltired2         = {false,SPR_ANGEL_TIRED2,40,NULL,NULL,&s_angeltired3};
statetype s_angeltired3         = {false,SPR_ANGEL_TIRED1,40,NULL,(statefunc)A_Breathing,&s_angeltired4};
statetype s_angeltired4         = {false,SPR_ANGEL_TIRED2,40,NULL,NULL,&s_angeltired5};
statetype s_angeltired5         = {false,SPR_ANGEL_TIRED1,40,NULL,(statefunc)A_Breathing,&s_angeltired6};
statetype s_angeltired6         = {false,SPR_ANGEL_TIRED2,40,NULL,NULL,&s_angeltired7};
statetype s_angeltired7         = {false,SPR_ANGEL_TIRED1,40,NULL,(statefunc)A_Breathing,&s_angelchase1};

statetype s_spark1              = {false,SPR_SPARK1,6,(statefunc)T_Projectile,NULL,&s_spark2};
statetype s_spark2              = {false,SPR_SPARK2,6,(statefunc)T_Projectile,NULL,&s_spark3};
statetype s_spark3              = {false,SPR_SPARK3,6,(statefunc)T_Projectile,NULL,&s_spark4};
statetype s_spark4              = {false,SPR_SPARK4,6,(statefunc)T_Projectile,NULL,&s_spark1};


void A_Slurpie (objtype *)
{
    SD_PlaySound(SLURPIESND);
}

void A_Breathing (objtype *)
{
    SD_PlaySound(ANGELTIREDSND);
}

/*
===============
=
= SpawnAngel
=
===============
*/

void SpawnAngel (int tilex, int tiley)
{
    if (SoundBlasterPresent && DigiMode != sds_Off)
        s_angeldie11.tictime = 105;

    SpawnNewObj (tilex,tiley,&s_angelstand);
    newobj->obclass = angelobj;
    
    if (mod.hitpoints[en_angel])
        newobj->hitpoints = mod.hitpoints[en_angel];
    else
        newobj->hitpoints = starthitpoints[gamestate.difficulty][en_angel];
        
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
=================
=
= A_Victory
=
=================
*/

void A_Victory (objtype *)
{
    playstate = ex_victorious;
}


/*
=================
=
= A_StartAttack
=
=================
*/

void A_StartAttack (objtype *ob)
{
    ob->temp1 = 0;
}


/*
=================
=
= A_Relaunch
=
=================
*/

void A_Relaunch (objtype *ob)
{
    if (++ob->temp1 == 3)
    {
        NewState (ob,&s_angeltired);
        return;
    }

    if (US_RndT()&1)
    {
        NewState (ob,&s_angelchase1);
        return;
    }
}




//
// spectre
//
void T_SpectreWait (objtype *ob);
void A_Dormant (objtype *ob);

extern  statetype s_spectrewait1;
extern  statetype s_spectrewait2;
extern  statetype s_spectrewait3;
extern  statetype s_spectrewait4;

extern  statetype s_spectrechase1;
extern  statetype s_spectrechase2;
extern  statetype s_spectrechase3;
extern  statetype s_spectrechase4;

extern  statetype s_spectredie1;
extern  statetype s_spectredie2;
extern  statetype s_spectredie3;
extern  statetype s_spectredie4;

extern  statetype s_spectrewake;

statetype s_spectrewait1        = {false,SPR_SPECTRE_W1,10,(statefunc)T_Stand,NULL,&s_spectrewait2};
statetype s_spectrewait2        = {false,SPR_SPECTRE_W2,10,(statefunc)T_Stand,NULL,&s_spectrewait3};
statetype s_spectrewait3        = {false,SPR_SPECTRE_W3,10,(statefunc)T_Stand,NULL,&s_spectrewait4};
statetype s_spectrewait4        = {false,SPR_SPECTRE_W4,10,(statefunc)T_Stand,NULL,&s_spectrewait1};

statetype s_spectrechase1       = {false,SPR_SPECTRE_W1,10,(statefunc)T_Ghosts,NULL,&s_spectrechase2};
statetype s_spectrechase2       = {false,SPR_SPECTRE_W2,10,(statefunc)T_Ghosts,NULL,&s_spectrechase3};
statetype s_spectrechase3       = {false,SPR_SPECTRE_W3,10,(statefunc)T_Ghosts,NULL,&s_spectrechase4};
statetype s_spectrechase4       = {false,SPR_SPECTRE_W4,10,(statefunc)T_Ghosts,NULL,&s_spectrechase1};

statetype s_spectredie1         = {false,SPR_SPECTRE_F1,10,NULL,NULL,&s_spectredie2};
statetype s_spectredie2         = {false,SPR_SPECTRE_F2,10,NULL,NULL,&s_spectredie3};
statetype s_spectredie3         = {false,SPR_SPECTRE_F3,10,NULL,NULL,&s_spectredie4};
statetype s_spectredie4         = {false,SPR_SPECTRE_F4,300,NULL,NULL,&s_spectrewake};
statetype s_spectrewake         = {false,SPR_SPECTRE_F4,10,NULL,(statefunc)A_Dormant,&s_spectrewake};

/*
===============
=
= SpawnSpectre
=
===============
*/

void SpawnSpectre (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_spectrewait1);
    newobj->obclass = spectreobj;
    
    if (mod.hitpoints[en_spectre])
        newobj->hitpoints = mod.hitpoints[en_spectre];
    else
        newobj->hitpoints = starthitpoints[gamestate.difficulty][en_spectre];
        
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH|FL_BONUS; // |FL_NEVERMARK|FL_NONMARK;
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
===============
=
= A_Dormant
=
===============
*/

void A_Dormant (objtype *ob)
{
    int32_t     deltax,deltay;
    int         xl,xh,yl,yh;
    int         x,y;
    uintptr_t   tile;

    deltax = ob->x - player->x;
    if (deltax < -MINACTORDIST || deltax > MINACTORDIST)
        goto moveok;
    deltay = ob->y - player->y;
    if (deltay < -MINACTORDIST || deltay > MINACTORDIST)
        goto moveok;

    return;
moveok:

    xl = (ob->x-MINDIST) >> TILESHIFT;
    xh = (ob->x+MINDIST) >> TILESHIFT;
    yl = (ob->y-MINDIST) >> TILESHIFT;
    yh = (ob->y+MINDIST) >> TILESHIFT;

    for (y=yl ; y<=yh ; y++)
        for (x=xl ; x<=xh ; x++)
        {
            tile = (uintptr_t)actorat[x][y];
            if (!tile)
                continue;
            if (!ISPOINTER(tile))
                return;
            if (((objtype *)tile)->flags&FL_SHOOTABLE)
                return;
        }

        ob->flags |= FL_AMBUSH | FL_SHOOTABLE;
        ob->flags &= ~FL_ATTACKMODE;
        ob->flags &= ~FL_NONMARK;      // stuck bugfix 1
        ob->dir = nodir;
        NewState (ob,&s_spectrewait1);
}


#endif

/*
=============================================================================

                            SCHABBS / GIFT / FAT

=============================================================================
*/

#ifndef SPEAR
/*
===============
=
= SpawnGhosts
=
===============
*/

void SpawnGhosts (int which, int tilex, int tiley)
{
    switch(which)
    {
        case en_blinky:
            SpawnNewObj (tilex,tiley,&s_blinkychase1);
            break;
        case en_clyde:
            SpawnNewObj (tilex,tiley,&s_clydechase1);
            break;
        case en_pinky:
            SpawnNewObj (tilex,tiley,&s_pinkychase1);
            break;
        case en_inky:
            SpawnNewObj (tilex,tiley,&s_inkychase1);
            break;
    }

    newobj->obclass = ghostobj;
    newobj->speed = SPDDOG;

    newobj->dir = east;
    newobj->flags |= FL_SHOOTABLE;
    newobj->flags |= FL_AMBUSH;
    newobj->attack = false;
    
    if (!loadedgame)
    {
        gamestate.killtotal++;
        gamestate.killcount++;
    }
}



void    T_Gift (objtype *ob);
void    T_Fat (objtype *ob);
void    T_FatThrow (objtype *ob);

//
// schabb
//

extern  statetype s_schabbstand;

extern  statetype s_schabbchase0;
extern  statetype s_schabbchase1;
extern  statetype s_schabbchase1s;
extern  statetype s_schabbchase2;
extern  statetype s_schabbchase3;
extern  statetype s_schabbchase3s;
extern  statetype s_schabbchase4;

extern  statetype s_schabbdie1;
extern  statetype s_schabbdie2;
extern  statetype s_schabbdie3;
extern  statetype s_schabbdie4;
extern  statetype s_schabbdie5;
extern  statetype s_schabbdie6;

extern  statetype s_schabbshoot1;
extern  statetype s_schabbshoot2;

extern  statetype s_needle1;
extern  statetype s_needle2;
extern  statetype s_needle3;
extern  statetype s_needle4;

extern  statetype s_schabbdeathcam;


statetype s_schabbstand         = {false,SPR_SCHABB_W4,0,(statefunc)T_Stand,NULL,&s_schabbstand};

statetype s_schabbchase0        = {false,SPR_SCHABB_W1,1,(statefunc)T_BossChase,(statefunc)T_Target,&s_schabbchase1};
statetype s_schabbchase1        = {false,SPR_SCHABB_W1,20,(statefunc)T_BossChase,(statefunc)T_Target,&s_schabbchase2};
statetype s_schabbchase2        = {false,SPR_SCHABB_W2,20,(statefunc)T_BossChase,(statefunc)T_Target,&s_schabbchase3};
statetype s_schabbchase3        = {false,SPR_SCHABB_W3,20,(statefunc)T_BossChase,(statefunc)T_Target,&s_schabbchase4};
statetype s_schabbchase4        = {false,SPR_SCHABB_W4,20,(statefunc)T_BossChase,(statefunc)T_Target,&s_schabbchase1};

statetype s_schabbdeathcam      = {false,SPR_SCHABB_W1,1,NULL,NULL,&s_schabbdie1};

statetype s_schabbdie1  = {false,SPR_SCHABB_DIE1,16,NULL,(statefunc)A_DeathScream,&s_schabbdie2};
statetype s_schabbdie2  = {false,SPR_SCHABB_DIE2,16,NULL,(statefunc)A_Thud,&s_schabbdie3};
statetype s_schabbdie3  = {false,SPR_SCHABB_DIE3,0,NULL,NULL,&s_schabbdie4};
statetype s_schabbdie4  = {false,SPR_SCHABB_DIE3,0,NULL,NULL,&s_schabbdie4};

statetype s_schabbshoot1        = {false,SPR_SCHABB_SHOOT1,28,NULL,NULL,&s_schabbshoot2};
statetype s_schabbshoot2        = {false,SPR_SCHABB_SHOOT2,12,NULL,(statefunc)T_Attacking,&s_schabbchase0};

statetype s_needle1             = {false,SPR_HYPO1,6,(statefunc)T_Projectile,NULL,&s_needle1};

//
// gift
//
extern  statetype s_giftstand;

extern  statetype s_giftchase0;
extern  statetype s_giftchase1;
extern  statetype s_giftchase1s;
extern  statetype s_giftchase2;
extern  statetype s_giftchase3;
extern  statetype s_giftchase3s;
extern  statetype s_giftchase4;

extern  statetype s_giftdie1;
extern  statetype s_giftdie2;
extern  statetype s_giftdie3;
extern  statetype s_giftdie4;
extern  statetype s_giftdie5;
extern  statetype s_giftdie6;

extern  statetype s_giftshoot1;
extern  statetype s_giftshoot2;
extern  statetype s_giftshoot3;
extern  statetype s_giftshoot4;
extern  statetype s_giftshoot5;

extern  statetype s_needle1;
extern  statetype s_needle2;
extern  statetype s_needle3;
extern  statetype s_needle4;

extern  statetype s_giftdeathcam;

extern  statetype s_boom1;
extern  statetype s_boom2;
extern  statetype s_boom3;


statetype s_giftstand           = {false,SPR_GIFT_W4,0,(statefunc)T_Stand,NULL,&s_giftstand};

statetype s_giftchase0          = {false,SPR_GIFT_W1,1,(statefunc)T_BossChase,(statefunc)T_Target,&s_giftchase1};
statetype s_giftchase1          = {false,SPR_GIFT_W1,20,(statefunc)T_BossChase,(statefunc)T_Target,&s_giftchase2};
statetype s_giftchase2          = {false,SPR_GIFT_W2,20,(statefunc)T_BossChase,(statefunc)T_Target,&s_giftchase3};
statetype s_giftchase3          = {false,SPR_GIFT_W3,20,(statefunc)T_BossChase,(statefunc)T_Target,&s_giftchase4};
statetype s_giftchase4          = {false,SPR_GIFT_W4,20,(statefunc)T_BossChase,(statefunc)T_Target,&s_giftchase1};

statetype s_giftdeathcam        = {false,SPR_GIFT_W1,1,NULL,NULL,&s_giftdie1};

statetype s_giftdie1	= {false,SPR_GIFT_DIE1,16,NULL,(statefunc)A_DeathScream,&s_giftdie2};
statetype s_giftdie2	= {false,SPR_GIFT_DIE2,16,NULL,(statefunc)A_Thud,&s_giftdie3};
statetype s_giftdie3	= {false,SPR_GIFT_DIE3,0,NULL,NULL,&s_giftdie4};
statetype s_giftdie4    = {false,SPR_GIFT_DIE3,0,NULL,NULL,&s_giftdie4};

statetype s_giftshoot1          = {false,SPR_GIFT_SHOOT1,28,NULL,NULL,&s_giftshoot2};
statetype s_giftshoot2          = {false,SPR_GIFT_SHOOT2,12,(statefunc)T_Bright,(statefunc)T_Attacking,&s_giftshoot3};
statetype s_giftshoot3          = {false,SPR_GIFT_SHOOT4,12,(statefunc)T_Bright,(statefunc)T_AltAttack,&s_giftshoot4};
statetype s_giftshoot4          = {false,SPR_GIFT_SHOOT3,12,(statefunc)T_Bright,(statefunc)T_Attacking,&s_giftshoot5};
statetype s_giftshoot5          = {false,SPR_GIFT_SHOOT4,12,(statefunc)T_Bright,(statefunc)T_AltAttack,&s_giftchase0};

//
// fat
//
extern  statetype s_fatstand;

extern  statetype s_fatchase0;
extern  statetype s_fatchase1;
extern  statetype s_fatchase1s;
extern  statetype s_fatchase2;
extern  statetype s_fatchase3;
extern  statetype s_fatchase3s;
extern  statetype s_fatchase4;

extern  statetype s_fatdie1;
extern  statetype s_fatdie2;
extern  statetype s_fatdie3;
extern  statetype s_fatdie4;
extern  statetype s_fatdie5;
extern  statetype s_fatdie6;

extern  statetype s_fatshoot1;
extern  statetype s_fatshoot2;
extern  statetype s_fatshoot3;
extern  statetype s_fatshoot4;
extern  statetype s_fatshoot5;
extern  statetype s_fatshoot6;
extern  statetype s_fatshoot7;

extern  statetype s_afatshoot1;
extern  statetype s_afatshoot2;
extern  statetype s_afatshoot3;
extern  statetype s_afatshoot4;
extern  statetype s_afatshoot5;
extern  statetype s_afatshoot6;
extern  statetype s_afatshoot7;

extern  statetype s_needle1;
extern  statetype s_needle2;
extern  statetype s_needle3;
extern  statetype s_needle4;

extern  statetype s_fatdeathcam;


statetype s_fatstand            = {false,SPR_FAT_W4,0,(statefunc)T_Stand,NULL,&s_fatstand};

statetype s_fatchase0           = {false,SPR_FAT_W1,1,(statefunc)T_BossChase,(statefunc)T_Target,&s_fatchase1};
statetype s_fatchase1           = {false,SPR_FAT_W1,12,(statefunc)T_BossChase,(statefunc)T_Target,&s_fatchase2};
statetype s_fatchase2           = {false,SPR_FAT_W2,12,(statefunc)T_BossChase,(statefunc)T_Target,&s_fatchase3};
statetype s_fatchase3           = {false,SPR_FAT_W3,12,(statefunc)T_BossChase,(statefunc)T_Target,&s_fatchase4};
statetype s_fatchase4           = {false,SPR_FAT_W4,12,(statefunc)T_BossChase,(statefunc)T_Target,&s_fatchase1};

statetype s_fatdeathcam         = {false,SPR_FAT_W1,1,NULL,NULL,&s_fatdie1};

statetype s_fatdie1             = {false,SPR_FAT_DIE1,16,NULL,(statefunc)A_DeathScream,&s_fatdie2};
statetype s_fatdie2             = {false,SPR_FAT_DIE2,16,NULL,(statefunc)A_Thud,&s_fatdie3};
statetype s_fatdie3             = {false,SPR_FAT_DIE3,0,NULL,NULL,&s_fatdie3};

statetype s_fatshoot1 	= {false,SPR_FAT_SHOOT1,40,NULL,NULL,&s_fatshoot2};
statetype s_fatshoot2   = {false,SPR_FAT_SHOOT2,8,NULL,(statefunc)T_Shoot,&s_fatshoot3};
statetype s_fatshoot3   = {false,SPR_FAT_SHOOT3,8,NULL,(statefunc)T_Shoot,&s_fatshoot4};
statetype s_fatshoot4   = {false,SPR_FAT_SHOOT2,8,NULL,(statefunc)T_Shoot,&s_fatshoot5};
statetype s_fatshoot5   = {false,SPR_FAT_SHOOT3,8,NULL,(statefunc)T_Shoot,&s_fatshoot6};
statetype s_fatshoot6   = {false,SPR_FAT_SHOOT2,8,NULL,(statefunc)T_Shoot,&s_fatshoot7};
statetype s_fatshoot7   = {false,SPR_FAT_SHOOT3,8,NULL,(statefunc)T_Shoot,&s_fatchase0};

statetype s_afatshoot1   = {false,SPR_FAT_SHOOT1,40,NULL,NULL,&s_afatshoot2};
statetype s_afatshoot2   = {false,SPR_FAT_SHOOT2,8,(statefunc)T_Bright,(statefunc)T_Attacking,&s_afatshoot3};
statetype s_afatshoot3   = {false,SPR_FAT_SHOOT3,8,(statefunc)T_Bright,(statefunc)T_Attacking,&s_afatshoot4};
statetype s_afatshoot4   = {false,SPR_FAT_SHOOT4,8,(statefunc)T_Bright,(statefunc)T_Attacking,&s_afatshoot5};
statetype s_afatshoot5   = {false,SPR_FAT_SHOOT3,8,(statefunc)T_Bright,(statefunc)T_Attacking,&s_afatshoot6};
statetype s_afatshoot6   = {false,SPR_FAT_SHOOT2,8,(statefunc)T_Bright,(statefunc)T_Attacking,&s_afatshoot7};
statetype s_afatshoot7   = {false,SPR_FAT_SHOOT3,8,(statefunc)T_Bright,(statefunc)T_Attacking,&s_fatchase0};


/*
===============
=
= SpawnSchabbs
=
===============
*/

void SpawnSchabbs (int tilex, int tiley, boolean ambush)
{
    //if (DigiMode != sds_Off)
    //    s_schabbdie2.tictime = 140;
    //else
    //    s_schabbdie2.tictime = 5;

    SpawnNewObj (tilex,tiley,&s_schabbstand);
    newobj->speed = SPDPATROL;

    newobj->obclass = schabbobj;
    
    if (mod.hitpoints[en_schabbs])
        newobj->hitpoints = mod.hitpoints[en_schabbs];
    else
        newobj->hitpoints = starthitpoints[gamestate.difficulty][en_schabbs];
        
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE;//|FL_AMBUSH;
    newobj->attack = false;
    
    if (ambush || MAPSPOT(tilex,tiley,0) < AREATILE)
       newobj->flags |= FL_AMBUSH;    
       
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
===============
=
= SpawnGift
=
===============
*/

void SpawnGift (int tilex, int tiley, boolean ambush)
{
    //if (DigiMode != sds_Off)
    //    s_giftdie2.tictime = 140;
    //else
    //    s_giftdie2.tictime = 5;

    SpawnNewObj (tilex,tiley,&s_giftstand);
    newobj->speed = SPDPATROL;

    newobj->obclass = giftobj;
    
    if (mod.hitpoints[en_gift])
        newobj->hitpoints = mod.hitpoints[en_gift];
    else
        newobj->hitpoints = starthitpoints[gamestate.difficulty][en_gift];
        
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE;//|FL_AMBUSH;
    newobj->attack = false;
    
    if (ambush || MAPSPOT(tilex,tiley,0) < AREATILE)
       newobj->flags |= FL_AMBUSH; 
       
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
===============
=
= SpawnFat
=
===============
*/

void SpawnFat (int tilex, int tiley, boolean ambush)
{
    //if (DigiMode != sds_Off)
    //    s_fatdie2.tictime = 140;
    //else
    //    s_fatdie2.tictime = 5;

    SpawnNewObj (tilex,tiley,&s_fatstand);
    newobj->speed = SPDPATROL;

    newobj->obclass = fatobj;
    
    if (mod.hitpoints[en_fat])
        newobj->hitpoints = mod.hitpoints[en_fat];
    else
        newobj->hitpoints = starthitpoints[gamestate.difficulty][en_fat];
        
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE;//|FL_AMBUSH;
    newobj->attack = false;
    
    if (ambush || MAPSPOT(tilex,tiley,0) < AREATILE)
       newobj->flags |= FL_AMBUSH;
       
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
=================
=
= T_SchabbThrow
=
=================
*/

void T_SchabbThrow (objtype *ob)
{
    int32_t deltax,deltay;
    float   angle;
    int     iangle;
    
    deltax = player->x - ob->x;
    deltay = ob->y - player->y;
    angle = (float) atan2((float) deltay, (float) deltax);
    if (angle<0)
        angle = (float) (M_PI*2+angle);
    iangle = (int) (angle/(M_PI*2)*ANGLES);

    GetNewActor ();
    newobj->state = &s_needle1;
    newobj->ticcount = 1;

    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->obclass = needleobj;
    newobj->dir = nodir;
    newobj->angle = iangle;
    newobj->silent = false;
    
    if (mod.prj_lifetime[en_need] >= 0)
        newobj->lifetime = mod.prj_lifetime[en_need];
        
    if (mod.prj_homingfactor[en_need])
        newobj->homingfactor = mod.prj_homingfactor[en_need];
    
    if (mod.prj_speed[en_need] >= 0) 
        newobj->speed = mod.prj_speed[en_need]; 
    else   
        newobj->speed = 0x2000l;

    newobj->flags = FL_NEVERMARK;
    newobj->active = ac_yes;
    newobj->attack = false;

    PlaySoundLocActor (SCHABBSTHROWSND,newobj);
}

/*
=================
=
= T_Rocket
=
=================
*/

void T_Rocket (objtype *ob)
{
    int32_t deltax,deltay;
    float   angle;
    int     iangle;

    deltax = player->x - ob->x;
    deltay = ob->y - player->y;
    angle = (float) atan2((float) deltay, (float) deltax);
    if (angle<0)
        angle = (float) (M_PI*2+angle);
    iangle = (int) (angle/(M_PI*2)*ANGLES);

    GetNewActor ();
    newobj->state = &s_drocket;
    newobj->ticcount = 1;

    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->obclass = drocketobj;
    newobj->dir = nodir;
    newobj->angle = iangle;
    newobj->silent = false;
    
    if (mod.prj_lifetime[en_miss] >= 0)
        newobj->lifetime = mod.prj_lifetime[en_miss];
        
    if (mod.prj_homingfactor[en_miss])
        newobj->homingfactor = mod.prj_homingfactor[en_miss];    
    
    if (mod.prj_speed[en_miss] >= 0) 
        newobj->speed = mod.prj_speed[en_miss]; 
    else   
        newobj->speed = 0x2000l;
        
    newobj->flags = FL_NEVERMARK;
    newobj->flags |= FL_FULLBRIGHT;
    newobj->active = ac_yes;

#ifndef APOGEE_1_0          // T_GiftThrow will never be called in shareware v1.0
    PlaySoundLocActor (MISSILEFIRESND,newobj);
#endif
}

/*
=================
=
= T_Fire
=
=================
*/

void T_Fire (objtype *ob)
{
    int32_t deltax,deltay;
    float   angle;
    int     iangle;

    deltax = player->x - ob->x;
    deltay = ob->y - player->y;
    angle = (float) atan2((float) deltay, (float) deltax);
    if (angle<0)
        angle = (float) (M_PI*2+angle);
    iangle = (int) (angle/(M_PI*2)*ANGLES);

    GetNewActor ();
    newobj->state = &s_flame;
    newobj->ticcount = 1;

    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->obclass = en_fireobj;
    newobj->dir = nodir;
    newobj->angle = iangle;
    newobj->silent = false;
    
    if (mod.prj_lifetime[en_fire] >= 0)
        newobj->lifetime = mod.prj_lifetime[en_fire];
        
    if (mod.prj_homingfactor[en_fire])
        newobj->homingfactor = mod.prj_homingfactor[en_fire];       
    
    if (mod.prj_speed[en_fire] >= 0) 
        newobj->speed = mod.prj_speed[en_fire]; 
    else   
        newobj->speed = 0x4200l;
        
    newobj->flags = FL_NEVERMARK;
    newobj->flags |= FL_FULLBRIGHT;
    newobj->active = ac_yes;

    PlaySoundLocActor (FLAMETHROWERSND,newobj);
}

/*
=============================================================================

                                    HITLERS

=============================================================================
*/


//
// fake
//
extern  statetype s_fakestand;

extern  statetype s_fakechase1;
extern  statetype s_fakechase1s;
extern  statetype s_fakechase2;
extern  statetype s_fakechase3;
extern  statetype s_fakechase3s;
extern  statetype s_fakechase4;

extern  statetype s_fakedie1;
extern  statetype s_fakedie2;
extern  statetype s_fakedie3;
extern  statetype s_fakedie4;
extern  statetype s_fakedie5;
extern  statetype s_fakedie6;

extern  statetype s_fakeshoot1;
extern  statetype s_fakeshoot2;
extern  statetype s_fakeshoot3;
extern  statetype s_fakeshoot4;
extern  statetype s_fakeshoot5;
extern  statetype s_fakeshoot6;
extern  statetype s_fakeshoot7;
extern  statetype s_fakeshoot8;
extern  statetype s_fakeshoot9;

extern  statetype s_fire1;
extern  statetype s_fire2;

statetype s_fakestand           = {false,SPR_FAKE_W1,0,(statefunc)T_Stand,NULL,&s_fakestand};

statetype s_fakechase1          = {false,SPR_FAKE_W1,10,(statefunc)T_Fake,NULL,&s_fakechase1s};
statetype s_fakechase1s         = {false,SPR_FAKE_W1,3,NULL,NULL,&s_fakechase2};
statetype s_fakechase2          = {false,SPR_FAKE_W2,8,(statefunc)T_Fake,NULL,&s_fakechase3};
statetype s_fakechase3          = {false,SPR_FAKE_W3,10,(statefunc)T_Fake,NULL,&s_fakechase3s};
statetype s_fakechase3s         = {false,SPR_FAKE_W3,3,NULL,NULL,&s_fakechase4};
statetype s_fakechase4          = {false,SPR_FAKE_W4,8,(statefunc)T_Fake,NULL,&s_fakechase1};

statetype s_fakedie1            = {false,SPR_FAKE_DIE1,10,NULL,(statefunc)A_DeathScream,&s_fakedie2};
statetype s_fakedie2            = {false,SPR_FAKE_DIE2,10,NULL,NULL,&s_fakedie3};
statetype s_fakedie3            = {false,SPR_FAKE_DIE3,10,NULL,NULL,&s_fakedie4};
statetype s_fakedie4            = {false,SPR_FAKE_DIE4,10,NULL,NULL,&s_fakedie5};
statetype s_fakedie5            = {false,SPR_FAKE_DIE5,10,NULL,NULL,&s_fakedie6};
statetype s_fakedie6            = {false,SPR_FAKE_DEAD,0,NULL,NULL,&s_fakedie6};

statetype s_fakeshoot1          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)T_FakeFire,&s_fakeshoot2};
statetype s_fakeshoot2          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)T_FakeFire,&s_fakeshoot3};
statetype s_fakeshoot3          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)T_FakeFire,&s_fakeshoot4};
statetype s_fakeshoot4          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)T_FakeFire,&s_fakeshoot5};
statetype s_fakeshoot5          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)T_FakeFire,&s_fakeshoot6};
statetype s_fakeshoot6          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)T_FakeFire,&s_fakeshoot7};
statetype s_fakeshoot7          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)T_FakeFire,&s_fakeshoot8};
statetype s_fakeshoot8          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)T_FakeFire,&s_fakeshoot9};
statetype s_fakeshoot9          = {false,SPR_FAKE_SHOOT,8,NULL,NULL,&s_fakechase1};

statetype s_fire1               = {false,SPR_FIRE1,6,NULL,(statefunc)T_Projectile,&s_fire2};
statetype s_fire2               = {false,SPR_FIRE2,6,NULL,(statefunc)T_Projectile,&s_fire1};

//
// hitler
//
extern  statetype s_mechachase0;
extern  statetype s_mechachase1;
extern  statetype s_mechachase1s;
extern  statetype s_mechachase2;
extern  statetype s_mechachase3;
extern  statetype s_mechachase3s;
extern  statetype s_mechachase4;

extern  statetype s_mechadie1;
extern  statetype s_mechadie2;
extern  statetype s_mechadie3;
extern  statetype s_mechadie4;

extern  statetype s_mechashoot1;
extern  statetype s_mechashoot2;
extern  statetype s_mechashoot3;
extern  statetype s_mechashoot4;
extern  statetype s_mechashoot5;
extern  statetype s_mechashoot6;
extern  statetype s_mechashoot7;

extern  statetype s_hitlerchase0;
extern  statetype s_hitlerchase1;
extern  statetype s_hitlerchase1s;
extern  statetype s_hitlerchase2;
extern  statetype s_hitlerchase3;
extern  statetype s_hitlerchase3s;
extern  statetype s_hitlerchase4;

extern  statetype s_hitlerdie1;
extern  statetype s_hitlerdie2;
extern  statetype s_hitlerdie3;
extern  statetype s_hitlerdie4;
extern  statetype s_hitlerdie5;

extern  statetype s_hitlershoot1;
extern  statetype s_hitlershoot2;
extern  statetype s_hitlershoot3;
extern  statetype s_hitlershoot4;
extern  statetype s_hitlershoot5;
extern  statetype s_hitlershoot6;
extern  statetype s_hitlershoot7;

extern  statetype s_hitlerdeathcam;

statetype s_mechastand          = {false,SPR_MECHA_W4,0,(statefunc)T_Stand,NULL,&s_mechastand};

statetype s_mechachase0         = {false,SPR_MECHA_W1,1,(statefunc)T_Chase,(statefunc)T_Target,&s_mechachase1};
statetype s_mechachase1         = {false,SPR_MECHA_W1,20,(statefunc)T_Chase,(statefunc)A_MechaSound,&s_mechachase2};
statetype s_mechachase2         = {false,SPR_MECHA_W2,20,(statefunc)T_Chase,(statefunc)T_Target,&s_mechachase3};
statetype s_mechachase3         = {false,SPR_MECHA_W3,20,(statefunc)T_Chase,(statefunc)A_MechaSound,&s_mechachase4};
statetype s_mechachase4         = {false,SPR_MECHA_W4,20,(statefunc)T_Chase,(statefunc)T_Target,&s_mechachase1};

statetype s_mechadie1           = {false,SPR_MECHA_DIE1,12,NULL,(statefunc)A_DeathScream,&s_mechadie2};
statetype s_mechadie2           = {false,SPR_MECHA_DIE2,12,NULL,NULL,&s_mechadie3};
statetype s_mechadie3           = {false,SPR_MECHA_DIE3,12,NULL,(statefunc)A_HitlerMorph,&s_mechadie4};
statetype s_mechadie4           = {false,SPR_MECHA_DIE4,0,NULL,NULL,&s_mechadie4};

statetype s_mechashoot1         = {false,SPR_MECHA_SHOOT1,28,NULL,NULL,&s_mechashoot2};
statetype s_mechashoot2         = {false,SPR_MECHA_SHOOT2,8,(statefunc)T_Bright,(statefunc)T_Shoot,&s_mechashoot3};
statetype s_mechashoot3         = {false,SPR_MECHA_SHOOT3,8,(statefunc)T_Bright,(statefunc)T_Shoot,&s_mechashoot4};
statetype s_mechashoot4         = {false,SPR_MECHA_SHOOT2,8,(statefunc)T_Bright,(statefunc)T_Shoot,&s_mechashoot5};
statetype s_mechashoot5         = {false,SPR_MECHA_SHOOT3,8,(statefunc)T_Bright,(statefunc)T_Shoot,&s_mechashoot6};
statetype s_mechashoot6         = {false,SPR_MECHA_SHOOT3,8,(statefunc)T_Bright,(statefunc)T_Shoot,&s_mechashoot7};
statetype s_mechashoot7         = {false,SPR_MECHA_SHOOT2,8,(statefunc)T_Bright,(statefunc)T_Shoot,&s_mechachase0};

statetype s_hitlerstand          = {false,SPR_HITLER_W4,0,(statefunc)T_Stand,NULL,&s_hitlerstand};

statetype s_hitlerchase0        = {false,SPR_HITLER_W1,1,(statefunc)T_Chase,(statefunc)T_Target,&s_hitlerchase1};
statetype s_hitlerchase1        = {false,SPR_HITLER_W1,12,(statefunc)T_Chase,(statefunc)T_Target,&s_hitlerchase2};
statetype s_hitlerchase2        = {false,SPR_HITLER_W2,8,(statefunc)T_Chase,(statefunc)T_Target,&s_hitlerchase3};
statetype s_hitlerchase3        = {false,SPR_HITLER_W3,12,(statefunc)T_Chase,(statefunc)T_Target,&s_hitlerchase4};
statetype s_hitlerchase4        = {false,SPR_HITLER_W4,8,(statefunc)T_Chase,(statefunc)T_Target,&s_hitlerchase1};

statetype s_hitlerdeathcam      = {false,SPR_HITLER_W1,10,NULL,NULL,&s_hitlerdie1};

statetype s_hitlerdie1          = {false,SPR_HITLER_DIE1,16,NULL,(statefunc)A_DeathScream,&s_hitlerdie2};
statetype s_hitlerdie2          = {false,SPR_HITLER_DIE2,16,NULL,NULL,&s_hitlerdie3};
statetype s_hitlerdie3          = {false,SPR_HITLER_DIE3,120,NULL,(statefunc)A_Thud,&s_hitlerdie4};
statetype s_hitlerdie4          = {false,SPR_HITLER_DIE3,240,NULL,NULL,&s_hitlerdie5};
statetype s_hitlerdie5          = {false,SPR_HITLER_DIE3,0,(statefunc)A_StartDeathCam,NULL,&s_hitlerdie5};

statetype s_hitlershoot1        = {false,SPR_HITLER_SHOOT1,28,NULL,NULL,&s_hitlershoot2};
statetype s_hitlershoot2        = {false,SPR_HITLER_SHOOT2,8,(statefunc)T_Bright,(statefunc)T_Shoot,&s_hitlershoot3};
statetype s_hitlershoot3        = {false,SPR_HITLER_SHOOT3,8,(statefunc)T_Bright,(statefunc)T_Shoot,&s_hitlershoot4};
statetype s_hitlershoot4        = {false,SPR_HITLER_SHOOT2,8,(statefunc)T_Bright,(statefunc)T_Shoot,&s_hitlershoot5};
statetype s_hitlershoot5        = {false,SPR_HITLER_SHOOT3,8,(statefunc)T_Bright,(statefunc)T_Shoot,&s_hitlerchase0};



/*
===============
=
= SpawnFakeHitler
=
===============
*/

void SpawnFakeHitler (int tilex, int tiley)
{
    //if (DigiMode != sds_Off)
    //    s_hitlerdie2.tictime = 140;
    //else
    //    s_hitlerdie2.tictime = 5;

    SpawnNewObj (tilex,tiley,&s_fakestand);
    newobj->speed = SPDPATROL;

    newobj->obclass = fakeobj;
    
    if (mod.hitpoints[en_fake])
        newobj->hitpoints = mod.hitpoints[en_fake];
    else
        newobj->hitpoints = starthitpoints[gamestate.difficulty][en_fake];
        
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
===============
=
= SpawnHitler
=
===============
*/

void SpawnHitler (int tilex, int tiley, boolean ambush)
{
//        if (DigiMode != sds_Off)
//                s_hitlerdie2.tictime = 140;
//        else
//		s_hitlerdie2.tictime = 5;


    SpawnNewObj (tilex,tiley,&s_mechastand);
    newobj->speed = SPDPATROL;

    newobj->obclass = mechahitlerobj;
    
    if (mod.hitpoints[en_hitler])
        newobj->hitpoints = mod.hitpoints[en_hitler];
    else
        newobj->hitpoints = starthitpoints[gamestate.difficulty][en_hitler];
        
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE;//|FL_AMBUSH;
    newobj->attack = false;
    
    if (ambush || MAPSPOT(tilex,tiley,0) < AREATILE)
       newobj->flags |= FL_AMBUSH;
       
    if (!loadedgame)
        gamestate.killtotal++;
}

/*
===============
=
= SpawnHitler
=
===============
*/

void SpawnRHitler (int tilex, int tiley, boolean ambush)
{
    short hitpoints[4]={500,500,500,500};
    
    SpawnNewObj (tilex,tiley,&s_hitlerstand);
    newobj->speed = SPDPATROL;

    newobj->obclass = realhitlerobj;
    
    if (mod.hitpoints[22])
        newobj->hitpoints = mod.hitpoints[22];
    else
        newobj->hitpoints = hitpoints[gamestate.difficulty];
        
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE;//|FL_AMBUSH;
    newobj->attack = false;
    
    if (ambush || MAPSPOT(tilex,tiley,0) < AREATILE)
       newobj->flags |= FL_AMBUSH;
    
    if (!loadedgame)
        gamestate.killtotal++;
}

/*
===============
=
= A_HitlerMorph
=
===============
*/

void A_HitlerMorph (objtype *ob)
{
   // short hitpoints[4]={500,700,800,900};
   // if (ActorPtr->hitpoints <= 250 && ActorPtr->hitpoints+damage > 250) -> mechamorph
    short hitpoints[4]={250,250,250,250};

    SpawnNewObj (ob->tilex,ob->tiley,&s_hitlerchase1);
    //newobj->speed = SPDPATROL*6;
    newobj->speed = 2560;

    newobj->x = ob->x;
    newobj->y = ob->y;

    newobj->distance = ob->distance;
    newobj->dir = ob->dir;
    newobj->flags = ob->flags | FL_SHOOTABLE;
    newobj->flags &= ~FL_NONMARK;   // hitler stuck with nodir fix
    newobj->attack = false;

    newobj->obclass = realhitlerobj;
    
    if (mod.hitpoints[22])
        newobj->hitpoints = mod.hitpoints[22];
    else
        newobj->hitpoints = hitpoints[gamestate.difficulty];
#ifdef ADDEDFIX // 29
    if (!loadedgame)               // Count real hitler for correct kill ratios 
        gamestate.killtotal++; 
#endif
}


////////////////////////////////////////////////////////
//
// A_MechaSound
// A_Slurpie
//
////////////////////////////////////////////////////////
void A_MechaSound (objtype *ob)
{
    if (ob->areanumber >= NUMAREAS || areabyplayer[ob->areanumber])
        PlaySoundLocActor (MECHSTEPSND,ob);
}

void A_Slurpie (objtype *)
{
    SD_PlaySound(SLURPIESND);
}

/*
=================
=
= T_FakeFire
=
=================
*/

void T_FakeFire (objtype *ob)
{
    int32_t deltax,deltay;
    float   angle;
    int     iangle;

    if (!objfreelist)       // stop shooting if over MAXACTORS
    {
        NewState (ob,&s_fakechase1);
        return;
    }

    deltax = player->x - ob->x;
    deltay = ob->y - player->y;
    angle = (float) atan2((float) deltay, (float) deltax);
    if (angle<0)
        angle = (float)(M_PI*2+angle);
    iangle = (int) (angle/(M_PI*2)*ANGLES);

    GetNewActor ();
    newobj->state = &s_fire1;
    newobj->ticcount = 1;

    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->dir = nodir;
    newobj->angle = iangle;
    newobj->obclass = fireobj;
    newobj->silent = false;
    
    if (mod.prj_lifetime[en_ffir] >= 0)
        newobj->lifetime = mod.prj_lifetime[en_ffir];
        
    if (mod.prj_homingfactor[en_ffir])
        newobj->homingfactor = mod.prj_homingfactor[en_ffir];    
    
    if (mod.prj_speed[en_ffir] >= 0) 
        newobj->speed = mod.prj_speed[en_ffir]; 
    else   
        newobj->speed = 0x1200l;
        
    newobj->flags = FL_NEVERMARK;
    newobj->flags |= FL_FULLBRIGHT;
    newobj->active = ac_yes;

    PlaySoundLocActor (FLAMETHROWERSND,newobj);
}



/*
=================
=
= T_Fake
=
=================
*/

void T_Fake (objtype *ob)
{
    int32_t move;

    if (CheckLine(ob))                      // got a shot at player?
    {
        ob->hidden = false;
        if ( (unsigned) US_RndT() < (tics<<1) && objfreelist)
        {
            //
            // go into attack frame
            //
            NewState (ob,&s_fakeshoot1);
            return;
        }
    }
    else
        ob->hidden = true;

    if (ob->dir == nodir)
    {
        SelectDodgeDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        SelectDodgeDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}

#endif
/*
============================================================================

STAND

============================================================================
*/


/*
===============
=
= T_Stand
=
===============
*/

void T_Stand (objtype *ob)
{
    SightPlayer (ob);
}


/*
============================================================================

ATTACK

============================================================================
*/


/*
===============
=
= T_Attacking
=
===============
*/
void T_Attacking (objtype *ob)
{
    if (!mod.en_attack[ob->obclass] || mod.en_attack[ob->obclass] == t_defatk)
    {
        if (ob->obclass == schabbobj)                           
            T_SchabbThrow (ob);
        else if (ob->obclass == giftobj)                           
            T_Rocket (ob);    
        else if (ob->obclass == dogobj)                           
            T_Bite (ob);  
        else if (ob->obclass == fakeobj)                           
            T_FakeFire (ob);        
        else           
            T_Shoot (ob);   
    } 
    else
    {
        if (mod.en_attack[ob->obclass] == t_shoot)
            T_Shoot (ob);
        else if (mod.en_attack[ob->obclass] == t_bite)
            T_Bite (ob);
        else if (mod.en_attack[ob->obclass] == t_rocket)
            T_Rocket (ob);
        else if (mod.en_attack[ob->obclass] == t_needle)
            T_SchabbThrow (ob);
        else if (mod.en_attack[ob->obclass] == t_fire)
            T_Fire (ob);   
        else if (mod.en_attack[ob->obclass] == t_ffire)
            T_FakeFire (ob);  
    }
}

/*
===============
=
= T_AltAttack
=
===============
*/
void T_AltAttack (objtype *ob)
{
    // exception for deathknight. cycles between missiles/bullets
    if (!mod.en_attack[giftobj] || mod.en_attack[giftobj] == t_defatk)
    {
        T_Shoot (ob);
        return; 
    }
     
    if (mod.en_attack[ob->obclass] == t_shoot)
        T_Shoot (ob);
    else if (mod.en_attack[ob->obclass] == t_bite)
        T_Bite (ob);
    else if (mod.en_attack[ob->obclass] == t_rocket)
        T_Rocket (ob);
    else if (mod.en_attack[ob->obclass] == t_needle)
        T_SchabbThrow (ob);
    else if (mod.en_attack[ob->obclass] == t_fire)
        T_Fire (ob);
    else if (mod.en_attack[ob->obclass] == t_ffire)
        T_FakeFire (ob);     
}

/*
===============
=
= T_AttackFrame
=
===============
*/

void T_AttackFrame (objtype *ob)
{
            //
            // go into attack frame
            //
            switch (ob->obclass)
            {
                case guardobj:
                    NewState (ob,&s_grdshoot1);
                    break;   
                case officerobj:
                    NewState (ob,&s_ofcshoot1);
                    break;
                case mutantobj:
                    NewState (ob,&s_mutshoot1);
                    break;
                case ssobj:
                    NewState (ob,&s_ssshoot1);
                    break;
                case bossobj:
                    NewState (ob,&s_bossshoot1);
                    break;
                case gretelobj:
                    NewState (ob,&s_gretelshoot1);
                    break;
                case mechahitlerobj:
                    NewState (ob,&s_mechashoot1);
                    break;
                case realhitlerobj:
                    NewState (ob,&s_hitlershoot1);
                    break;
                case giftobj:
                    NewState (ob,&s_giftshoot1);    
                    break;
                case schabbobj:
                    NewState (ob,&s_schabbshoot1);
                    break;    
                case fatobj:    
                    if (bossfix)
                        NewState (ob,&s_afatshoot1);
                    else
                        NewState (ob,&s_fatshoot1);   
                    break;  
                case dogobj:
                    NewState (ob,&s_dogjump1);
                    break;   
            }
}

/*
=================
=
= T_Target
=
=================
*/
word shootchance[8] = {256,64,32,24,20,16,12,8};

void T_Target (objtype *ob)
{
    int32_t move,target;
    int     dx,dy,dist,chance;
    boolean dodge;

    if (gamestate.victoryflag || gamestate.victorycam)
        return;
     
    // if an enemy is set to pacman in modcfg, dont enter attackframe    
    if (mod.en_chase[ob->obclass] == t_pac)
        return;    
        
    // dont target player if on or opposite of ex-pushwall spot
    if (quarterwalls[player->tilex][player->tiley] == 15 || !areabyplayer[ob->areanumber] || !CheckLine(ob)) 
    {
		ob->hidden = true;
		return;
	}  
    // next to spot 
	else if((
	quarterwalls[player->tilex-1][player->tiley] == 15 ||
	quarterwalls[player->tilex+1][player->tiley] == 15 ||
    quarterwalls[player->tilex][player->tiley-1] == 15 ||
    quarterwalls[player->tilex][player->tiley+1] == 15) && 
    quarterwalls[ob->tilex][ob->tiley] == 15)
	{
		ob->hidden = true;
		return;
	}
	// opposite of it
    else if(
	(quarterwalls[player->tilex-1][player->tiley] == 15 && quarterwalls[ob->tilex+1][ob->tiley] == 15) ||
	(quarterwalls[player->tilex+1][player->tiley] == 15 && quarterwalls[ob->tilex-1][ob->tiley] == 15) ||
    (quarterwalls[player->tilex][player->tiley-1] == 15 && quarterwalls[ob->tilex][ob->tiley+1] == 15) ||
    (quarterwalls[player->tilex][player->tiley+1] == 15 && quarterwalls[ob->tilex][ob->tiley-1] == 15) )     
	{
		ob->hidden = true;
		return;
	}  
	
	ob->hidden = false;                           //  you see enemy, enemy sees you, k?

    //if (CheckLine(ob))      // got a shot at player?
    //{ 
        dx = abs(ob->tilex - player->tilex);
        dy = abs(ob->tiley - player->tiley);
        dist = dx>dy ? dx : dy;
        
        if (ob->obclass == schabbobj && dist <= 4 && !mod.en_chase[schabbobj])
        {
            NewState (ob,&s_schabbshoot1);            
            return;
        }
        else if (ob->obclass == dogobj && !mod.en_chase[dogobj]) 
        {
            if (LABS (player->x - obj->x) < 0x15DFF && LABS(player->y - ob->y) < 0x15DFF) 
                NewState (ob,&s_dogjump1);           
                 
            return;   
        }
        
        // if an enemy is set to melee in modcfg
        if (mod.en_chase[ob->obclass] == t_melee) 
        {
            if (LABS (player->x - obj->x) < 0x15DFF && LABS(player->y - ob->y) < 0x15DFF) 
                T_AttackFrame (ob);                
            return;   
        }
        
            
        if (dist >= 8)
          chance = 0;
        else
          chance = shootchance[dist];
          
        if (gamestate.difficulty >= 2)  // Increase chance 
		    chance <<= 1;		

        if ((dist < 8 && US_RndT() < chance) || (LABS (player->x - obj->x) < 0x15DFF && LABS(player->y - ob->y) < 0x15DFF)) // always attack in biterange
        {
            T_AttackFrame (ob);
            return;
        }
    //}

}

/*
============================================================================

CHASE

============================================================================
*/

/*
=================
=
= T_Flee
=
=================
*/
void T_Flee (objtype *ob)
{
    int32_t move,target;
    int     dx,dy,dist,chance;
    boolean dodge;
        
    ob->attack = false; 

    move = ob->speed*tics;

    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            DEMOIF_SDL
            {
                TryWalk(ob);
            }
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        SelectRunDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}

/*
=================
=
= T_Chase
=
=================
*/
void T_Chase (objtype *ob)
{
    int32_t move,target;
    int     dx,dy,dist,chance;
    boolean dodge;
        
    ob->attack = false; 

    if (gamestate.victoryflag || gamestate.victorycam)
        return;
        
    if (mod.en_chase[ob->obclass] == t_melee)
    {
        T_DogChase (ob);
        return;
    }
    else if (mod.en_chase[ob->obclass] == t_boss)
    {
        T_BossChase (ob);
        return;
    } 
    else if (mod.en_chase[ob->obclass] == t_pac)
    {
        T_Ghosts (ob);
        return;
    }
    else if (mod.en_chase[ob->obclass] == t_flee)
    {
        T_Flee (ob);
        return;
    }
 
    /*dodge = false;
    if (CheckLine(ob))      // got a shot at player?
    {
        ob->hidden = false;
        dx = abs(ob->tilex - player->tilex);
        dy = abs(ob->tiley - player->tiley);
        dist = dx>dy ? dx : dy;

#ifdef PLAYDEMOLIKEORIGINAL
        if(DEMOCOND_ORIG)
        {
            if(!dist || (dist == 1 && ob->distance < 0x4000))
                chance = 300;
            else
                chance = (tics<<4)/dist;
        }
        else
#endif
        {
            if (dist)
                chance = (tics<<4)/dist;
            else
                chance = 300;

            if (dist == 1)
            {
                target = abs(ob->x - player->x);
                if (target < 0x14000l)
                {
                    target = abs(ob->y - player->y);
                    if (target < 0x14000l)
                        chance = 300;
                }
            }
        }
        /*
        if (dist > 8)
          chance = 0;
        else
          chance = shootchance[dist];
          
        if (gamestate.difficulty >= 2)  // Increase chance 
		    chance <<= 1;		

        if ((dist < 8 && US_RndT() < 0))// || (LABS (player->x - obj->x) < 0x15DFF && LABS(player->y - ob->y) < 0x15DFF)) // always attack in biterange
        {
            //
            // go into attack frame
            //
            switch (ob->obclass)
            {
                case guardobj:
                    NewState (ob,&s_grdshoot1);
                    break;
                case officerobj:
                    NewState (ob,&s_ofcshoot1);
                    break;
                case mutantobj:
                    NewState (ob,&s_mutshoot1);
                    break;
                case ssobj:
                    NewState (ob,&s_ssshoot1);
                    break;
                case bossobj:
                    NewState (ob,&s_bossshoot1);
                    break;
                case gretelobj:
                    NewState (ob,&s_gretelshoot1);
                    break;
                case mechahitlerobj:
                    NewState (ob,&s_mechashoot1);
                    break;
                case realhitlerobj:
                    NewState (ob,&s_hitlershoot1);
                    break;
            }
            return;
        }*/
     /*   dodge = true;
    }
    else
        ob->hidden = true;*/

   /* if (ob->dir == nodir)
    {
        //if (dodge)
        if (!ob->hidden)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }*/

    move = ob->speed*tics;

    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            DEMOIF_SDL
            {
                TryWalk(ob);
            }
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        //if (dodge)
        if (!ob->hidden)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}


/*
=================
=
= T_BossChase
=
=================
*/

void T_BossChase (objtype *ob)
{
    int32_t move;
    int     dx,dy,dist;
    boolean dodge;
    
    ob->attack = false;
    
    if (gamestate.victoryflag || gamestate.victorycam)
        return;
        
    if (mod.en_chase[ob->obclass] == t_melee)
    {
        T_DogChase (ob);
        return;
    }
    else if (mod.en_chase[ob->obclass] == t_pac)
    {
        T_Ghosts (ob);
        return;
    } 
    else if (mod.en_chase[ob->obclass] == t_range)
    {
        T_Chase (ob);
        return;
    }
    else if (mod.en_chase[ob->obclass] == t_flee)
    {
        T_Flee (ob);
        return;
    }

    /*dodge = false;
    dx = abs(ob->tilex - player->tilex);
    dy = abs(ob->tiley - player->tiley);
    dist = dx>dy ? dx : dy;

    if (CheckLine(ob))                                              // got a shot at player?
    {
        ob->hidden = false;
        if ( (unsigned) US_RndT() < (tics<<3))// && objfreelist)
        {
            //
            // go into attack frame
            //
            if (bossfix)
               NewState (ob,&s_afatshoot1);
            else
               NewState (ob,&s_fatshoot1);
            return;
        }
        dodge = true;
    }
    else
        ob->hidden = true;*/

    if (ob->dir == nodir)
    {
        //if (dodge)
        if (!ob->hidden)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            TryWalk(ob);
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        //if (dist <4)
        //    SelectRunDir (ob);
        //else if (dodge)
        if (!ob->hidden)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}

/*
=================
=
= T_Ghosts
=
=================
*/

void T_Ghosts (objtype *ob)
{
    int32_t move;
    
    if (gamestate.victoryflag || gamestate.victorycam)
        return;
    
    if (mod.en_chase[ob->obclass] == t_melee)
    {
        T_DogChase (ob);
        return;
    }
    else if (mod.en_chase[ob->obclass] == t_boss)
    {
        T_BossChase (ob);
        return;
    }
    else if (mod.en_chase[ob->obclass] == t_range)
    {
        T_Chase (ob);
        return;
    }
    else if (mod.en_chase[ob->obclass] == t_flee)
    {
        T_Flee (ob);
        return;
    }

    if (ob->dir == nodir)
    {
        SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        SelectChaseDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}

/*
=================
=
= T_DogChase
=
=================
*/

void T_DogChase (objtype *ob)
{
    int32_t    move;
    int32_t    dx,dy;
    
    ob->attack = false; 

    if (gamestate.victoryflag || gamestate.victorycam)
        return;
        
    if (mod.en_chase[ob->obclass] == t_pac)
    {
        T_Ghosts (ob);
        return;
    }
    else if (mod.en_chase[ob->obclass] == t_boss)
    {
        T_BossChase (ob);
        return;
    }
    else if (mod.en_chase[ob->obclass] == t_range)
    {
        T_Chase (ob);
        return;
    }   
    else if (mod.en_chase[ob->obclass] == t_flee)
    {
        T_Flee (ob);
        return;
    } 
    
    if (ob->dir == nodir)
    {
        if (!ob->hidden)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);
            
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        //
        // check for byte range
        //
        dx = player->x - ob->x;
        if (dx<0)
            dx = -dx;
        dx -= move;
        if (dx <= 0x15DFF)//MINACTORDIST)
        {
            dy = player->y - ob->y;
            if (dy<0)
                dy = -dy;
            dy -= move;
            if (dy <= 0x15DFF)//MINACTORDIST)
            {
                //NewState (ob,&s_dogjump1);
                T_AttackFrame (ob);
                return;
            }
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        if (!ob->hidden)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}



/*
============================================================================

                                    PATH

============================================================================
*/


/*
===============
=
= SelectPathDir
=
===============
*/

void SelectPathDir (objtype *ob)
{
    unsigned spot;

    spot = MAPSPOT(ob->tilex,ob->tiley,1)-ICONARROWS;

    if (spot<8)
    {
        // new direction
        ob->dir = (dirtype)(spot);
    }

    ob->distance = TILEGLOBAL;

    if (!TryWalk (ob))
        ob->dir = nodir;
}


/*
===============
=
= T_Path
=
===============
*/

void T_Path (objtype *ob)
{
    int32_t    move;

    if (SightPlayer (ob))
        return;

    if (ob->dir == nodir)
    {
        SelectPathDir (ob);
        if (ob->dir == nodir)
            return;                                 // all movement is blocked
    }


    move = ob->speed*tics;

    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            DEMOIF_SDL
            {
                TryWalk(ob);
            }
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        if (ob->tilex>MAPSIZE || ob->tiley>MAPSIZE)
        {
            sprintf (str, "T_Path hit a wall at %u,%u, dir %u",
                ob->tilex,ob->tiley,ob->dir);
            Quit (str);
        }

        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
        move -= ob->distance;

        SelectPathDir (ob);

        if (ob->dir == nodir)
            return;                                 // all movement is blocked
    }
}


/*
=============================================================================

                                    FIGHT

=============================================================================
*/

void T_ChaseFrame (objtype *ob)
{
            //
            // go into attack frame
            //
            switch (ob->obclass)
            {
                case guardobj:
                    NewState (ob,&s_grdshoot1);
                    break;   
                case officerobj:
                    NewState (ob,&s_ofcshoot1);
                    break;
                case mutantobj:
                    NewState (ob,&s_mutshoot1);
                    break;
                case ssobj:
                    NewState (ob,&s_ssshoot1);
                    break;
                case bossobj:
                    NewState (ob,&s_bossshoot1);
                    break;
                case gretelobj:
                    NewState (ob,&s_gretelshoot1);
                    break;
                case mechahitlerobj:
                    NewState (ob,&s_mechashoot1);
                    break;
                case realhitlerobj:
                    NewState (ob,&s_hitlershoot1);
                    break;
                case giftobj:
                    NewState (ob,&s_giftshoot1);    
                    break;
                case schabbobj:
                    NewState (ob,&s_schabbshoot1);
                    break;    
                case fatobj:    
                    NewState (ob,&s_fatshoot1);   
                    break;  
                case dogobj:
                    NewState (ob,&s_dogjump1);
                    break;   
            }
}

/*
===============
=
= T_Shoot
=
= Try to damage the player, based on skill level and player's speed
=
===============
*/

void T_Shoot (objtype *ob)
{
    //int     dx,dy,dist;
    int32_t dx,dy,dist;
    int     hitchance,damage;

    hitchance = 128;

    if (ob->areanumber < NUMAREAS && !areabyplayer[ob->areanumber])
        return;
    
    madenoise = true; 
    //dx = abs(ob->tilex - player->tilex);
    //dy = abs(ob->tiley - player->tiley);
    dx = abs(ob->x - player->x);
    dy = abs(ob->y - player->y);
    dist = dx>dy ? dx:dy;
    
    if (dist >= TILEGLOBAL*16) 	// Too far away? 
		return;  
	
    if (CheckLine (ob))                    // player is not behind a wall
    {
        /*if (ob->obclass == ssobj || ob->obclass == bossobj)
            dist = dist*2/3;                                        // ss are better shots

        if (thrustspeed >= RUNSPEED)
        {
            if (ob->flags&FL_VISABLE)
                hitchance = 160-dist*16;                // player can see to dodge
            else
                hitchance = 160-dist*8;
        }
        else
        {
            if (ob->flags&FL_VISABLE)
                hitchance = 256-dist*16;                // player can see to dodge
            else
                hitchance = 256-dist*8;
        }*/
        
        if (ob->obclass == ssobj || ob->obclass == bossobj) 	// better shots 
        {
		    if (dist < TILEGLOBAL) 
			  dist = 0;	
		    else 
			  dist -= TILEGLOBAL;
		}

	    if (thrustspeed > 0)        // harder to hit when moving*/
        {	
		    if (dist >= 14*TILEGLOBAL) 
			return;
			
			dist += 2*TILEGLOBAL;
		}	

        // see if the shot was a hit

        //if (US_RndT()<hitchance) 
        if (US_RndT() < (256-dist/TILEGLOBAL*16)) 
        {
            /*if (dist<2)
                damage = US_RndT()>>2;
            else if (dist<4)
                damage = US_RndT()>>3;
            else
                damage = US_RndT()>>4;*/
                
            if (gamestate.difficulty == 0)
			   damage = (US_RndT()&3)+1;
		    else if (gamestate.difficulty == 1)
			   damage = (US_RndT()&7)+1;
			else
			   damage = (US_RndT()&7)+3;

		    if (dist < 2*TILEGLOBAL) 
			    damage <<= 2;
	        else if (dist < 4*TILEGLOBAL) 
			    damage <<= 1;   

           if (quarterwalls[player->tilex][player->tiley] != 15) // vacuum hack, dont get shoot when on pushwall spot
               TakeDamage (damage,ob);
        }
        
    switch(ob->obclass)
    {
	 case ssobj:
	   PlaySoundLocActor(SSFIRESND,ob);
	   break;
	 case giftobj:
//           PlaySoundLocActor(MISSILEFIRESND,ob); // APN: don't duplicate real missile sound
	   break;
	 case mechahitlerobj:
	 case realhitlerobj:
	 case bossobj:
         case gretelobj:          // APN: added to make Trans shoot like Hans
         case fatobj:             // APN: moved to make Uber shoot like Hans
	   PlaySoundLocActor(BOSSFIRESND,ob);
	   break;
	 case schabbobj:
	   PlaySoundLocActor(SCHABBSTHROWSND,ob);
	   break;
	 case fakeobj:
	   PlaySoundLocActor(FLAMETHROWERSND,ob);
	   break;
	 case dogobj:  
	   PlaySoundLocActor(DOGATTACKSND,ob); 
	   break;

	 default:
	   PlaySoundLocActor(NAZIFIRESND,ob);
    }
    
    }
}


/*
===============
=
= T_Bite
=
===============
*/

void T_Bite (objtype *ob)
{
    int32_t    dx,dy;
    int        damage;

    switch(ob->obclass)
    {
	 case ssobj:
	   PlaySoundLocActor(SSFIRESND,ob);
	   break;
	 case giftobj:
//           PlaySoundLocActor(MISSILEFIRESND,ob); // APN: don't duplicate real missile sound
	   break;
	 case mechahitlerobj:
	 case realhitlerobj:
	 case bossobj:
         case gretelobj:          // APN: added to make Trans shoot like Hans
         case fatobj:             // APN: moved to make Uber shoot like Hans
	   PlaySoundLocActor(BOSSFIRESND,ob);
	   break;
	 case schabbobj:
	   PlaySoundLocActor(SCHABBSTHROWSND,ob);
	   break;
	 case fakeobj:
	   PlaySoundLocActor(FLAMETHROWERSND,ob);
	   break;
	 case dogobj:  
	   PlaySoundLocActor(DOGATTACKSND,ob); 
	   break;

	 default:
	   PlaySoundLocActor(NAZIFIRESND,ob);
    }

    dx = player->x - ob->x;
    if (dx<0)
        dx = -dx;
    dx -= TILEGLOBAL;
    if (dx <= 0x15DFF)// MINACTORDIST)
    {
        dy = player->y - ob->y;
        if (dy<0)
            dy = -dy;
        dy -= TILEGLOBAL;
        if (dy <= 0x15DFF)// MINACTORDIST)
        {
            /*if (US_RndT()<180)
            {
                TakeDamage (US_RndT()>>4,ob);
                return;
            }*/
            if (gamestate.difficulty == 0) 
			     damage = (US_RndT()&3)+3;	
			else if (gamestate.difficulty == 1) 
			     damage = (US_RndT()&7)+3;	
			else
			     damage = (US_RndT()&15)+4;	
		    
		    TakeDamage (damage,ob);
		    return;
        }
    }
}


#ifndef SPEAR
/*
============================================================================

                                    BJ VICTORY

============================================================================
*/


//
// BJ victory
//

void T_BJRun (objtype *ob);
void T_BJJump (objtype *ob);
void T_BJDone (objtype *ob);
void T_BJYell (objtype *ob);

void T_DeathCam (objtype *ob);

extern  statetype s_bjrun1;
extern  statetype s_bjrun1s;
extern  statetype s_bjrun2;
extern  statetype s_bjrun3;
extern  statetype s_bjrun3s;
extern  statetype s_bjrun4;

extern  statetype s_bjjump1;
extern  statetype s_bjjump2;
extern  statetype s_bjjump3;
extern  statetype s_bjjump4;


statetype s_bjrun1              = {false,SPR_BJ_W1,12,(statefunc)T_BJRun,NULL,&s_bjrun1s};
statetype s_bjrun1s             = {false,SPR_BJ_W1,3, NULL,NULL,&s_bjrun2};
statetype s_bjrun2              = {false,SPR_BJ_W2,8,(statefunc)T_BJRun,NULL,&s_bjrun3};
statetype s_bjrun3              = {false,SPR_BJ_W3,12,(statefunc)T_BJRun,NULL,&s_bjrun3s};
statetype s_bjrun3s             = {false,SPR_BJ_W3,3, NULL,NULL,&s_bjrun4};
statetype s_bjrun4              = {false,SPR_BJ_W4,8,(statefunc)T_BJRun,NULL,&s_bjrun1};


statetype s_bjjump1             = {false,SPR_BJ_JUMP1,14,(statefunc)T_BJJump,NULL,&s_bjjump2};
statetype s_bjjump2             = {false,SPR_BJ_JUMP2,14,(statefunc)T_BJJump,(statefunc)T_BJYell,&s_bjjump3};
statetype s_bjjump3             = {false,SPR_BJ_JUMP3,14,(statefunc)T_BJJump,NULL,&s_bjjump4};
statetype s_bjjump4             = {false,SPR_BJ_JUMP4,300,NULL,(statefunc)T_BJDone,&s_bjjump4};


statetype s_deathcam            = {false,0,0,NULL,NULL,NULL};


/*
===============
=
= SpawnBJVictory
=
===============
*/

void SpawnBJVictory (void)
{
    SpawnNewObj (player->tilex,player->tiley+1,&s_bjrun1);
    newobj->x = player->x;
    newobj->y = player->y;
    newobj->obclass = bjobj;
    newobj->dir = north;
    newobj->temp1 = 6;                      // tiles to run forward
}



/*
===============
=
= T_BJRun
=
===============
*/

void T_BJRun (objtype *ob)
{
    int32_t    move;

    move = BJRUNSPEED*tics;

    while (move)
    {
        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }


        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
        move -= ob->distance;

        SelectPathDir (ob);

        if ( !(--ob->temp1) )
        {
            NewState (ob,&s_bjjump1);
            return;
        }
    }
}


/*
===============
=
= T_BJJump
=
===============
*/

void T_BJJump (objtype *ob)
{
    int32_t    move;

    move = BJJUMPSPEED*tics;
    MoveObj (ob,move);
}


/*
===============
=
= T_BJYell
=
===============
*/

void T_BJYell (objtype *ob)
{
    PlaySoundLocActor(YEAHSND,ob);  // JAB
}


/*
===============
=
= T_BJDone
=
===============
*/

void T_BJDone (objtype *)
{
    playstate = ex_victorious;                              // exit castle tile
}



//===========================================================================


/*
===============
=
= CheckPosition
=
===============
*/

boolean CheckPosition (objtype *ob)
{
    int     x,y,xl,yl,xh,yh;
    objtype *check;

    xl = (ob->x-PLAYERSIZE) >> TILESHIFT;
    yl = (ob->y-PLAYERSIZE) >> TILESHIFT;

    xh = (ob->x+PLAYERSIZE) >> TILESHIFT;
    yh = (ob->y+PLAYERSIZE) >> TILESHIFT;

    //
    // check for solid walls
    //
    for (y=yl;y<=yh;y++)
    {
        for (x=xl;x<=xh;x++)
        {
            check = actorat[x][y];
            if (check && !ISPOINTER(check))
                return false;
        }
    }

    return true;
}


/*
===============
=
= A_StartDeathCam
=
===============
*/
int32_t endtimer = 0;

void    A_StartVictoryCam (boolean boss)
{ 
    int i;    
        
  if (boss)
  {              
      // dont end the game if its not the last map
      if (!finalmap[gamestate.episode*10+mapon+1])
      {
          playstate = ex_completed;  
          return;               
      }   
      // go to secret level if there is one after last map
      /*else if (finalmap[gamestate.episode*10+mapon+1] && secretlevel[i] == gamestate.mapon+2)
      {
          playstate = ex_secretlevel; 
          return;          
      } */
        
      // if player is on another floor code, dont end it   
      //if (player->areanumber == gamestate.victoryarea-AREATILE)
      if (MAPSPOT(player->tilex,player->tiley,0) != gamestate.victoryarea) 
          return;
  }
   
     gamestate.victorycam = true;  
    
     endtimer += tics;  
          
     if ((buttonstate[bt_use] && endtimer > 120) || endtimer > 8*60)
     {
            endtimer = 0;  
            //gamestate.victorycam = false;                   
            playstate = ex_completed;  
     }
}

void  A_StartDeathCam (void)
{ 
     A_StartVictoryCam (true);
}

void    A_StartDeathCamOld (objtype *ob)
{
    int32_t dx,dy;
    float   fangle;
    int32_t xmove,ymove;
    int32_t dist;

    FinishPaletteShifts ();

    VW_WaitVBL (100);

    if (gamestate.victoryflag)
    {
        playstate = ex_victorious;                              // exit castle tile
        return;
    }

    if(usedoublebuffering) VH_UpdateScreen();

    gamestate.victoryflag = true;
    unsigned fadeheight = viewsize != 21 ? screenHeight-scaleFactor*STATUSLINES : screenHeight;
    VL_BarScaledCoord (0, 0, screenWidth, fadeheight, bordercol);
    FizzleFade(screenBuffer, 0, 0, screenWidth, fadeheight, 70, false);

    if (bordercol != VIEWCOLOR)
    {
        CA_CacheGrChunk (STARTFONT+1);
        fontnumber = 1;
        SETFONTCOLOR(15,bordercol);
        PrintX = 68; PrintY = 45;
        US_Print (STR_SEEAGAIN);
        UNCACHEGRCHUNK(STARTFONT+1);
    }
    else
    {
        CacheLump(LEVELEND_LUMP_START,LEVELEND_LUMP_END);
#ifdef JAPAN
#ifndef JAPDEMO
        CA_CacheScreen(C_LETSSEEPIC);
#endif
#else
        Write(0,7,STR_SEEAGAIN);
#endif
    }

    VW_UpdateScreen ();
    if(usedoublebuffering) VH_UpdateScreen();

    IN_UserInput(300);

    //
    // line angle up exactly
    //
    NewState (player,&s_deathcam);

    player->x = gamestate.killx;
    player->y = gamestate.killy;

    dx = ob->x - player->x;
    dy = player->y - ob->y;

    fangle = (float) atan2((float) dy, (float) dx);          // returns -pi to pi
    if (fangle<0)
        fangle = (float) (M_PI*2+fangle);

    player->angle = (short) (fangle/(M_PI*2)*ANGLES);

    //
    // try to position as close as possible without being in a wall
    //
    dist = 0x14000l;
    do
    {
        xmove = FixedMul(dist,costable[player->angle]);
        ymove = -FixedMul(dist,sintable[player->angle]);

        player->x = ob->x - xmove;
        player->y = ob->y - ymove;
        dist += 0x1000;

    } while (!CheckPosition (player));
    plux = (word)(player->x >> UNSIGNEDSHIFT);                      // scale to fit in unsigned
    pluy = (word)(player->y >> UNSIGNEDSHIFT);
    player->tilex = (word)(player->x >> TILESHIFT);         // scale to tile values
    player->tiley = (word)(player->y >> TILESHIFT);

    //
    // go back to the game
    //

    DrawPlayBorder ();

    fizzlein = true;

    switch (ob->obclass)
    {
#ifndef SPEAR
        case schabbobj:
            NewState (ob,&s_schabbdeathcam);
            break;
        case realhitlerobj:
            NewState (ob,&s_hitlerdeathcam);
            break;
        case giftobj:
            NewState (ob,&s_giftdeathcam);
            break;
        case fatobj:
            NewState (ob,&s_fatdeathcam);
            break;
#endif
    }
}

#endif
