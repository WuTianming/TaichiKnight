#include "render.h"

namespace TK {
    void drawPlayer(SDL_Renderer *r, Player &p) {
        SDL_Rect rect = { (int)p.x - 32, (int)p.y - 32, 64, 64 };
        SDL_RenderCopyEx(r, p.tex[2],
                NULL, &rect,
                (TK::Pi/2 - p.phi) / TK::Pi * 180, NULL, SDL_FLIP_NONE);
    }

    void drawMonster(SDL_Renderer *r, Monster &p) {
    }

    void drawBoss(SDL_Renderer *r, Boss &p){
    }

    void drawBullet(SDL_Renderer *r, Bullet &p) {
        SDL_Rect rect = { (int)p.x-5, (int)p.y-5, 10, 10 };
        SDL_RenderCopyEx(r, p.tex,
                NULL, &rect,
                (TK::Pi/2 - p.phi) / TK::Pi * 180, NULL, SDL_FLIP_NONE);

        // SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
        // SDL_RenderDrawRect(r, &rect);
    }
}

