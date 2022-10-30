// WL_DRAW.C

#include "wl_def.h"
#pragma hdrstop

#include "wl_cloudsky.h"
#include "wl_atmos.h"
#include "wl_shade.h"

/*d
=============================================================================

                               LOCAL CONSTANTS

=============================================================================
*/

// the door is the last picture before the sprites
#define DOORWALL        (PMSpriteStart-10)   // Poet: add a locked door

#define ACTORSIZE       0x4000

/*
=============================================================================

                              GLOBAL VARIABLES

=============================================================================
*/

static byte *vbuf = NULL;
unsigned vbufPitch = 0;

int32_t  deathCount = 0;
int32_t  lasttimecount;
int32_t  frameon;
boolean  fpscounter;


int fps_frames=0, fps_time=0, fps=0;

int *wallheight;
int min_wallheight;

//
// math tables
//
short *pixelangle;
int32_t finetangent[FINEANGLES/4];
fixed sintable[ANGLES+ANGLES/4];
fixed *costable = sintable+(ANGLES/4);

//
// refresh variables
//
fixed   viewx,viewy;                    // the focal point
short   viewangle;
fixed   viewsin,viewcos;

void    TransformActor (objtype *ob);
void    BuildTables (void);
void    ClearScreen (void);
int     CalcRotate (objtype *ob);
void    DrawScaleds (void);
void    CalcTics (void);
void    ThreeDRefresh (void);

#ifdef USE_SKYWALLPARALLAX
void    ScaleSkyPost();
#endif


//
// wall optimization variables
//
int     lastside;               // true for vertical
int32_t    lastintercept;
int     lasttilehit;
int     lasttexture;

//
// ray tracing variables
//
short    focaltx,focalty,viewtx,viewty;
longword xpartialup,xpartialdown,ypartialup,ypartialdown;

short   midangle,angle;

fixed   scales;
word    tilehit;
word    quarterhit;
word    darkhit;
int     pixx;

short   xtile,ytile;
short   xtilestep,ytilestep;
int32_t    xintercept,yintercept;
word    xstep,ystep;
word    xspot,yspot;
int     texdelta;
byte    spotsaw[MAPSIZE][MAPSIZE];

word horizwall[MAXWALLTILES],vertwall[MAXWALLTILES];


/*
============================================================================

                           3 - D  DEFINITIONS

============================================================================
*/

/*
========================
=
= TransformActor
=
= Takes paramaters:
=   gx,gy               : globalx/globaly of point
=
= globals:
=   viewx,viewy         : point of view
=   viewcos,viewsin     : sin/cos of viewangle
=   scale               : conversion from global value to screen value
=
= sets:
=   screenx,transx,transy,screenheight: projected edge location and size
=
========================
*/


//
// transform actor
//
void TransformActor (objtype *ob)
{
    fixed gx,gy,gxt,gyt,nx,ny;

//
// translate point to view centered coordinates
//
    gx = ob->x-viewx;
    gy = ob->y-viewy;

//
// calculate newx
//
    gxt = FixedMul(gx,viewcos);
    gyt = FixedMul(gy,viewsin);
    
    if (player_macstyle)
        nx = gxt-gyt;
    else
        nx = gxt-gyt-ACTORSIZE;     // fudge the shape forward a bit, because
                                    // the midpoint could put parts of the shape
                                    // into an adjacent wall

//
// calculate newy
//
    gxt = FixedMul(gx,viewsin);
    gyt = FixedMul(gy,viewcos);
    ny = gyt+gxt;

//
// calculate perspective ratio
//
    ob->transx = nx;
    ob->transy = ny;

    if (nx<MINDIST)                 // too close, don't overflow the divide
    {
        ob->viewheight = 0;
        return;
    }

    if (player_macstyle)
        ob->viewx = (word)(centerx + ny*scales/nx);
    else
        ob->viewx = (word)(centerx + ny*scale/nx);

//
// calculate height (heightnumerator/(nx>>8))
//
    ob->viewheight = (word)(heightnumerator/(nx>>8));
}

//==========================================================================

/*
========================
=
= TransformTile
=
= Takes paramaters:
=   tx,ty               : tile the object is centered in
=
= globals:
=   viewx,viewy         : point of view
=   viewcos,viewsin     : sin/cos of viewangle
=   scale               : conversion from global value to screen value
=
= sets:
=   screenx,transx,transy,screenheight: projected edge location and size
=
= Returns true if the tile is withing getting distance
=
========================
*/

boolean TransformTile (int tx, int ty, short *dispx, short *dispheight, boolean inwall)
{
    fixed gx,gy,gxt,gyt,nx,ny;

//
// translate point to view centered coordinates
//
    gx = ((int32_t)tx<<TILESHIFT)+0x8000-viewx;
    gy = ((int32_t)ty<<TILESHIFT)+0x8000-viewy;

//
// calculate newx
//
    gxt = FixedMul(gx,viewcos);
    gyt = FixedMul(gy,viewsin);
    
    if (player_macstyle)
        nx = gxt-gyt;
    else
        nx = gxt-gyt-0x2000;            // 0x2000 is size of object      

//
// calculate newy
//
    gxt = FixedMul(gx,viewsin);
    gyt = FixedMul(gy,viewcos);
    ny = gyt+gxt;


//
// calculate height / perspective ratio
//
    if (nx<MINDIST)                 // too close, don't overflow the divide
        *dispheight = 0;
    else
    {
        if (player_macstyle)
            *dispx = (short)(centerx + ny*scales/nx);
        else 
            *dispx = (short)(centerx + ny*scale/nx);
        *dispheight = (short)(heightnumerator/(nx>>8));
    }

//
// see if it should be grabbed
//
    if (nx<TILEGLOBAL && ny>-TILEGLOBAL/2 && ny<TILEGLOBAL/2 && !inwall)
        return true;
    else if (nx<(TILEGLOBAL) && ny>-TILEGLOBAL/2 && ny<TILEGLOBAL/2 && inwall) //bigger grab radius when item is inside wall
        return true;    
    else
        return false;
}

//==========================================================================

/*
====================
=
= CalcHeight
=
= Calculates the height of xintercept,yintercept from viewx,viewy
=
====================
*/

int CalcHeight()
{
    fixed z = FixedMul(xintercept - viewx, viewcos)
        - FixedMul(yintercept - viewy, viewsin);
    if(z < MINDIST) z = MINDIST;
    int height = heightnumerator / (z >> 8);
    if(height < min_wallheight) min_wallheight = height;
    return height;
}

//==========================================================================

/*
===================
=
= ScalePost
=
===================
*/

byte *postsource;
int postx;
int postwidth;
#ifdef USE_SKYWALLPARALLAX
byte *postsourcesky;
#endif

void ScalePost(boolean dark)
{
    int ywcount, yoffs, yw, yd, yendoffs;
    byte col;
    byte *curshades;

#ifdef USE_SKYWALLPARALLAX
    if (tilehit == 16)
    {
        ScaleSkyPost();
        return;
    }
#endif

    if (shadestrength[gamestate.episode*10+mapon+1])
        curshades = shadetable[GetShade(wallheight[postx])];

    ywcount = yd = wallheight[postx] >> 3;
    if(yd <= 0) yd = 100;

    yoffs = (viewheight / 2 - ywcount) * vbufPitch;
    if(yoffs < 0) yoffs = 0;
    yoffs += postx;

    yendoffs = viewheight / 2 + ywcount - 1;
    yw=texturesize-1;

    while(yendoffs >= viewheight)
    {
        ywcount -= texturesize/2;
        while(ywcount <= 0)
        {
            ywcount += yd;
            yw--;
        }
        yendoffs--;
    }
    if(yw < 0) return;

    if (shadestrength[gamestate.episode*10+mapon+1])
    {
        if (dark)
            col = curshades[darkt[postsource[yw]]];
        else                                            
            col = curshades[postsource[yw]];
    }
    else
    {
        if (dark)
            col = darkt[postsource[yw]];
        else
            col = postsource[yw];  
    }

    yendoffs = yendoffs * vbufPitch + postx;
    while(yoffs <= yendoffs)
    {
        vbuf[yendoffs] = col;
        ywcount -= texturesize/2;
        if(ywcount <= 0)
        {
            do
            {
                ywcount += yd;
                yw--;
            }
            while(ywcount <= 0);
            if(yw < 0) break;
            
            
            if (shadestrength[gamestate.episode*10+mapon+1])
            {
               if (dark)
                   col = curshades[darkt[postsource[yw]]];
               else                                            
                   col = curshades[postsource[yw]];
            }
            else
            {
               if (dark)
                   col = darkt[postsource[yw]];
               else
                   col = postsource[yw];  
            }

        }
        yendoffs -= vbufPitch;
    }
}

void ScalePost(void)
{
   ScalePost (false);  
}

void GlobalScalePost(byte *vidbuf, unsigned pitch)
{
    vbuf = vidbuf;
    vbufPitch = pitch;
    ScalePost();
}

#ifdef USE_SKYWALLPARALLAX
void ScaleSkyPost()
{
    int ywcount, yoffs, yendoffs, texoffs;
    byte col;
    int midy, y, skyheight;

    skyheight = viewheight;
    ywcount = wallheight[postx] >> 3;

    midy = (viewheight / 2) - 1;

    yoffs = midy * vbufPitch;
    if(yoffs < 0) yoffs = 0;
    yoffs += postx;

    yendoffs = midy + (ywcount * 2) - 1;

    if (yendoffs >= viewheight)
        yendoffs = viewheight - 1;

    int curang = pixelangle[postx] + midangle;
    if(curang < 0)
        curang += FINEANGLES;
    else if(curang >= FINEANGLES)
        curang -= FINEANGLES;
    int xtex = curang * USE_SKYWALLPARALLAX * texturesize / FINEANGLES;
    texoffs = texmask - ((xtex & (texturesize - 1)) << textureshift);

    y = yendoffs;
    yendoffs = yendoffs * vbufPitch + postx;
    while(yoffs <= yendoffs)
    {
        vbuf[yendoffs] = postsourcesky[texoffs + (y * texturesize) / skyheight];
        yendoffs -= vbufPitch;
        y--;
    }
}
#endif

/*
====================
=
= CheckAdjacentTile
=
= BrotherTank: fix for the 
= disappearing door side bug
=
====================
*/

int CheckAdjacentTile (int x, int y) 
{  // Part of 64+ Wall Tiles - Make sure within Map Boundaries 
   // to fix bug of not allowing 64+ in outer edge of map 
   // Checks Adjacent Tile for Bit 7 Set - Door or Pushwall 
   if ((y-1 >= 0) && (tilemap[x][y-1] & 0x80) 
   || (y+1 <= 63) && (tilemap[x][y+1] & 0x80) 
   || (x-1 >= 0) && (tilemap[x-1][y] & 0x80) 
   || (x+1 <= 63) && (tilemap[x+1][y] & 0x80)) 
    return 1; 
   else 
    return 0; 
} 

/*
====================
=
= HitVertWall
=
= tilehit bit 7 is 0, because it's not a door tile
= if bit 6 is 1 and the adjacent tile is a door tile, use door side pic
=
====================
*/

void HitVertWall (void)
{
    int wallpic;
    int texture;
    boolean dark;
    boolean flip;

    //if (MAPSPOT(xtile,ytile,2) == 666 || MAPSPOT(xtile,ytile,2) == 667 || darkhit == 6 || darkhit == 7) 
    if (darkhit == 6 || darkhit == 7) 
      dark = true;
    else 
      dark = false;
            
    texture = ((yintercept+texdelta)>>texfromfixedshift)&texmask;
    if (xtilestep == -1)
    {            
        texture = texmask-texture; 
        xintercept += TILEGLOBAL;
        
        //if (mirwall[MAPSPOT(xtile,ytile,0)] >= 2)           // flip the wall ( 1 on side, 2 other, 3 both)
        if (mirwall[tilemap[xtile][ytile]] >= 2)  
            texture = texmask-texture; 
    }
    
    //if (mirwall[wallpic] > 0)    
    //if (mirwall[MAPSPOT(xtile,ytile,0)] >= 2)           // flip the wall ( 1 on side, 2 other, 3 both)
    //   texture = TEXTUREMASK-texture; 

    /*if(lastside==1 && lastintercept==xtile && lasttilehit==tilehit && !(lasttilehit & BIT_WALL) && !CheckAdjacentTile(xtile,ytile))
    {
        if((pixx&3) && texture == lasttexture)
        {
            if (dark)        
               ScalePost(true);
            else
               ScalePost(false);   
            postx = pixx;
            wallheight[pixx] = wallheight[pixx-1];
            return;
        }
        if (dark)        
           ScalePost(true);
        else
           ScalePost(false); 
        wallheight[pixx] = CalcHeight();
        postsource+=texture-lasttexture;
        postwidth=1;
        postx=pixx;
        lasttexture=texture;
        return;
    }

    if(lastside!=-1) 
    {
        if (dark)        
           ScalePost(true);
        else
           ScalePost(false); 
    }*/

    lastside=1;
    lastintercept=xtile;
    lasttilehit=tilehit;
    lasttexture=texture;
    wallheight[pixx] = CalcHeight();
    postx = pixx;
    postwidth = 1;

    if (tilehit & BIT_WALL)
    {                                                               // check for adjacent doors
        ytile = (short)(yintercept>>TILESHIFT);
        if ( tilemap[xtile-xtilestep][ytile]&BIT_DOOR &&  MAPSPOT (xtile-xtilestep,ytile,1) != 237
           && MAPSPOT (xtile-xtilestep,ytile,1) != 238 && MAPSPOT (xtile-xtilestep,ytile,1) != 239) // 666 removes door frame
            wallpic = DOORWALL+3;
        else
            wallpic = vertwall[tilehit & ~BIT_WALL];
    }
    else
        wallpic = vertwall[tilehit];
        
    postsource = PM_GetTexture(wallpic) + texture;
#ifdef USE_SKYWALLPARALLAX
    postsourcesky = postsource - texture;
#endif

    if (dark)        
        ScalePost(true);
    else
        ScalePost(false);   
}


/*
====================
=
= HitHorizWall
=
= tilehit bit 7 is 0, because it's not a door tile
= if bit 6 is 1 and the adjacent tile is a door tile, use door side pic
=
====================
*/

void HitHorizWall (void)
{
    int wallpic;
    int texture;
    boolean dark;
    
    if (darkhit == 3 || darkhit == 7) 
       dark = true;
    else 
       dark = false;
      
    texture = ((xintercept+texdelta)>>texfromfixedshift)&texmask;
    if (ytilestep == -1)
        yintercept += TILEGLOBAL;
    else
        texture = texmask-texture;
          
   // if (ytilestep != -1 && (mirwall[MAPSPOT(xtile,ytile,0)] == 1 || mirwall[MAPSPOT(xtile,ytile,0)] == 3))           // flip the wall ( 1 on side, 2 other, 3 both)
    if (ytilestep != -1 && (mirwall[tilemap[xtile][ytile]] == 1 || mirwall[tilemap[xtile][ytile]] == 3))
        texture = texmask-texture;    

    /*if(lastside==0 && lastintercept==ytile && lasttilehit==tilehit && !(lasttilehit & BIT_WALL) && !CheckAdjacentTile(xtile,ytile))
    {
        if((pixx&3) && texture == lasttexture)
        {
            if (dark)        
               ScalePost(true);
            else
               ScalePost(false);            
            postx=pixx;
            wallheight[pixx] = wallheight[pixx-1];
            return;
        }
        if (dark)        
           ScalePost(true);
        else
           ScalePost(false); 
        wallheight[pixx] = CalcHeight();
        postsource+=texture-lasttexture;
        postwidth=1;
        postx=pixx;
        lasttexture=texture;
        return;
    }

    if(lastside!=-1) 
    {
        if (dark)        
           ScalePost(true);
        else
           ScalePost(false);              
    }*/

    lastside=0;
    lastintercept=ytile;
    lasttilehit=tilehit;
    lasttexture=texture;
    wallheight[pixx] = CalcHeight();
    postx = pixx;
    postwidth = 1;

    if (tilehit & BIT_WALL)
    {                                                               // check for adjacent doors
        xtile = (short)(xintercept>>TILESHIFT);
        if ( tilemap[xtile][ytile-ytilestep]&BIT_DOOR &&  MAPSPOT (xtile,ytile-ytilestep,1) != 237
           && MAPSPOT (xtile,ytile-ytilestep,1) != 238 && MAPSPOT (xtile,ytile-ytilestep,1) != 239) // 666 removes door frame
            wallpic = DOORWALL+2;
        else
            wallpic = horizwall[tilehit & ~BIT_WALL];
    }
    else
        wallpic = horizwall[tilehit];
        
      
    //if (mirwall[tilemap[xtile][ytile]] == 1 || mirwall[tilemap[xtile][ytile]] == 3)           // flip the wall ( 1 on side, 2 other, 3 both)
    //   texture = TEXTUREMASK-texture;      

    postsource = PM_GetTexture(wallpic) + texture;
#ifdef USE_SKYWALLPARALLAX
    postsourcesky = postsource - texture;
#endif

    if (dark)        
        ScalePost(true);
    else
        ScalePost(false);   
}

//==========================================================================

/*
====================
=
= HitHorizDoor
=
====================
*/

void HitHorizDoor (void)
{
    int doorpage;
    int doornum;
    int texture;
    boolean dark;
    
    if (darkhit == 3 || darkhit == 7) 
       dark = true;
    else 
       dark = false; 

    doornum = tilehit&LAST_DOORNUM;
    texture = ((xintercept-doorposition[doornum])>>texfromfixedshift)&texmask;
    
    //if (mirwall[tilehit&BIT_DOOR] == 1 || mirwall[tilehit&BIT_DOOR] == 3)           // flip the door
    //   texture = TEXTUREMASK-texture;
     
    // doorobjlist[doornum].lock 91 = 0, 93 = 1, 95 = 2, 101 = 5 
    if (ytilestep != -1 && (mirwall[2*doorobjlist[doornum].lock+91] == 1 || mirwall[2*doorobjlist[doornum].lock+91] == 3))           // flip the door
       texture = texmask-texture;         

    /*if(lasttilehit==tilehit)
    {
        if((pixx&3) && texture == lasttexture)
        {
            ScalePost();
            postx=pixx;
            wallheight[pixx] = wallheight[pixx-1];
            return;
        }
        ScalePost();
        wallheight[pixx] = CalcHeight();
        postsource+=texture-lasttexture;
        postwidth=1;
        postx=pixx;
        lasttexture=texture;
        return;
    }

    if(lastside!=-1) ScalePost();*/

    lastside=2;
    lasttilehit=tilehit;
    lasttexture=texture;
    wallheight[pixx] = CalcHeight();
    postx = pixx;
    postwidth = 1;

    switch(doorobjlist[doornum].lock)
    {
		case dr_normal:
			doorpage = DOORWALL;
			break;
		case dr_lock1:
			doorpage = DOORWALL+6;
			break;
		case dr_lock2:
            doorpage = DOORWALL+8;  // another locked door
			break;
		case dr_lock3:
		case dr_lock4:
		case dr_elevator:
			doorpage = DOORWALL+4;
			break;
    }

    postsource = PM_GetTexture(doorpage) + texture;
    
    if (dark)        
        ScalePost(true);
    else
        ScalePost(false); 
}

//==========================================================================

/*
====================
=
= HitVertDoor
=
====================
*/

void HitVertDoor (void)
{
    int doorpage;
    int doornum;
    int texture;
    boolean dark;
    
    if (darkhit == 6 || darkhit == 7) 
       dark = true;
    else 
       dark = false;

    doornum = tilehit&LAST_DOORNUM;
    texture = ((yintercept-doorposition[doornum])>>texfromfixedshift)&texmask;


    // doorobjlist[doornum].lock 90 = 0, 92 = 1, 94 = 2, 100 = 5 
    if (xtilestep == -1 && mirwall[2*doorobjlist[doornum].lock+90] >= 2)           // flip the door
       texture = texmask-texture; 
        
    /*if(lasttilehit==tilehit)
    {
        if((pixx&3) && texture == lasttexture)
        {
            ScalePost();
            postx=pixx;
            wallheight[pixx] = wallheight[pixx-1];
            return;
        }
        ScalePost();
        wallheight[pixx] = CalcHeight();
        postsource+=texture-lasttexture;
        postwidth=1;
        postx=pixx;
        lasttexture=texture;
        return;
    }

    if(lastside!=-1) ScalePost();*/

    lastside=2;
    lasttilehit=tilehit;
    lasttexture=texture;
    wallheight[pixx] = CalcHeight();
    postx = pixx;
    postwidth = 1;

    switch(doorobjlist[doornum].lock)
    {
		case dr_normal:
			doorpage = DOORWALL;
			break;
		case dr_lock1:
			doorpage = DOORWALL+6;
			break;
		case dr_lock2:
            doorpage = DOORWALL+8;  // another locked door
			break;
		case dr_lock3:
		case dr_lock4:
		case dr_elevator:
			doorpage = DOORWALL+4;
			break;
    }

    postsource = PM_GetTexture(doorpage) + texture;
    
    if (dark)        
        ScalePost(true);
    else
        ScalePost(false); 
}

//==========================================================================

#define HitHorizBorder HitHorizWall
#define HitVertBorder HitVertWall

//==========================================================================

byte vgaCeiling[]=
{
#ifndef SPEAR
 0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,
 0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,
 0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,

 0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,
 0x7d,0x1d,0x2d,0x2d,0xdd,0xd7,0x1d,0x1d,0x1d,0x2d,
 0x1d,0x1d,0x1d,0x1d,0xdd,0xdd,0x7d,0xdd,0xdd,0xdd
#else
 0x6f,0x4f,0x1d,0xde,0xdf,0x2e,0x7f,0x9e,0xae,0x7f,
 0x1d,0xde,0xdf,0xde,0xdf,0xde,0xe1,0xdc,0x2e,0x1d,0xdc
#endif
};

/*
=====================
=
= VGAClearScreen
=
=====================
*/

void VGAClearScreen (void)
{
    //byte ceiling=vgaCeiling[gamestate.episode*10+mapon];
  byte ceiling, floor;
  byte *ptr = vbuf; 
  int y;
  
  if (!cfg_ceiling[gamestate.episode*10+mapon+1])
     ceiling = ceil_default; 
  else   
     ceiling = cfg_ceiling[gamestate.episode*10+mapon+1];
  
  if (!cfg_floor[gamestate.episode*10+mapon+1])
     floor = floor_default;
  else     
     floor = cfg_floor[gamestate.episode*10+mapon+1];  
    
  if (shadestrength[gamestate.episode*10+mapon+1])
  {
    for(y = 0; y < viewheight / 2; y++, ptr += vbufPitch)
        memset(ptr, shadetable[GetShade((viewheight / 2 - y) << 3)][ceiling], viewwidth);
    for(; y < viewheight; y++, ptr += vbufPitch)
        memset(ptr, shadetable[GetShade((y - viewheight / 2) << 3)][floor], viewwidth);
  }
  else
  {
    for(y = 0; y < viewheight / 2; y++, ptr += vbufPitch)
        memset(ptr, ceiling, viewwidth); //47 ceiling
    for(; y < viewheight; y++, ptr += vbufPitch)
        memset(ptr, floor, viewwidth); //42 floor
  }
}

//==========================================================================

/*
=====================
=
= CalcRotate
=
=====================
*/

int CalcRotate (objtype *ob)
{
    int angle, viewangle;

    // this isn't exactly correct, as it should vary by a trig value,
    // but it is close enough with only eight rotations

#ifdef FIXCALCROTATE
    viewangle = (int)( player->angle + (centerx - ob->viewx) / (8 * viewwidth / 320.0) );
#else
    viewangle = player->angle + (centerx - ob->viewx)/8;
#endif


    if (ob->obclass == rocketobj || ob->obclass == drocketobj)           // APN: incoming rockets
		angle = (viewangle-180)- ob->angle;
    else
        angle = (viewangle-180) - dirangle[ob->dir];

    angle+=ANGLES/16;
    while (angle>=ANGLES)
        angle-=ANGLES;
    while (angle<0)
        angle+=ANGLES;

    if (ob->state->rotate == 2)             // 2 rotation pain frame
        return 0;                           // pain with shooting frame bugfix

    return angle/(ANGLES/8);
}

void ScaleShape (int xcenter, int shapenum, unsigned height, uint32_t flags)
{
    t_compshape *shape;
    unsigned scale,pixheight;
    unsigned starty,endy;
    word *cmdptr;
    byte *cline;
    byte *line;
    byte *vmem;
    int actx,i,upperedge;
    short newstart;
    int scrstarty,screndy,lpix,rpix,pixcnt,ycnt;
    unsigned j;
    byte col;
    byte *curshades;

    if (shadestrength[gamestate.episode*10+mapon+1])
    {
       if(flags & FL_FULLBRIGHT)
           curshades = shadetable[0];
       else
           curshades = shadetable[GetShade(height)];
    }
           
    shape = (t_compshape *) PM_GetSprite(shapenum);

    scale=height>>3;                 // low three bits are fractional
    if(!scale) return;   // too close or far away

    pixheight=scale*spritescale;
    actx=xcenter-scale;
    upperedge=viewheight/2-scale;

    cmdptr=(word *) shape->dataofs;

    for(i=shape->leftpix,pixcnt=i*pixheight,rpix=(pixcnt>>6)+actx;i<=shape->rightpix;i++,cmdptr++)
    {
        lpix=rpix;
        if(lpix>=viewwidth) break;
        pixcnt+=pixheight;
        rpix=(pixcnt>>6)+actx;
        if(lpix!=rpix && rpix>0)
        {
            if(lpix<0) lpix=0;
            if(rpix>viewwidth) rpix=viewwidth,i=shape->rightpix+1;
            cline=(byte *)shape + *cmdptr;
            while(lpix<rpix)
            {
                if(wallheight[lpix]<=(int)height)
                {
                    line=cline;
                    while((endy = READWORD(line)) != 0)
                    {
                        endy >>= 1;
                        newstart = READWORD(line);
                        starty = READWORD(line) >> 1;
                        j=starty;
                        ycnt=j*pixheight;
                        screndy=(ycnt>>6)+upperedge;
                        if(screndy<0) vmem=vbuf+lpix;
                        else vmem=vbuf+screndy*vbufPitch+lpix;
                        for(;j<endy;j++)
                        {
                            scrstarty=screndy;
                            ycnt+=pixheight;
                            screndy=(ycnt>>6)+upperedge;
                            if(scrstarty!=screndy && screndy>0)
                            {
                                if (shadestrength[gamestate.episode*10+mapon+1])
                                   col=curshades[((byte *)shape)[newstart+j]];
                                else                     
                                   col=((byte *)shape)[newstart+j];

                                if(scrstarty<0) scrstarty=0;
                                if(screndy>viewheight) screndy=viewheight,j=endy;

                                while(scrstarty<screndy)
                                {
                                    *vmem=col;
                                    vmem+=vbufPitch;
                                    scrstarty++;
                                }
                            }
                        }
                    }
                }
                lpix++;
            }
        }
    }
}

void SimpleScaleShape (int xcenter, int shapenum, unsigned height)
{
    t_compshape   *shape;
    unsigned scale,pixheight;
    unsigned starty,endy;
    word *cmdptr;
    byte *cline;
    byte *line;
    int actx,i,upperedge;
    short newstart;
    int scrstarty,screndy,lpix,rpix,pixcnt,ycnt;
    unsigned j;
    byte col;
    byte *vmem;

    shape = (t_compshape *) PM_GetSprite(shapenum);

    //scale=(height/6) - (height/192);                // should match MAC @ 640x480
    
    //scale=height>>1;
    //pixheight=scale*SPRITESCALEFACTOR;
    //actx=xcenter-scale;
    //upperedge=viewheight/2-scale;
    //upperedge=viewheight/2-scale+scale*2+scale/8;
    //scale=(400/6) - (400/192);  
    
    scale=height/5;
    
    if (shapenum == SPR_DEMO)
    {
      scale=(height>>3)-FixedMul(0x5,sintable[gamestate.TimeCount]);             
      pixheight=scale*spritescale*2;
    }
    else if (shapenum == SPR_DEATHCAM)
    {
      scale=(height>>1)-FixedMul(0x15,sintable[deathCount]);             
      pixheight=scale*spritescale*2;
    }
    else
      pixheight=scale*spritescale;
      
    if (shapenum == SPR_DEATHCAM)  // center it on x
      actx=xcenter-scale-scale/4;
    else
      actx=xcenter-scale;
    
    if (shapenum == SPR_DEMO || shapenum == SPR_DEATHCAM)
      upperedge=viewheight/2-scale-scale;
    else
      //upperedge=viewheight/2-scale+scale*2+scale/8;    // should match MAC @ 640x480
      upperedge=viewheight/2-scale/2+scale;

    cmdptr=shape->dataofs;

    for(i=shape->leftpix,pixcnt=i*pixheight,rpix=(pixcnt>>6)+actx;i<=shape->rightpix;i++,cmdptr++)
    {
        lpix=rpix;
        if(lpix>=viewwidth) break;
        if (shapenum == SPR_DEMO || shapenum == SPR_DEATHCAM)
          pixcnt+=pixheight/2;
        else
          pixcnt+=pixheight;
        rpix=(pixcnt>>6)+actx;
        if(lpix!=rpix && rpix>0)
        {
            if(lpix<0) lpix=0;
            if(rpix>viewwidth) rpix=viewwidth,i=shape->rightpix+1;
            cline = (byte *)shape + *cmdptr;
            while(lpix<rpix)
            {
                line=cline;
                while((endy = READWORD(line)) != 0)
                {
                    endy >>= 1;
                    newstart = READWORD(line);
                    starty = READWORD(line) >> 1;
                    j=starty;
                    ycnt=j*pixheight;
                    screndy=(ycnt>>6)+upperedge;
                    if(screndy<0) vmem=vbuf+lpix;
                    else vmem=vbuf+screndy*vbufPitch+lpix;
                    for(;j<endy;j++)
                    {
                        scrstarty=screndy;
                        ycnt+=pixheight;
                        screndy=(ycnt>>6)+upperedge;
                        if(scrstarty!=screndy && screndy>0)
                        {
                            col=((byte *)shape)[newstart+j];
                            if(scrstarty<0) scrstarty=0;
                            if(screndy>viewheight) screndy=viewheight,j=endy;

                            while(scrstarty<screndy)
                            {
                                *vmem=col;
                                vmem+=vbufPitch;
                                scrstarty++;
                            }
                        }
                    }
                }
                lpix++;
            }
        }
    }
}

/*
=====================
=
= DrawScaleds
=
= Draws all objects that are visable
=
=====================
*/

#define MAXVISABLE 500
#define MACVISABLE 64

typedef struct
{
    short      viewx,
               viewheight,
               shapenum;
    short      flags;          // this must be changed to uint32_t, when you
                               // you need more than 16-flags for drawing
#ifdef USE_DIR3DSPR
    statobj_t *transsprite;
#endif
} visobj_t;

visobj_t vislist[MAXVISABLE];
visobj_t *visptr,*visstep,*farthest;

void DrawScaleds (void)
{
    int      i,least,numvisable,height;
    byte     *visspot;
    tiletype *tilespot;
    unsigned spotloc;
    int32_t  sx,sy;
    int      rot;

    statobj_t *statptr;
    objtype   *obj;

    visptr = &vislist[0];

//
// place static objects
//
    for (statptr = &statobjlist[0] ; statptr !=laststatobj ; statptr++)
    {
        if ((visptr->shapenum = statptr->shapenum) == -1)
            continue;                                               // object has been deleted

        //if (MAPSPOT(statptr->tilex,statptr->tiley,0) < AREATILE)         // object is inside the wall but can still be picked up
        //{
            sx = ((int32_t)statptr->tilex<<TILESHIFT)+0x8000;
	        sy = ((int32_t)statptr->tiley<<TILESHIFT)+0x8000;
	        
            if (LABS (player->x - sx) < 0xFF00 && LABS(player->y - sy) < 0xFF00 && statptr->flags & FL_BONUS)                                                        
            {
               GetBonus (statptr);
               if(statptr->shapenum == -1)
                  continue;                                           // object has been taken
            }
        //}

        if (!*statptr->visspot)
            continue;                                               // not visable
  
        // The old bonus thingy
        /*
        if (TransformTile (statptr->tilex,statptr->tiley,
            &visptr->viewx,&visptr->viewheight, false) && statptr->flags & FL_BONUS)
        {
            GetBonus (statptr);
            if(statptr->shapenum == -1)
            
                continue;                                           // object has been taken
        }
        */
        TransformTile (statptr->tilex,statptr->tiley,&visptr->viewx,&visptr->viewheight, false);

        if (!visptr->viewheight)
            continue;                                               // to close to the object

        // lamps shining on things, hardcoded YAY
        if (MAPSPOT(statptr->tilex,statptr->tiley,1) == 26 || MAPSPOT(statptr->tilex,statptr->tiley,1) == 27 ||
            MAPSPOT(statptr->tilex,statptr->tiley,1) == 37)
        {   
            if (!mod.statobj_lit[MAPSPOT(statptr->tilex,statptr->tiley,1)])     // only set bright if no mod setting
                statptr->flags |= FL_FULLBRIGHT;  
        }
        else  
        {
            if (!mod.statobj_lit[MAPSPOT(statptr->tilex,statptr->tiley,1)])     // only go dark if no mod setting
                statptr->flags &= ~FL_FULLBRIGHT;   
        }
            
#ifdef USE_DIR3DSPR
        if(statptr->flags & FL_DIR_MASK)
            visptr->transsprite=statptr;
        else
            visptr->transsprite=NULL;
#endif
        if (macactorlimit)
        {
           if (visptr < &vislist[MACVISABLE-1])    // don't let it overflow
           {
               visptr->flags = (short) statptr->flags;
               visptr++;
           }
        }
        else
        {
           if (visptr < &vislist[MAXVISABLE-1])    // don't let it overflow
           {
               visptr->flags = (short) statptr->flags;
               visptr++;
           }
        }
    }

//
// place active objects
//
    for (obj = player->next;obj;obj=obj->next)
    {
        if ((visptr->shapenum = obj->state->shapenum)==0)
            continue;                                               // no shape

        spotloc = (obj->tilex<<mapshift)+obj->tiley;   // optimize: keep in struct?
        visspot = &spotvis[0][0]+spotloc;
        tilespot = &tilemap[0][0]+spotloc;

        //
        // could be in any of the nine surrounding tiles
        //
        if (*visspot
            || ( *(visspot-1) && !*(tilespot-1) )
            || ( *(visspot+1) && !*(tilespot+1) )
            || ( *(visspot-(MAPSIZE+1)) && !*(tilespot-(MAPSIZE+1)) )
            || ( *(visspot-(MAPSIZE)) && !*(tilespot-(MAPSIZE)) )
            || ( *(visspot-(MAPSIZE-1)) && !*(tilespot-(MAPSIZE-1)) )
            || ( *(visspot+(MAPSIZE+1)) && !*(tilespot+(MAPSIZE+1)) )
            || ( *(visspot+(MAPSIZE)) && !*(tilespot+(MAPSIZE)) )
            || ( *(visspot+(MAPSIZE-1)) && !*(tilespot+(MAPSIZE-1)) ) )
        {
            obj->active = ac_yes;
            TransformActor (obj);
            if (!obj->viewheight)
                continue;                                               // too close or far away

            visptr->viewx = obj->viewx;
            visptr->viewheight = obj->viewheight;
            if (visptr->shapenum == -1)
                visptr->shapenum = obj->temp1;  // special shape
                
            // lamps shining on things      
            if (MAPSPOT(obj->tilex,obj->tiley,1) == 26 || MAPSPOT(obj->tilex,obj->tiley,1) == 27 ||
                MAPSPOT(obj->tilex,obj->tiley,1) == 37)
                obj->flags |= FL_FULLBRIGHT;  
            else  
            {
                if (obj->attack)                    // shooting enemies are bright
                    obj->flags |= FL_FULLBRIGHT;  
                else
                    obj->flags &= ~FL_FULLBRIGHT;
            }
          
            if (obj->state->rotate)
                visptr->shapenum += CalcRotate (obj);
                
            if (mod.en_rotation)
            {
                  // make this more automatic one day              
                  if (obj->obclass == guardobj)
                  {
                     if (obj->state->shapenum >= SPR_GRD_W1_1 && obj->state->shapenum <= SPR_GRD_W4_1 && obj->state != &s_grdstand)        
                         visptr->shapenum = SPR_GRD_W1_0 + 8*(obj->state->shapenum-SPR_GRD_W1_1) + CalcRotate (obj); 
                     else if (obj->state == &s_grdstand)   
                         visptr->shapenum = SPR_GRD_S1_1 + CalcRotate (obj);                             
                  }   
                  else if (obj->obclass == ssobj)
                  {
                     if (obj->state->shapenum >= SPR_SS_W1_1 && obj->state->shapenum <= SPR_SS_W4_1 && obj->state != &s_ssstand)        
                         visptr->shapenum = SPR_SS_W1_0 + 8*(obj->state->shapenum-SPR_SS_W1_1) + CalcRotate (obj); 
                     else if (obj->state == &s_ssstand)   
                         visptr->shapenum = SPR_SS_S1_1 + CalcRotate (obj);                             
                  } 
                  else if (obj->obclass == officerobj)
                  {
                     if (obj->state->shapenum >= SPR_OFC_W1_1 && obj->state->shapenum <= SPR_OFC_W4_1 && obj->state != &s_ofcstand)        
                         visptr->shapenum = SPR_OFC_W1_0 + 8*(obj->state->shapenum-SPR_OFC_W1_1) + CalcRotate (obj); 
                     else if (obj->state == &s_ofcstand)   
                         visptr->shapenum = SPR_OFC_S1_1 + CalcRotate (obj);                             
                  } 
                  else if (obj->obclass == mutantobj)
                  {
                     if (obj->state->shapenum >= SPR_MUT_W1_1 && obj->state->shapenum <= SPR_MUT_W4_1 && obj->state != &s_mutstand)        
                         visptr->shapenum = SPR_MUT_W1_0 + 8*(obj->state->shapenum-SPR_MUT_W1_1) + CalcRotate (obj); 
                     else if (obj->state == &s_mutstand)   
                         visptr->shapenum = SPR_MUT_S1_1 + CalcRotate (obj);                             
                  } 
                  else if (obj->obclass == dogobj)
                  {
                     if (obj->state->shapenum >= SPR_DOG_W1_1 && obj->state->shapenum <= SPR_DOG_W4_1 && obj->state != &s_dogstand)        
                         visptr->shapenum = SPR_DOG_W1_0 + 8*(obj->state->shapenum-SPR_DOG_W1_1) + CalcRotate (obj); 
                     else if (obj->state == &s_dogstand)   
                         visptr->shapenum = SPR_DOG_S1_1 + CalcRotate (obj);                             
                  } 
            }
            
            if (mod.boss_rotation)
            {     
                  // bosses
                  //
                  if (!mod.en_rotation)                    // shift the the order of the sprites. start after missing statobj
                      rot = 200;
                  else
                      rot = 0;
                  
                  if (obj->obclass == bossobj)
                  {
                     if (obj->state->shapenum >= SPR_BOSS_W1 && obj->state->shapenum <= SPR_BOSS_W4 && obj->state != &s_bossstand)        
                         visptr->shapenum = SPR_BOSS_W1_0 - rot + 8*(obj->state->shapenum-SPR_BOSS_W1) + CalcRotate (obj); 
                     else if (obj->state == &s_bossstand)   
                         visptr->shapenum = SPR_BOSS_S1_1 - rot + CalcRotate (obj);                             
                  }      
                  else if (obj->obclass == schabbobj)
                  {
                     if (obj->state->shapenum >= SPR_SCHABB_W1 && obj->state->shapenum <= SPR_SCHABB_W4 && obj->state != &s_schabbstand)        
                         visptr->shapenum = SPR_SCHABB_W1_0 - rot + 8*(obj->state->shapenum-SPR_SCHABB_W1) + CalcRotate (obj); 
                     else if (obj->state == &s_schabbstand)   
                         visptr->shapenum = SPR_SCHABB_S1_1 - rot + CalcRotate (obj);                             
                  } 
                  else if (obj->obclass == mechahitlerobj)
                  {
                     if (obj->state->shapenum >= SPR_MECHA_W1 && obj->state->shapenum <= SPR_MECHA_W4 && obj->state != &s_mechastand)        
                         visptr->shapenum = SPR_MECHA_W1_0 - rot + 8*(obj->state->shapenum-SPR_MECHA_W1) + CalcRotate (obj); 
                     else if (obj->state == &s_mechastand)   
                         visptr->shapenum = SPR_MECHA_S1_1 - rot + CalcRotate (obj);                             
                  } 
                  else if (obj->obclass == realhitlerobj)
                  {
                     if (obj->state->shapenum >= SPR_HITLER_W1 && obj->state->shapenum <= SPR_HITLER_W4 && obj->state != &s_hitlerstand)        
                         visptr->shapenum = SPR_HITLER_W1_0 - rot + 8*(obj->state->shapenum-SPR_HITLER_W1) + CalcRotate (obj); 
                     else if (obj->state == &s_hitlerstand)   
                         visptr->shapenum = SPR_HITLER_S1_1 - rot + CalcRotate (obj);                             
                  }   
                  else if (obj->obclass == fatobj)
                  {
                     if (obj->state->shapenum >= SPR_FAT_W1 && obj->state->shapenum <= SPR_FAT_W4 && obj->state != &s_fatstand)        
                         visptr->shapenum = SPR_FAT_W1_0 - rot + 8*(obj->state->shapenum-SPR_FAT_W1) + CalcRotate (obj); 
                     else if (obj->state == &s_fatstand)   
                         visptr->shapenum = SPR_FAT_S1_1 - rot + CalcRotate (obj);                             
                  }  
                  else if (obj->obclass == gretelobj)
                  {
                     if (obj->state->shapenum >= SPR_GRETEL_W1 && obj->state->shapenum <= SPR_GRETEL_W4 && obj->state != &s_gretelstand)        
                         visptr->shapenum = SPR_GRETEL_W1_0 - rot + 8*(obj->state->shapenum-SPR_GRETEL_W1) + CalcRotate (obj); 
                     else if (obj->state == &s_gretelstand)   
                         visptr->shapenum = SPR_GRETEL_S1_1 - rot + CalcRotate (obj);                             
                  }              
                  else if (obj->obclass == giftobj)
                  {
                     if (obj->state->shapenum >= SPR_GIFT_W1 && obj->state->shapenum <= SPR_GIFT_W4 && obj->state != &s_giftstand)        
                         visptr->shapenum = SPR_GIFT_W1_0 - rot + 8*(obj->state->shapenum-SPR_GIFT_W1) + CalcRotate (obj); 
                     else if (obj->state == &s_giftstand)   
                         visptr->shapenum = SPR_GIFT_S1_1 - rot + CalcRotate (obj);                             
                  }              
            } 

            if (macactorlimit)
            {    
                if (visptr < &vislist[MACVISABLE-1])    // don't let it overflow
                {
                    visptr->flags = (short) obj->flags;
                    #ifdef USE_DIR3DSPR
                    visptr->transsprite = NULL;
                    #endif
                    visptr++;
                }
            }
            else
            {    
                if (visptr < &vislist[MAXVISABLE-1])    // don't let it overflow
                {
                    visptr->flags = (short) obj->flags;
                    #ifdef USE_DIR3DSPR
                    visptr->transsprite = NULL;
                    #endif
                    visptr++;
                }
            }
            
            obj->flags |= FL_VISABLE;
        }
        else
            obj->flags &= ~FL_VISABLE;
    }

//
// draw from back to front
//
    numvisable = (int) (visptr-&vislist[0]);

    if (!numvisable)
        return;                                                                 // no visable objects

    for (i = 0; i<numvisable; i++)
    {
        least = 32000;
        for (visstep=&vislist[0] ; visstep<visptr ; visstep++)
        {
            height = visstep->viewheight;
            if (height < least)
            {
                least = height;
                farthest = visstep;
            }
        }
        //
        // draw farthest
        //
#ifdef USE_DIR3DSPR
        if(farthest->transsprite)
            Scale3DShape(vbuf, vbufPitch, farthest->transsprite);
        else
#endif
            ScaleShape(farthest->viewx, farthest->shapenum, farthest->viewheight, farthest->flags);

        farthest->viewheight = 32000;
    }
}

//==========================================================================

/*
==============
=
= DrawPlayerWeapon
=
= Draw the player's hands
=
==============
*/

int	weaponscale[NUMWEAPONS] = {SPR_KNIFEREADY,SPR_PISTOLREADY,SPR_MACHINEGUNREADY,SPR_CHAINREADY,SPR_FTHROWREADY,SPR_MLAUNCHREADY}; 

void DrawPlayerWeapon (void)
{
    int shapenum;
          
    if (gamestate.victorycam) 
    {                   
            deathCount += tics*10;   
                     
            if (deathCount > 358) 
                deathCount = 1;
                
             SimpleScaleShape(viewwidth/2,SPR_DEATHCAM,viewheight+75);      
    }

    if (gamestate.weapon != -1)
    {
        shapenum = weaponscale[gamestate.weapon]+gamestate.weaponframe;
        SimpleScaleShape(viewwidth/2,shapenum,viewheight+1);
    }

//        if (demorecord || demoplayback)
//                SimpleScaleShape(viewwidth/2,SPR_DEMO,viewheight+1);

// Game Over sprite after player game ends

        if (playstate == ex_died && gamestate.lives == 0)
        {
                SimpleScaleShape(viewwidth/2,SPR_DEMO,viewheight+75);
        }
}


/*
==============
=
= DrawHud
=
= draw them constantly on screen
=
==============
*/
void DrawHud (void)
{
    if (!hudmod) return;
     
    //if (mod.hud_y[statusbar]*(screenHeight/100) < (screenHeight-scale640*STATUSLINES*2))
        DrawStatusBar (); 
        
    //if (mod.hud_y[timer])  
        DrawTime ();             
        DrawHealth();           
        DrawFace();                 
        DrawLevel();    
        DrawLives();           
        DrawItems();    
        DrawItemsTotal();    
        DrawScore();   
        DrawSecrets();   
        DrawSecretsTotal();
        DrawAmmo();   
        DrawKeys();
        DrawKills();
        DrawEnemies();
     
}


//==========================================================================


/*
=====================
=
= CalcTics
=
=====================
*/

void CalcTics (void)
{
//
// calculate tics since last refresh for adaptive timing
//
    if (lasttimecount > (int32_t) GetTimeCount())
        lasttimecount = GetTimeCount();    // if the game was paused a LONG time

    uint32_t curtime = SDL_GetTicks();
    tics = (curtime * 6) / 100 - lasttimecount; //7
    if(!tics)
    {
        // wait until end of current tic
        SDL_Delay(((lasttimecount + 1) * 100) / 6 - curtime); //7
        tics = 1;
    }

    lasttimecount += tics;

    if (tics>MAXTICS)
        tics = MAXTICS;
}


//==========================================================================
longword xpart,ypart;

bool RefreshCorner (bool flip)
{
    if (tilemap[xtile][ytile] && (xtile-xtilestep) == (xintercept>>TILESHIFT) && (ytile-ytilestep) == (yintercept>>TILESHIFT))
    {
        if (lastside == flip) return true;
        if (lasttilehit&(MAXWALLTILES<<1)) lastside=3;
        tilehit = lasttilehit;
    }
    return false;
}
 
/*
=====================
=
= AsmRefresh
=
=====================
*/

void AsmRefresh()
{
    int32_t xstep,ystep;
    longword xpartial,ypartial;
    boolean playerInPushwallBackTile = tilemap[focaltx][focalty] == BIT_WALL;

    for(pixx=0;pixx<viewwidth;pixx++)
    {
        short angl=midangle+pixelangle[pixx];
        if(angl<0) angl+=FINEANGLES;
        if(angl>=3600) angl-=FINEANGLES;
        if(angl<900)
        {
            xtilestep=1;
            ytilestep=-1;
            xstep=finetangent[900-1-angl];
            ystep=-finetangent[angl];
            xpartial=xpartialup;
            ypartial=ypartialdown;
        }
        else if(angl<1800)
        {
            xtilestep=-1;
            ytilestep=-1;
            xstep=-finetangent[angl-900];
            ystep=-finetangent[1800-1-angl];
            xpartial=xpartialdown;
            ypartial=ypartialdown;
        }
        else if(angl<2700)
        {
            xtilestep=-1;
            ytilestep=1;
            xstep=-finetangent[2700-1-angl];
            ystep=finetangent[angl-1800];
            xpartial=xpartialdown;
            ypartial=ypartialup;//+ypartup;
            //ypart = 0;
        }
        else if(angl<3600)
        {
            xtilestep=1;
            ytilestep=1;
            xstep=finetangent[angl-2700];
            ystep=finetangent[3600-1-angl];
            xpartial=xpartialup;
            ypartial=ypartialup;//+ypartup;
           // ypart = 0;
        }
        
        //////////////////////////////////////
        // ypartialdown = viewy&(TILEGLOBAL-1); ypartialup = TILEGLOBAL-ypartialdown;  
        // xpart = 0; 
        //if (tilehit == 1)
        // ypartial+=ypart; 

        yintercept=FixedMul(ystep,xpartial)+viewy; 
        xtile=focaltx+xtilestep;
        xspot=(word)((xtile<<mapshift)+((uint32_t)yintercept>>16));
        xintercept=FixedMul(xstep,ypartial)+viewx;  
        ytile=focalty+ytilestep;
        yspot=(word)((((uint32_t)xintercept>>16)<<mapshift)+ytile);
        texdelta=0;

        // Special treatment when player is in back tile of pushwall
        if(playerInPushwallBackTile)
        {
            if(    pwalldir == di_east && xtilestep ==  1
                || pwalldir == di_west && xtilestep == -1)
            {
                int32_t yintbuf = yintercept - ((ystep * (64 - pwallpos)) >> 6);
                if((yintbuf >> 16) == focalty)   // ray hits pushwall back?
                {
                    if(pwalldir == di_east)
                        xintercept = (focaltx << TILESHIFT) + (pwallpos << 10);
                    else
                        xintercept = (focaltx << TILESHIFT) - TILEGLOBAL + ((64 - pwallpos) << 10);
                    yintercept = yintbuf;
                    ytile = (short) (yintercept >> TILESHIFT);
                    tilehit = pwalltile;
                    HitVertWall();
                    continue;
                }
            }
            else if(pwalldir == di_south && ytilestep ==  1
                ||  pwalldir == di_north && ytilestep == -1)
            {
                int32_t xintbuf = xintercept - ((xstep * (64 - pwallpos)) >> 6);
                if((xintbuf >> 16) == focaltx)   // ray hits pushwall back?
                {
                    xintercept = xintbuf;
                    if(pwalldir == di_south)
                        yintercept = (focalty << TILESHIFT) + (pwallpos << 10);
                    else
                        yintercept = (focalty << TILESHIFT) - TILEGLOBAL + ((64 - pwallpos) << 10);
                    xtile = (short) (xintercept >> TILESHIFT);
                    tilehit = pwalltile;
                    HitHorizWall();
                    continue;
                }
            }
        }

        do
        {
            if(ytilestep==-1 && (yintercept>>16)<=ytile) goto horizentry;
            if(ytilestep==1 && (yintercept>>16)>=ytile) goto horizentry;
vertentry:
            if((uint32_t)yintercept>mapheight*65536-1 || (word)xtile>=mapwidth)
            {
                if(xtile<0) xintercept=0, xtile=0;
                else if(xtile>=mapwidth) xintercept=mapwidth<<TILESHIFT, xtile=mapwidth-1;
                else xtile=(short) (xintercept >> TILESHIFT);
                if(yintercept<0) yintercept=0, ytile=0;
                else if(yintercept>=(mapheight<<TILESHIFT)) yintercept=mapheight<<TILESHIFT, ytile=mapheight-1;
                yspot=0xffff;
                tilehit=0;
                HitHorizBorder();
                break;
            }
            if(xspot>=maparea) break;
            tilehit=((tiletype *)tilemap)[xspot];
            darkhit=((tiletype *)darkmap)[xspot];
            quarterhit = ((tiletype *)quarterwalls)[xspot];
            //ypart = 0; 
            //if (RefreshCorner(false)) goto horizentry;
            if(tilehit)
            { 
                if(tilehit&BIT_DOOR)
                {
                    // push door towards player x-                
                    if (MAPSPOT (xtile,ytile-ytilestep,1)==239 || MAPSPOT (xtile,ytile-ytilestep,1)==241)   
                    {            
                        if (xtilestep == -1)                      // push door back when player is right of the tile   || P
                        { 
                         
                            int32_t yintbuf=yintercept+ystep;
                            
                            if((yintbuf>>16)!=(yintercept>>16) && lastside!=3)
                                goto passvert;
                            if((word)yintbuf<doorposition[tilehit&LAST_DOORNUM])
                                goto passvert;
                            yintercept=yintbuf;
                            xintercept=(xtile<<TILESHIFT);
                            ytile = (short) (yintercept >> TILESHIFT);
                            HitVertDoor();
                        }
                        else if (xtilestep == 1)                  // push door forward when player is left of the tile P ||
                        {
                            int32_t yintbuf=yintercept;
                            if((word)yintbuf<doorposition[tilehit&LAST_DOORNUM])
                                goto passvert;

                            xintercept=(xtile<<TILESHIFT);
                            ytile = (short) (yintercept >> TILESHIFT);
                            HitVertDoor();
                        }
                    }
                    // push door away from player x+
                    else if (MAPSPOT (xtile,ytile-ytilestep,1)==238 || MAPSPOT (xtile,ytile-ytilestep,1)==240)   
                    {
                        if (xtilestep == 1)                            // push door back when player is left of the tile  P ||
                        { 
                            int32_t yintbuf=yintercept+ystep;

                            if((yintbuf>>16)!=(yintercept>>16) && lastside!=3)
                                goto passvert;
                            if((word)yintbuf<doorposition[tilehit&LAST_DOORNUM])
                                goto passvert;
                            yintercept=yintbuf;
                            xintercept=(xtile<<TILESHIFT)|0xffff;
                            ytile = (short) (yintercept >> TILESHIFT);
                            HitVertDoor();
                        }
                        else if (xtilestep == -1)                  // push door forward when player is right of the tile  || P
                        {
                            int32_t yintbuf=yintercept;
                            if((word)yintbuf<doorposition[tilehit&LAST_DOORNUM])
                                goto passvert;

                            xintercept=(xtile<<TILESHIFT)+0xffff;
                            ytile = (short) (yintercept >> TILESHIFT);
                            HitVertDoor();
                        }
                    }
                    else
                    {
                        int32_t yintbuf=yintercept+(ystep>>1);
                       
                       // if (~MAPSPOT (xtile,ytile-ytilestep,0) & 0x80)  
                       // { 
                            if((yintbuf>>16)!=(yintercept>>16) && lastside!=3)
                                goto passvert;
                       // }
                            
                        if((word)yintbuf<doorposition[tilehit&LAST_DOORNUM])
                            goto passvert;
                        yintercept=yintbuf;
                        xintercept=(xtile<<TILESHIFT)|0x8000;
                        ytile = (short) (yintercept >> TILESHIFT);
                        HitVertDoor();
                    }
                }
                else
                {
                    if(tilehit==BIT_WALL)
                    {
                        if(pwalldir==di_west || pwalldir==di_east)
                        {
	                        int32_t yintbuf;
                            int pwallposnorm;
                            int pwallposinv;
                            if(pwalldir==di_west)
                            {
                                pwallposnorm = 64-pwallpos;
                                pwallposinv = pwallpos;
                            }
                            else
                            {
                                pwallposnorm = pwallpos;
                                pwallposinv = 64-pwallpos;
                            }
                            if(pwalldir == di_east && xtile==pwallx && ((uint32_t)yintercept>>16)==pwally
                                || pwalldir == di_west && !(xtile==pwallx && ((uint32_t)yintercept>>16)==pwally))
                            {
                                yintbuf=yintercept+((ystep*pwallposnorm)>>6);
                                if((yintbuf>>16)!=(yintercept>>16))
                                    goto passvert;

                                xintercept=(xtile<<TILESHIFT)+TILEGLOBAL-(pwallposinv<<10);
                                yintercept=yintbuf;
                                ytile = (short) (yintercept >> TILESHIFT);
                                tilehit=pwalltile;
                                HitVertWall();
                            }
                            else
                            {
                                yintbuf=yintercept+((ystep*pwallposinv)>>6);
                                if((yintbuf>>16)!=(yintercept>>16))
                                    goto passvert;

                                xintercept=(xtile<<TILESHIFT)-(pwallposinv<<10);
                                yintercept=yintbuf;
                                ytile = (short) (yintercept >> TILESHIFT);
                                tilehit=pwalltile;
                                HitVertWall();
                            }
                        }
                        else
                        {
                            int pwallposi = pwallpos;
                            if(pwalldir==di_north) pwallposi = 64-pwallpos;
                            if(pwalldir==di_south && (word)yintercept<(pwallposi<<10)
                                || pwalldir==di_north && (word)yintercept>(pwallposi<<10))
                            {
                                if(((uint32_t)yintercept>>16)==pwally && xtile==pwallx)
                                {
                                    if(pwalldir==di_south && (int32_t)((word)yintercept)+ystep<(pwallposi<<10)
                                            || pwalldir==di_north && (int32_t)((word)yintercept)+ystep>(pwallposi<<10))
                                        goto passvert;

                                    if(pwalldir==di_south)
                                        yintercept=(yintercept&0xffff0000)+(pwallposi<<10);
                                    else
                                        yintercept=(yintercept&0xffff0000)-TILEGLOBAL+(pwallposi<<10);
                                    xintercept=xintercept-((xstep*(64-pwallpos))>>6);
                                    xtile = (short) (xintercept >> TILESHIFT);
                                    tilehit=pwalltile;
                                    HitHorizWall();
                                }
                                else
                                {
                                    texdelta = -(pwallposi<<10);
                                    xintercept=xtile<<TILESHIFT;
                                    ytile = (short) (yintercept >> TILESHIFT);
                                    tilehit=pwalltile;
                                    HitVertWall();
                                }
                            }
                            else
                            {
                                if(((uint32_t)yintercept>>16)==pwally && xtile==pwallx)
                                {
                                    texdelta = -(pwallposi<<10);
                                    xintercept=xtile<<TILESHIFT;
                                    ytile = (short) (yintercept >> TILESHIFT);
                                    tilehit=pwalltile;
                                    HitVertWall();
                                }
                                else
                                {
                                    if(pwalldir==di_south && (int32_t)((word)yintercept)+ystep>(pwallposi<<10)
                                            || pwalldir==di_north && (int32_t)((word)yintercept)+ystep<(pwallposi<<10))
                                        goto passvert;

                                    if(pwalldir==di_south)
                                        yintercept=(yintercept&0xffff0000)-((64-pwallpos)<<10);
                                    else
                                        yintercept=(yintercept&0xffff0000)+((64-pwallpos)<<10);
                                    xintercept=xintercept-((xstep*pwallpos)>>6);
                                    xtile = (short) (xintercept >> TILESHIFT);
                                    tilehit=pwalltile;
                                    HitHorizWall();
                                }
                            }
                        }
                    }
                    else
                    {    
                         
                        // make all of this nonsense into one function one day     
                        if (quarterhit == 1)
                        {
                          if (!PartVertWallNW (xstep,ystep))             
                              goto passvert;
                        }
                        else if (quarterhit == 2)
                        {
                          if (!PartVertWallNE (xstep,ystep))             
                              goto passvert;
                        } 
                        else if (quarterhit == 3)
                        {
                          if (!PartVertWallSE (xstep,ystep))             
                              goto passvert;
                        }
                        else if (quarterhit == 4)
                        {
                          if (!PartVertWallSW (xstep,ystep))             
                              goto passvert;
                        }  
                        else if (quarterhit == 5)
                        {
                          if (!HalfVertWallTopW2E(xstep,ystep))             
                              goto passvert;
                        }
                        else if (quarterhit == 6)
                        {
                          if (!HalfVertWallBotW2E(xstep,ystep))             
                              goto passvert;
                        }  
                        else if (quarterhit == 7)
                        {
                          if (!HalfVertWallLeftN2S(xstep,ystep))             
                              goto passvert;
                        }  
                        else if (quarterhit == 8)
                        {
                          if (!HalfVertWallRightN2S(xstep,ystep))             
                              goto passvert;
                        }  
                        else if (quarterhit == 9)
                        {
                          if (!TriVertWallNW(xstep,ystep))             
                              goto passvert;
                        }  
                        else if (quarterhit == 10)
                        {
                          if (!TriVertWallNE(xstep,ystep))             
                              goto passvert;
                        }  
                        else if (quarterhit == 11)
                        {
                          if (!TriVertWallSE(xstep,ystep))             
                              goto passvert;
                        }  
                        else if (quarterhit == 12)
                        {
                          if (!TriVertWallSW(xstep,ystep))             
                              goto passvert;
                        }  
                        else if (quarterhit == 13)
                        {
                          if (!TwoVertWallNW2SE(xstep,ystep))             
                              goto passvert;
                        }  
                        else if (quarterhit == 14)
                        {
                          if (!TwoVertWallNE2SW(xstep,ystep))             
                              goto passvert;
                        }  
                        else if (quarterhit == 15)
                        {           
                              goto passvert;
                        }  
                       /* { 
                            // CUTTING INTO VERTICAL SIDE TO DRAW HORIZONTAL WALL          
                            if((word)yintercept<0x8000) //if ((int32_t)((word)yintercept)+ystep<0x8000)
                            {                                              
                                   if (xtilestep == -1 && ytilestep == -1)             // SE
                                   {
                                       int32_t xintbuf=xintercept-(xstep>>1);
                                       int32_t xint=xintercept;

                                      // if((xintbuf>>16)!=(xint>>16))
                                      // if((int32_t)((word)yintercept)+ystep<0x8000) //full tile
                                       if((int32_t)((word)yintercept)+(ystep>>1)<0x8000) //half tile
                                          goto passvert;

                                       yintercept=(yintercept&0xffff0000)+0x8000;
                                       xintercept=xintercept-(xstep>>1);
                                    
                                       xtile = (short) (xintercept >> TILESHIFT);
                                       HitHorizDoor();
                                   }
                                   else if (xtilestep == 1 && ytilestep == 1)          // NW
                                   {
                                       int32_t xintbuf=xintercept-(xstep>>1);

                                       //if((int32_t)((word)yintercept)+ystep<0x8000) //full tile
                                       if((int32_t)((word)yintercept)+(ystep>>1)<0x8000) //half tile
                                          goto passvert;

                                       //yintercept=(yintercept&0xffff0000)+0x8000;
                                       yintercept=(ytile<<TILESHIFT)-0x8000;
                                       xintercept=xintercept-(xstep>>1);
                                    
                                       xtile = (short) (xintercept >> TILESHIFT);
                                       HitHorizWall();
                                   }
                                   else if (xtilestep == -1  && ytilestep == 1)          // NE - meeting cut corners
                                   {                                           
                                       if((int32_t)((word)yintercept)+(ystep>>1)>0x8000)
                                       {
                                                                                                                                     
                                             int32_t yintbuf=yintercept+(ystep>>1);
                                             if((yintbuf>>16)!=(yintercept>>16))
                                             goto passvert;
                               
                                          //   if((word)yintbuf<TILEGLOBAL/2) goto passvert;  
                                          
                                       //if((int32_t)((word)yintercept)+ystep>0x8000) // step from opposite side a full tile ahead to cut it
                                       //  goto passvert;  

                                       xintercept=(xtile<<TILESHIFT)-0x8000;
                                       yintercept=yintercept+(ystep>>1);
                                    
                                       ytile = (short) (yintercept >> TILESHIFT);
                                       HitVertWall();                                       
                                       }
                                       else {
                                       int32_t xintbuf=xintercept+(xstep>>1);

                                       //if((xintbuf>>16)!=(xintercept>>16)) goto passvert;
                                        
                                       //if((int32_t)((word)yintercept)+ystep<0x8000) //full tile
                                       //if((int32_t)((word)yintercept)+(ystep>>1)<0x8000) //half tile
                                       //  goto passvert;     
                                        if((int32_t)((word)yintercept)+ystep<0x8000) // step from opposite side a full tile ahead to cut it
                                         goto passvert;   
                                           
                                       yintercept=(yintercept&0xffff0000)+0x8000;
                                       xintercept=xintercept-(xstep>>1);
                                    
                                       xtile = (short) (xintercept >> TILESHIFT);
                                       HitHorizWall();
                                       }
                                      /* }
                                       else
                                       {
                                       
                                       int32_t yintbuf=yintercept+(ystep>>1);
                                       if((yintbuf>>16)!=(yintercept>>16))
                                          goto passvert;
                               
                                       if((word)yintbuf<TILEGLOBAL/2) goto passvert;  

                                       xintercept=(xtile<<TILESHIFT)-0x8000;
                                       yintercept=yintercept+(ystep>>1);
                                    
                                       ytile = (short) (yintercept >> TILESHIFT);
                                       HitVertWall();*/
                                     //  }
                            /*       }  
                                   else if (xtilestep == 1 && ytilestep == -1)          // SW
                                   {
                                       int32_t xintbuf=xintercept-(xstep>>1);
                                       int32_t xint=xintercept;

                                       //if((int32_t)((word)yintercept)+ystep<0x8000) //full tile
                                       if((int32_t)((word)yintercept)+(ystep>>1)<0x8000) //half tile
                                          goto passvert;

                                       yintercept=(yintercept&0xffff0000)+0x8000;
                                       xintercept=xintercept-(xstep>>1);
                                    
                                       xtile = (short) (xintercept >> TILESHIFT);
                                       HitHorizWall();
                                   }
                            }
                                // VERTICAL REAL SIDE
                            else
                            {  
                                   // int32_t xintbuf=xintercept+(xstep>>1);
                                   //if((xintbuf>>16)!=(xintercept>>16) && lastside!=3)
                                   //  goto passhoriz;

                                   if (xtilestep == 1)
                                   {
                                        //if((word)yintercept<0x8000) goto passvert;           
                                        xintercept=(xtile<<TILESHIFT);//-0x8000;
                                   }
                                   else if (xtilestep == -1  && ytilestep == 1)  // NE
                                   {
                                       int32_t yintbuf=yintercept+(ystep>>1);
                                       if((yintbuf>>16)!=(yintercept>>16))
                                          goto passvert;
                               
                                      // if((int32_t)((word)yintercept)+ystep > TILEGLOBAL/2) goto passvert;  

                                       yintercept = yintercept+(ystep>>1);      
                                       xintercept=(xtile<<TILESHIFT)-0x8000;
                             
                                   }
                                   else   
                                   {
                                   
                                       int32_t yintbuf=yintercept+(ystep>>1);
                                       if((yintbuf>>16)!=(yintercept>>16))// && lastside!=3)
                                          goto passvert;
                               
                                       if((word)yintbuf<TILEGLOBAL/2) goto passvert;  

                                       yintercept = yintbuf;      
                                       xintercept=(xtile<<TILESHIFT)-0x8000; 
                                   }
                           
                                //if((word)yintercept>0x8000) goto passvert;  
                                //xintercept=(xtile<<TILESHIFT);//-0x8000;
                                ytile = (short) (yintercept >> TILESHIFT);
                                *((byte *)spotsaw+xspot)=1;
                                HitVertWall();
                            }
                       }  */
                       else
                       {               
                           xintercept=xtile<<TILESHIFT;
                           ytile = (short) (yintercept >> TILESHIFT);
                           *((byte *)spotsaw+xspot)=1;
                           HitVertWall();
                       }
                    }
                }
                break;
            }
passvert:
            *((byte *)spotvis+xspot)=1;
            *((byte *)spotsaw+xspot)= 1;
            xtile+=xtilestep;
            yintercept+=ystep;
            xspot=(word)((xtile<<mapshift)+((uint32_t)yintercept>>16));
        }
        while(1);
        continue;

        do
        {
            if(xtilestep==-1 && (xintercept>>16)<=xtile) goto vertentry;
            if(xtilestep==1 && (xintercept>>16)>=xtile) goto vertentry;
horizentry:
            if((uint32_t)xintercept>mapwidth*65536-1 || (word)ytile>=mapheight)
            {
                if(ytile<0) yintercept=0, ytile=0;
                else if(ytile>=mapheight) yintercept=mapheight<<TILESHIFT, ytile=mapheight-1;
                else ytile=(short) (yintercept >> TILESHIFT);
                if(xintercept<0) xintercept=0, xtile=0;
                else if(xintercept>=(mapwidth<<TILESHIFT)) xintercept=mapwidth<<TILESHIFT, xtile=mapwidth-1;
                xspot=0xffff;
                tilehit=0;
                HitVertBorder();
                break;
            }
            if(yspot>=maparea) break;
            tilehit=((tiletype *)tilemap)[yspot];
            darkhit=((tiletype *)darkmap)[yspot];
            quarterhit =((tiletype *)quarterwalls)[yspot];            
            //if (RefreshCorner(true)) goto vertentry;
            if(tilehit)
            { 
                if(tilehit&BIT_DOOR)
                {    
                     // push door towards player x-             
                    if (MAPSPOT (xtile-xtilestep,ytile,1)==239 || MAPSPOT (xtile-xtilestep,ytile,1)==241)  
                    {            
                        if (ytilestep == -1)                      // push door back when player is right of the tile   || P
                        {   
                            int32_t xintbuf=xintercept+xstep;
                            
                            if((xintbuf>>16)!=(xintercept>>16) && lastside!=3)
                                goto passhoriz;
                            if((word)xintbuf<doorposition[tilehit&LAST_DOORNUM])
                                goto passhoriz;
                            xintercept=xintbuf;
                            yintercept=(ytile<<TILESHIFT);
                            xtile = (short) (xintercept >> TILESHIFT);
                            HitHorizDoor();
                        }
                        else if (ytilestep == 1)                  // push door forward when player is left of the tile P ||
                        {
                            int32_t xintbuf=xintercept;
                            if((word)xintbuf<doorposition[tilehit&LAST_DOORNUM])
                                goto passhoriz;

                            yintercept=(ytile<<TILESHIFT);
                            xtile = (short) (xintercept >> TILESHIFT);
                            HitHorizDoor();
                        }
                    }
                     // push door away from player x+
                    else if (MAPSPOT (xtile-xtilestep,ytile,1)==238 || MAPSPOT (xtile-xtilestep,ytile,1)==240)  
                    {
                        if (ytilestep == 1)                            // push door back when player is left of the tile  P ||
                        { 
                            int32_t xintbuf=xintercept+xstep;
                            
                            if((xintbuf>>16)!=(xintercept>>16) && lastside!=3)
                                goto passhoriz;
                            if((word)xintbuf<doorposition[tilehit&LAST_DOORNUM])
                                goto passhoriz;
                            xintercept=xintbuf;
                            yintercept=(ytile<<TILESHIFT)|0xffff;
                            xtile = (short) (xintercept >> TILESHIFT);
                            HitHorizDoor();
                        }
                        else if (ytilestep == -1)                  // push door forward when player is right of the tile  || P
                        {
                            int32_t xintbuf=xintercept;
                            if((word)xintbuf<doorposition[tilehit&LAST_DOORNUM])
                                goto passhoriz;

                            yintercept=(ytile<<TILESHIFT)+0xffff;
                            xtile = (short) (xintercept >> TILESHIFT);
                            HitHorizDoor();
                        }
                    }
                    else
                    {                              
                         int32_t xintbuf=xintercept+(xstep>>1);

                         if((xintbuf>>16)!=(xintercept>>16) && lastside!=3)
                             goto passhoriz;
                         if((word)xintbuf<doorposition[tilehit&LAST_DOORNUM])
                             goto passhoriz;
                         xintercept=xintbuf;
                         yintercept=(ytile<<TILESHIFT)+0x8000;
                         xtile = (short) (xintercept >> TILESHIFT);
                         HitHorizDoor();
                    }
                }
                else
                {
                    if(tilehit==BIT_WALL)
                    {
                        if(pwalldir==di_north || pwalldir==di_south)
                        {
                            int32_t xintbuf;
                            int pwallposnorm;
                            int pwallposinv;
                            if(pwalldir==di_north)
                            {
                                pwallposnorm = 64-pwallpos;
                                pwallposinv = pwallpos;
                            }
                            else
                            {
                                pwallposnorm = pwallpos;
                                pwallposinv = 64-pwallpos;
                            }
                            if(pwalldir == di_south && ytile==pwally && ((uint32_t)xintercept>>16)==pwallx
                                || pwalldir == di_north && !(ytile==pwally && ((uint32_t)xintercept>>16)==pwallx))
                            {
                                xintbuf=xintercept+((xstep*pwallposnorm)>>6);
                                if((xintbuf>>16)!=(xintercept>>16))
                                    goto passhoriz;

                                yintercept=(ytile<<TILESHIFT)+TILEGLOBAL-(pwallposinv<<10);
                                xintercept=xintbuf;
                                xtile = (short) (xintercept >> TILESHIFT);
                                tilehit=pwalltile;
                                HitHorizWall();
                            }
                            else
                            {
                                xintbuf=xintercept+((xstep*pwallposinv)>>6);
                                if((xintbuf>>16)!=(xintercept>>16))
                                    goto passhoriz;

                                yintercept=(ytile<<TILESHIFT)-(pwallposinv<<10);
                                xintercept=xintbuf;
                                xtile = (short) (xintercept >> TILESHIFT);
                                tilehit=pwalltile;
                                HitHorizWall();
                            }
                        }
                        else
                        {
                            int pwallposi = pwallpos;
                            if(pwalldir==di_west) pwallposi = 64-pwallpos;
                            if(pwalldir==di_east && (word)xintercept<(pwallposi<<10)
                                    || pwalldir==di_west && (word)xintercept>(pwallposi<<10))
                            {
                                if(((uint32_t)xintercept>>16)==pwallx && ytile==pwally)
                                {
                                    if(pwalldir==di_east && (int32_t)((word)xintercept)+xstep<(pwallposi<<10)
                                            || pwalldir==di_west && (int32_t)((word)xintercept)+xstep>(pwallposi<<10))
                                        goto passhoriz;

                                    if(pwalldir==di_east)
                                        xintercept=(xintercept&0xffff0000)+(pwallposi<<10);
                                    else
                                        xintercept=(xintercept&0xffff0000)-TILEGLOBAL+(pwallposi<<10);
                                    yintercept=yintercept-((ystep*(64-pwallpos))>>6);
                                    ytile = (short) (yintercept >> TILESHIFT);
                                    tilehit=pwalltile;
                                    HitVertWall();
                                }
                                else
                                {
                                    texdelta = -(pwallposi<<10);
                                    yintercept=ytile<<TILESHIFT;
                                    xtile = (short) (xintercept >> TILESHIFT);
                                    tilehit=pwalltile;
                                    HitHorizWall();
                                }
                            }
                            else
                            {
                                if(((uint32_t)xintercept>>16)==pwallx && ytile==pwally)
                                {
                                    texdelta = -(pwallposi<<10);
                                    yintercept=ytile<<TILESHIFT;
                                    xtile = (short) (xintercept >> TILESHIFT);
                                    tilehit=pwalltile;
                                    HitHorizWall();
                                }
                                else
                                {
                                    if(pwalldir==di_east && (int32_t)((word)xintercept)+xstep>(pwallposi<<10)
                                            || pwalldir==di_west && (int32_t)((word)xintercept)+xstep<(pwallposi<<10))
                                        goto passhoriz;

                                    if(pwalldir==di_east)
                                        xintercept=(xintercept&0xffff0000)-((64-pwallpos)<<10);
                                    else
                                        xintercept=(xintercept&0xffff0000)+((64-pwallpos)<<10);
                                    yintercept=yintercept-((ystep*pwallpos)>>6);
                                    ytile = (short) (yintercept >> TILESHIFT);
                                    tilehit=pwalltile;
                                    HitVertWall();
                                }
                            }
                        }
                    }
                    else
                    {
                        // make all of this nonsense into one function one day     
                        if (quarterhit == 1)
                        {
                          if (!PartHorizWallNW (xstep,ystep))             
                              goto passhoriz;
                        }
                        else if (quarterhit == 2)
                        {
                          if (!PartHorizWallNE (xstep,ystep))             
                              goto passhoriz;
                        } 
                        else if (quarterhit == 3)
                        {
                          if (!PartHorizWallSE (xstep,ystep))             
                              goto passhoriz;
                        }
                        else if (quarterhit == 4)
                        {
                          if (!PartHorizWallSW (xstep,ystep))             
                              goto passhoriz;
                        }  
                        else if (quarterhit == 5)
                        {
                          if (!HalfHorizWallTopW2E(xstep,ystep))             
                              goto passhoriz;
                        }
                        else if (quarterhit == 6)
                        {
                          if (!HalfHorizWallBotW2E(xstep,ystep))             
                              goto passhoriz;
                        }  
                        else if (quarterhit == 7)
                        {
                          if (!HalfHorizWallLeftN2S(xstep,ystep))             
                              goto passhoriz;
                        }  
                        else if (quarterhit == 8)
                        {
                          if (!HalfHorizWallRightN2S(xstep,ystep))             
                              goto passhoriz;
                        }  
                        else if (quarterhit == 9)
                        {
                          if (!TriHorizWallNW(xstep,ystep))             
                              goto passhoriz;
                        }  
                        else if (quarterhit == 10)
                        {
                          if (!TriHorizWallNE(xstep,ystep))             
                              goto passhoriz;
                        }  
                        else if (quarterhit == 11)
                        {
                          if (!TriHorizWallSE(xstep,ystep))             
                              goto passhoriz;
                        }  
                        else if (quarterhit == 12)
                        {
                          if (!TriHorizWallSW(xstep,ystep))             
                              goto passhoriz;
                        }  
                        else if (quarterhit == 13)
                        {
                          if (!TwoHorizWallNW2SE(xstep,ystep))             
                              goto passhoriz;
                        }  
                        else if (quarterhit == 14)
                        {
                          if (!TwoHorizWallNE2SW(xstep,ystep))             
                              goto passhoriz;
                        }  
                        else if (quarterhit == 15)
                        {           
                              goto passhoriz;
                        }    
                        /*        
                            // ytilestep = -1       south of wall
                            // ytilestep = 1        north of wall
                            // xtilestep = -1       east of wall       
                            // xtilestep = 1        west of wall  
                            
                            //
                            // side of horizontal quarter, clip into == vertical wall
                            // 
                            if((word)xintercept>0x8000)
                            {   
                                 if (ytilestep == -1)             // S
                                 {
                                     int32_t yintbuf=yintercept-(ystep>>1);

                                     //if((int32_t)((word)xintercept)+xstep>0x8000) //full tile
                                     if((int32_t)((word)xintercept)+(xstep>>1)>0x8000) //half tile
                                         goto passhoriz;

                                     //xintercept=(xintercept&0xffff0000)-0x8000;
                                     xintercept=(xtile<<TILESHIFT)+0x8000;
                                     yintercept=yintercept-(ystep>>1);
                                    
                                     ytile = (short) (yintercept >> TILESHIFT);
                                     HitVertWall();
                                 }
                                 else if (xtilestep == -1 && ytilestep == 1)               // NE                  
                                 {                    
                                     /*int32_t yintbuf=yintercept-(ystep>>1);
                                     
                                     //if((yintbuf>>16)!=(yintercept>>16))
                                     //   goto passhoriz;

                                     //if((int32_t)((word)xintercept)-xstep<0x8000) //full tile
                                     if((int32_t)((word)xintercept)+(xstep>>1)<0x8000) //half tile
                                         goto passhoriz;

                                     xintercept=(xintercept&0xffff0000)-0x8000;
                                     yintercept=yintercept-(ystep>>1);
                                    
                                     ytile = (short) (yintercept >> TILESHIFT);
                                     HitVertWall();*/
                            /*         
                                     
                                    int32_t xintbuf=xintercept+(xstep>>1);

                                  if((word)xintbuf>TILEGLOBAL/2)
                                  {  
                                     int32_t yintbuf=yintercept+(ystep>>1);
                                                                
                                     //if((yintbuf>>16)!=(yintercept>>16))
                                     //   goto passhoriz;

                                     //if((int32_t)((word)xintercept)-xstep<0x8000) //full tile
                                     if((int32_t)((word)xintercept)+(xstep>>1)<0x8000) //half tile (right side)
                                        goto passhoriz;
                                        
                                     if((int32_t)((word)xintercept)+xstep>0x8000) // step full tile from pos to opposite end to cut (left side)
                                        goto passhoriz;

                                     xintercept=(xintercept&0xffff0000)-0x8000;
                                     yintercept=yintercept-(ystep>>1);
                                    
                                     ytile = (short) (yintercept >> TILESHIFT);
                                     HitVertWall();                           
                                  }else
                                  {
                                    if((xintbuf>>16)!=(xintercept>>16))
                                       goto passhoriz;
                               
                                   // if((word)xintbuf>TILEGLOBAL/2) goto passhoriz;  

                                   xintercept = xintbuf;      
                                   yintercept=(ytile<<TILESHIFT)+0x8000;
                               
                                   xtile = (short) (xintercept >> TILESHIFT);
                                   *((byte *)spotsaw+yspot)=1;
                                   HitHorizWall();
                                   }
                                 }
                                 else if (xtilestep == 1 && ytilestep == 1)               // NW                 
                                 {                    
                                     //int32_t yintbuf=yintercept+(ystep>>1);
                                     
                                     //if((yintbuf>>16)!=(yintercept>>16))
                                     //   goto passhoriz;

                                     //if((int32_t)((word)xintercept)+xstep>0x8000) //full tile
                                     if((int32_t)((word)xintercept)+(xstep>>1)>0x8000) //half tile
                                         goto passhoriz;

                                     xintercept=(xintercept&0xffff0000)+0x8000;
                                     yintercept=yintercept-(ystep>>1);
                                    
                                     ytile = (short) (yintercept >> TILESHIFT);
                                     HitVertDoor();
                                 }
                            }
                              
                            //
                            // side of horizontal quarter wall, cut it at half tile etc
                            // 
                            else
                            {         
                               // push real y-tile-coordinate forward?
                               //if (ytilestep == 1)
                               //   ypart = TILEGLOBAL/2; 
                               //else 
                               //   ypart = 0;
                                                            
                               if (ytilestep == 1)
                               {     
                                   int32_t xintbuf=xintercept+(xstep>>1);
                                    if((xintbuf>>16)!=(xintercept>>16))// && lastside!=3)
                                       goto passhoriz;
                               
                                   if((word)xintbuf>TILEGLOBAL/2) goto passhoriz;  

                                   xintercept = xintbuf;      
                                   yintercept=(ytile<<TILESHIFT)+0x8000;
                               }
                               else if (ytilestep == -1)
                               {
                                   //if((word)xintercept>TILEGLOBAL/2) goto passhoriz;                          
                                   yintercept=(ytile<<TILESHIFT);
                               }
                               
                                xtile = (short) (xintercept >> TILESHIFT);
                                *((byte *)spotsaw+yspot)=1;
                                HitHorizWall();
                            }
                       }  */
                       else
                       { 
                           yintercept=ytile<<TILESHIFT;
                           xtile = (short) (xintercept >> TILESHIFT);
                           *((byte *)spotsaw+yspot)=1;
                           HitHorizWall();
                       }
                    }      
                }
                break;
            }
passhoriz:
            *((byte *)spotvis+yspot)=1;
            *((byte *)spotsaw+yspot)=1;
            ytile+=ytilestep;
            xintercept+=xstep;
            yspot=(word)((((uint32_t)xintercept>>16)<<mapshift)+ytile);
        }
        while(1);
    }
}

/*
====================
=
= WallRefresh
=
====================
*/

void WallRefresh (void)
{
    xpartialdown = viewx&(TILEGLOBAL-1);
    xpartialup = TILEGLOBAL-xpartialdown;
    ypartialdown = viewy&(TILEGLOBAL-1);
    ypartialup = TILEGLOBAL-ypartialdown;

    min_wallheight = viewheight;
    lastside = -1;                  // the first pixel is on a new wall
    AsmRefresh ();
    ScalePost ();                   // no more optimization on last post
}

void CalcViewVariables()
{
    viewangle = player->angle;
    midangle = viewangle*(FINEANGLES/ANGLES);
    viewsin = sintable[viewangle];
    viewcos = costable[viewangle];
    viewx = player->x - FixedMul(focallength,viewcos);
    viewy = player->y + FixedMul(focallength,viewsin);

    focaltx = (short)(viewx>>TILESHIFT);
    focalty = (short)(viewy>>TILESHIFT);

    viewtx = (short)(player->x >> TILESHIFT);
    viewty = (short)(player->y >> TILESHIFT);
}

//==========================================================================

/*
========================
=
= ThreeDRefresh
=
========================
*/

void    ThreeDRefresh (void)
{
//
// clear out the traced array
//
    memset(spotvis,0,maparea);
#ifdef PLAYDEMOLIKEORIGINAL      // ADDEDFIX 30 - Chris
    if (DEMOCOND_SDL)
#endif
    if (!tilemap[player->tilex][player->tiley] ||
         tilemap[player->tilex][player->tiley] & BIT_DOOR)
    spotvis[player->tilex][player->tiley] = 1;       // Detect all sprites over player fix

    vbuf = VL_LockSurface(screenBuffer);
    if(vbuf == NULL) return;

    vbuf += screenofs;
    vbufPitch = bufferPitch;

    CalcViewVariables();

//
// follow the walls from there to the right, drawing as we go
//
    VGAClearScreen ();
#if defined(USE_FEATUREFLAGS) && defined(USE_STARSKY)
    if(GetFeatureFlags() & FF_STARSKY)
        DrawStarSky(vbuf, vbufPitch);
#endif

    WallRefresh ();

//#if defined(USE_FEATUREFLAGS) && defined(USE_PARALLAX)
//    if(GetFeatureFlags() & FF_PARALLAXSKY)
    if (parallaxstart[gamestate.episode*10+mapon+1])
        DrawParallax(vbuf, vbufPitch);
//#endif
#if defined(USE_FEATUREFLAGS) && defined(USE_CLOUDSKY)
    if(GetFeatureFlags() & FF_CLOUDSKY)
        DrawClouds(vbuf, vbufPitch, min_wallheight);
#endif
#ifdef USE_FLOORCEILINGTEX
   if (tflats)
    DrawFloorAndCeiling(vbuf, vbufPitch, min_wallheight);
#endif

//
// draw all the scaled images
//
    DrawScaleds();                  // draw scaled stuff

#if defined(USE_FEATUREFLAGS) && defined(USE_RAIN)
    if(GetFeatureFlags() & FF_RAIN)
        DrawRain(vbuf, vbufPitch);
#endif
#if defined(USE_FEATUREFLAGS) && defined(USE_SNOW)
    if(GetFeatureFlags() & FF_SNOW)
        DrawSnow(vbuf, vbufPitch);
#endif

    DrawPlayerWeapon ();    // draw player's hands

    if(Keyboard[sc_Tab] && viewsize == 21 && gamestate.weapon != -1 && !hudmod)
        ShowActStatus();
        
    DrawHud ();

    VL_UnlockSurface(screenBuffer);
    vbuf = NULL;

//
// show screen and time last cycle
//

    if (fizzlein)
    {
        FizzleFade(screenBuffer, 0, 0, screenWidth, screenHeight, 20, false);
        fizzlein = false;

        lasttimecount = GetTimeCount();          // don't make a big tic count
    }
    else
    {
#ifndef REMDEBUG
        if (fpscounter)
        {
            fontnumber = 0;
            SETFONTCOLOR(7,127);
            PrintX=4; PrintY=1;
            VWB_Bar(0,0,50,10,bordercol);
            US_PrintSigned(fps);
            US_Print(" fps");
        }
#endif
        SDL_BlitSurface(screenBuffer, NULL, screen, NULL);
        SDL_Flip(screen);
    }

#ifndef REMDEBUG
    if (fpscounter)
    {
        fps_frames++;
        fps_time+=tics;

        if(fps_time>60)
        {
            fps_time-=60;
            fps=fps_frames;//<<1;
            fps_frames=0;
        }
    }
#endif
}
