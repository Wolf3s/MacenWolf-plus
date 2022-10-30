//
// Nexy Super Sexy Modding Madness
//
//

#include "WL_DEF.H"
#include "wl_conf.h"
  
//===========================================================================

modfile   mod;

boolean   modded = false;
boolean   vswap64 = false;

static const char modcfg[] = "mod";

byte darkt[256];
char darktablename[30];
char modname[40];

char authnote[300];

int texturesize;
int textureshift;
int texmask;
int texfromfixedshift;
int spritescale;

#define STR_IGAMMO      "ammo"
#define STR_IGROCKETS   "rockets"
#define STR_IGGAS       "gas"
#define STR_IGHEALTH    "health" 
   
//===========================================================================
 
// do: make a parser combining words and items
// make it uh more automatic or so

int ReadModStuff(void)
{
    int value;
    char *cfg_string;
    
    if(strncmp(cfg_line, "ammo_start", 10) == 0)
    {   
        gameVars.char_index = 10;
        ReadNumber();
 
        mod.startammo = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "ammo_max", 8) == 0)
    {   
        gameVars.char_index = 8;
        ReadNumber();
 
        mod.maxammo = gameVars.cfg_number;
        mod.oldmaxammo = mod.maxammo;
    }  
    else if(strncmp(cfg_line, "gas_start", 9) == 0)
    {   
        gameVars.char_index = 9;
        ReadNumber();
 
        mod.startgas = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "rocket_start", 12) == 0)
    {   
        gameVars.char_index = 12;
        ReadNumber();
 
        mod.startrockets = gameVars.cfg_number;
    }
         
    else if(strncmp(cfg_line, "rocket_max", 10) == 0)
    {   
        gameVars.char_index = 10;
        ReadNumber();
 
        mod.maxrockets = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "gas_max", 7) == 0)
    {   
        gameVars.char_index = 7;
        ReadNumber();
 
        mod.maxgas = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "ammo_bag_max", 12) == 0)
    {   
        gameVars.char_index = 12;
        ReadNumber();
 
        mod.maxbagammo = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "ammo_bag", 8) == 0)
    {   
        gameVars.char_index = 8;
        ReadNumber();
 
        mod.bagammo = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "health_start", 12) == 0)
    {   
        gameVars.char_index = 12;
        ReadNumber();
 
        mod.starthealth = gameVars.cfg_number;
    }   
    else if(strncmp(cfg_line, "health_max", 10) == 0)
    {   
        gameVars.char_index = 10;
        ReadNumber();
 
        mod.maxhealth = gameVars.cfg_number;
    } 
    else if(strncmp(cfg_line, "lives_start", 11) == 0)
    {   
        gameVars.char_index = 11;
        ReadNumber();
 
        mod.startlives = gameVars.cfg_number;
    }
    //--------------------------------------------------------------
    //
    // weapon stuff
    //
    else if(strncmp(cfg_line, "weapon_damage_wp", 16) == 0)	
    {   
        gameVars.char_index = 16;     
        ReadNumber();                  
        value = gameVars.cfg_number;     
        ReadNumber();             

        mod.wp_damage[value] = gameVars.cfg_number;
    }  
    else if(strncmp(cfg_line, "weapon_speed_wp", 15) == 0)	
    {   
        gameVars.char_index = 15;     
        ReadNumber();                  
        value = gameVars.cfg_number;     
        ReadNumber();             

        mod.wp_speed[value] = gameVars.cfg_number;
    } 
    else if(strncmp(cfg_line, "weapon_got_wp", 13) == 0)
    {   
        gameVars.char_index = 13;     
        ReadNumber();                  
        value = gameVars.cfg_number;     
        ReadString();      

        if(strncmp(gameVars.cfg_string, "yes", 3) == 0)  
           mod.wp_start[value] = 2; 
        else if(strncmp(gameVars.cfg_string, "no", 2) == 0) 
           mod.wp_start[value] = 1;   
    } 
    else if(strncmp(cfg_line, "weapon_firerate_wp", 18) == 0)	
    {   
        gameVars.char_index = 18;     
        ReadNumber();                  
        value = gameVars.cfg_number;     
        ReadNumber();             

        mod.wp_firerate[value] = gameVars.cfg_number;
    }  
    else if(strncmp(cfg_line, "weapon_maxdist_wp", 17) == 0)	
    {   
        gameVars.char_index = 17;     
        ReadNumber();                  
        value = gameVars.cfg_number;     
        ReadNumber();             

        mod.wp_dist[value] = gameVars.cfg_number;
    }  
    else if(strncmp(cfg_line, "weapon_knockback_wp", 19) == 0)	
    {   
        gameVars.char_index = 19;     
        ReadNumber();                  
        value = gameVars.cfg_number;     
        ReadNumber();             

        mod.wp_knockback[value] = gameVars.cfg_number;
    }
    else if(strncmp(cfg_line, "weapon_ammorate_wp", 18) == 0)	
    {   
        gameVars.char_index = 18;     
        ReadNumber();                  
        value = gameVars.cfg_number;     
        ReadNumber();             

        mod.wp_ammousage[value] = gameVars.cfg_number;
    } 
    else if(strncmp(cfg_line, "weapon_pamount_wp", 17) == 0)	
    {   
        gameVars.char_index = 17;     
        ReadNumber();                  
        value = gameVars.cfg_number;     
        ReadNumber();             

        mod.wp_prjamount[value] = gameVars.cfg_number;
    } 
    else if(strncmp(cfg_line, "weapon_pangle_wp", 16) == 0)	
    {   
        gameVars.char_index = 16;     
        ReadNumber();                  
        value = gameVars.cfg_number;     
        ReadNumber();             

        mod.wp_prjangle[value] = gameVars.cfg_number;
    } 
    else if(strncmp(cfg_line, "weapon_silent_wp", 16) == 0)	
    {   
        gameVars.char_index = 16;     
        ReadNumber();                          
        value = gameVars.cfg_number;    
        
        mod.wp_silenced[value] = true;
    } 
    else if(strncmp(cfg_line, "weapon_type_wp", 14) == 0)	
    {   
        gameVars.char_index = 14;     
        ReadNumber();                  
        value = gameVars.cfg_number;     
        ReadString();      

        if(strncmp(gameVars.cfg_string, "melee",5) == 0)  
           mod.wp_type[value] = w_melee; 
        else if(strncmp(gameVars.cfg_string, "hitscan", 7) == 0) 
           mod.wp_type[value] = w_bullets;  
        else if(strncmp(gameVars.cfg_string, "rocket", 6) == 0) 
           mod.wp_type[value] = w_rocket;        
        else if(strncmp(gameVars.cfg_string, "fire", 4) == 0) 
           mod.wp_type[value] = w_fire;    
        else if(strncmp(gameVars.cfg_string, "needle", 6) == 0) 
           mod.wp_type[value] = w_needle;   
        else if(strncmp(gameVars.cfg_string, "fakefire", 8) == 0) 
           mod.wp_type[value] = w_fakefire;                 
    }  
    //--------------------------------------------------------------
    //
    // cross
    //
    else if(strncmp(cfg_line, "cross_damage", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_cross][item_damage] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "cross_health", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_cross][item_health] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "cross_ammo", 10) == 0)
    {   
        gameVars.char_index = 10;     
        ReadNumber();      

        mod.item[bo_cross][item_ammo] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "cross_gas", 9) == 0)
    {   
        gameVars.char_index = 9;     
        ReadNumber();      

        mod.item[bo_cross][item_gas] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "cross_rockets", 13) == 0)
    {   
        gameVars.char_index = 13;     
        ReadNumber();      

        mod.item[bo_cross][item_rockets] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "cross_time", 10) == 0)
    {   
        gameVars.char_index = 10;     
        ReadNumber();      

        mod.item[bo_cross][item_time] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "cross_ignore", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadString();   
           
        if(strncmp(gameVars.cfg_string, STR_IGAMMO, 4) == 0)
           mod.item_ignore[bo_cross] = item_ammo; 
        else if(strncmp(gameVars.cfg_string, STR_IGROCKETS, 7) == 0)              
           mod.item_ignore[bo_cross] = item_rockets; 
        else if(strncmp(gameVars.cfg_string, STR_IGGAS, 3) == 0)              
           mod.item_ignore[bo_cross] = item_gas;    
        else if(strncmp(gameVars.cfg_string, STR_IGHEALTH, 6) == 0)              
           mod.item_ignore[bo_cross] = item_health;     
    }

    //
    // CHALICE
    //
    else if(strncmp(cfg_line, "chalice_damage", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadNumber();      

        mod.item[bo_chalice][item_damage] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "chalice_health", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadNumber();      

        mod.item[bo_chalice][item_health] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "chalice_ammo", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_chalice][item_ammo] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "chalice_gas", 11) == 0)
    {   
        gameVars.char_index = 11;     
        ReadNumber();      

        mod.item[bo_chalice][item_gas] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "chalice_rockets", 15) == 0)
    {   
        gameVars.char_index = 15;     
        ReadNumber();      

        mod.item[bo_chalice][item_rockets] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "chalice_time", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_chalice][item_time] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "chalice_ignore", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadString();   
                      
        if(strncmp(gameVars.cfg_string, STR_IGAMMO, 4) == 0)
           mod.item_ignore[bo_cross] = item_ammo; 
        else if(strncmp(gameVars.cfg_string, STR_IGROCKETS, 7) == 0)              
           mod.item_ignore[bo_cross] = item_rockets; 
        else if(strncmp(gameVars.cfg_string, STR_IGGAS, 3) == 0)              
           mod.item_ignore[bo_cross] = item_gas;     
        else if(strncmp(gameVars.cfg_string, STR_IGHEALTH, 6) == 0)              
           mod.item_ignore[bo_chalice] = item_health;     
    }
    
    //
    // CROWN
    //
    else if(strncmp(cfg_line, "crown_damage", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_crown][item_damage] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "crown_health", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_crown][item_health] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "crown_ammo", 10) == 0)
    {   
        gameVars.char_index = 10;     
        ReadNumber();      

        mod.item[bo_crown][item_ammo] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "crown_gas", 9) == 0)
    {   
        gameVars.char_index = 9;     
        ReadNumber();      

        mod.item[bo_crown][item_gas] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "crown_rockets", 13) == 0)
    {   
        gameVars.char_index = 13;     
        ReadNumber();      

        mod.item[bo_crown][item_rockets] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "crown_time", 10) == 0)
    {   
        gameVars.char_index = 10;     
        ReadNumber();      

        mod.item[bo_crown][item_time] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "crown_ignore", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadString();   
                      
        if(strncmp(gameVars.cfg_string, STR_IGAMMO, 4) == 0)
           mod.item_ignore[bo_crown] = item_ammo; 
        else if(strncmp(gameVars.cfg_string, STR_IGROCKETS, 7) == 0)              
           mod.item_ignore[bo_crown] = item_rockets; 
        else if(strncmp(gameVars.cfg_string, STR_IGGAS, 3) == 0)              
           mod.item_ignore[bo_crown] = item_gas;     
        else if(strncmp(gameVars.cfg_string, STR_IGHEALTH, 6) == 0)              
           mod.item_ignore[bo_crown] = item_health;     
    }
     
    //
    // CHEST
    //
    else if(strncmp(cfg_line, "chest_damage", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_bible][item_damage] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "chest_health", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_bible][item_health] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "chest_ammo", 10) == 0)
    {   
        gameVars.char_index = 10;     
        ReadNumber();      

        mod.item[bo_bible][item_ammo] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "chest_gas", 9) == 0)
    {   
        gameVars.char_index = 9;     
        ReadNumber();      

        mod.item[bo_bible][item_gas] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "chest_rockets", 13) == 0)
    {   
        gameVars.char_index = 13;     
        ReadNumber();      

        mod.item[bo_bible][item_rockets] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "chest_time", 10) == 0)
    {   
        gameVars.char_index = 10;     
        ReadNumber();      

        mod.item[bo_bible][item_time] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "chest_ignore", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadString();   
                      
        if(strncmp(gameVars.cfg_string, STR_IGAMMO, 4) == 0)
           mod.item_ignore[bo_bible] = item_ammo; 
        else if(strncmp(gameVars.cfg_string, STR_IGROCKETS, 7) == 0)              
           mod.item_ignore[bo_bible] = item_rockets; 
        else if(strncmp(gameVars.cfg_string, STR_IGGAS, 3) == 0)              
           mod.item_ignore[bo_bible] = item_gas;     
        else if(strncmp(gameVars.cfg_string, STR_IGHEALTH, 6) == 0)              
           mod.item_ignore[bo_bible] = item_health;     
    }

    //
    // AMMO CLIP
    //
    else if(strncmp(cfg_line, "ammoclip_damage", 15) == 0)
    {   
        gameVars.char_index = 15;     
        ReadNumber();      

        mod.item[bo_clip][item_damage] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "ammoclip_health", 15) == 0)
    {   
        gameVars.char_index = 15;     
        ReadNumber();      

        mod.item[bo_clip][item_health] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "ammoclip_ammo", 13) == 0)
    {   
        gameVars.char_index = 13;     
        ReadNumber();      

        mod.item[bo_clip][item_ammo] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "ammoclip_gas", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_clip][item_gas] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "ammoclip_rockets", 16) == 0)
    {   
        gameVars.char_index = 16;     
        ReadNumber();      

        mod.item[bo_clip][item_rockets] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "ammoclip_time", 13) == 0)
    {   
        gameVars.char_index = 13;     
        ReadNumber();      

        mod.item[bo_clip][item_time] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "ammoclip_ignore", 15) == 0)
    {   
        gameVars.char_index = 15;     
        ReadString();   
                      
        if(strncmp(gameVars.cfg_string, STR_IGAMMO, 4) == 0)
           mod.item_ignore[bo_clip] = item_ammo; 
        else if(strncmp(gameVars.cfg_string, STR_IGROCKETS, 7) == 0)              
           mod.item_ignore[bo_clip] = item_rockets; 
        else if(strncmp(gameVars.cfg_string, STR_IGGAS, 3) == 0)              
           mod.item_ignore[bo_clip] = item_gas;     
        else if(strncmp(gameVars.cfg_string, STR_IGHEALTH, 6) == 0)              
           mod.item_ignore[bo_clip] = item_health;     
    }

    //
    // AMMO CLIP 2
    //
    else if(strncmp(cfg_line, "ammoclip2_damage", 16) == 0)
    {   
        gameVars.char_index = 16;     
        ReadNumber();      

        mod.item[bo_clip2][item_damage] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "ammoclip2_health", 16) == 0)
    {   
        gameVars.char_index = 16;     
        ReadNumber();      

        mod.item[bo_clip2][item_health] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "ammoclip2_ammo", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadNumber();      

        mod.item[bo_clip2][item_ammo] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "ammoclip2_gas", 13) == 0)
    {   
        gameVars.char_index = 13;     
        ReadNumber();      

        mod.item[bo_clip2][item_gas] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "ammoclip2_rockets", 17) == 0)
    {   
        gameVars.char_index = 17;     
        ReadNumber();      

        mod.item[bo_clip2][item_rockets] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "ammoclip2_time", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadNumber();      

        mod.item[bo_clip2][item_time] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "ammoclip2_ignore", 16) == 0)
    {   
        gameVars.char_index = 16;     
        ReadString();   
                      
        if(strncmp(gameVars.cfg_string, STR_IGAMMO, 4) == 0)
           mod.item_ignore[bo_clip2] = item_ammo; 
        else if(strncmp(gameVars.cfg_string, STR_IGROCKETS, 7) == 0)              
           mod.item_ignore[bo_clip2] = item_rockets; 
        else if(strncmp(gameVars.cfg_string, STR_IGGAS, 3) == 0)              
           mod.item_ignore[bo_clip2] = item_gas;     
        else if(strncmp(gameVars.cfg_string, STR_IGHEALTH, 6) == 0)              
           mod.item_ignore[bo_clip2] = item_health;     
    }

    //
    // AMMO BOX
    //
    else if(strncmp(cfg_line, "ammobox_damage", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadNumber();      

        mod.item[bo_25clip][item_damage] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "ammobox_health", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadNumber();      

        mod.item[bo_25clip][item_health] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "ammobox_ammo", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_25clip][item_ammo] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "ammobox_gas", 11) == 0)
    {   
        gameVars.char_index = 11;     
        ReadNumber();      

        mod.item[bo_25clip][item_gas] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "ammobox_rockets", 15) == 0)
    {   
        gameVars.char_index = 15;     
        ReadNumber();      

        mod.item[bo_25clip][item_rockets] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "ammobox_time", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_25clip][item_time] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "ammobox_ignore", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadString();   
                      
        if(strncmp(gameVars.cfg_string, STR_IGAMMO, 4) == 0)
           mod.item_ignore[bo_25clip] = item_ammo; 
        else if(strncmp(gameVars.cfg_string, STR_IGROCKETS, 7) == 0)              
           mod.item_ignore[bo_25clip] = item_rockets; 
        else if(strncmp(gameVars.cfg_string, STR_IGGAS, 3) == 0)              
           mod.item_ignore[bo_25clip] = item_gas;      
        else if(strncmp(gameVars.cfg_string, STR_IGHEALTH, 6) == 0)              
           mod.item_ignore[bo_25clip] = item_health;     
    }

    //
    // DOG FOOD
    //
    else if(strncmp(cfg_line, "dogfood_damage", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadNumber();      

        mod.item[bo_alpo][item_damage] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "dogfood_health", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadNumber();      

        mod.item[bo_alpo][item_health] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "dogfood_ammo", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_alpo][item_ammo] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "dogfood_gas", 11) == 0)
    {   
        gameVars.char_index = 11;     
        ReadNumber();      

        mod.item[bo_alpo][item_gas] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "dogfood_rockets", 15) == 0)
    {   
        gameVars.char_index = 15;     
        ReadNumber();      

        mod.item[bo_alpo][item_rockets] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "dogfood_time", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_alpo][item_time] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "dogfood_ignore", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadString();   
                      
        if(strncmp(gameVars.cfg_string, STR_IGAMMO, 4) == 0)
           mod.item_ignore[bo_alpo] = item_ammo; 
        else if(strncmp(gameVars.cfg_string, STR_IGROCKETS, 7) == 0)              
           mod.item_ignore[bo_alpo] = item_rockets; 
        else if(strncmp(gameVars.cfg_string, STR_IGGAS, 3) == 0)              
           mod.item_ignore[bo_alpo] = item_gas;     
        else if(strncmp(gameVars.cfg_string, STR_IGHEALTH, 6) == 0)              
           mod.item_ignore[bo_alpo] = item_health;     
    }
    
    //
    // FOOD
    //
    else if(strncmp(cfg_line, "food_damage", 11) == 0)
    {   
        gameVars.char_index = 11;     
        ReadNumber();      

        mod.item[bo_food][item_damage] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "food_health", 11) == 0)
    {   
        gameVars.char_index = 11;     
        ReadNumber();      

        mod.item[bo_food][item_health] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "food_ammo", 9) == 0)
    {   
        gameVars.char_index = 9;     
        ReadNumber();      

        mod.item[bo_food][item_ammo] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "food_gas", 8) == 0)
    {   
        gameVars.char_index = 8;     
        ReadNumber();      

        mod.item[bo_food][item_gas] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "food_rockets", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_food][item_rockets] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "food_time", 9) == 0)
    {   
        gameVars.char_index = 9;     
        ReadNumber();      

        mod.item[bo_food][item_rockets] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "food_ignore", 11) == 0)
    {   
        gameVars.char_index = 11;     
        ReadString();   
                      
        if(strncmp(gameVars.cfg_string, STR_IGAMMO, 4) == 0)
           mod.item_ignore[bo_food] = item_ammo;
        else if(strncmp(gameVars.cfg_string, STR_IGROCKETS, 7) == 0)              
           mod.item_ignore[bo_food] = item_rockets; 
        else if(strncmp(gameVars.cfg_string, STR_IGGAS, 3) == 0)              
           mod.item_ignore[bo_food] = item_gas;      
        else if(strncmp(gameVars.cfg_string, STR_IGHEALTH, 6) == 0)              
           mod.item_ignore[bo_food] = item_health;     
    }
    
    //
    // MEDIKIT
    //
    else if(strncmp(cfg_line, "medikit_damage", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadNumber();      

        mod.item[bo_firstaid][item_damage] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "medikit_health", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadNumber();      

        mod.item[bo_firstaid][item_health] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "medikit_ammo", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_firstaid][item_ammo] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "medikit_gas", 11) == 0)
    {   
        gameVars.char_index = 11;     
        ReadNumber();      

        mod.item[bo_firstaid][item_gas] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "medikit_rockets", 15) == 0)
    {   
        gameVars.char_index = 15;     
        ReadNumber();      

        mod.item[bo_firstaid][item_rockets] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "medikit_time", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_firstaid][item_time] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "medikit_ignore", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadString();   
                      
        if(strncmp(gameVars.cfg_string, STR_IGAMMO, 4) == 0)
           mod.item_ignore[bo_firstaid] = item_ammo; 
        else if(strncmp(gameVars.cfg_string, STR_IGROCKETS, 7) == 0)              
           mod.item_ignore[bo_firstaid] = item_rockets; 
        else if(strncmp(gameVars.cfg_string, STR_IGGAS, 3) == 0)              
           mod.item_ignore[bo_firstaid] = item_gas;    
        else if(strncmp(gameVars.cfg_string, STR_IGHEALTH, 6) == 0)              
           mod.item_ignore[bo_firstaid] = item_health;     
    }
    
    //
    // LIFE
    //
    else if(strncmp(cfg_line, "life_damage", 11) == 0)
    {   
        gameVars.char_index = 11;     
        ReadNumber();      

        mod.item[bo_fullheal][item_damage] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "life_health", 11) == 0)
    {   
        gameVars.char_index = 11;     
        ReadNumber();      

        mod.item[bo_fullheal][item_health] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "life_ammo", 9) == 0)
    {   
        gameVars.char_index = 9;     
        ReadNumber();      

        mod.item[bo_fullheal][item_ammo] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "life_gas", 8) == 0)
    {   
        gameVars.char_index = 8;     
        ReadNumber();      

        mod.item[bo_fullheal][item_gas] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "life_rockets", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_fullheal][item_rockets] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "life_time", 9) == 0)
    {   
        gameVars.char_index = 9;     
        ReadNumber();      

        mod.item[bo_fullheal][item_time] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "life_ignore", 11) == 0)
    {   
        gameVars.char_index = 11;     
        ReadString();   
                      
        if(strncmp(gameVars.cfg_string, STR_IGAMMO, 4) == 0)
           mod.item_ignore[bo_fullheal] = item_ammo; 
        else if(strncmp(gameVars.cfg_string, STR_IGROCKETS, 7) == 0)              
           mod.item_ignore[bo_fullheal] = item_rockets; 
        else if(strncmp(gameVars.cfg_string, STR_IGGAS, 3) == 0)              
           mod.item_ignore[bo_fullheal] = item_gas;    
        else if(strncmp(gameVars.cfg_string, STR_IGHEALTH, 6) == 0)              
           mod.item_ignore[bo_fullheal] = item_health;     
    }
    //
    // GASCAN
    //
    else if(strncmp(cfg_line, "gascan_damage", 13) == 0)
    {   
        gameVars.char_index = 13;     
        ReadNumber();      

        mod.item[bo_gascan][item_damage] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "gascan_health", 13) == 0)
    {   
        gameVars.char_index = 13;     
        ReadNumber();      

        mod.item[bo_gascan][item_health] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "gascan_ammo", 11) == 0)
    {   
        gameVars.char_index = 11;     
        ReadNumber();      

        mod.item[bo_gascan][item_ammo] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "gascan_gas", 10) == 0)
    {   
        gameVars.char_index = 10;     
        ReadNumber();      

        mod.item[bo_gascan][item_gas] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "gascan_rockets", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadNumber();      

        mod.item[bo_gascan][item_rockets] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "gascan_time", 11) == 0)
    {   
        gameVars.char_index = 11;     
        ReadNumber();      

        mod.item[bo_gascan][item_time] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "gascan_ignore", 13) == 0)
    {   
        gameVars.char_index = 13;     
        ReadString();   
                      
        if(strncmp(gameVars.cfg_string, STR_IGAMMO, 4) == 0)
           mod.item_ignore[bo_gascan] = item_ammo; 
        else if(strncmp(gameVars.cfg_string, STR_IGROCKETS, 7) == 0)              
           mod.item_ignore[bo_gascan] = item_rockets; 
        else if(strncmp(gameVars.cfg_string, STR_IGGAS, 3) == 0)              
           mod.item_ignore[bo_gascan] = item_gas;    
        else if(strncmp(gameVars.cfg_string, STR_IGHEALTH, 6) == 0)              
           mod.item_ignore[bo_gascan] = item_health;     
    }
    //
    // MISSILES
    //
    else if(strncmp(cfg_line, "rockets_damage", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadNumber();      

        mod.item[bo_rockets][item_damage] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "rockets_health", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadNumber();      

        mod.item[bo_rockets][item_health] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "rockets_ammo", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_rockets][item_ammo] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "rockets_gas", 11) == 0)
    {   
        gameVars.char_index = 11;     
        ReadNumber();      

        mod.item[bo_rockets][item_gas] = gameVars.cfg_number;  
    }
    else if(strncmp(cfg_line, "rockets_rockets", 15) == 0)
    {   
        gameVars.char_index = 15;     
        ReadNumber();      

        mod.item[bo_rockets][item_rockets] = gameVars.cfg_number;  
    } 
    else if(strncmp(cfg_line, "rockets_time", 12) == 0)
    {   
        gameVars.char_index = 12;     
        ReadNumber();      

        mod.item[bo_rockets][item_time] = gameVars.cfg_number;  
    } 
    else if(strncmp(cfg_line, "rockets_ignore", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadString();   
                      
        if(strncmp(gameVars.cfg_string, STR_IGAMMO, 4) == 0)
           mod.item_ignore[bo_rockets] = item_ammo; 
        else if(strncmp(gameVars.cfg_string, STR_IGROCKETS, 7) == 0)              
           mod.item_ignore[bo_rockets] = item_rockets; 
        else if(strncmp(gameVars.cfg_string, STR_IGGAS, 3) == 0)              
           mod.item_ignore[bo_rockets] = item_gas;       
        else if(strncmp(gameVars.cfg_string, STR_IGHEALTH, 6) == 0)              
           mod.item_ignore[bo_rockets] = item_health;     
    }
    
    //--------------------------------------------------------------
    //
    // PLAYER MOVEMENT
    //
    else if(strncmp(cfg_line, "movement_speed", 14) == 0)	
    {   
        gameVars.char_index = 14;     
        ReadNumber();                            

        mod.player_speed = gameVars.cfg_number;
    } 
    //--------------------------------------------------------------
    //
    // ENEMIES
    //
    
    //
    // enable rotation for standard enemies
    //
    else if(strncmp(cfg_line, "8-dir-enemies", 13) == 0)
    {   
        mod.en_rotation = true;
    } 
    //
    // enable rotation for boss enemies
    //
    else if(strncmp(cfg_line, "8-dir-bosses", 12) == 0)
    {   
        mod.boss_rotation = true;
    } 
    
    //--------------------------------------------------------------
    //
    // enemy hitpoints
    //
    else if(strncmp(cfg_line, "ss_hp", 5) == 0)
    {   
        gameVars.char_index = 5;     
        ReadNumber();                            

        mod.hitpoints[en_ss] = gameVars.cfg_number;
    } 
    
    else if(strncmp(cfg_line, "grd_hp", 6) == 0
         || strncmp(cfg_line, "ofc_hp", 6) == 0	
         || strncmp(cfg_line, "mut_hp", 6) == 0	
         || strncmp(cfg_line, "dog_hp", 6) == 0)
    {   
        gameVars.char_index = 6;     
        ReadNumber();                            

        if(strncmp(cfg_line, "grd_hp", 6) == 0)
           mod.hitpoints[en_guard] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "ofc_hp", 6) == 0)
           mod.hitpoints[en_officer] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "mut_hp", 6) == 0)
           mod.hitpoints[en_mutant] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "dog_hp", 6) == 0)
           mod.hitpoints[en_dog] = gameVars.cfg_number;        
    } 
    
    else if(strncmp(cfg_line, "fake_hp", 7) == 0
         || strncmp(cfg_line, "mech_hp", 7) == 0
         || strncmp(cfg_line, "hitl_hp", 7) == 0
         || strncmp(cfg_line, "hans_hp", 7) == 0	
         || strncmp(cfg_line, "sbbs_hp", 7) == 0
         || strncmp(cfg_line, "uber_hp", 7) == 0
         || strncmp(cfg_line, "trns_hp", 7) == 0
         || strncmp(cfg_line, "deth_hp", 7) == 0
         )
    {   
        gameVars.char_index = 7;     
        ReadNumber();                            

        if(strncmp(cfg_line, "fake_hp", 7) == 0)
           mod.hitpoints[en_fake] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "mech_hp", 7) == 0)
           mod.hitpoints[en_hitler] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "hitl_hp", 7) == 0)
           mod.hitpoints[22] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "hans_hp", 7) == 0)
           mod.hitpoints[en_boss] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "sbbs_hp", 7) == 0)
           mod.hitpoints[en_schabbs] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "uber_hp", 7) == 0)
           mod.hitpoints[en_fat] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "trns_hp", 7) == 0)
           mod.hitpoints[en_gretel] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "deth_hp", 7) == 0)
           mod.hitpoints[en_gift] = gameVars.cfg_number;
    } 
    //
    // enemy damage
    //
    else if(strncmp(cfg_line, "grd_damage", 10) == 0
         || strncmp(cfg_line, "ofc_damage", 10) == 0	
         || strncmp(cfg_line, "mut_damage", 10) == 0	
         || strncmp(cfg_line, "dog_damage", 10) == 0
         || strncmp(cfg_line, "pac_damage", 10) == 0)
    {   
        gameVars.char_index = 10;     
        ReadNumber();                            

        if(strncmp(cfg_line, "grd_damage", 10) == 0)
           mod.en_damage[guardobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "ofc_damage", 10) == 0)
           mod.en_damage[officerobj] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "mut_damage", 10) == 0)
           mod.en_damage[mutantobj] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "dog_damage", 10) == 0)
          mod.en_damage[dogobj] = gameVars.cfg_number;       
        else if(strncmp(cfg_line, "pac_damage", 10) == 0)
          mod.en_damage[ghostobj] = gameVars.cfg_number;    
    } 

    else if(strncmp(cfg_line, "ss_damage", 9) == 0)
    {   
        gameVars.char_index = 9;     
        ReadNumber();                            

        mod.en_damage[ssobj] = gameVars.cfg_number;     
    } 
    
    else if(strncmp(cfg_line, "fake_damage", 11) == 0
         || strncmp(cfg_line, "mech_damage", 11) == 0
         || strncmp(cfg_line, "hitl_damage", 11) == 0
         || strncmp(cfg_line, "hans_damage", 11) == 0	
         || strncmp(cfg_line, "sbbs_damage", 11) == 0
         || strncmp(cfg_line, "uber_damage", 11) == 0
         || strncmp(cfg_line, "trns_damage", 11) == 0
         || strncmp(cfg_line, "deth_damage", 11) == 0
         )
    {   
        gameVars.char_index = 11;     
        ReadNumber();                            

        if(strncmp(cfg_line, "fake_damage", 11) == 0)
           mod.en_damage[fakeobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "mech_damage", 11) == 0)
           mod.en_damage[mechahitlerobj] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "hitl_damage", 11) == 0)
           mod.en_damage[realhitlerobj] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "hans_damage", 11) == 0)
           mod.en_damage[bossobj] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "sbbs_damage", 11) == 0)
           mod.en_damage[schabbobj] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "uber_damage", 11) == 0)
           mod.en_damage[fatobj] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "trns_damage", 11) == 0)
           mod.en_damage[gretelobj] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "deth_damage", 11) == 0)
           mod.en_damage[giftobj] = gameVars.cfg_number;       
    } 
    //
    // enemy speed
    //
    else if(strncmp(cfg_line, "ss_speed", 8) == 0)
    {   
        gameVars.char_index = 8;     
        ReadNumber();                            

        mod.en_speed[en_ss] = gameVars.cfg_number;
    } 
    
    else if(strncmp(cfg_line, "grd_speed", 9) == 0
         || strncmp(cfg_line, "ofc_speed", 9) == 0	
         || strncmp(cfg_line, "mut_speed", 9) == 0	
         || strncmp(cfg_line, "dog_speed", 9) == 0
         || strncmp(cfg_line, "pac_speed", 9) == 0)
    {   
        gameVars.char_index = 9;     
        ReadNumber();                            

        if(strncmp(cfg_line, "grd_speed", 9) == 0)
           mod.en_speed[en_guard] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "ofc_speed", 9) == 0)
           mod.en_speed[en_officer] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "mut_speed", 9) == 0)
           mod.en_speed[en_mutant] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "dog_speed", 9) == 0)
           mod.en_speed[en_dog] = gameVars.cfg_number;        
        else if(strncmp(cfg_line, "pac_speed", 9) == 0)
        {
           mod.en_speed[en_clyde] = gameVars.cfg_number;   
        } 
    } 
    
    else if(strncmp(cfg_line, "fake_speed", 10) == 0
         || strncmp(cfg_line, "mech_speed", 10) == 0
         || strncmp(cfg_line, "hitl_speed", 10) == 0
         || strncmp(cfg_line, "hans_speed", 10) == 0	
         || strncmp(cfg_line, "sbbs_speed", 10) == 0
         || strncmp(cfg_line, "uber_speed", 10) == 0
         || strncmp(cfg_line, "trns_speed", 10) == 0
         || strncmp(cfg_line, "deth_speed", 10) == 0
         )
    {   
        gameVars.char_index = 10;     
        ReadNumber();                             

        if(strncmp(cfg_line, "fake_speed", 10) == 0)
           mod.en_speed[en_fake] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "mech_speed", 10) == 0)
           mod.en_speed[en_hitler] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "hitl_speed", 10) == 0)
           mod.en_speed[22] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "hans_speed", 10) == 0)
           mod.en_speed[en_boss] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "sbbs_speed", 10) == 0)
           mod.en_speed[en_schabbs] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "uber_speed", 10) == 0)
           mod.en_speed[en_fat] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "trns_speed", 10) == 0)
           mod.en_speed[en_gretel] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "deth_speed", 10) == 0)
           mod.en_speed[en_gift] = gameVars.cfg_number;
    } 
        
    //
    // enemy frame duration
    //
    else if(strncmp(cfg_line, "grd_animspd", 11) == 0
         || strncmp(cfg_line, "ofc_animspd", 11) == 0	
         || strncmp(cfg_line, "mut_animspd", 11) == 0	
         || strncmp(cfg_line, "dog_animspd", 11) == 0
         || strncmp(cfg_line, "pac_animspd", 11) == 0)
    {   
        gameVars.char_index = 11;     
        ReadNumber();                            

        if(strncmp(cfg_line, "grd_animspd", 11) == 0)
           mod.en_animspeed[guardobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "ofc_animspd", 11) == 0)
           mod.en_animspeed[officerobj] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "mut_animspd", 11) == 0)
           mod.en_animspeed[mutantobj] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "dog_animspd", 11) == 0)
          mod.en_animspeed[dogobj] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "pac_animspd", 11) == 0)
          mod.en_animspeed[ghostobj] = gameVars.cfg_number;       
    } 

    else if(strncmp(cfg_line, "ss_animspd", 10) == 0)
    {   
        gameVars.char_index = 10;     
        ReadNumber();                            

        mod.en_animspeed[ssobj] = gameVars.cfg_number;     
    } 
    
    else if(strncmp(cfg_line, "fake_animspd", 12) == 0
         || strncmp(cfg_line, "mech_animspd", 12) == 0
         || strncmp(cfg_line, "hitl_animspd", 12) == 0
         || strncmp(cfg_line, "hans_animspd", 12) == 0	
         || strncmp(cfg_line, "sbbs_animspd", 12) == 0
         || strncmp(cfg_line, "uber_animspd", 12) == 0
         || strncmp(cfg_line, "trns_animspd", 12) == 0
         || strncmp(cfg_line, "deth_animspd", 12) == 0
         )
    {   
        gameVars.char_index = 12;     
        ReadNumber();                            

        if(strncmp(cfg_line, "fake_animspd", 12) == 0)
           mod.en_animspeed[fakeobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "mech_animspd", 12) == 0)
           mod.en_animspeed[mechahitlerobj] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "hitl_animspd", 12) == 0)
           mod.en_animspeed[realhitlerobj] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "hans_animspd", 12) == 0)
           mod.en_animspeed[bossobj] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "sbbs_animspd", 12) == 0)
           mod.en_animspeed[schabbobj] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "uber_animspd", 12) == 0)
           mod.en_animspeed[fatobj] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "trns_animspd", 12) == 0)
           mod.en_animspeed[gretelobj] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "deth_animspd", 12) == 0)
           mod.en_animspeed[giftobj] = gameVars.cfg_number;          
    } 
    //
    // enemy chase
    //
    else if(strncmp(cfg_line, "grd_chase", 9) == 0
         || strncmp(cfg_line, "ofc_chase", 9) == 0	
         || strncmp(cfg_line, "mut_chase", 9) == 0	
         || strncmp(cfg_line, "dog_chase", 9) == 0
         || strncmp(cfg_line, "pac_chase", 9) == 0)
    {   
        gameVars.char_index = 9;     
        ReadString();      

        if(strncmp(gameVars.cfg_string, "default", 7) == 0)  
           gameVars.cfg_number = t_default;
        else if(strncmp(gameVars.cfg_string, "range", 5) == 0)  
           gameVars.cfg_number = t_range;
        else if(strncmp(gameVars.cfg_string, "melee", 5) == 0)  
           gameVars.cfg_number = t_melee;  
        else if(strncmp(gameVars.cfg_string, "boss", 4) == 0)  
           gameVars.cfg_number = t_boss;     
        else if(strncmp(gameVars.cfg_string, "ghost", 5) == 0)  
           gameVars.cfg_number = t_pac; 
        else if(strncmp(gameVars.cfg_string, "flee", 4) == 0)  
           gameVars.cfg_number = t_flee;                                              

        if(strncmp(cfg_line, "grd_chase", 9) == 0)
           mod.en_chase[guardobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "ofc_chase", 9) == 0)
           mod.en_chase[officerobj] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "mut_chase", 9) == 0)
           mod.en_chase[mutantobj] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "dog_chase", 9) == 0)
          mod.en_chase[dogobj] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "pac_chase", 9) == 0)
          mod.en_chase[ghostobj] = gameVars.cfg_number;          
    } 

    else if(strncmp(cfg_line, "ss_chase", 8) == 0)
    {   
        gameVars.char_index = 8;     
        ReadString();      

        if(strncmp(gameVars.cfg_string, "default", 7) == 0)  
           gameVars.cfg_number = t_default;
        else if(strncmp(gameVars.cfg_string, "range", 5) == 0)  
           gameVars.cfg_number = t_range;
        else if(strncmp(gameVars.cfg_string, "melee", 5) == 0)  
           gameVars.cfg_number = t_melee;  
        else if(strncmp(gameVars.cfg_string, "boss", 4) == 0)  
           gameVars.cfg_number = t_boss;    
        else if(strncmp(gameVars.cfg_string, "ghost", 5) == 0)  
           gameVars.cfg_number = t_pac; 
        else if(strncmp(gameVars.cfg_string, "flee", 4) == 0)  
           gameVars.cfg_number = t_flee;                               

        mod.en_chase[ssobj] = gameVars.cfg_number;     
    } 
    
    else if(strncmp(cfg_line, "fake_chase", 10) == 0
         || strncmp(cfg_line, "mech_chase", 10) == 0
         || strncmp(cfg_line, "hitl_chase", 10) == 0
         || strncmp(cfg_line, "hans_chase", 10) == 0	
         || strncmp(cfg_line, "sbbs_chase", 10) == 0
         || strncmp(cfg_line, "uber_chase", 10) == 0
         || strncmp(cfg_line, "trns_chase", 10) == 0
         || strncmp(cfg_line, "deth_chase", 10) == 0)
    {   
        gameVars.char_index = 10;     
        ReadString();      

        if(strncmp(gameVars.cfg_string, "default", 7) == 0)  
           gameVars.cfg_number = t_default;
        else if(strncmp(gameVars.cfg_string, "range", 5) == 0)  
           gameVars.cfg_number = t_range;
        else if(strncmp(gameVars.cfg_string, "melee", 5) == 0)  
           gameVars.cfg_number = t_melee;  
        else if(strncmp(gameVars.cfg_string, "boss", 4) == 0)  
           gameVars.cfg_number = t_boss; 
        else if(strncmp(gameVars.cfg_string, "ghost", 5) == 0)  
           gameVars.cfg_number = t_pac;      
        else if(strncmp(gameVars.cfg_string, "flee", 4) == 0)  
           gameVars.cfg_number = t_flee;                             

        if(strncmp(cfg_line, "fake_chase", 10) == 0)
           mod.en_chase[fakeobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "mech_chase", 10) == 0)
           mod.en_chase[mechahitlerobj] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "hitl_chase", 10) == 0)
           mod.en_chase[realhitlerobj] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "hans_chase", 10) == 0)
           mod.en_chase[bossobj] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "sbbs_chase", 10) == 0)
           mod.en_chase[schabbobj] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "uber_chase", 10) == 0)
           mod.en_chase[fatobj] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "trns_chase", 10) == 0)
           mod.en_chase[gretelobj] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "deth_chase", 10) == 0)
           mod.en_chase[giftobj] = gameVars.cfg_number;            
    } 
    
    //
    // enemy attack
    //
    else if(strncmp(cfg_line, "grd_attack", 10) == 0
         || strncmp(cfg_line, "ofc_attack", 10) == 0	
         || strncmp(cfg_line, "mut_attack", 10) == 0	
         || strncmp(cfg_line, "dog_attack", 10) == 0
         || strncmp(cfg_line, "pac_attack", 10) == 0)
    {   
        gameVars.char_index = 10;     
        ReadString();      

        if(strncmp(gameVars.cfg_string, "default", 7) == 0)  
           gameVars.cfg_number = t_defatk;
        else if(strncmp(gameVars.cfg_string, "needle", 6) == 0)  
           gameVars.cfg_number = t_needle;
        else if(strncmp(gameVars.cfg_string, "rocket", 6) == 0)  
           gameVars.cfg_number = t_rocket;  
        else if(strncmp(gameVars.cfg_string, "bullet", 6) == 0)  
           gameVars.cfg_number = t_shoot;
        else if(strncmp(gameVars.cfg_string, "melee", 5) == 0)  
           gameVars.cfg_number = t_bite;    
        else if(strncmp(gameVars.cfg_string, "fire", 4) == 0)  
           gameVars.cfg_number = t_fire;    
        else if(strncmp(gameVars.cfg_string, "fakefire", 8) == 0)  
           gameVars.cfg_number = t_ffire;                               

        if(strncmp(cfg_line, "grd_attack", 10) == 0)
           mod.en_attack[guardobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "ofc_attack", 10) == 0)
           mod.en_attack[officerobj] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "mut_attack", 10) == 0)
           mod.en_attack[mutantobj] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "dog_attack", 10) == 0)
          mod.en_attack[dogobj] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "pac_attack", 10) == 0)
          mod.en_attack[ghostobj] = gameVars.cfg_number;          
    } 

    else if(strncmp(cfg_line, "ss_attack", 9) == 0)
    {   
        gameVars.char_index = 9;     
        ReadString();      

        if(strncmp(gameVars.cfg_string, "default", 7) == 0)  
           gameVars.cfg_number = t_defatk;
        else if(strncmp(gameVars.cfg_string, "needle", 6) == 0)  
           gameVars.cfg_number = t_needle;
        else if(strncmp(gameVars.cfg_string, "rocket", 6) == 0)  
           gameVars.cfg_number = t_rocket;  
        else if(strncmp(gameVars.cfg_string, "bullet", 6) == 0)  
           gameVars.cfg_number = t_shoot;
        else if(strncmp(gameVars.cfg_string, "melee", 5) == 0)  
           gameVars.cfg_number = t_bite;   
        else if(strncmp(gameVars.cfg_string, "fire", 4) == 0)  
           gameVars.cfg_number = t_fire;    
        else if(strncmp(gameVars.cfg_string, "fakefire", 8) == 0)  
           gameVars.cfg_number = t_ffire;                          

        mod.en_attack[ssobj] = gameVars.cfg_number;     
    } 
    
    else if(strncmp(cfg_line, "fake_attack", 11) == 0
         || strncmp(cfg_line, "mech_attack", 11) == 0
         || strncmp(cfg_line, "hitl_attack", 11) == 0
         || strncmp(cfg_line, "hans_attack", 11) == 0	
         || strncmp(cfg_line, "sbbs_attack", 11) == 0
         || strncmp(cfg_line, "uber_attack", 11) == 0
         || strncmp(cfg_line, "trns_attack", 11) == 0
         || strncmp(cfg_line, "deth_attack", 11) == 0)
    {   
        gameVars.char_index = 10;     
        ReadString();      

        if(strncmp(gameVars.cfg_string, "default", 7) == 0)  
           gameVars.cfg_number = t_defatk;
        else if(strncmp(gameVars.cfg_string, "needle", 6) == 0)  
           gameVars.cfg_number = t_needle;
        else if(strncmp(gameVars.cfg_string, "rocket", 6) == 0)  
           gameVars.cfg_number = t_rocket;  
        else if(strncmp(gameVars.cfg_string, "bullet", 6) == 0)  
           gameVars.cfg_number = t_shoot;
        else if(strncmp(gameVars.cfg_string, "melee", 5) == 0)  
           gameVars.cfg_number = t_bite;  
        else if(strncmp(gameVars.cfg_string, "fire", 4) == 0)  
           gameVars.cfg_number = t_fire;    
        else if(strncmp(gameVars.cfg_string, "fakefire", 8) == 0)  
           gameVars.cfg_number = t_ffire;                                

        if(strncmp(cfg_line, "fake_attack", 11) == 0)
           mod.en_attack[fakeobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "mech_attack", 11) == 0)
           mod.en_attack[mechahitlerobj] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "hitl_attack", 11) == 0)
           mod.en_attack[realhitlerobj] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "hans_attack", 11) == 0)
           mod.en_attack[bossobj] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "sbbs_attack", 11) == 0)
           mod.en_attack[schabbobj] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "uber_attack", 11) == 0)
           mod.en_attack[fatobj] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "trns_attack", 11) == 0)
           mod.en_attack[gretelobj] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "deth_attack", 11) == 0)
           mod.en_attack[giftobj] = gameVars.cfg_number;            
    } 
    //--------------------------------------------------------------
    //
    // PROJECTILES
    //
    
    //
    // projectile speed
    //
    else if(strncmp(cfg_line, "pl_fire_speed", 13) == 0
         || strncmp(cfg_line, "pl_miss_speed", 13) == 0	
         || strncmp(cfg_line, "en_miss_speed", 13) == 0	
         || strncmp(cfg_line, "en_fire_speed", 13) == 0
         || strncmp(cfg_line, "en_ffir_speed", 13) == 0
         || strncmp(cfg_line, "en_need_speed", 13) == 0)
    {   
        gameVars.char_index = 13;     
        ReadNumber();  
        
        
        if(strncmp(cfg_line, "pl_fire_speed", 13) == 0)
           mod.prj_speed[pl_fire] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "pl_miss_speed", 13) == 0)
           mod.prj_speed[pl_miss] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "en_miss_speed", 13) == 0)
           mod.prj_speed[en_miss] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "en_fire_speed", 13) == 0)
           mod.prj_speed[en_fire] = gameVars.cfg_number;    
        else if(strncmp(cfg_line, "en_need_speed", 13) == 0)
           mod.prj_speed[en_need] = gameVars.cfg_number; 
        else if(strncmp(cfg_line, "en_ffir_speed", 13) == 0)
           mod.prj_speed[en_ffir] = gameVars.cfg_number;      
    } 
    //
    // projectile damage
    //
    else if(strncmp(cfg_line, "pl_need_damage", 14) == 0
         || strncmp(cfg_line, "pl_fire_damage", 14) == 0
         || strncmp(cfg_line, "pl_miss_damage", 14) == 0
         || strncmp(cfg_line, "pl_ffir_damage", 14) == 0
         
         || strncmp(cfg_line, "en_need_damage", 14) == 0
         || strncmp(cfg_line, "en_fire_damage", 14) == 0
         || strncmp(cfg_line, "en_miss_damage", 14) == 0
         || strncmp(cfg_line, "en_ffir_damage", 14) == 0
         )
    {   
        gameVars.char_index = 14;     
        ReadNumber();                            
  
        if(strncmp(cfg_line, "pl_need_damage", 14) == 0)
           mod.prj_damage[pl_needleobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "pl_fire_damage", 14) == 0)
           mod.prj_damage[pl_fireobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "pl_miss_damage", 14) == 0)
           mod.prj_damage[rocketobj] = gameVars.cfg_number;    
        else if(strncmp(cfg_line, "pl_ffir_damage", 14) == 0)
           mod.prj_damage[pl_fakefireobj] = gameVars.cfg_number;    
           
        else if(strncmp(cfg_line, "en_need_damage", 14) == 0)
           mod.en_damage[needleobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "en_fire_damage", 14) == 0)
           mod.en_damage[en_fireobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "en_miss_damage", 14) == 0)
           mod.en_damage[drocketobj] = gameVars.cfg_number;    
        else if(strncmp(cfg_line, "en_ffir_damage", 14) == 0)
           mod.en_damage[fireobj] = gameVars.cfg_number;      
    } 
    //
    // projectile lifetime
    //
    else if(strncmp(cfg_line, "pl_fire_lifetime", 16) == 0
         || strncmp(cfg_line, "pl_miss_lifetime", 16) == 0	
         || strncmp(cfg_line, "en_miss_lifetime", 16) == 0	
         || strncmp(cfg_line, "en_fire_lifetime", 16) == 0
         || strncmp(cfg_line, "en_ffir_lifetime", 16) == 0
         || strncmp(cfg_line, "en_need_lifetime", 16) == 0)
    {   
        gameVars.char_index = 16;     
        ReadNumber();                            

        if(strncmp(cfg_line, "pl_fire_lifetime", 16) == 0)
           mod.prj_lifetime[pl_fire] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "pl_miss_lifetime", 16) == 0)
           mod.prj_lifetime[pl_miss] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "en_miss_lifetime", 16) == 0)
           mod.prj_lifetime[en_miss] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "en_fire_lifetime", 16) == 0)
          mod.prj_lifetime[en_fire] = gameVars.cfg_number;    
        else if(strncmp(cfg_line, "en_need_lifetime", 16) == 0)
          mod.prj_lifetime[en_need] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "en_ffir_lifetime", 16) == 0)
          mod.prj_lifetime[en_ffir] = gameVars.cfg_number;       
    } 
    //
    // projectile animation cycle speed
    //
    else if(strncmp(cfg_line, "pl_fire_animspd", 15) == 0
         || strncmp(cfg_line, "pl_miss_animspd", 15) == 0	
         || strncmp(cfg_line, "en_miss_animspd", 15) == 0	
         || strncmp(cfg_line, "en_fire_animspd", 15) == 0
         || strncmp(cfg_line, "en_ffir_animspd", 15) == 0
         || strncmp(cfg_line, "en_need_animspd", 15) == 0)
    {   
        gameVars.char_index = 15;     
        ReadNumber();     
                               
        if(strncmp(cfg_line, "pl_miss_animspd", 15) == 0)
           mod.en_animspeed[rocketobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "pl_fire_animspd", 15) == 0)
           mod.en_animspeed[pl_fireobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "en_miss_animspd", 15) == 0)
           mod.en_animspeed[drocketobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "en_fire_animspd", 15) == 0)
           mod.en_animspeed[en_fireobj] = gameVars.cfg_number;    
        else if(strncmp(cfg_line, "en_ffir_animspd", 15) == 0)
           mod.en_animspeed[fireobj] = gameVars.cfg_number;          
        else if(strncmp(cfg_line, "en_need_animspd", 15) == 0)
           mod.en_animspeed[needleobj] = gameVars.cfg_number;        
    }  
    //
    // projectile style
    //
    else if(strncmp(cfg_line, "pl_miss_homing", 14) == 0
         || strncmp(cfg_line, "pl_fire_homing", 14) == 0
         || strncmp(cfg_line, "en_miss_homing", 14) == 0
         || strncmp(cfg_line, "en_fire_homing", 14) == 0	
         || strncmp(cfg_line, "en_ffir_homing", 14) == 0
         || strncmp(cfg_line, "en_need_homing", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadString();          

        if(strncmp(gameVars.cfg_string, "none", 4) == 0)  
           gameVars.cfg_number = homing_none;
        else if(strncmp(gameVars.cfg_string, "low", 3) == 0)  
           gameVars.cfg_number = homing_low;
        else if(strncmp(gameVars.cfg_string, "medium", 6) == 0)  
           gameVars.cfg_number = homing_med;
        else if(strncmp(gameVars.cfg_string, "high", 4) == 0)  
           gameVars.cfg_number = homing_high;
           
        if(strncmp(cfg_line, "pl_fire_homing", 14) == 0)
           mod.prj_homingfactor[pl_fire] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "pl_miss_homing", 14) == 0)
           mod.prj_homingfactor[pl_miss] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "en_miss_homing", 14) == 0)
           mod.prj_homingfactor[en_miss] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "en_fire_homing", 14) == 0)
          mod.prj_homingfactor[en_fire] = gameVars.cfg_number;    
        else if(strncmp(cfg_line, "en_need_homing", 14) == 0)
          mod.prj_homingfactor[en_need] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "en_ffir_homing", 14) == 0)
          mod.prj_homingfactor[en_ffir] = gameVars.cfg_number;                                        
    }  
    //
    // projectile splash damage
    //
    else if(strncmp(cfg_line, "pl_miss_splashdmg", 17) == 0
         || strncmp(cfg_line, "pl_fire_splashdmg", 17) == 0
         || strncmp(cfg_line, "en_miss_splashdmg", 17) == 0
         || strncmp(cfg_line, "en_fire_splashdmg", 17) == 0	
         || strncmp(cfg_line, "en_need_splashdmg", 17) == 0
         || strncmp(cfg_line, "en_ffir_splashdmg", 17) == 0)
    {   
        gameVars.char_index = 17;     
        ReadNumber();          
           
        if(strncmp(cfg_line, "pl_fire_splashdmg", 17) == 0)
           mod.prj_splashdmg[pl_fire] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "pl_miss_splashdmg", 17) == 0)
           mod.prj_splashdmg[pl_miss] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "en_miss_splashdmg", 17) == 0)
           mod.prj_splashdmg[en_miss] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "en_fire_splashdmg", 17) == 0)
           mod.prj_splashdmg[en_fire] = gameVars.cfg_number;    
        else if(strncmp(cfg_line, "en_need_splashdmg", 17) == 0)
           mod.prj_splashdmg[en_need] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "en_ffir_splashdmg", 17) == 0)
           mod.prj_splashdmg[en_ffir] = gameVars.cfg_number;                                           
    }  
    //
    // projectile splash radius
    //
    else if(strncmp(cfg_line, "pl_miss_splashdist", 18) == 0
         || strncmp(cfg_line, "pl_fire_splashdist", 18) == 0
         || strncmp(cfg_line, "en_miss_splashdist", 18) == 0
         || strncmp(cfg_line, "en_fire_splashdist", 18) == 0	
         || strncmp(cfg_line, "en_need_splashdist", 18) == 0
         || strncmp(cfg_line, "en_ffir_splashdist", 18) == 0)
    {   
        gameVars.char_index = 18;     
        ReadNumber();          
           
        if(strncmp(cfg_line, "pl_fire_splashdist", 18) == 0)
           mod.prj_splashradius[pl_fire] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "pl_miss_splashdist", 18) == 0)
           mod.prj_splashradius[pl_miss] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "en_miss_splashdist", 18) == 0)
           mod.prj_splashradius[en_miss] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "en_fire_splashdist", 18) == 0)
           mod.prj_splashradius[en_fire] = gameVars.cfg_number;    
        else if(strncmp(cfg_line, "en_need_splashdist", 18) == 0)
           mod.prj_splashradius[en_need] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "en_ffir_splashdist", 18) == 0)
           mod.prj_splashradius[en_ffir] = gameVars.cfg_number;                                           
    } 
    //
    // projectile splash damage type
    //
    else if(strncmp(cfg_line, "pl_miss_selfdmg", 15) == 0
         || strncmp(cfg_line, "pl_fire_selfdmg", 15) == 0
         || strncmp(cfg_line, "en_miss_selfdmg", 15) == 0
         || strncmp(cfg_line, "en_fire_selfdmg", 15) == 0	
         || strncmp(cfg_line, "en_need_selfdmg", 15) == 0
         || strncmp(cfg_line, "en_ffir_selfdmg", 15) == 0)
    {   
        gameVars.char_index = 15;     
        ReadString();   
        
        if(strncmp(gameVars.cfg_string, "none", 4) == 0)  
           gameVars.cfg_number = 0;
        else if(strncmp(gameVars.cfg_string, "player", 6) == 0)  
           gameVars.cfg_number = 1;
        else if(strncmp(gameVars.cfg_string, "enemies", 7) == 0)  
           gameVars.cfg_number = 2;
        else if(strncmp(gameVars.cfg_string, "all", 3) == 0)  
           gameVars.cfg_number = 3;
                      
        if(strncmp(cfg_line, "pl_fire_selfdmg", 15) == 0)
           mod.prj_selfdmg[pl_fire] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "pl_miss_selfdmg", 15) == 0)
           mod.prj_selfdmg[pl_miss] = gameVars.cfg_number;   
        else if(strncmp(cfg_line, "en_miss_selfdmg", 15) == 0)
           mod.prj_selfdmg[en_miss] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "en_fire_selfdmg", 15) == 0)
           mod.prj_selfdmg[en_fire] = gameVars.cfg_number;    
        else if(strncmp(cfg_line, "en_need_selfdmg", 15) == 0)
           mod.prj_selfdmg[en_need] = gameVars.cfg_number;  
        else if(strncmp(cfg_line, "en_ffir_selfdmg", 15) == 0)
           mod.prj_selfdmg[en_ffir] = gameVars.cfg_number;                                           
    }  
    //
    // projectile explosion player knockback
    //
    else if(strncmp(cfg_line, "pl_need_playerknock", 19) == 0
         || strncmp(cfg_line, "pl_fire_playerknock", 19) == 0
         || strncmp(cfg_line, "pl_miss_playerknock", 19) == 0
         || strncmp(cfg_line, "pl_ffir_playerknock", 19) == 0
         || strncmp(cfg_line, "en_need_playerknock", 19) == 0
         || strncmp(cfg_line, "en_fire_playerknock", 19) == 0
         || strncmp(cfg_line, "en_miss_playerknock", 19) == 0
         || strncmp(cfg_line, "en_ffir_playerknock", 19) == 0
         )
    {   
        gameVars.char_index = 19;     
        ReadNumber();                            
  
        if(strncmp(cfg_line, "pl_need_playerknock", 19) == 0)
           mod.prj_playerknockback[pl_needleobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "pl_fire_playerknock", 19) == 0)
           mod.prj_playerknockback[pl_fireobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "pl_miss_playerknock", 19) == 0)
           mod.prj_playerknockback[rocketobj] = gameVars.cfg_number;    
        else if(strncmp(cfg_line, "pl_ffir_playerknock", 19) == 0)
           mod.prj_playerknockback[pl_fakefireobj] = gameVars.cfg_number;    
           
        else if(strncmp(cfg_line, "en_need_playerknock", 19) == 0)
           mod.prj_playerknockback[needleobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "en_fire_playerknock", 19) == 0)
           mod.prj_playerknockback[en_fireobj] = gameVars.cfg_number;
        else if(strncmp(cfg_line, "en_miss_playerknock", 19) == 0)
           mod.prj_playerknockback[drocketobj] = gameVars.cfg_number;    
        else if(strncmp(cfg_line, "en_ffir_playerknock", 19) == 0)
           mod.prj_playerknockback[fireobj] = gameVars.cfg_number;      
    }
    //--------------------------------------------------------------
    //
    //
    //
    else if(strncmp(cfg_line, "statobj_nobonus_id", 18) == 0)
    {   
        gameVars.char_index = 18;
        ReadNumber();            
        value = gameVars.cfg_number;     

        mod.statobj_nobonus[value] = true;      
    }  
    else if(strncmp(cfg_line, "statobj_lit_id", 14) == 0)
    {   
        gameVars.char_index = 14;
        ReadNumber();            
        value = gameVars.cfg_number;     
        ReadString();
        
        if(strncmp(gameVars.cfg_string, "yes", 3) == 0)  
            mod.statobj_lit[value] = 2;
        else if(strncmp(gameVars.cfg_string, "no", 2) == 0)  
            mod.statobj_lit[value] = 1;      
    }  
    else if(strncmp(cfg_line, "statobj_solid_id", 16) == 0)
    {   
        gameVars.char_index = 16;
        ReadNumber();            
        value = gameVars.cfg_number;     
        ReadString();
        
        if(strncmp(gameVars.cfg_string, "yes", 3) == 0)  
            mod.statobj_solid[value] = 2;
        else if(strncmp(gameVars.cfg_string, "no", 2) == 0)  
            mod.statobj_solid[value] = 1;         
    }  
    else if(strncmp(cfg_line, "statobj_board_id", 16) == 0)
    {   
        gameVars.char_index = 16;
        ReadNumber();            
        value = gameVars.cfg_number;     
        ReadString();
        
        if(strncmp(gameVars.cfg_string, "vert_front", 10) == 0)  
            mod.statobj_board[value] = 1;
        else if(strncmp(gameVars.cfg_string, "vert_back", 9) == 0)  
            mod.statobj_board[value] = 2; 
        else if(strncmp(gameVars.cfg_string, "vert_mid", 8) == 0)  
            mod.statobj_board[value] = 3; 
        else if(strncmp(gameVars.cfg_string, "horiz_front", 11) == 0)  
            mod.statobj_board[value] = 4;
        else if(strncmp(gameVars.cfg_string, "horiz_back", 10) == 0)  
            mod.statobj_board[value] = 5; 
        else if(strncmp(gameVars.cfg_string, "horiz_mid", 9) == 0)  
            mod.statobj_board[value] = 6;         
    } 
    //--------------------------------------------------------------
    //
    //
    //
    /*else if(strncmp(cfg_line, "hud_area_x", 10) == 0)
    {   
        gameVars.char_index = 10;     
        ReadNumber();      
        mod.hud_x[area] = gameVars.cfg_number;  
    } 
    else if(strncmp(cfg_line, "hud_items_x", 11) == 0)
    {   
        gameVars.char_index = 11;     
        ReadNumber();      
        mod.hud_x[item] = gameVars.cfg_number;  
    } */
    else if(strncmp(cfg_line, "hud_x", 5) == 0)
    {   
        gameVars.char_index = 5;     
        ReadNumber();  
        value = gameVars.cfg_number;   
        ReadNumber();    
        mod.hud_x[value] = gameVars.cfg_number;  
        hudmod = true;
    }
    else if(strncmp(cfg_line, "hud_y", 5) == 0)
    {   
        gameVars.char_index = 5;     
        ReadNumber();  
        value = gameVars.cfg_number;   
        ReadNumber();    
        mod.hud_y[value] = gameVars.cfg_number;  
        hudmod = true;
    }
    else if(strncmp(cfg_line, "hud_align", 9) == 0)
    {   
        gameVars.char_index = 9;     
        ReadNumber();  
        value = gameVars.cfg_number;   
        ReadString();
        
        if(strncmp(gameVars.cfg_string, "top_left", 8) == 0)  
            mod.hud_align[value] = northwest;
        else if(strncmp(gameVars.cfg_string, "top_center", 10) == 0)  
            mod.hud_align[value] = north;    
        else if(strncmp(gameVars.cfg_string, "top_right", 9) == 0)  
            mod.hud_align[value] = northeast;    
        
        else if(strncmp(gameVars.cfg_string, "bottom_left", 11) == 0)  
            mod.hud_align[value] = southwest;    
        else if(strncmp(gameVars.cfg_string, "bottom_center", 13) == 0)  
            mod.hud_align[value] = south;    
        else if(strncmp(gameVars.cfg_string, "bottom_right", 12) == 0)  
            mod.hud_align[value] = southeast;        
    }
    
    //--------------------------------------------------------------
    //
    //
    //
    else if(strncmp(cfg_line, "countdown_time", 14) == 0)
    {   
        gameVars.char_index = 14;     
        ReadNumber();      

        mod.mapTime = gameVars.cfg_number;  
    } 
    else if(strncmp(cfg_line, "countdown_end", 13) == 0)
    {   
        gameVars.char_index = 13;     
        ReadString();       

        if(strncmp(gameVars.cfg_string, "die", 3) == 0)  
           mod.mapTime_end = 1;
        else if(strncmp(gameVars.cfg_string, "endmap", 6) == 0) 
           mod.mapTime_end = 2; 
        else if(strncmp(gameVars.cfg_string, "victory", 7) == 0) 
           mod.mapTime_end = 3;    
    } 
    else if(strncmp(cfg_line, "start_from_scratch", 18) == 0)
    {        
        mod.startfromzero = true;
    }  
    else if(strncmp(cfg_line, "mission", 7) == 0)
    {   
        gameVars.char_index = 7;     
        ReadString();      

        if(strncmp(gameVars.cfg_string, "killall", 7) == 0)  
           mod.mission = 1;
        else if(strncmp(gameVars.cfg_string, "treasurehunt", 12) == 0)  
           mod.mission = 2;  
        else if(strncmp(gameVars.cfg_string, "finditem", 8) == 0) 
           mod.mission = 3;  
        else
           mod.mission = 0;   
    } 
    else if(strncmp(cfg_line, "targetitem", 10) == 0)
    {   
        gameVars.char_index = 10;       
        ReadNumber();
        mod.missionitem = gameVars.cfg_number;     
    }    
    
    //
    //
    //
    else if(strncmp(cfg_line, "darktable", 9) == 0)	
    {   
        gameVars.char_index = 9;                   
        ReadString();  
        
        strcpy(darktablename, gameVars.cfg_string);           
    }
    else if(strncmp(cfg_line, "vswap64", 7) == 0)
    {   
        vswap64 = true;
    } 
    else if(strncmp(cfg_line, "info", 4) == 0)	
    {   
        gameVars.char_index = 4;                   
        ReadString();  
        
        strcpy(authnote, gameVars.cfg_string);            
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
= SetupModVars
=
=====================
*/

void SetupModVars (void)
{
     
    memset (&mod,0,sizeof(mod));
    
    vswap64 = false;
    
    // just to get 0 (like startmmo = 0) for some of the vars. genius duh!
    mod.startlives = mod.startammo = mod.startgas = mod.startrockets = mod.player_speed = -1;
    
    for (int n = 0; n < NUMWEAPONS; n++) 
    {
         mod.wp_damage[n] = -1;
         mod.wp_ammousage[n] = -1;
         mod.wp_firerate[n] = -1;
    }
    
    for (int n = 0; n < HUDELEMENTS; n++)  // fix up Y coordinate for resolution scaling or alignment
    {
         mod.hud_x[n] = -1;
         mod.hud_y[n] = -1;
    }

    for (int n = 0; n < NUMENEMIES+20; n++)      
    {     
         mod.en_damage[n] = -1;  
         mod.en_animspeed[n] = -1;  
    } 
    
    for (int n = 0; n < NUMENEMIES; n++)               
         mod.en_speed[n] = -1;
         
    for (int n = 0; n < NUMPROJECTILES; n++)      
    {
         mod.prj_speed[n] = -1;  
         mod.prj_lifetime[n] = -1;
         mod.prj_damage[n] = -1;
    }
}

/*
=====================
=
= SetupVswap
=
=====================
*/

void SetupVswap (void)
{   
    
    if (vswap64)
    {
         textureshift = 6;
         texturesize = (1<<textureshift);
         texmask = (texturesize*(texturesize-1));
         texfromfixedshift = 4;
         spritescale = 2;                   
    }
    else
    {
         textureshift = TEXTURESHIFT;
         texturesize = TEXTURESIZE;
         texmask = (TEXTURESIZE*(TEXTURESIZE-1));
         texfromfixedshift = TEXTUREFROMFIXEDSHIFT;
         spritescale = SPRITESCALEFACTOR;    
    }   
    
    if (!modded) 
        return;
    
    if (vswap64)
    {
        PM_Shutdown ();            
        PM_Startup (4096);
    }
    else
    {
        PM_Shutdown ();   
        
        if (modded)
           PM_Startup (1);  
        else       
           PM_Startup (0);
    } 
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

void LoadModConfig (void)
{
    char filename[30]; 
    char filepath[300];
    char folder[10] = "conf\\";
    char levelfilename[40];
        
    SetupModVars ();

    modded = false;
    
    strcpy(levelfilename,folder);
    
    if (modname[0] != 0)
        strcat(levelfilename,modname);
    else
        strcat(levelfilename,modcfg);

    strcat(levelfilename,"xx");
    
    // more sane numbering for the levels
    if (gamestate.mapon == 9)
    {
        levelfilename[strlen(levelfilename)-2] = '0'+gamestate.episode+1;                
        levelfilename[strlen(levelfilename)-1] = '0'+gamestate.mapon-9;
    }
    else
    {
        levelfilename[strlen(levelfilename)-2] = '0'+gamestate.episode;
        levelfilename[strlen(levelfilename)-1] = '0'+gamestate.mapon+1;
    }
    
    strcat(levelfilename,".cfg");
    
    if(filedir[0])
        snprintf(filepath, sizeof(filepath), "%s%s", filedir, levelfilename);
    else
        strcpy(filepath, levelfilename);
  
    gamefile = open(filepath, O_RDONLY | O_TEXT);
    
    if (gamefile == -1)
    {
        strcpy(filepath, levelfilename);
        gamefile = open(filepath, O_RDONLY | O_TEXT);
    
    if (gamefile == -1)
    {  
        strcpy(filename,folder);
        
        if (modname[0] != 0)
            strcat(filename,modname);
        else          
            strcat(filename,modcfg);
            
        strcat(filename,".cfg");    
  
        if(filedir[0])
            snprintf(filepath, sizeof(filepath), "%s%s", filedir, filename);
        else
            strcpy(filepath, filename);
    
        gamefile = open(filepath, O_RDONLY | O_TEXT);
    
        if (gamefile == -1)  
        {
            strcpy(filepath, filename);
            gamefile = open(filepath, O_RDONLY | O_TEXT);
            if (gamefile == -1)         
            return;
        }
}    
    }
        
    modded = true; // to raise map limits a bit and maybe some other things    

    if((cfg_buffer = (char *) malloc(MAX_BUF_LENGTH)) > 0)
    {
        if((cfg_line = (char *) malloc(MAX_LIN_LENGTH)) > 0)
        {
              if((gameVars.cfg_string = (char *) malloc(MAX_STR_LENGTH)) > 0)
              {
                    if(!LoadFileBuffer())
                       Quit("RIP mconf");  // empty broken cfg

                    while(1)               // get values till end of file or till it breaks
                    {
                         if(gameVars.char_count != 0)                   
                            ReadModStuff (); 

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
    // Set NEW Darktable
    //   
    byte nupal[256];
    
    #define RGBX(r, g, b) {(r), (g), (b), 0}

    if(filedir[0])
       snprintf(filepath, sizeof(filepath), "%s%s", filedir, darktablename);
    else
       strcpy(filepath, darktablename);
     
    int file = open(filepath, O_RDONLY | O_BINARY);

    if (file != -1)                   
    {
        read(file,nupal,sizeof(nupal)); 
        memcpy(darkt, nupal, sizeof(nupal)); 
    }
    else if (file == -1)                     // use default one
        memcpy(darkt, darktable, 256); 
    
    close(file);
}
