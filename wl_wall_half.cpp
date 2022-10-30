// WL_WALL.C
//
// HexNex Magic Hax
//
// aka Nexion's super blocky quartered tiles
//
// recreating MAC's bsp walls the insane way
//

#include "wl_def.h"
#pragma hdrstop

/*
=====================
=
= HalfHorizWallRightN2S
=
= draws horizontal side of half walls
= returns true to draw
= returns false to tell rays to go thru (goto passhoriz)
=      N 
=     ___
= NW |  ||  NE
= SW |__||  SE
=      S
=====================
*/

boolean HalfHorizWallRightN2S (int32_t xstep, int32_t ystep)
{                           
     //
     // half of horizontal wall, cuts into to draw vertical wall part
     // 
     
     if ((word)xintercept < 0x8000) 
     {   
          if (xtilestep == 1)                                      // W
          {
                if ((int32_t)((word)xintercept)+xstep < 0x8000) //full tile
                      return false;

                xintercept=(xtile<<TILESHIFT)-0x8000;
                yintercept=yintercept-(ystep>>1);                  
                ytile = (short) (yintercept >> TILESHIFT);
                HitVertWall();
          }
          else if (xtilestep == -1)                                 // E                 
          {                    
                      return false;
          }
     }
                              
     //
     // rest of horizontal wall
     // 
     else
     {                                                               
          yintercept=(ytile<<TILESHIFT);                               
          xtile = (short) (xintercept >> TILESHIFT);
          HitHorizWall();
     }
     
     *((byte *)spotsaw+yspot)=1;
     return true;
}

/*
=====================
=
= HalfVertWallRightN2S
=
= draws vertical side of half walls
= returns true to draw
= returns false to tell rays to go thru (goto passvert)
=
=====================
*/

boolean HalfVertWallRightN2S (int32_t xstep, int32_t ystep)
{                                            
          if (xtilestep == -1)                           // E
          {          
                xintercept=(xtile<<TILESHIFT);
          }
          else if (xtilestep == 1 )                    // W
          {
                int32_t yintbuf=yintercept+(ystep>>1);
                if ((yintbuf>>16)!=(yintercept>>16))
                     return false;

                yintercept = yintercept+(ystep>>1);      
                xintercept=(xtile<<TILESHIFT)+0x8000;                   
          }
                           
          ytile = (short) (yintercept >> TILESHIFT);
          HitVertWall();
      
     *((byte *)spotsaw+xspot)=1;
     return true;
}   

/*
=====================
=
= HalfHorizWallLeftN2S
=
= draws horizontal side of half walls
= returns true to draw
= returns false to tell rays to go thru (goto passhoriz)
=      N 
=     ___
= NW ||| |  NE
= SW |||_|  SE
=      S
=====================
*/

boolean HalfHorizWallLeftN2S (int32_t xstep, int32_t ystep)
{                           
     //
     // half of horizontal wall, cuts into to draw vertical wall part
     // 
     if ((word)xintercept > 0x8000) 
     {   
          if (xtilestep == -1)                                      // E
          {
                if ((int32_t)((word)xintercept)+xstep > 0x8000) //full tile
                      return false;

                xintercept=(xtile<<TILESHIFT)+0x8000;
                yintercept=yintercept-(ystep>>1);                  
                ytile = (short) (yintercept >> TILESHIFT);
                HitVertWall();
          }
          else if (xtilestep == 1)                                 // W                 
          {                    
                      return false;
          }
     }
                              
     //
     // rest of horizontal wall
     // 
     else
     {                                                               
          yintercept=(ytile<<TILESHIFT);                               
          xtile = (short) (xintercept >> TILESHIFT);
          HitHorizWall();
     }
     
     *((byte *)spotsaw+yspot)=1;
     return true;
}

/*
=====================
=
= HalfVertWallLeftN2S
=
= draws vertical side of half walls
= returns true to draw
= returns false to tell rays to go thru (goto passvert)
=
=====================
*/

boolean HalfVertWallLeftN2S (int32_t xstep, int32_t ystep)
{                                            
          if (xtilestep == 1)                           // W
          {          
                xintercept=(xtile<<TILESHIFT);
          }
          else if (xtilestep == -1 )                    // E
          {
                int32_t yintbuf=yintercept+(ystep>>1);
                if ((yintbuf>>16)!=(yintercept>>16))
                     return false;

                yintercept = yintercept+(ystep>>1);      
                xintercept=(xtile<<TILESHIFT)-0x8000;                   
          }
                           
          ytile = (short) (yintercept >> TILESHIFT);
          HitVertWall();
      
     *((byte *)spotsaw+xspot)=1;
     return true;
}   

/*
=====================
=
= HalfHorizWallBotW2E
=
= draws horizontal side of half walls
= returns true to draw
= returns false to tell rays to go thru (goto passhoriz)
=      N 
=     ___
= NW |   |  NE
= SW |---|  SE
=      S
=====================
*/

boolean HalfHorizWallBotW2E(int32_t xstep, int32_t ystep)
{                                                                                                
          if (ytilestep == 1)
          {     
                int32_t xintbuf=xintercept+(xstep>>1);
                if ((xintbuf>>16)!=(xintercept>>16))// && lastside!=3)
                     return false;

                xintercept = xintbuf;      
                yintercept=(ytile<<TILESHIFT)+0x8000;
          }
          else if (ytilestep == -1)
          {                         
                yintercept=(ytile<<TILESHIFT);
          }
                               
          xtile = (short) (xintercept >> TILESHIFT);
          HitHorizWall();
     
     *((byte *)spotsaw+yspot)=1;
     return true;
}

/*
=====================
=
= HalfVertWallBotW2E
=
= draws vertical side of half walls
= returns true to draw
= returns false to tell rays to go thru (goto passvert)
=      N 
=     ___
= NW |   |  NE
= SW |---|  SE
=      S
=====================
*/

boolean HalfVertWallBotW2E(int32_t xstep, int32_t ystep)
{
     //   
     // CUT INTO VERTICAL TILE SIDE TO DRAW THE HORIZONTAL PART      
     //  
     if((word)yintercept<0x8000)
     {                                              
          if (ytilestep == -1)             // S
          { 
                    return false;
          }
          else if (ytilestep == 1)          // N
          {                                           
                     //int32_t xintbuf=xintercept+(xstep>>1);
                     //if((xintbuf>>16)!=(xintercept>>16)) goto passvert;
                                        
                     //if((int32_t)((word)yintercept)+(ystep>>1)<0x8000) //half tile
                     //  goto passvert;     
                     if ((int32_t)((word)yintercept)+ ystep < 0x8000) // step from opposite side a full tile ahead to cut it
                           return false;
                                           
                     yintercept=(ytile<<TILESHIFT)-0x8000;
                     xintercept=xintercept-(xstep>>1);
                     xtile = (short) (xintercept >> TILESHIFT);
                     HitHorizWall();    
          } 
     }
     
     //
     // THE NORMAL VERTICAL SIDE, part that isnt cut into
     //
     else
     {      
          xintercept=(xtile<<TILESHIFT);                   
          ytile = (short) (yintercept >> TILESHIFT);
          HitVertWall();
     }  
     
     *((byte *)spotsaw+xspot)=1;
     return true;
}     

/*
=====================
=
= HalfHorizWallTopW2E
=
= draws horizontal side of half walls
= returns true to draw
= returns false to tell rays to go thru (goto passhoriz)
=      N 
=     ___
= NW |===|  NE
= SW |___|  SE
=      S
=====================
*/

boolean HalfHorizWallTopW2E(int32_t xstep, int32_t ystep)
{                                                                                                
          if (ytilestep == -1)
          {     
                int32_t xintbuf=xintercept+(xstep>>1);
                if ((xintbuf>>16)!=(xintercept>>16))// && lastside!=3)
                     return false;

                xintercept = xintbuf;      
                yintercept=(ytile<<TILESHIFT)-0x8000;
          }
          else if (ytilestep == 1)
          {                         
                yintercept=(ytile<<TILESHIFT);
          }
                               
          xtile = (short) (xintercept >> TILESHIFT);
          HitHorizWall();
     
     *((byte *)spotsaw+yspot)=1;
     return true;
}

/*
=====================
=
= HalfVertWallTopW2E
=
= draws vertical side of half walls
= returns true to draw
= returns false to tell rays to go thru (goto passvert)
=      N 
=     ___
= NW |===|  NE
= SW |___|  SE
=      S
=====================
*/

boolean HalfVertWallTopW2E(int32_t xstep, int32_t ystep)
{
     //   
     // CUT INTO VERTICAL TILE SIDE TO DRAW THE HORIZONTAL PART      
     //  
     if((word)yintercept>0x8000)
     {                                              
          if (ytilestep == 1)             // N
          { 
                // if ((int32_t)((word)yintercept)+ystep < 0x8000) //full tile
                if ((int32_t)((word)yintercept)+(ystep>>1) > 0x8000) //half tile
                    return false;
          }
          else if (ytilestep == -1)          // S 
          {                                           
                     //int32_t xintbuf=xintercept+(xstep>>1);
                     //if((xintbuf>>16)!=(xintercept>>16)) goto passvert;
                                        
                     //if((int32_t)((word)yintercept)+(ystep>>1)<0x8000) //half tile
                     //  goto passvert;     
                     if ((int32_t)((word)yintercept)+ ystep > 0x8000) // step from opposite side a full tile ahead to cut it
                           return false;
                                           
                     yintercept=(ytile<<TILESHIFT)+0x8000;
                     xintercept=xintercept-(xstep>>1);
                     xtile = (short) (xintercept >> TILESHIFT);
                     HitHorizWall();    
          } 
     }
     
     //
     // THE NORMAL VERTICAL SIDE, part that isnt cut into
     //
     else
     {      
          xintercept=(xtile<<TILESHIFT);                   
          ytile = (short) (yintercept >> TILESHIFT);
          HitVertWall();
     }  
     
     *((byte *)spotsaw+xspot)=1;
     return true;
}     

