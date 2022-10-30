#include "version.h"

#ifdef USE_PARALLAX

#include "wl_def.h"

#ifdef USE_FEATUREFLAGS

// The lower left tile of every map determines the start texture of the parallax sky.
static int GetParallaxStartTexture()
{
    int startTex = ffDataBottomLeft;
    assert(startTex >= 0 && startTex < PMSpriteStart);
    return startTex;
}

#else

static int GetParallaxStartTexture()
{
    int startTex;
    switch(gamestate.episode * 10 + mapon)
    {
        case  0: startTex = 20; break;
        default: startTex =  0; break;
    }
    assert(startTex >= 0 && startTex < PMSpriteStart);
    return startTex;
}

#endif

void DrawParallax(byte *vbuf, unsigned vbufPitch)
{
    int startpage = parallaxstart[gamestate.episode*10+mapon+1];
    int midangle = player->angle * (FINEANGLES / ANGLES);
    int skyheight = viewheight >> 1;
    int curtex = -1;
    byte *skytex;

    startpage += parallaxcount[gamestate.episode*10+mapon+1] - 1;

    for(int x = 0; x < viewwidth; x++)
    {
        int curang = pixelangle[x] + midangle;
        if(curang < 0) curang += FINEANGLES;
        else if(curang >= FINEANGLES) curang -= FINEANGLES;
        int xtex = curang * parallaxcount[gamestate.episode*10+mapon+1] * texturesize / FINEANGLES;
        int newtex = xtex >> textureshift;
        if(newtex != curtex)
        {
            curtex = newtex;
            skytex = PM_GetTexture(startpage - curtex);
        }
        int texoffs = texmask - ((xtex & (texturesize - 1)) << textureshift);
        int yend = skyheight - (wallheight[x] >> 3);
        if(yend <= 0) continue;

        for(int y = 0, offs = x; y < yend; y++, offs += vbufPitch)
            vbuf[offs] = skytex[texoffs + (y * texturesize) / skyheight];
    }
}

#endif
