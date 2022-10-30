// WL_WALL.C
//
// NexHex Magic Hax
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
= PartHorizWallNW
=
= draws horizontal side of quarter walls
= returns true to draw
= returns false to tell rays to go thru (goto passhoriz)
=      N 
=     ___
= NW |_| |  NE
= SW |___|  SE
=      S
=====================
*/

boolean PartHorizWallNW(int32_t xstep, int32_t ystep)
{                            
     //
     // half of horizontal wall, clips into to draw vertical wall part
     // 
     if((word)xintercept>0x8000) 
     {   
          if (ytilestep == 1)                                      // N (NW+NE)
          { 
                //if((int32_t)((word)xintercept)+xstep>0x8000) //full tile
                if ((int32_t)((word)xintercept)+(xstep>>1)>0x8000) //half tile
                      return false;

                xintercept=(xtile<<TILESHIFT)+0x8000;
                yintercept=yintercept-(ystep>>1);                  
                ytile = (short) (yintercept >> TILESHIFT);
                HitVertWall();
          }
          else if (xtilestep == -1 && ytilestep == -1)               // SE  - both cuts meet here               
          {                                                                  
                int32_t xintbuf=xintercept+(xstep>>1);

                if((word)xintbuf > TILEGLOBAL/2)
                {  
                     //int32_t yintbuf=yintercept+(ystep>>1);
                                                                
                     //if ((yintbuf>>16)!=(yintercept>>16)) goto passhoriz;

                     //if ((int32_t)((word)xintercept)-xstep<0x8000) //full tile
                     if ((int32_t)((word)xintercept)+(xstep>>1)<0x8000) //half tile (right side)
                            return false;
                                        
                     if((int32_t)((word)xintercept)+xstep>0x8000) // step full tile from pos to opposite end to cut (left side)
                            return false;

                     xintercept=(xintercept&0xffff0000)-0x8000;
                     yintercept=yintercept-(ystep>>1);
                     ytile = (short) (yintercept >> TILESHIFT);
                     HitVertWall();                           
                }
                else
                {
                     if ((xintbuf>>16)!=(xintercept>>16)) return false;
                               
                     //if ((word)xintbuf>TILEGLOBAL/2) goto passhoriz;  

                     xintercept = xintbuf;      
                     yintercept=(ytile<<TILESHIFT)-0x8000;
                     xtile = (short) (xintercept >> TILESHIFT);
                     HitHorizWall();
                }
          }
          else if (xtilestep == 1 && ytilestep == -1)               // SW                 
          {                    
                //if ((int32_t)((word)xintercept)+xstep>0x8000) //full tile
                if ((int32_t)((word)xintercept)+(xstep>>1)>0x8000) //half tile
                      return false;
          }
     }
                              
     //
     // rest side of horizontal quarter wall, cut it at half tile etc
     // 
     else
     {                                                                     
          if (ytilestep == -1)
          {     
                int32_t xintbuf=xintercept+(xstep>>1);
                if ((xintbuf>>16)!=(xintercept>>16))// && lastside!=3)
                     return false;
                               
                if((word)xintbuf>TILEGLOBAL/2) return false; 

                xintercept = xintbuf;      
                yintercept=(ytile<<TILESHIFT)-0x8000;
          }
          else if (ytilestep == 1)
          {                         
                yintercept=(ytile<<TILESHIFT);
          }
                               
          xtile = (short) (xintercept >> TILESHIFT);
          HitHorizWall();
     }
     
     *((byte *)spotsaw+yspot)=1;
     return true;
}

/*
=====================
=
= PartVertWallSW
=
= draws vertical side of quarter walls
= returns true to draw
= returns false to tell rays to go thru (goto passvert)
=
=====================
*/

boolean PartVertWallNW(int32_t xstep, int32_t ystep)
{
     //   
     // CUTTING INTO VERTICAL TILE SIDE TO DRAW THE 1/2 HORIZONTAL WALL PART      
     //  
     if((word)yintercept>0x8000)
     {                                              
          if (xtilestep == -1 && ytilestep == 1)             // NE
          { 
                // if ((int32_t)((word)yintercept)+ystep<0x8000) //full tile
                if ((int32_t)((word)yintercept)+(ystep>>1) > 0x8000) //half tile
                    return false;
          }
          else if (xtilestep == -1  && ytilestep == -1)          // SE - meeting of the cut corners
          {                                           
                if ((int32_t)((word)yintercept)+(ystep>>1) < 0x8000)
                {                                                                                                                  
                     int32_t yintbuf=yintercept+(ystep>>1);
                     if((yintbuf>>16)!=(yintercept>>16))
                         return false;
                               
                     //if ((word)yintbuf<TILEGLOBAL/2) goto passvert;  
                                          
                     //if((int32_t)((word)yintercept)+ystep>0x8000) // step from opposite side a full tile ahead to cut it
                     //  goto passvert;  

                     xintercept=(xtile<<TILESHIFT)-0x8000;
                     yintercept=yintercept+(ystep>>1);             
                     ytile = (short) (yintercept >> TILESHIFT);
                     HitVertWall();                                       
                }
                else 
                {
                     int32_t xintbuf=xintercept+(xstep>>1);

                     //if((xintbuf>>16)!=(xintercept>>16)) goto passvert;
                                        
                     //if((int32_t)((word)yintercept)+ystep<0x8000) //full tile
                     //if((int32_t)((word)yintercept)+(ystep>>1)<0x8000) //half tile
                     //  goto passvert;     
                     if ((int32_t)((word)yintercept)+ystep > 0x8000) // step from opposite side a full tile ahead to cut it
                           return false;
                                           
                     yintercept=(ytile<<TILESHIFT)+0x8000;
                     xintercept=xintercept-(xstep>>1);
                     xtile = (short) (xintercept >> TILESHIFT);
                     HitHorizWall();
                }
          } 
          else if (xtilestep == 1 )          // SW+NW
          {
                //if((int32_t)((word)yintercept)+ystep<0x8000) //full tile
                if ((int32_t)((word)yintercept)+(ystep>>1) > 0x8000) //half tile
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
          if (xtilestep == 1)                                         // W                        
          {
                //if((word)yintercept<0x8000) goto passvert;           
                xintercept=(xtile<<TILESHIFT);//-0x8000;
          }
          else if (xtilestep == -1)                                    // E
          {
                int32_t yintbuf=yintercept+(ystep>>1);
                if ((yintbuf>>16)!=(yintercept>>16))
                     return false;
                               
                //if ((int32_t)((word)yintercept)+ystep > TILEGLOBAL/2) goto passvert;  
                if((word)yintbuf > TILEGLOBAL/2) return false; 

                yintercept = yintercept+(ystep>>1);      
                xintercept=(xtile<<TILESHIFT)-0x8000;                   
          }

          ytile = (short) (yintercept >> TILESHIFT);
          HitVertWall();
     }  
     
     *((byte *)spotsaw+xspot)=1;
     return true;
}     

/*
=====================
=
= PartHorizWallNE
=
= draws horizontal side of quarter walls
= returns true to draw
= returns false to tell rays to go thru (goto passhoriz)
=      N 
=     ___
= NW | |_|  NE
= SW |___|  SE
=      S
=====================
*/

boolean PartHorizWallNE(int32_t xstep, int32_t ystep)
{                                   
     //
     // half of horizontal wall, clips into to draw vertical wall part
     // 
     if((word)xintercept<0x8000) 
     {   
          if (ytilestep == 1)                                      // N (NW+NE)
          {
                //if((int32_t)((word)xintercept)+xstep>0x8000) //full tile
                if ((int32_t)((word)xintercept)+(xstep>>1)<0x8000) //half tile
                      return false;

                xintercept=(xtile<<TILESHIFT)-0x8000;
                yintercept=yintercept-(ystep>>1);                  
                ytile = (short) (yintercept >> TILESHIFT);
                HitVertWall();
          }
          else if (xtilestep == 1 && ytilestep == -1)               // SW  - cuts meet here               
          {                                                                  
                int32_t xintbuf=xintercept+(xstep>>1);

                if((word)xintbuf < TILEGLOBAL/2)
                {   
                     //if ((int32_t)((word)xintercept)-xstep<0x8000) //full tile
                     if ((int32_t)((word)xintercept)+(xstep>>1) > 0x8000) //half tile (right side)
                            return false;
                                        
                     if((int32_t)((word)xintercept)+xstep<0x8000) // step full tile from pos to opposite end to cut (left side)
                            return false;

                     xintercept=(xtile<<TILESHIFT)-0x8000;
                     yintercept=yintercept-(ystep>>1);
                     ytile = (short) (yintercept >> TILESHIFT);
                     HitVertWall();                           
                }
                else
                {
                     if ((xintbuf>>16)!=(xintercept>>16)) return false;
                               
                     //if ((word)xintbuf>TILEGLOBAL/2) goto passhoriz;  

                     xintercept = xintbuf;      
                     yintercept=(ytile<<TILESHIFT)-0x8000;
                     xtile = (short) (xintercept >> TILESHIFT);
                     HitHorizWall();
                }
          }
          else if (xtilestep == -1 && ytilestep == -1)               // SE                 
          {                    
                      return false;
          }
     }
                              
     //
     // rest side of horizontal quarter wall, cut it at half tile etc
     // 
     else
     {                                                                     
          if (ytilestep == -1)                                     // S           
          {     
                int32_t xintbuf=xintercept+(xstep>>1);
                if ((xintbuf>>16)!=(xintercept>>16))
                     return false;
                               
                if((word)xintbuf<TILEGLOBAL/2) return false; 

                xintercept = xintbuf;      
                yintercept=(ytile<<TILESHIFT)-0x8000;
          }
          else if (ytilestep == 1)                             // N
          {                      
                yintercept=(ytile<<TILESHIFT);
          }
                               
          xtile = (short) (xintercept >> TILESHIFT);
          HitHorizWall();
     }
     
     *((byte *)spotsaw+yspot)=1;
     return true;
}

/*
=====================
=
= PartVertWallNE
=
= draws vertical side of quarter walls
= returns true to draw
= returns false to tell rays to go thru (goto passvert)
=
=====================
*/

boolean PartVertWallNE(int32_t xstep, int32_t ystep)
{
     //   
     // CUTTING INTO VERTICAL TILE SIDE TO DRAW THE 1/2 HORIZONTAL WALL PART      
     //  
     // =      N yint < 0x8000
     // =     ___
     // = NW | |_|  NE
     // = SW |___|  SE
     // =      S yint > 0x8000
     if((word)yintercept>0x8000) 
     {                                              
          if (xtilestep == 1 && ytilestep == 1)             // NW
          {     
                return false;            
          }
          else if (xtilestep == 1 && ytilestep == -1)          // SW - meeting of the cut corners
          {                                         
                if ((int32_t)((word)yintercept)+(ystep>>1) < 0x8000)
                {                                                                                                                  
                     int32_t yintbuf=yintercept+(ystep>>1);
                     if((yintbuf>>16)!=(yintercept>>16))
                         return false;
                               
                     //if ((word)yintbuf<TILEGLOBAL/2) goto passvert;  
                                          
                     //if((int32_t)((word)yintercept)+ystep>0x8000) // step from opposite side a full tile ahead to cut it
                     //  goto passvert;  

                     xintercept=(xtile<<TILESHIFT)+0x8000;
                     yintercept=yintercept+(ystep>>1);             
                     ytile = (short) (yintercept >> TILESHIFT);
                     HitVertWall();                                       
                }
                else 
                {
                     int32_t xintbuf=xintercept+(xstep>>1);

                     //if((xintbuf>>16)!=(xintercept>>16)) goto passvert;
                                        
                     //if((int32_t)((word)yintercept)+ystep<0x8000) //full tile
                     //if((int32_t)((word)yintercept)+(ystep>>1)<0x8000) //half tile
                     //  goto passvert;     
                     if ((int32_t)((word)yintercept)+ystep > 0x8000) // step from opposite side a full tile ahead to cut it
                           return false;
                                           
                     yintercept=(ytile<<TILESHIFT)+0x8000;
                     xintercept=xintercept-(xstep>>1);
                     xtile = (short) (xintercept >> TILESHIFT);
                     HitHorizWall();
                }
          } 
          else if (xtilestep == -1)                    // SE+NE
          {  
                //if((int32_t)((word)yintercept)+ystep<0x8000) //full tile
                if ((int32_t)((word)yintercept)+(ystep>>1)>0x8000) //half tile
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
          if (xtilestep == -1)                        // E (NE+SE)
          {         
                xintercept=(xtile<<TILESHIFT);
          }
          else if (xtilestep == 1)// && ytilestep == -1)  // SW+NW
          {  
                int32_t yintbuf=yintercept+(ystep>>1);
                if ((yintbuf>>16)!=(yintercept>>16))
                     return false;
                               
                if((word)yintbuf>TILEGLOBAL/2) return false; 

                yintercept = yintercept+(ystep>>1);      
                xintercept=(xtile<<TILESHIFT)+0x8000;                   
          }
                           
          ytile = (short) (yintercept >> TILESHIFT);
          HitVertWall();
     }  
     
     *((byte *)spotsaw+xspot)=1;
     return true;
} 


/*
=====================
=
= PartHorizWallSE
=
= draws horizontal side of quarter walls
= returns true to draw
= returns false to tell rays to go thru (goto passhoriz)
=      N 
=     ___
= NW |  _|  NE
= SW |_|_|  SE
=      S
=====================
*/

boolean PartHorizWallSE(int32_t xstep, int32_t ystep)
{                                   
     //
     // half of horizontal wall, clips into to draw vertical wall part
     // 
     if((word)xintercept<0x8000) 
     {   
          if (ytilestep == -1)                                      // S (SW+SE)
          {
                //if((int32_t)((word)xintercept)+xstep>0x8000) //full tile
                if ((int32_t)((word)xintercept)+(xstep>>1)<0x8000) //half tile
                      return false;

                xintercept=(xtile<<TILESHIFT)-0x8000;
                yintercept=yintercept-(ystep>>1);                  
                ytile = (short) (yintercept >> TILESHIFT);
                HitVertWall();
          }
          else if (xtilestep == 1 && ytilestep == 1)               // NW  - cuts meet here               
          {                                                                  
                int32_t xintbuf=xintercept+(xstep>>1);

                if((word)xintbuf<TILEGLOBAL/2)
                {  
                     //if ((int32_t)((word)xintercept)-xstep<0x8000) //full tile
                     if ((int32_t)((word)xintercept)+(xstep>>1)>0x8000) //half tile (right side)
                            return false;
                                        
                     if((int32_t)((word)xintercept)+xstep<0x8000) // step full tile from pos to opposite end to cut (left side)
                            return false;

                     xintercept=(xtile<<TILESHIFT)-0x8000;
                     yintercept=yintercept-(ystep>>1);
                     ytile = (short) (yintercept >> TILESHIFT);
                     HitVertWall();                           
                }
                else
                {
                     if ((xintbuf>>16)!=(xintercept>>16)) return false;
                               
                     //if ((word)xintbuf>TILEGLOBAL/2) goto passhoriz;  

                     xintercept = xintbuf;      
                     yintercept=(ytile<<TILESHIFT)+0x8000;
                     xtile = (short) (xintercept >> TILESHIFT);
                     HitHorizWall();
                }
          }
          else if (xtilestep == -1 && ytilestep == 1)               // NE                 
          {                    
                //if ((int32_t)((word)xintercept)+xstep>0x8000) //full tile
                if ((int32_t)((word)xintercept)+(xstep>>1)<0x8000) //half tile
                      return false;
          }
     }
                              
     //
     // rest side of horizontal quarter wall, cut it at half tile etc
     // 
     else
     {                                                                     
          if (ytilestep == 1)                                     // N            
          {     
                int32_t xintbuf=xintercept+(xstep>>1);
                if ((xintbuf>>16)!=(xintercept>>16))
                     return false;
                               
                if((word)xintbuf<TILEGLOBAL/2) return false; 

                xintercept = xintbuf;      
                yintercept=(ytile<<TILESHIFT)+0x8000;
          }
          else if (ytilestep == -1)                             // S
          {                      
                yintercept=(ytile<<TILESHIFT);
          }
                               
          xtile = (short) (xintercept >> TILESHIFT);
          HitHorizWall();
     }
     
     *((byte *)spotsaw+yspot)=1;
     return true;
}

/*
=====================
=
= PartVertWallSE
=
= draws vertical side of quarter walls
= returns true to draw
= returns false to tell rays to go thru (goto passvert)
=
=====================
*/

boolean PartVertWallSE(int32_t xstep, int32_t ystep)
{
     //   
     // CUTTING INTO VERTICAL TILE SIDE TO DRAW THE 1/2 HORIZONTAL WALL PART      
     //  
     if((word)yintercept<0x8000) 
     {                                              
          if (xtilestep == 1 && ytilestep == -1)             // SW
          {                
                // if ((int32_t)((word)yintercept)+ystep<0x8000) //full tile
                if ((int32_t)((word)yintercept)+(ystep>>1)<0x8000) //half tile
                    return false;
          }
          else if (xtilestep == 1 && ytilestep == 1)          // NW - meeting of the cut corners
          {                                           
                if ((int32_t)((word)yintercept)+(ystep>>1)>0x8000)
                {                                                                                                                  
                     int32_t yintbuf=yintercept+(ystep>>1);
                     if((yintbuf>>16)!=(yintercept>>16))
                         return false;
                               
                     //if ((word)yintbuf<TILEGLOBAL/2) goto passvert;  
                                          
                     //if((int32_t)((word)yintercept)+ystep>0x8000) // step from opposite side a full tile ahead to cut it
                     //  goto passvert;  

                     xintercept=(xtile<<TILESHIFT)+0x8000;
                     yintercept=yintercept+(ystep>>1);             
                     ytile = (short) (yintercept >> TILESHIFT);
                     HitVertWall();                                       
                }
                else 
                {
                     int32_t xintbuf=xintercept+(xstep>>1);

                     //if((xintbuf>>16)!=(xintercept>>16)) goto passvert;
                                        
                     //if((int32_t)((word)yintercept)+ystep<0x8000) //full tile
                     //if((int32_t)((word)yintercept)+(ystep>>1)<0x8000) //half tile
                     //  goto passvert;     
                     if ((int32_t)((word)yintercept)+ystep<0x8000) // step from opposite side a full tile ahead to cut it
                           return false;
                                           
                     yintercept=(ytile<<TILESHIFT)-0x8000;
                     xintercept=xintercept-(xstep>>1);
                     xtile = (short) (xintercept >> TILESHIFT);
                     HitHorizWall();
                }
          } 
          else if (xtilestep == -1)                    // SE+NE
          {
                //if((int32_t)((word)yintercept)+ystep<0x8000) //full tile
                if ((int32_t)((word)yintercept)+(ystep>>1)<0x8000) //half tile
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
          if (xtilestep == -1)                        // E (NE+SE)
          {         
                xintercept=(xtile<<TILESHIFT);
          }
          else if (xtilestep == 1 )                   // W (NW+SW)
          {
                int32_t yintbuf=yintercept+(ystep>>1);
                if ((yintbuf>>16)!=(yintercept>>16))
                     return false;

                if((word)yintbuf<TILEGLOBAL/2) return false;

                yintercept = yintercept+(ystep>>1);      
                xintercept=(xtile<<TILESHIFT)+0x8000;                   
          }
                           
          ytile = (short) (yintercept >> TILESHIFT);
          HitVertWall();
     }  
     
     *((byte *)spotsaw+xspot)=1;
     return true;
} 

/*
=====================
=
= PartHorizWallSW
=
= draws horizontal side of quarter walls
= returns true to draw
= returns false to tell rays to go thru (goto passhoriz)
=      N 
=     ___
= NW |_  |  NE
= SW |_|_|  SE
=      S
=====================
*/

boolean PartHorizWallSW(int32_t xstep, int32_t ystep)
{       
     // ytilestep = -1       south of wall
     // ytilestep = 1        north of wall
     // xtilestep = -1       east of wall       
     // xtilestep = 1        west of wall  
                            
     //
     // half of horizontal wall, clips into to draw vertical wall part
     // 
     if((word)xintercept>0x8000) 
     {   
          if (ytilestep == -1)                                      // S (SW+SE)
          {
                //if((int32_t)((word)xintercept)+xstep>0x8000) //full tile
                if ((int32_t)((word)xintercept)+(xstep>>1)>0x8000) //half tile
                      return false;

                xintercept=(xtile<<TILESHIFT)+0x8000;
                yintercept=yintercept-(ystep>>1);                  
                ytile = (short) (yintercept >> TILESHIFT);
                HitVertWall();
          }
          else if (xtilestep == -1 && ytilestep == 1)               // NE  - cuts meet here               
          {                                                                  
                int32_t xintbuf=xintercept+(xstep>>1);

                if((word)xintbuf>TILEGLOBAL/2)
                {  
                     //int32_t yintbuf=yintercept+(ystep>>1);
                                                                
                     //if ((yintbuf>>16)!=(yintercept>>16)) goto passhoriz;

                     //if ((int32_t)((word)xintercept)-xstep<0x8000) //full tile
                     if ((int32_t)((word)xintercept)+(xstep>>1)<0x8000) //half tile (right side)
                            return false;
                                        
                     if((int32_t)((word)xintercept)+xstep>0x8000) // step full tile from pos to opposite end to cut (left side)
                            return false;

                     xintercept=(xintercept&0xffff0000)-0x8000;
                     yintercept=yintercept-(ystep>>1);
                     ytile = (short) (yintercept >> TILESHIFT);
                     HitVertWall();                           
                }
                else
                {
                     if ((xintbuf>>16)!=(xintercept>>16)) return false;
                               
                     //if ((word)xintbuf>TILEGLOBAL/2) goto passhoriz;  

                     xintercept = xintbuf;      
                     yintercept=(ytile<<TILESHIFT)+0x8000;
                     xtile = (short) (xintercept >> TILESHIFT);
                     HitHorizWall();
                }
          }
          else if (xtilestep == 1 && ytilestep == 1)               // NW                 
          {                    
                //int32_t yintbuf=yintercept+(ystep>>1);
                                     
                //if ((yintbuf>>16)!=(yintercept>>16))   goto passhoriz;

                //if ((int32_t)((word)xintercept)+xstep>0x8000) //full tile
                if ((int32_t)((word)xintercept)+(xstep>>1)>0x8000) //half tile
                      return false;

                //xintercept=(xintercept&0xffff0000)+0x8000;
                //yintercept=yintercept-(ystep>>1);
                //ytile = (short) (yintercept >> TILESHIFT);
                //HitVertDoor();
          }
     }
                              
     //
     // rest side of horizontal quarter wall, cut it at half tile etc
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
                if ((xintbuf>>16)!=(xintercept>>16))// && lastside!=3)
                     return false;
                               
                if((word)xintbuf>TILEGLOBAL/2) return false; 

                xintercept = xintbuf;      
                yintercept=(ytile<<TILESHIFT)+0x8000;
          }
          else if (ytilestep == -1)
          {
                //if ((word)xintercept>TILEGLOBAL/2) goto passhoriz;                          
                yintercept=(ytile<<TILESHIFT);
          }
                               
          xtile = (short) (xintercept >> TILESHIFT);
          HitHorizWall();
     }
     
     *((byte *)spotsaw+yspot)=1;
     return true;
}

/*
=====================
=
= PartVertWallSW
=
= draws vertical side of quarter walls
= returns true to draw
= returns false to tell rays to go thru (goto passvert)
=
=====================
*/

boolean PartVertWallSW(int32_t xstep, int32_t ystep)
{
     //   
     // CUTTING INTO VERTICAL TILE SIDE TO DRAW THE 1/2 HORIZONTAL WALL PART      
     //  
     if((word)yintercept<0x8000) //if ((int32_t)((word)yintercept)+ystep<0x8000)
     {                                              
          if (xtilestep == -1 && ytilestep == -1)             // SE
          {
                //int32_t xintbuf=xintercept-(xstep>>1);
                // if ((xintbuf>>16)!=(xintercept>>16)) return false;
                
                // if ((int32_t)((word)yintercept)+ystep<0x8000) //full tile
                if ((int32_t)((word)yintercept)+(ystep>>1)<0x8000) //half tile
                    return false;
                    
                //yintercept=(yintercept&0xffff0000)+0x8000;
                //xintercept=xintercept-(xstep>>1);                    
                //xtile = (short) (xintercept >> TILESHIFT);
                //HitHorizDoor();
          }
          else if (xtilestep == -1  && ytilestep == 1)          // NE - meeting of the cut corners
          {                                           
                if ((int32_t)((word)yintercept)+(ystep>>1)>0x8000)
                {                                                                                                                  
                     int32_t yintbuf=yintercept+(ystep>>1);
                     if((yintbuf>>16)!=(yintercept>>16))
                         return false;
                               
                     //if ((word)yintbuf<TILEGLOBAL/2) goto passvert;  
                                          
                     //if((int32_t)((word)yintercept)+ystep>0x8000) // step from opposite side a full tile ahead to cut it
                     //  goto passvert;  

                     xintercept=(xtile<<TILESHIFT)-0x8000;
                     yintercept=yintercept+(ystep>>1);             
                     ytile = (short) (yintercept >> TILESHIFT);
                     HitVertWall();                                       
                }
                else 
                {
                     int32_t xintbuf=xintercept+(xstep>>1);

                     //if((xintbuf>>16)!=(xintercept>>16)) goto passvert;
                                        
                     //if((int32_t)((word)yintercept)+ystep<0x8000) //full tile
                     //if((int32_t)((word)yintercept)+(ystep>>1)<0x8000) //half tile
                     //  goto passvert;     
                     if ((int32_t)((word)yintercept)+ystep<0x8000) // step from opposite side a full tile ahead to cut it
                           return false;
                                           
                     yintercept=(yintercept&0xffff0000)+0x8000;
                     xintercept=xintercept-(xstep>>1);
                     xtile = (short) (xintercept >> TILESHIFT);
                     HitHorizWall();
                }
          } 
          else if (xtilestep == 1 )          // SW+NW
          {
                //if((int32_t)((word)yintercept)+ystep<0x8000) //full tile
                if ((int32_t)((word)yintercept)+(ystep>>1)<0x8000) //half tile
                       return false;

                yintercept=(ytile<<TILESHIFT)-0x8000; //(yintercept&0xffff0000)+0x8000;
                xintercept=xintercept-(xstep>>1);
                xtile = (short) (xintercept >> TILESHIFT);
                HitHorizWall();
          }
          /*else if (xtilestep == 1 && ytilestep == -1)          // SW
          {
                int32_t xintbuf=xintercept-(xstep>>1);
                int32_t xint=xintercept;

                //if((int32_t)((word)yintercept)+ystep<0x8000) //full tile
                if ((int32_t)((word)yintercept)+(ystep>>1)<0x8000) //half tile
                       return false;

                yintercept=(yintercept&0xffff0000)+0x8000;
                xintercept=xintercept-(xstep>>1);
                xtile = (short) (xintercept >> TILESHIFT);
                HitHorizWall();
          }
          else if (xtilestep == 1 && ytilestep == 1)            // NW
          {
                //if ((int32_t)((word)yintercept)+ystep<0x8000) //full tile
                if ((int32_t)((word)yintercept)+(ystep>>1)<0x8000) //half tile
                     return false;

                yintercept=(ytile<<TILESHIFT)-0x8000; //(yintercept&0xffff0000)+0x8000;
                xintercept=xintercept-(xstep>>1);
                                    
                xtile = (short) (xintercept >> TILESHIFT);
                HitHorizWall();
          }*/
     }
     
     //
     // THE NORMAL VERTICAL SIDE, part that isnt cut into
     //
     else
     {  
          if (xtilestep == 1)
          {
                //if((word)yintercept<0x8000) goto passvert;           
                xintercept=(xtile<<TILESHIFT);//-0x8000;
          }
          else if (xtilestep == -1  && ytilestep == 1)  // NE
          {
                int32_t yintbuf=yintercept+(ystep>>1);
                if ((yintbuf>>16)!=(yintercept>>16))
                     return false;
                               
                //if ((int32_t)((word)yintercept)+ystep > TILEGLOBAL/2) goto passvert;  

                yintercept = yintercept+(ystep>>1);      
                xintercept=(xtile<<TILESHIFT)-0x8000;                   
          }
          else   
          {
                int32_t yintbuf=yintercept+(ystep>>1);
                if ((yintbuf>>16)!=(yintercept>>16))// && lastside!=3)
                     return false;
                               
                if((word)yintbuf<TILEGLOBAL/2) return false; 

                yintercept = yintbuf;      
                xintercept=(xtile<<TILESHIFT)-0x8000; 
          }
                           
          //if ((word)yintercept>0x8000) goto passvert;  
          //xintercept=(xtile<<TILESHIFT);//-0x8000;
          ytile = (short) (yintercept >> TILESHIFT);
          HitVertWall();
     }  
     
     *((byte *)spotsaw+xspot)=1;
     return true;
}     
