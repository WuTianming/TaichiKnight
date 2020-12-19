#ifndef RENDER_H
#define RENDER_H

#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "player.h"
#include "mon.h"

namespace TK {
    extern double magnify;
    extern const int MWIDTH, MHEIGHT, WWIDTH, WHEIGHT;
    extern SDL_Texture *background;
    extern int bx, by;          // frame starting point

    template <class T>
    T &truncate(T &x, T Mn, T Mx) { if (x > Mx) x = Mx; if (x < Mn) x = Mn; return x; }

    void mapMagnify(double mag);

    void drawHUD(SDL_Renderer *r, Player &p);

    void drawPlayer(SDL_Renderer *r, Player &p, int drawR = 0);

    void drawMonster(SDL_Renderer *r, Monster &p);

    void drawBoss(SDL_Renderer *r, Boss &p);

    void drawBullet(SDL_Renderer *r, Bullet &p);
}

#endif
