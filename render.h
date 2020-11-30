#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "player.h"
#include "mon.h"

namespace TK {
    void drawHUD();

    void drawPlayer(Player &p);

    void drawMonster(Monster &p);

    void drawBoss(Boss &p);
}

#endif
