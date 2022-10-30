// WL_CONF.CPP

#include "WL_DEF.H"
#include "wl_conf.h"

//===========================================================================

gameConfig  gameVars; 
 
int  titlemusic;
int  intermusic;
int  dwalls[100];
int  mirwall[256]; 
int32_t  partime[MAXLEVELS];
 
boolean thirdencounter = false;

boolean tflats = false;
byte    parallaxstart[MAXLEVELS];
byte    parallaxcount[MAXLEVELS];
byte    shadestrength[MAXLEVELS];
byte    shadingR[MAXLEVELS];
byte    shadingG[MAXLEVELS];
byte    shadingB[MAXLEVELS];

byte ceil_default;
byte floor_default;
byte cfg_ceiling[MAXLEVELS];
byte cfg_floor[MAXLEVELS];
byte secretlevel[MAXLEVELS];
byte backfromsecret[MAXLEVELS];
byte finalmap[MAXLEVELS];
byte musicpick[MAXLEVELS];
byte hudmapnum[MAXLEVELS]; 

// episode stuff
int  epstart[13];
int  epend[13];
int  epactive[13];
char epname[13][36];

char gamepalname[30];
char interpalname[30];
char macpalname[30];
char idpalname[30];
char titlepalname[30];
char menupalname[30];

SDL_Color newpal[256];
SDL_Color Interpal[256];
SDL_Color idpal[256];
SDL_Color macpal[256];
SDL_Color titlepal[256];
SDL_Color menupal[256];
byte nupal[768];
 
static const char cfgname[] = "gamedict.";
    
int   gamefile;

int   cfg_lineNumber; 
char  *cfg_line;
char  *cfg_buffer;
int   buffer_index;
int   buffer_total;

//===========================================================================

/*
=====================
=
= Load File into Buffer
=
=====================
*/

int LoadFileBuffer(void)
{
    buffer_index = 0;
    buffer_total = read(gamefile, cfg_buffer, MAX_BUF_LENGTH);
    
    return buffer_total;
}

/*
=====================
=
= Ignore Empty Space
=
=====================
*/

int IgnoreEmptySpace(void)
{
    while(gameVars.char_index < gameVars.char_count)
    {
        // if space is occupied                                                
        if(cfg_line[gameVars.char_index] != ' ' && cfg_line[gameVars.char_index] != '\t')
            break;

        // space is empty, continue
        gameVars.char_index++;
    }
    
    return gameVars.char_index;    // which spot
}

/*
=====================
=
= Read Lines
=
=====================
*/

int ReadLine(void)
{
    char chr;
    int result;

    gameVars.char_index = 0;
    gameVars.char_count = 0;

    do
    {
        while(buffer_index < buffer_total)
        {
            if((chr = cfg_buffer[buffer_index++]) == '\n')       // new line
            {
                cfg_line[gameVars.char_count] = '\0';  // eol
                cfg_lineNumber++;

                return gameVars.char_count;
            }

            if(gameVars.char_count >= MAX_LIN_LENGTH)          // line too long
                return -1; 

            cfg_line[gameVars.char_count++] = chr;
        }

        if((result = LoadFileBuffer()) < 0)
            return result;

        if(!result) // end of file
            break; 

    } while(1);

    cfg_lineNumber++;
    cfg_line[gameVars.char_count] = '\0';

    return gameVars.char_count;  // how many chaaaaaars
}

/*
=====================
=
= Read Text String
=
= \' 	single quote 	byte 0x27 in ASCII
= \" 	double quote 	byte 0x22 in ASCII
= \? 	question mark 	byte 0x3f in ASCII
= \\ 	backslash 	    byte 0x5c in ASCII
= \b 	backspace 	    byte 0x08 in ASCII
= \f 	new page 	    byte 0x0c in ASCII
= \n 	new line 	    byte 0x0a in ASCII
= \t 	tab 	        byte 0x09 in ASCII
=
=====================
*/

int ReadString(void)
{
    int index, EscapeSeq;
    char chr;

    if( !IgnoreEmptySpace() ) //sudden end of line
       return -1;

    if(cfg_line[gameVars.char_index] != '"') // no quotes for string allowed
       return -1;

    index = 0;
    
    do
    {   
        gameVars.char_index++;                // skip the quote

        EscapeSeq = 0;
        while(1)
        {
            if(gameVars.char_index == gameVars.char_count) //Unexpected end of line, expecting closing quote, on line
                return -1;
 
            chr = cfg_line[gameVars.char_index++];

            if(index >= MAX_STR_LENGTH-1 && !EscapeSeq) // string too long
                return -1;

            if(EscapeSeq)    // end escape sequence, index adds to the overall string amount
            {   
                EscapeSeq = 0;
                switch(chr)
                {
                    case '\\':
                        gameVars.cfg_string[index++] = '\\'; // \
                        break;         
                           
                    case 'n':
                        gameVars.cfg_string[index++] = '\n'; // n
                        break;
                        
                    case '"':
                        gameVars.cfg_string[index++] = '"'; // "
                        break;
                }  
            }
            else if(chr == '"')   // " string end
                break;
            else if(chr == '\\')  // \ begin escape sequence
            {   
                EscapeSeq = 1;
            }    

            else                  // actual character                
                gameVars.cfg_string[index++] = chr;
        }

        if(gameVars.char_index == gameVars.char_count)   // end of chars, end of value
            break; 
            
    } while(1);

    gameVars.cfg_string[index] = '\0'; // end of the string

    return index;
}

/*
=====================
=
= ReadNumbers
=
=====================
*/
 
int ReadNumber(void)
{
    char chr;

    if( !IgnoreEmptySpace() )                        // sudden end
        return -1;

    chr = cfg_line[gameVars.char_index];    // start the position at given spot (e.g. gameVars.char_index = 8)
    
    if(!(chr >= '0' && chr <= '9'))                  // not a number?
        return -1;

    gameVars.cfg_number = chr - '0';                 // get the number
    gameVars.char_index++;                           // move on

    while(gameVars.char_index < gameVars.char_count)  // more digits, havent reached end yet
    {
        chr = cfg_line[gameVars.char_index++];

        if(!(chr >= '0' && chr <= '9'))               // not a number, end it
            break;

        gameVars.cfg_number *= 10;
        gameVars.cfg_number += chr - '0';
    }

    return 0;
}

//===========================================================================

/*
=====================
=
= ReadValues
=
= load all the game vars
=
=====================
*/

int ReadValues(void)
{
    int value;
    int muschunk;
    char *cfg_string;

    if(strncmp(cfg_line, "episode", 7) == 0)		// episode string comparison
    {   
        gameVars.char_index = 7;                            // set the start pos for the input reader  
        ReadNumber();            
        value = gameVars.cfg_number;            
        ReadString();
        
        if (!ReadString())
           epactive[value] = 0;
        else 
           epactive[value] = 1;
          
        strcpy(epname[value], gameVars.cfg_string); 
    }
    else if(strncmp(cfg_line, "startepisode", 12) == 0)	// episode starts
    {   
        gameVars.char_index = 12; 
        ReadNumber();    
        value = gameVars.cfg_number;
        ReadNumber();
        
        if (!ReadNumber())
          epstart[value] = 1;
        else
          epstart[value] = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "finalmap", 8) == 0)	// episode end
    {   
        gameVars.char_index = 8; 
        ReadNumber();    
        value = gameVars.cfg_number;
        //ReadNumber();

        //epend[value] = gameVars.cfg_number;
        
        if (value)
           finalmap[value] = 1;
        else 
           finalmap[value] = 0;
    }
    else if(strncmp(cfg_line, "palette", 7) == 0)	 // palette names
    {   
        gameVars.char_index = 7;                  
        ReadNumber();            
        value = gameVars.cfg_number;   
        ReadString();  
        
        if (value == 1)
           strcpy(gamepalname, gameVars.cfg_string);
        else if (value == 2)
           strcpy(interpalname, gameVars.cfg_string);   
        else if (value == 3)
           strcpy(idpalname, gameVars.cfg_string);  
        else if (value == 4)
           strcpy(macpalname, gameVars.cfg_string);     
        else if (value == 5)
           strcpy(titlepalname, gameVars.cfg_string);  
        else if (value == 6)
           strcpy(menupalname, gameVars.cfg_string);           
    }
    else if(strncmp(cfg_line, "dwalls", 6) == 0)		// darker walls
    {   
        gameVars.char_index = 6;
        ReadNumber();
        value = gameVars.cfg_number;
        ReadNumber();

        dwalls[value] = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "mirwall", 7) == 0)		// flipped walls
    {   
        gameVars.char_index = 7;
        ReadNumber();
        value = gameVars.cfg_number;
        ReadNumber();

        mirwall[value] = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "ceilcolor", 9) == 0)	// ceil
    {   
        gameVars.char_index = 9;
        //if(ReadNumber() < 0) return -1;  
        ReadNumber();    
        value = gameVars.cfg_number;
        ReadNumber();
        //if(ReadNumber() < 0) return -1;

        cfg_ceiling[value] = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "ceil_default", 12) == 0)	// default ceil
    {   
        gameVars.char_index = 12; 
        ReadNumber();    

        ceil_default = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "floorcolor", 10) == 0)	// floor
    {   
        gameVars.char_index = 10;        // set the start point to get number from   
        ReadNumber();                    // read it     
        value = gameVars.cfg_number;     // floorcolor id (e.g. floorcolor12)
        ReadNumber();                    // game var value

        cfg_floor[value] = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "floor_default", 13) == 0)	// default floor
    {   
        gameVars.char_index = 13; 
        ReadNumber();    

        floor_default = gameVars.cfg_number;
    }   
    else if(strncmp(cfg_line, "areanum", 7) == 0)	// statusbar map numbers
    {   
        gameVars.char_index = 7;       
        ReadNumber();                    
        value = gameVars.cfg_number;    
        ReadNumber();           

        hudmapnum[value] = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "partime", 7) == 0)	// par time for the levels
    {   
        gameVars.char_index = 7;       
        ReadNumber();                    
        value = gameVars.cfg_number;    
        ReadNumber();           

        if (!ReadNumber()) 
           partime[value] = 0;
        else
           partime[value] = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "secretlevel", 11) == 0)	// secret level
    {   
        gameVars.char_index = 11;     
        ReadNumber();                    
        value = gameVars.cfg_number;    
        ReadNumber();                   

        secretlevel[value] = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "secretback", 10) == 0)	// sercet leading back to 
    {   
        gameVars.char_index = 10;        
        ReadNumber();                   
        value = gameVars.cfg_number;  
        ReadNumber();           

        backfromsecret[value] = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "3rdenc", 6) == 0)	// fixes/changes for 3rd encounter sets (locks door for bosses, shorter knife, fixed vacuum pwall)
    {   
        thirdencounter = true;
    }
    else if(strncmp(cfg_line, "texflats", 8) == 0)	// enable 3rd plane floor/ceiling textures - disables manual darkwall setting by 3rd plane
    {   
        tflats = true;
    }
    else if(strncmp(cfg_line, "shading", 7) == 0)	// shading
    {   
        gameVars.char_index = 7;        
        ReadNumber();                   
        value = gameVars.cfg_number;  
        ReadNumber();           

        shadestrength[value] = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "shadered", 8) == 0)	// shading red
    {   
        gameVars.char_index = 8;        
        ReadNumber();                   
        value = gameVars.cfg_number;  
        ReadNumber();           

        shadingR[value] = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "shadegreen", 10) == 0)	// shading green
    {   
        gameVars.char_index = 10;        
        ReadNumber();                   
        value = gameVars.cfg_number;  
        ReadNumber();           

        shadingG[value] = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "shadeblue", 9) == 0)	  // shading b
    {   
        gameVars.char_index = 9;        
        ReadNumber();                   
        value = gameVars.cfg_number;  
        ReadNumber();           

        shadingB[value] = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "parallaxstart", 13) == 0)	// parallax sky
    {   
        gameVars.char_index = 13;        
        ReadNumber();                   
        value = gameVars.cfg_number;  
        ReadNumber();           

        parallaxstart[value] = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "parallaxcount", 13) == 0)	// amount of parallax sky tex
    {   
        gameVars.char_index = 13;        
        ReadNumber();                   
        value = gameVars.cfg_number;  
        ReadNumber();           

        parallaxcount[value] = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "levelmusic", 10) == 0
     || strncmp(cfg_line, "titlemusic", 10) == 0
     || strncmp(cfg_line, "intermusic", 10) == 0)	// music per level
    {   
        gameVars.char_index = 10;                     
        ReadNumber();            
        value = gameVars.cfg_number;            
        ReadString();
          
        /*if(strncmp(gameVars.cfg_string, "TITLE", 5) == 0)  
           musicpick[value] = (musicnames) TITLE_MUS;
        else if(strncmp(gameVars.cfg_string, "ORIGINAL", 8) == 0)  
           musicpick[value] = (musicnames) DUNGEON_MUS;
        else if(strncmp(gameVars.cfg_string, "GRUNGE", 6) == 0)  
           musicpick[value] = (musicnames) WARMARCH_MUS;
        else if(strncmp(gameVars.cfg_string, "PLODDING", 8) == 0)  
           musicpick[value] = (musicnames) GETTHEM_MUS;
        else if(strncmp(gameVars.cfg_string, "ROCKED", 6) == 0)  
           musicpick[value] = (musicnames) HEADACHE_MUS;
        else if(strncmp(gameVars.cfg_string, "UNLEASHED", 9) == 0)  
           musicpick[value] = (musicnames) HITLWLTZ_MUS;
        else if(strncmp(gameVars.cfg_string, "BETWEEN", 7) == 0)  
           musicpick[value] = (musicnames) INTROCW3_MUS;
        else if(strncmp(gameVars.cfg_string, "DOOM", 4) == 0)  
           musicpick[value] = (musicnames) NAZI_NOR_MUS;    
           
        else if(strncmp(gameVars.cfg_string, "TRACK08", 7) == 0)  
           musicpick[value] = (musicnames) NAZI_OMI_MUS;   
        else if(strncmp(gameVars.cfg_string, "TRACK09", 7) == 0)  
           musicpick[value] = (musicnames) POW_MUS;   
        else if(strncmp(gameVars.cfg_string, "TRACK10", 7) == 0)  
           musicpick[value] = (musicnames) SALUTE_MUS;   
        else if(strncmp(gameVars.cfg_string, "TRACK11", 7) == 0)  
           musicpick[value] = (musicnames) SEARCHN_MUS;   
        else if(strncmp(gameVars.cfg_string, "TRACK12", 7) == 0)  
           musicpick[value] = (musicnames) SUSPENSE_MUS;   
        else if(strncmp(gameVars.cfg_string, "TRACK13", 7) == 0)  
           musicpick[value] = (musicnames) VICTORS_MUS;
        else if(strncmp(gameVars.cfg_string, "TRACK14", 7) == 0)  
           musicpick[value] = (musicnames) WONDERIN_MUS;   
        else if(strncmp(gameVars.cfg_string, "TRACK15", 7) == 0)  
           musicpick[value] = (musicnames) FUNKYOU_MUS; 
        else if(strncmp(gameVars.cfg_string, "TRACK16", 7) == 0)  
           musicpick[value] = (musicnames) ENDLEVEL_MUS;   */
           
        if(strncmp(gameVars.cfg_string, "TITLE", 5) == 0)  
           muschunk = (musicnames) TITLE_MUS;
        else if(strncmp(gameVars.cfg_string, "ORIGINAL", 8) == 0)  
           muschunk = (musicnames) DUNGEON_MUS;
        else if(strncmp(gameVars.cfg_string, "GRUNGE", 6) == 0)  
           muschunk = (musicnames) WARMARCH_MUS;
        else if(strncmp(gameVars.cfg_string, "PLODDING", 8) == 0)  
           muschunk = (musicnames) GETTHEM_MUS;
        else if(strncmp(gameVars.cfg_string, "ROCKED", 6) == 0)  
           muschunk = (musicnames) HEADACHE_MUS;
        else if(strncmp(gameVars.cfg_string, "UNLEASHED", 9) == 0)  
           muschunk = (musicnames) HITLWLTZ_MUS;
        else if(strncmp(gameVars.cfg_string, "BETWEEN", 7) == 0)  
           muschunk = (musicnames) INTROCW3_MUS;
        else if(strncmp(gameVars.cfg_string, "DOOM", 4) == 0)  
           muschunk = (musicnames) NAZI_NOR_MUS;      
        else if(strncmp(gameVars.cfg_string, "TRACK08", 7) == 0)  
           muschunk = (musicnames) NAZI_OMI_MUS;   
        else if(strncmp(gameVars.cfg_string, "TRACK09", 7) == 0)  
           muschunk = (musicnames) POW_MUS;   
        else if(strncmp(gameVars.cfg_string, "TRACK10", 7) == 0)  
           muschunk = (musicnames) SALUTE_MUS;   
        else if(strncmp(gameVars.cfg_string, "TRACK11", 7) == 0)  
           muschunk = (musicnames) SEARCHN_MUS;   
        else if(strncmp(gameVars.cfg_string, "TRACK12", 7) == 0)  
           muschunk = (musicnames) SUSPENSE_MUS;   
        else if(strncmp(gameVars.cfg_string, "TRACK13", 7) == 0)  
           muschunk = (musicnames) VICTORS_MUS;
        else if(strncmp(gameVars.cfg_string, "TRACK14", 7) == 0)  
           muschunk = (musicnames) WONDERIN_MUS;   
        else if(strncmp(gameVars.cfg_string, "TRACK15", 7) == 0)  
           muschunk = (musicnames) FUNKYOU_MUS; 
        else if(strncmp(gameVars.cfg_string, "TRACK16", 7) == 0)  
           muschunk = (musicnames) ENDLEVEL_MUS;   
          
        if(strncmp(cfg_line, "levelmusic", 10) == 0)  
           musicpick[value] = muschunk;   
        else if(strncmp(cfg_line, "titlemusic", 10) == 0)
           titlemusic = muschunk; 
        else if(strncmp(cfg_line, "intermusic", 10) == 0)   
           intermusic = muschunk;                                                           
    }
    
    else if(strncmp(cfg_line, "modcfg", 6) == 0)	 // modconfig
    {   
        gameVars.char_index = 6;                           
        ReadString();  
        
        strcpy(modname, gameVars.cfg_string);        
    }
    
    else //  stuff is missing
    {
        return -1; // set it bad
    }
    
    if(gameVars.char_count >= 2 && cfg_line[0] == '/' && cfg_line[1] == '/') // comment
        return -1; 

    return value;
}


/*
=====================
=
= LoadGameConfig
=
= loads the file and all
=
=====================
*/

void LoadGameConfig (void)
{
  char filename[30];
  char filepath[300];
 
  strcpy(filename,cfgname);
  strcat(filename,extension);
  
  if(filedir[0])
      snprintf(filepath, sizeof(filepath), "%s%s", filedir, filename);
  else
      strcpy(filepath, filename);
    
  gamefile = open(filepath, O_RDONLY | O_TEXT);
    
  if (gamefile == -1)  
      Quit("Missing game file");
       
  modname[0] = 0;    

  if((cfg_buffer = (char *) malloc(MAX_BUF_LENGTH)) > 0)
  {
      if((cfg_line = (char *) malloc(MAX_LIN_LENGTH)) > 0)
      {
            if((gameVars.cfg_string = (char *) malloc(MAX_STR_LENGTH)) > 0)
            {
                  if(!LoadFileBuffer())
                     Quit("RIP config"); // empty broken cfg

                  while(1)               // get values till end of file or till it breaks
                  {
                       if(gameVars.char_count != 0)
                            ReadValues (); 

                       if(buffer_total == 0)
                          break;
            
                       if (ReadLine() < 0) 
                          break; 
                  }

                  free(gameVars.cfg_string);
                  gameVars.cfg_string = 0;
            }                 

            free(cfg_line);
            cfg_line = 0;
      }

      free(cfg_buffer);
      cfg_buffer = 0;
  }

  close(gamefile);
  gamefile = 0;

   //
   // set up some stuff after the game loads

   //
   // LOAD NEW GAMEPAL
   //
   #define RGBX(r, g, b) {(r), (g), (b), 0}
   
   if(filedir[0])
       snprintf(filepath, sizeof(filepath), "%s%s", filedir, gamepalname);
   else
       strcpy(filepath, gamepalname);
     
   int file = open(filepath, O_RDONLY | O_BINARY);
   if (file != -1)                          // if no file, dont do anything; default pal
   {
      //Quit("Missing pal file");

      //read(file,&nupal,sizeof(SDL_Color) * 256);
      read(file,nupal,sizeof(nupal)); 

      SDL_Color newpal[]=
      {  
         #include "newpal.inc"  
      };
 
      VL_SetPalette(newpal, true); 
      memcpy(gamepal, newpal, sizeof(SDL_Color) * 256); 
      
      //VL_ConvertPalette(grsegs[palette], newpal, 256);
      //SDL_SetColors(screenBuffer, gamepal, 0, 256);
      //SDL_SetColors(screen, newpal, 0, 256);
      //memcpy(curpal, newpal, sizeof(SDL_Color) * 256);
      //SDL_SetColors(screenBuffer, newpal, 0, 256);
   }
   close(file);
   
   // INTERMISSION PAL
   //
   if(filedir[0])
       snprintf(filepath, sizeof(filepath), "%s%s", filedir, interpalname);
   else
       strcpy(filepath, interpalname);
     
   file = open(filepath, O_RDONLY | O_BINARY);
   
   if (file == -1)                                                // if no palette, check main folder
       file = open(interpalname, O_RDONLY | O_BINARY);
   
   if (file != -1)                         
   {
      read(file,nupal,sizeof(nupal)); 

      SDL_Color newpal[]=
      {  
         #include "newpal.inc"  
      };
 
      memcpy(Interpal, newpal, sizeof(SDL_Color) * 256); 
   }   
   
   close(file);
   
   // ID PAL
   //
   if(filedir[0])
       snprintf(filepath, sizeof(filepath), "%s%s", filedir, idpalname);
   else
       strcpy(filepath, idpalname);
     
   file = open(filepath, O_RDONLY | O_BINARY);
   
   if (file == -1)                                                // if no palette, check main folder
       file = open(idpalname, O_RDONLY | O_BINARY);
       
   if (file != -1)                         
   {
      read(file,nupal,sizeof(nupal)); 

      SDL_Color newpal[]=
      {  
         #include "newpal.inc"  
      };
 
      memcpy(idpal, newpal, sizeof(SDL_Color) * 256); 
   }   
   close(file);
   
   // MAC PAL
   //
   if(filedir[0])
       snprintf(filepath, sizeof(filepath), "%s%s", filedir, macpalname);
   else
       strcpy(filepath, macpalname);
     
   file = open(filepath, O_RDONLY | O_BINARY);
   
   if (file == -1)                                                // if no palette, check main folder
       file = open(macpalname, O_RDONLY | O_BINARY);
   
   if (file != -1)                         
   {
      read(file,nupal,sizeof(nupal)); 

      SDL_Color newpal[]=
      {  
         #include "newpal.inc"  
      };
 
      memcpy(macpal, newpal, sizeof(SDL_Color) * 256); 
   }  
   close(file);
    
   // TITLE PAL
   //
   if(filedir[0])
       snprintf(filepath, sizeof(filepath), "%s%s", filedir, titlepalname);
   else
       strcpy(filepath, titlepalname);
     
   file = open(filepath, O_RDONLY | O_BINARY);
   
   if (file == -1)                                                // if no palette, check main folder
       file = open(titlepalname, O_RDONLY | O_BINARY);
   
   if (file != -1)                         
   {
      read(file,nupal,sizeof(nupal)); 

      SDL_Color newpal[]=
      {  
         #include "newpal.inc"  
      };
 
      memcpy(titlepal, newpal, sizeof(SDL_Color) * 256); 
   }   
   close(file);
   
   // MENU PAL
   //
   if(filedir[0])
       snprintf(filepath, sizeof(filepath), "%s%s", filedir, menupalname);
   else
       strcpy(filepath, menupalname);
     
   file = open(filepath, O_RDONLY | O_BINARY);
   
   if (file == -1)                                                // if no palette, check main folder
       file = open(menupalname, O_RDONLY | O_BINARY);
   
   if (file != -1)                         
   {
      read(file,nupal,sizeof(nupal)); 

      SDL_Color newpal[]=
      {  
         #include "newpal.inc"  
      };
 
      memcpy(menupal, newpal, sizeof(SDL_Color) * 256); 
   }   
   close(file);
}
