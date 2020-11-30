#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "player.h"
#include "mon.h"

namespace TK {
    void drawHUD();

    void drawPlayer(SDL_Renderer *r, Player &p);

    void drawMonster(SDL_Renderer *r, Monster &p);

    void drawBoss(SDL_Renderer *r, Boss &p);

    void drawBullet(SDL_Renderer *r, Bullet &p);
}

#endif
