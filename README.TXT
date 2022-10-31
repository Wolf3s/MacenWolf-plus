Wolf4SDL by Moritz "Ripper" Kroll (http://www.chaos-software.de.vu)
Original Wolfenstein 3D by id Software (http://www.idsoftware.com)
Modifications to r262 by Andy_Nonymous and others
    (http://diehardwolfers.areyep.com/viewtopic.php?t=6693)
=============================================================================

MacenWolf+ is an continuation of the MacenWolf that adds some functions/fixes that the original one doesn´t have.
This source port is based on the MacenWolf v1.10 and origianaly Wolf4SDL 1.9R

The main features are:
--------------
 - Cross-platform:
      Supported operating systems are at least:
       - Windows 98, Windows ME, Windows 2000, Windows XP, Windows Vista
         (32 and 64 bit), Windows 7 (32 and 64 bit)
       - Linux
       - BSD variants
       - Mac OS X (x86)
       - KallistiOS (used for Dreamcast)
      Only little endian platforms like x86, ARM and SH-4 are supported, yet.

 - AdLib sounds and music:
      This port includes the OPL2 emulator from MAME, so you can not only
      hear the AdLib sounds but also music without any AdLib-compatible
      soundcard in near to perfect quality!

 - Multichannel digitized sounds:
      Digitized sounds play on 8 channels! So in a fire fight you will
      always hear, when a guard opens the door behind you ;)

 - Higher screen resolutions:
      Aside from the original 320x200 resolution, MacenWolf+ currently
      supports any resolutions being multiples of 320x200 or 320x240,
      the default being 640x400.
      Unlike some other ports, MacenWolf+ does NOT apply any bilinear
      or similar filtering, so the graphics are NOT blurred but
      pixelated just as we love it.

 - Fully playable with only a game controller:
      MacenWolf+ can be played completely without a keyboard. At least two
      buttons are required (shoot/YES and open door/NO), but five or more
      are recommended (run, strafe, ESC).

Additional features:
--------------------

 - Two additional view sizes:
      MacenWolf+ supports one view size using the full width of the screen
      and showing the status bar, like in Mac-enstein, and one view size
      filling the whole screen (press TAB to see the status bar).

 - (Nearly) unlimited sound and song lengths:
      Mod developers are not restricted to 64kB for digitized sounds and
      IMF songs anymore, so longer songs and digitized sounds with better
      quality are possible.

 - Resuming ingame music:
      When you come back to the game from the menu or load a save game, the
      music will be resumed where it was suspended rather than started from
      the beginning.

 - Freely movable pushwalls:
      Moving pushwalls can be viewed from all sides, allowing mod developers
      to place them with fewer restrictions. The player can also follow the
      pushwall directly instead of having to wait until the pushwall has left
      a whole tile.

 - Optional integrated features for mod developers:
      MacenWolf+ already contains the shading, directional 3D sprites,
      floor and ceiling textures, high resolution textures/sprites,
      parallax sky, cloud sky and outside atmosphere features, which
      can be easily activated in version.h.


The following versions of MacenWolf+ data files are currently supported
by the source code (choose the version by commenting/uncommenting lines in
version.h as described in that file):

- MacenWolf Second Encounter
- MacenWolf Third Encounter


How to play:
------------

To play MacenWolf mod with MacenWolf+, you just have to copy the original data
files (e.g. *.WL6) into the same directory as the MacenWolf+ executable.
Please make sure, that you use the correct version of the executable with the
according data files version as the differences are hardcoded into the binary!

On Windows SDL.dll and SDL_mixer.dll must also be copied into this directory.
They are also available at:
SDL.dll
https://github.com/wolfysdl/SDL-archive/blob/main/SDL-1.2.15-win32.zip

SDL_mixer
https://github.com/wolfysdl/SDL_mixer-archive/blob/main/SDL_mixer-1.2.12-win32.zip

If you play in windowed mode (--windowed parameter), press SCROLLLOCK or F12
to grab the mouse. Press it again to release the mouse.


Usage:
------

MacenWolf+ supports the following command line options:
 --help                 This help page
 --tedlevel <level>     Starts the game in the given level
 --baby                 Sets the difficulty to baby for tedlevel
 --easy                 Sets the difficulty to easy for tedlevel
 --normal               Sets the difficulty to normal for tedlevel
 --hard                 Sets the difficulty to hard for tedlevel
 --nowait               Skips intro screens
 --windowed[-mouse]     Starts the game in a window [and grabs mouse]
 --res <width> <height> Sets the screen resolution
                        (must be multiple of 320x200 or 320x240)
 --resf <w> <h>         Sets any screen resolution >= 320x200
                        (which may result in graphic errors)
 --bits <b>             Sets the screen color depth
                        (Use this when you have palette/fading problem
                        or perhaps to optimize speed on old systems.
                        Allowed: 8, 16, 24, 32, default: "best" depth)
 --nodblbuf             Don't use SDL's double buffering
 --extravbls <vbls>     Sets a delay after each frame, which may help to
                        reduce flickering (SDL does not support vsync...)
                        (unit is currently 8 ms, default: 0)
 --joystick <index>     Use the index-th joystick if available
 --joystickhat <index>  Enables movement with the given coolie hat
 --samplerate <rate>    Sets the sound sample rate (given in Hz)
 --audiobuffer <size>   Sets the size of the audio buffer (-> sound latency)
                        (given in bytes)
 --ignorenumchunks      Ignores the number of chunks in VGAHEAD.*
                        (may be useful for some broken mods)
 --configdir <dir>      Directory where config file and save games are stored
                        (Windows default: current directory,
                        others: $HOME/.wolf4sdl)

For Spear of Destiny the following additional options are available:
 --mission <mission>    Mission number to play (1-3)
 --goodtimes            Disable copy protection quiz


Compiling from source code:
---------------------------

The current version of the source code is available in the svn repository at:
   https://github.com/wolfysdl/MacenWolf-plus/tree/main

The following ways of compiling the source code are supported:
 - Makefile (for Linux, BSD variants and MinGW/MSYS)
 - Visual C++ 2008 (MacenWolf.VC9.sln and MacenWolf.VC9.vcproj)
 - Visual C++ 2005 (MacenWolf.sln and MacenWolf+.vcproj)
 - Visual C++ 6 (MacenWolf.dsw and MacenWolf.dsp)
 - Code::Blocks 8.02 (MacenWolf.cbp)
 - Dev-C++ 5.0 Beta 9.2 (4.9.9.2) (MacenWolf.dev) (see README-devcpp.txt)
 - Xcode (for Mac OS X, macosx/MacenWolf.xcodeproj/project.pbxproj)
 - Special compiling for Dreamcast (see README-dc.txt)
 - Special compiling for GP2X (see README-GP2X.txt)

To compile the source code you need the development libraries of
 - SDL (https://github.com/wolfysdl/SDL-Archive/blob/main/SDL-devel-1.2.15-VC.zip) and
 - SDL_mixer (https://github.com/wolfysdl/SDL_mixer-archive/blob/main/SDL_mixer-devel-1.2.12-VC.zip)
and have to adjust the include and library paths in the projects accordingly.

Please note, that there is no official SDL_mixer development pack for MinGW,
yet, but you can get the needed files from a Dev-C++ package here:
http://sourceforge.net/project/showfiles.php?group_id=94270&package_id=151751
Just rename the file extension from ".devpack" to ".tar.bz2" and unpack it
with for example WinRAR. Then add the directories include/SDL and lib to the
according search paths in your project.

IMPORTANT: Do not forget to take care of version.h!
   By default it compiles for "Wolfenstein 3D v1.4 full GT/ID/Activision"!

TODOs:
------
 - Exact Pac-man ghosts behavior in case something is off
 - More accurate victory/gameover sprite scaling
 - Fixes: adjust statusbar key coords (e.g. 1280x600), weapon sprite size, fix viewsize<19 for various video resolutions
 - Fix Mac-style projection view (Distorted walls when too close. Maybe better fineangles, tangent?)
 - Fix whatever needs fixing
 - Fix anything that doesn't make sense
 - Delete all and just port it to Doom like every other game
 - Move it to SDL2 however keep the SDL1 for more compatibilty 
 - Add aspect ratio options
 - Move the config.wl6 to the main folder and have a highscore config for mod folders
 - Simplify the file systems (igagraph, vgagraph, single image files, etc.) or load mac files directly (lzss compressed)
 - Frontend menu loader for mods because a batch file (command line) is too hardcore for some people
 - Add a mouse cursor to menu
 - Add more script features for mods
 - Port more bugs and glitches from the original just for fun or for mods that require them
 - Port it to other systems

Known bugs:
-----------

 - None! ;D


Troubleshooting:
----------------

 - If your frame rate is low, consider using the original screen resolution
   (--res 320 200) or lowering the sound quality (--samplerate 22050)


Credits:
--------
 - Special thanks to id Software! Without the source code we would still have
   to pelt Wolfenstein 3D with hex editors and disassemblers ;D
 - Special thanks to MacPlay Team Wolfenstein 3D - Mac version
 - Special thanks for the Ripper for Port Wolfenstein 3D source code to SDL.
 - Special thanks to the DOSBox team for providing a GPL'ed OPL2/3 emulator!
 - Special thanks to the MAME developer team for providing the source code
   of the OPL2 emulator!
 - Many Special Thanks to DHW Community for the Various Wolf3D and Wolf4SDL contributions over the years
 - Many thanks to "Der Tron" for hosting the svn repository, making Wolf4SDL
   FreeBSD compatible, testing, bugfixing and cleaning up the code!
 - Thanks to Chris Chokan for his improvements on Wolf4GW (base of Wolf4SDL)
 - Thanks to Pickle for the GP2X support and help on 320x240 support
 - Thanks to fackue for the Dreamcast support
 - Thanks to Chris Ballinger for the Mac OS X support
 - Thanks to Xilinx, Inc. for providing a list of maximum-length LFSR counters
   used for higher resolutions of fizzle fade
 - Thanks to Nexion for The MacenWolf engine assembly and additional features
 - Thanks to serpens for Heading MacenWolf, analyzing and investigating the Mac version
 - Thanks to Chris to for supporting Automap
 - Thanks to AlumiuN for the Sound manager
 - Thanks to Andy Nonymous for the Mac-enstein basics
 - Thanks to You and For anyone that is missing
 
Licenses:
---------
 - The original source code of Wolfenstein 3D:
     At your choice:
     - license-id.txt or
     - license-gpl.txt
 - The OPL2 emulator:
     At your choice:
     - license-mame.txt (fmopl.cpp)
     - license-gpl.txt (dbopl.cpp, USE_GPL define in version.h or set GPL=1 for Makefile)
