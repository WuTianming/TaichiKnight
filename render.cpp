#include "render.h"

namespace TK {
    double magnify = 1.00;

    void drawHUD(SDL_Renderer *r, Player &p) {
        SDL_Rect rect = { 0, 0, 300, 20 };
        rect.w = 300;
        SDL_SetRenderDrawColor(r, 229, 229, 229, 255);
        SDL_RenderFillRect(r, &rect);
        rect.w = p.hp * 3;
        SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
        SDL_RenderFillRect(r, &rect);
        rect.w = 300;
        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
        SDL_RenderDrawRect(r, &rect);

        rect.y += rect.h + 1;
        rect.w = 120;
        SDL_SetRenderDrawColor(r, 229, 229, 229, 255);
        SDL_RenderFillRect(r, &rect);
        rect.w = p.mp * 3;      // max = 40
        SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
        SDL_RenderFillRect(r, &rect);
        rect.w = 120;
        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
        SDL_RenderDrawRect(r, &rect);
    }

    void drawPlayer(SDL_Renderer *r, Player &p) {
        SDL_Rect rect = {
            (int)(p.x - 32 * magnify), (int)(p.y - 32 * magnify),
            int(64 * magnify), int(64 * magnify) };
        SDL_RenderCopyEx(r, p.tex[2],
                NULL, &rect,
                (TK::Pi/2 - p.phi) / TK::Pi * 180, NULL, SDL_FLIP_NONE);
    }

    void drawMonster(SDL_Renderer *r, Monster &p) {
        ;
    }

    void drawBoss(SDL_Renderer *r, Boss &p){
        ;
    }

    void drawBullet(SDL_Renderer *r, Bullet &p) {
        SDL_Rect rect = { (int)(p.x-5*magnify), (int)(p.y-5*magnify), int(10*magnify), int(10*magnify) };
        SDL_RenderCopyEx(r, p.tex,
                NULL, &rect,
                (TK::Pi/2 - p.phi) / TK::Pi * 180, NULL, SDL_FLIP_NONE);

        // SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
        // SDL_RenderDrawRect(r, &rect);
    }
}

