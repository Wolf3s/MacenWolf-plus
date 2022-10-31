/*
VODKA-INDUCED ENTERTAINMENT ADVANCED SOUND MANAGER v0.9.1
COMPATIBILITY LAYER FOR WOLF4SDL v1.6 - HEADER FILE
BY GERARD 'ALUMIUN' WATSON
*/

#ifndef ID_VIEASM_H
#define ID_VIEASM_H

#include <SDL_mixer.h> // Duh.
#include <string.h>    // For memset
#include <stdio.h>     // For printf
#include "WL_DEF.H"

//#define VERBOSE        // Verbose mode?

/* defines for management */

#define ASM_ABSMAXCHANNELS 256              // Absolute maximum amount for maxchan in ASM_Open

#define NUMAMBIENTS     ASM_ABSMAXCHANNELS  // Number of ambient sound slots

#define TickBase        60      // 60Hz per tick - used as a base for timer 0

#define GetTimeCount()  ((SDL_GetTicks()*TickBase)/1000)

/* #defined functions */

// ASM_Start
// Calls ASM_Open with parameters and good defaults
// Has a max channel number of half of ASM_ABSMAXCHANNELS (128 by default)

#define ASM_Start(vol_sound, vol_music) ASM_Open(22050, 16, ASM_ABSMAXCHANNELS/2, 2048, vol_sound, vol_music)

// ASM_HaltMusic
// Ends music and frees it

#define ASM_HaltMusic() Mix_HaltMusic()

// ASM_PauseSound
// Pauses all sound channels

#define ASM_PauseSound() Mix_Pause(-1)

// ASM_ResumeSound
// Resumes all sound channels

#define ASM_ResumeSound() Mix_Resume(-1)

// ASM_PauseMusic
// Pauses music

#define ASM_PauseMusic() Mix_PauseMusic()

// ASM_ResumeMusic
// Resumes music

#define ASM_ResumeMusic() Mix_ResumeMusic()

// ASM_PlayDirect
// Plays a sound directly at player location

#define ASM_PlayDirect(sample, ambient) ASM_PlaySound(sample, 0, 0, ambient)

#define SD_SoundPlaying() 0
#define UNCACHEAUDIOCHUNK(chunk) ASM_Uncache(ASM_Audiosegs[chunk]);
#define SD_WaitSoundDone()
#define SD_PrepareSound(which) int nullps = which
#define SD_AbortIfMusOff if (!SD_MusIsOn()) return
#define SD_AbortIfSndOff if (!SD_SndIsOn()) return
#define SD_StopDigitized() SD_StopSound()
#define SD_ContinueMusic(chunk, startoffs) SD_StartMusic(chunk)
#define SD_ChangeVolume(snd, mus) ASM_ChangeVolume(snd, mus)
#define SD_GetVolume(snd, mus) ASM_ReturnVolume(snd, mus)
#define SD_Reverse(reverse) ASM_ReverseStereo(reverse)


typedef struct
{
    Mix_Chunk* chunk;
    char* name;
} sample;

// These enums are now treated differently. Because there are no seperate devices,
// any value other than sxm_Off will turn the device on, and SDSMode is no longer used.

typedef enum
{
    sdm_Off, sdm_AdLib
} SDMode;

typedef enum
{
    smm_Off, smm_AdLib
} SMMode;

typedef struct
{
    int valid;
    fixed globalsoundx, globalsoundy;
} globalsoundpos;

extern globalsoundpos channelSoundPos[];

extern  SDMode          SoundMode;
extern  SMMode          MusicMode;

extern bool switching;

extern sample ASM_Audiosegs[NUMSOUNDS];



// ASM_AbortIfClosed
// Checks if the device is open, returns if it isn't
// Cunning lack of semi-colon means return values can be used appropriately :)

#ifdef VERBOSE
#define ASM_AbortIfClosed if(!ASM_IsOpen()) printf("Not open.\n"); if(!ASM_IsOpen()) return
#else
#define ASM_AbortIfClosed if(!ASM_IsOpen()) return
#endif

// ASM_Step
// Check if the music switching needs to be checked

#define ASM_Step() ASM_AbortIfClosed; if (switching) ASM_SwitchStep()

/* Other, 'real' functions */

extern bool ASM_Open(int frequency, int channels, int maxchan, int buffersize, Uint8 sndvolume, Uint8 musvolume, bool reverse);
extern bool ASM_IsOpen(void);
extern bool ASM_PlayMusic(char* musfile);
extern bool ASM_SwitchMus(char* loadmus, int fadems, bool fade);
extern bool ASM_FadeInMus(char* loadmus, int fadems);
extern sample ASM_Cache(char* sndfile, const char* name);
extern sample ASM_CacheFromMem(void* ptr, int size, const char* name);
extern int ASM_PlaySound(sample& sound, int angle, Uint8 distance, bool ambient);
extern void ASM_Uncache(sample& chunk);
extern void ASM_FadeOutMus(int fadems);
extern void ASM_HaltSound(void);
extern void ASM_Pause(void);
extern void ASM_Resume(void);
extern void ASM_Halt(void);
extern void ASM_ChangeVolume(Uint8 sndvolume, Uint8 musvolume);
extern void ASM_ReturnVolume(Uint8* retsnd, Uint8* retmus);
extern void ASM_Close(void);
extern void ASM_StopChannel(int channel);
extern void ASM_SwitchStep(void);
extern void ASM_ReverseStereo(bool reverse);
extern void ASM_ChannelDone(int channel);
extern int ASM_CurChannels(void);



inline void Delay(int wolfticks)
{
    if (wolfticks > 0)
        SDL_Delay(wolfticks * 1000 / TickBase);
}

// Functions
extern  void    SD_Startup(void),                                           // DONE
SD_Shutdown(void);                                          // DONE

extern  void    SD_PositionSound(int leftvol, int rightvol);                // DONE, basically the same
extern  byte    SD_PlaySound(soundnames sound);                             // DONE, just calls SD_PlayDigitized
extern  void    SD_SetPosition(int channel, int leftpos, int rightpos);     // DONE, basically the same
extern  void    SD_StopSound(void);                                         // DONE, just calls ASM_HaltSound()

extern  void    SD_StartMusic(int chunk);                                   // DONE
extern  void    SD_ContinueMusic(int chunk, int startoffs);                 // DONE, but always starts at beginning (Sorry WSJ :p)
extern  void    SD_MusicOn(void),                                           // DONE, just resumes
SD_FadeOutMusic(int fade);                                  // DONE
extern  int     SD_MusicOff(void);                                          // DONE, just pauses

extern  bool    SD_MusicPlaying(void);                                      // DONE
extern  bool    SD_SetSoundMode(SDMode mode);                               // DONE
extern  bool    SD_SetMusicMode(SMMode mode);                               // DONE

extern  int     SD_PlayDigitized(word which, int leftpos, int rightpos, bool amb);   // DONE
extern  void    CA_LoadAllSounds(void);

// Ambient sound prototypes

extern  int     SD_AmbientSound(void);
extern  void    SD_StopAmbient(int ambindex);

#endif  // ID_VIEASM_H
