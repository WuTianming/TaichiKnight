#ifndef RENDER_H
#define RENDER_H

#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDL2_gfxPrimitives.h"

#include "player.h"
#include "mon.h"

namespace TK {
    extern const int MWIDTH, MHEIGHT, WWIDTH, WHEIGHT;
    extern SDL_Texture *background;
    extern SDL_Texture *mainmenu[10];
    extern int bx, by;          // frame starting point

    template <class T>
    T &truncate(T &x, T Mn, T Mx)
    { if (x > Mx) x = Mx; if (x < Mn) x = Mn; return x; }

    void drawHUD(SDL_Renderer *r, Player &p);

    void drawPlayer(SDL_Renderer *r, Player &p, int drawR = 0);

    void drawMonster(SDL_Renderer *r, Monster &p);

    void drawBoss(SDL_Renderer *r, Boss &p);

    void drawBullet(SDL_Renderer *r, Bullet &p);

    void drawmBullet(SDL_Renderer *r, mBullet &p);
    
    void drawMonsterBullet(SDL_Renderer *r, Monster::Bullet &p);
}

#endif
