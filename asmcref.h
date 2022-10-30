/*
VODKA-INDUCED ENTERTAINMENT ADVANCED SOUND MANAGER v0.9
COMPATIBILITY LAYER FOR WOLF4SDL v1.6 - SOUND NAMES REFERENCE FILE
BY GERARD 'ALUMIUN' WATSON
*/

#ifndef _ASMCREF_H_
#define _ASMCREF_H_

// This table holds the file names for the sounds defined in AUDIOWL6.H.
// Change these to suit your sounds and your AUDIOHED.WL6 or SOD file.
// Currently these are set up for Wolfenstein, not SoD.
const char *ASM_Sounddir = "sounds\\";

const char *ASM_Soundnames[NUMSOUNDS] = {
	"HITWALLSND.wav",              // 0
    "SELECTWPNSND.wav",            // 1
    "SELECTITEMSND.wav",           // 2
    "HEARTBEATSND.wav",            // 3
    "MOVEGUN2SND.wav",             // 4
    "MOVEGUN1SND.wav",             // 5
    "NOWAYSND.wav",                // 6
    "NAZIHITPLAYERSND.wav",        // 7
    "SCHABBSTHROWSND.wav",         // 8
    "PLAYERDEATHSND.wav",          // 9
    "DOGDEATHSND.wav",             // 10
    "ATKGATLINGSND.wav",           // 11
    "GETKEYSND.wav",               // 12
    "NOITEMSND.wav",               // 13
    "WALK1SND.wav",                // 14
    "WALK2SND.wav",                // 15
    "TAKEDAMAGESND.wav",           // 16
    "GAMEOVERSND.wav",             // 17
    "OPENDOORSND.wav",             // 18
    "CLOSEDOORSND.wav",            // 19
    "DONOTHINGSND.wav",            // 20
    "HALTSND.wav",                 // 21
    "DEATHSCREAM2SND.wav",         // 22
    "ATKKNIFESND.wav",             // 23
    "ATKPISTOLSND.wav",            // 24
    "DEATHSCREAM3SND.wav",         // 25
    "ATKMACHINEGUNSND.wav",        // 26
    "HITENEMYSND.wav",             // 27
    "SHOOTDOORSND.wav",            // 28
    "DEATHSCREAM1SND.wav",         // 29
    "GETMACHINESND.wav",           // 30
    "GETAMMOSND.wav",              // 31
    "SHOOTSND.wav",                // 32
    "HEALTH1SND.wav",              // 33
    "HEALTH2SND.wav",              // 34
    "BONUS1SND.wav",               // 35
    "BONUS2SND.wav",               // 36
    "BONUS3SND.wav",               // 37
    "GETGATLINGSND.wav",           // 38
    "ESCPRESSEDSND.wav",           // 39
    "LEVELDONESND.wav",            // 40
    "DOGBARKSND.wav",              // 41
    "ENDBONUS1SND.wav",            // 42
    "ENDBONUS2SND.wav",            // 43
    "BONUS1UPSND.wav",             // 44
    "BONUS4SND.wav",               // 45
    "PUSHWALLSND.wav",             // 46
    "NOBONUSSND.wav",              // 47
    "PERCENT100SND.wav",           // 48
    "BOSSACTIVESND.wav",           // 49
    "MUTTISND.wav",                // 50
    "SCHUTZADSND.wav",             // 51
    "AHHHGSND.wav",                // 52
    "DIESND.wav",                  // 53
    "EVASND.wav",                  // 54
    "GUTENTAGSND.wav",             // 55
    "LEBENSND.wav",                // 56
    "SCHEISTSND.wav",              // 57
    "NAZIFIRESND.wav",             // 58
    "BOSSFIRESND.wav",             // 59
    "SSFIRESND.wav",               // 60
    "SLURPIESND.wav",              // 61
    "TOT_HUNDSND.wav",             // 62
    "MEINGOTTSND.wav",             // 63
    "SCHABBSHASND.wav",            // 64
    "HITLERHASND.wav",             // 65
    "SPIONSND.wav",                // 66
    "NEINSOVASSND.wav",            // 67
    "DOGATTACKSND.wav",            // 68
    "FLAMETHROWERSND.wav",         // 69
    "MECHSTEPSND.wav",             // 70
    "PUSHSTOPSND.wav",             // 71
    "YEAHSND.wav",                 // 72
#ifndef APOGEE_1_0
    "DEATHSCREAM4SND.wav",         // 73
    "DEATHSCREAM5SND.wav",         // 74
    "DEATHSCREAM6SND.wav",         // 75
    "DEATHSCREAM7SND.wav",         // 76
    "DEATHSCREAM8SND.wav",         // 77
    "DEATHSCREAM9SND.wav",         // 78
    "DONNERSND.wav",               // 79
    "EINESND.wav",                 // 80
    "ERLAUBENSND.wav",             // 81
    "KEINSND.wav",                 // 82
    "MEINSND.wav",                 // 83
    "ROSESND.wav",                 // 84
    "MISSILEFIRESND.wav",          // 85
    "MISSILEHITSND.wav"            // 86
#endif
};

// This table holds the file names for the music defined in AUDIOWL6.H.
// Change these to suit your music and your AUDIOHED.WL6 or SOD file.
// Currently these are set up for Wolfenstein, not SoD.
// Make sure you put the right format on the end here
// Compatible extentions:
//  -- WAV (If you're using WAVs for music, you're mad.)
//  -- OGG
//  -- MP3
//  -- MID
//  -- MOD, XM, IT, 669, S3M, MED
const char *ASM_Musicdir = "music\\";

const char *ASM_Musicnames[NUMMUSICS] = {
	"Title.ogg",               // 0 - intro
	"Original.ogg",            // 1
	"Grunge.ogg",              // 2
	"Plodding.ogg",            // 3
	"Rocked.ogg",              // 4
	"Unleashed.ogg",           // 5
	"Between.ogg",             // 6
	"Doom.ogg",                // 7 - intermission
	"track08.ogg",             // 8
	"track09.ogg",             // 9
	"track10.ogg",             // 10
	"track11.ogg",             // 11
	"track12.ogg",             // 12
	"track13.ogg",             // 13
	"track14.ogg",             // 14
	"track15.ogg",             // 15
	"track17.ogg",             // 16
	"track19.ogg",             // 17
	"track20.ogg",             // 18
	"track21.ogg",             // 19
	"track22.ogg",             // 20
	"track13.ogg",             // 21
	"track24.ogg",             // 22
	"track25.ogg",             // 23
	"track26.ogg",             // 24
	"track27.ogg",             // 25
	"track28.ogg"              // 26
};

#endif  // _ASMCREF_H_
