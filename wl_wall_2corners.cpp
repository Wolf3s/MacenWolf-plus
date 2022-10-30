// WL_WALL.C
//
// HexNex Magic Hax
//
// aka Nexion's super blocky quartered tiles
//
// recreating MAC's bsp walls the insane way
// they are only visual exploits in the original

#include "wl_def.h"
#pragma hdrstop

/*
=====================
=
= TwoHorizWallNW2SE 
=               
= draws horizontal side of \-shape walls
= returns true to draw
= returns false to tell rays to go thru (goto passhoriz)
=      N 
=     ___
= NW ||| |  NE
= SW |_|||  SE
=      S
=====================
*/
boolean TwoHorizWallNW2SE (int32_t xstep, int32_t ystep)
{                           
     //
     // left side when looking north
     // 
     if ((word)xintercept < 0x8000 && ytilestep == -1) 
     {   
          if (xtilestep == 1)                                      // W
          {
                if ((int32_t)((word)xintercept)+(xstep>>1) < 0x8000) //half tile in
                {         
                    yintercept=(ytile<<TILESHIFT)-0x8000; 
                    xintercept=xintercept+(xstep>>1);            
                    xtile = (short) (xintercept >> TILESHIFT);       
                    HitHorizWall();     
                }
                else
                {
                    xintercept=(xtile<<TILESHIFT)-0x8000;
                    yintercept=yintercept-(ystep>>1);                  
                    ytile = (short) (yintercept >> TILESHIFT);
                    HitVertWall();  
                }
          } 
          else if (xtilestep == -1)                                 // E  (dent part is visible when facing close to the middle of the tile)            
          {    
               if ((int32_t)((word)xintercept)+(xstep>>1) < 0x8000) //half tile in
               {    
                   int32_t xintbuf=xintercept+(xstep>>1);
                   if ((xintbuf>>16)!=(xintercept>>16))
                         return false;  
                         
                   yintercept=(ytile<<TILESHIFT)-0x8000; 
                   xintercept=xintercept+(xstep>>1);            
                   xtile = (short) (xintercept >> TILESHIFT);       
                   HitHorizWall();      
               }
               else             
                 return false;
          }
     }  
     
     //
     // right side when looking south
     // 
     else if ((word)xintercept > 0x8000 && ytilestep == 1) 
     {   
          if (xtilestep == -1)                                      // E
          {
                if ((int32_t)((word)xintercept)+(xstep>>1) > 0x8000) //half tile in
                {         
                    yintercept=(ytile<<TILESHIFT)+0x8000; 
                    xintercept=xintercept+(xstep>>1);            
                    xtile = (short) (xintercept >> TILESHIFT);       
                    HitHorizWall();     
                }
                else
                {
                    xintercept=(xtile<<TILESHIFT)+0x8000;
                    yintercept=yintercept-(ystep>>1);                  
                    ytile = (short) (yintercept >> TILESHIFT);
                    HitVertWall();  
                }
          } 
          else if (xtilestep == 1)                                 // W  (dent part is visible when facing close to the middle of the tile)            
          {    
               if ((int32_t)((word)xintercept)+(xstep>>1) > 0x8000) //half tile in
               {    
                   int32_t xintbuf=xintercept+(xstep>>1);
                   if ((xintbuf>>16)!=(xintercept>>16))
                         return false;  
                         
                   yintercept=(ytile<<TILESHIFT)+0x8000; 
                   xintercept=xintercept+(xstep>>1);            
                   xtile = (short) (xintercept >> TILESHIFT);       
                   HitHorizWall();      
               }
               else             
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
= TwoVertWallNW2SE
=             
= draws vertical side of \ walls
= returns true to draw
= returns false to tell rays to go thru (goto passvert)
=      N 
=     ___
= NW ||| |  NE
= SW |_|||  SE
=      S
=====================
*/

boolean TwoVertWallNW2SE (int32_t xstep, int32_t ystep)
{                                            
          if (xtilestep == -1 )                    // E
          {                    
                if((word)yintercept > 0x8000)           //half tile, lower E 
                {
                    xintercept=(xtile<<TILESHIFT);     
                    ytile = (short) (yintercept >> TILESHIFT);
                    HitVertWall();           
                }  
                else                                     // upper E    
                {              
                    int32_t yintbuf=yintercept+(ystep>>1);
                    
                    if ((int32_t)((word)yintercept)+(ystep>>1) > 0x8000)
                    {
                         //return false;                         
                        yintercept=(ytile<<TILESHIFT)-0x8000; 
                        xintercept=xintercept-(xstep>>1);            
                        xtile = (short) (xintercept >> TILESHIFT);       
                        HitHorizWall();     
                    }
                    else
                    {   
                        if ((yintbuf>>16)!=(yintercept>>16)) return false;
                            
                        xintercept=(xtile<<TILESHIFT)-0x8000;
                        yintercept=yintercept+(ystep>>1); 
                        ytile = (short) (yintercept >> TILESHIFT);
                        HitVertWall(); 
                    }                 
                }                
          } 
          else if (xtilestep == 1 )                    // W
          {                    
                if((word)yintercept < 0x8000)           //half tile, lower W 
                {
                    xintercept=(xtile<<TILESHIFT);     
                    ytile = (short) (yintercept >> TILESHIFT);
                    HitVertWall();           
                }  
                else                                     // upper W    
                {              
                    int32_t yintbuf=yintercept+(ystep>>1);
                    
                    if ((int32_t)((word)yintercept)+(ystep>>1) < 0x8000)
                    {                        
                        yintercept=(ytile<<TILESHIFT)+0x8000; 
                        xintercept=xintercept-(xstep>>1);            
                        xtile = (short) (xintercept >> TILESHIFT);       
                        HitHorizWall();     
                    }
                    else
                    {   
                        if ((yintbuf>>16)!=(yintercept>>16)) return false;
                            
                        xintercept=(xtile<<TILESHIFT)+0x8000;
                        yintercept=yintercept+(ystep>>1); 
                        ytile = (short) (yintercept >> TILESHIFT);
                        HitVertWall(); 
                    }                 
                }                
          }     
      
     *((byte *)spotsaw+xspot)=1;
     return true;
} 

/*
=====================
=
= TwoHorizWallNE2SW 
=              
= draws horizontal side of /-shape walls
= returns true to draw
= returns false to tell rays to go thru (goto passhoriz)
=      N 
=     ___
= NW | |||  NE
= SW |||_|  SE
=      S
=====================
*/
boolean TwoHorizWallNE2SW (int32_t xstep, int32_t ystep)
{                           
     //
     // right side when looking north
     // 
     if ((word)xintercept > 0x8000 && ytilestep == -1) 
     {   
          if (xtilestep == -1)                                      // E
          {
                if ((int32_t)((word)xintercept)+(xstep>>1) > 0x8000) //half tile in
                {         
                    yintercept=(ytile<<TILESHIFT)-0x8000; 
                    xintercept=xintercept+(xstep>>1);            
                    xtile = (short) (xintercept >> TILESHIFT);       
                    HitHorizWall();     
                }
                else
                {
                    xintercept=(xtile<<TILESHIFT)+0x8000;
                    yintercept=yintercept-(ystep>>1);                  
                    ytile = (short) (yintercept >> TILESHIFT);
                    HitVertWall();  
                }
          } 
          else if (xtilestep == 1)                                 // W  (dent part is visible when facing close to the middle of the tile)            
          {    
               if ((int32_t)((word)xintercept)+(xstep>>1) > 0x8000) //half tile in
               {    
                   int32_t xintbuf=xintercept+(xstep>>1);
                   if ((xintbuf>>16)!=(xintercept>>16))
                         return false;  
                         
                   yintercept=(ytile<<TILESHIFT)-0x8000; 
                   xintercept=xintercept+(xstep>>1);            
                   xtile = (short) (xintercept >> TILESHIFT);       
                   HitHorizWall();      
               }
               else             
                 return false;
          }
     }
     
     //
     // left side when looking south
     //       
     else if ((word)xintercept < 0x8000 && ytilestep == 1) 
     {   
          if (xtilestep == 1)                                      // W
          {
                if ((int32_t)((word)xintercept)+(xstep>>1)  < 0x8000) //half tile in
                {         
                    yintercept=(ytile<<TILESHIFT)+0x8000; 
                    xintercept=xintercept+(xstep>>1);            
                    xtile = (short) (xintercept >> TILESHIFT);       
                    HitHorizWall();     
                    //  return false;
                }
                else
                {
                    xintercept=(xtile<<TILESHIFT)-0x8000;
                    yintercept=yintercept-(ystep>>1);                  
                    ytile = (short) (yintercept >> TILESHIFT);
                    HitVertWall();  
                }
          } 
          else if (xtilestep == -1)                                 // E  (dent part is visible when facing close to the middle of the tile)            
          {    
               if ((int32_t)((word)xintercept)+(xstep>>1) < 0x8000) //half tile in
               {    
                   int32_t xintbuf=xintercept+(xstep>>1);
                   if ((xintbuf>>16)!=(xintercept>>16))
                         return false;  
                         
                   yintercept=(ytile<<TILESHIFT)+0x8000; 
                   xintercept=xintercept+(xstep>>1);            
                   xtile = (short) (xintercept >> TILESHIFT);       
                   HitHorizWall();      
               }
               else             
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
= TwoVertWallNE2SW 
=      
= draws vertical side of / walls
= returns true to draw
= returns false to tell rays to go thru (goto passvert)
=      N 
=     ___
= NW | |||  NE
= SW |||_|  SE
=      S
=====================
*/

boolean TwoVertWallNE2SW (int32_t xstep, int32_t ystep)
{                                            
          if (xtilestep == 1 )                    // W
          {                    
                if((word)yintercept > 0x8000)          //half tile, lower W 
                {
                    xintercept=(xtile<<TILESHIFT);     
                    ytile = (short) (yintercept >> TILESHIFT);
                    HitVertWall();           
                }  
                else                                     // upper W    
                {              
                    int32_t yintbuf=yintercept+(ystep>>1);
                    
                    if ((int32_t)((word)yintercept)+(ystep>>1) > 0x8000)
                    {                        
                        yintercept=(ytile<<TILESHIFT)-0x8000; 
                        xintercept=xintercept-(xstep>>1);            
                        xtile = (short) (xintercept >> TILESHIFT);       
                        HitHorizWall();     
                    }
                    else
                    {   
                        if ((yintbuf>>16)!=(yintercept>>16)) return false;
                            
                        xintercept=(xtile<<TILESHIFT)+0x8000;
                        yintercept=yintercept+(ystep>>1); 
                        ytile = (short) (yintercept >> TILESHIFT);
                        HitVertWall(); 
                    }                 
                }                
          } 
          else if (xtilestep == -1 )                    // E
          {                    
                if((word)yintercept < 0x8000)           //half tile, lower E 
                {
                    xintercept=(xtile<<TILESHIFT);     
                    ytile = (short) (yintercept >> TILESHIFT);
                    HitVertWall();           
                }  
                else                                     // upper E    
                {              
                    int32_t yintbuf=yintercept+(ystep>>1);
                    
                    if ((int32_t)((word)yintercept)+(ystep>>1) < 0x8000)
                    {                       
                        yintercept=(ytile<<TILESHIFT)+0x8000; 
                        xintercept=xintercept-(xstep>>1);            
                        xtile = (short) (xintercept >> TILESHIFT);       
                        HitHorizWall();     
                    }
                    else
                    {   
                        if ((yintbuf>>16)!=(yintercept>>16)) return false;
                            
                        xintercept=(xtile<<TILESHIFT)-0x8000;
                        yintercept=yintercept+(ystep>>1); 
                        ytile = (short) (yintercept >> TILESHIFT);
                        HitVertWall(); 
                    }                 
                }                
          }     
      
     *((byte *)spotsaw+xspot)=1;
     return true;
}   
