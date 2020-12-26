#include "render.h"

namespace TK {
    const int MWIDTH = 2300 / 2, MHEIGHT = 1475 / 2, WWIDTH = 640, WHEIGHT = 480;
    SDL_Texture *background;
    SDL_Texture *mainmenu[10];
    int bx, by;

    void drawHUD(SDL_Renderer *r, Player &p) {
        // hp
        SDL_Rect rect = { 0, 0, 300, 20 };
        rect.w = 300;
        SDL_SetRenderDrawColor(r, 229, 229, 229, 255);      // gray
        SDL_RenderFillRect(r, &rect);
        rect.w = p.hp * 3;
        SDL_SetRenderDrawColor(r, 255, 0, 0, 255);          // red for hp
        SDL_RenderFillRect(r, &rect);
        rect.w = 300;
        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);            // black frame
        SDL_RenderDrawRect(r, &rect);

        // mp
        rect.x = WWIDTH - 20;
        rect.w = 20, rect.h = 125;
        SDL_SetRenderDrawColor(r, 229, 229, 229, 255);
        SDL_RenderFillRect(r, &rect);
        rect.y = 125 - p.mp / 4;      // max = 500
        rect.h = p.mp / 4;
        if (p.mp < 500)
            SDL_SetRenderDrawColor(r, 0, 0, 255, 255);
        else
            SDL_SetRenderDrawColor(r, 0xff, 0x8a, 0x0c, 0xff);
        SDL_RenderFillRect(r, &rect);
        rect.h = 125, rect.y = 0;
        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
        SDL_RenderDrawRect(r, &rect);

        // buff
        rect.w = 80, rect.h = 45;
        for (int i = 0; i < p.buffs.size(); ++i) {
            rect.x = WWIDTH - 20 - 80;
            rect.y = i * 45;
            SDL_RenderCopy(r, p.tex[p.buffs[i].texID], NULL, &rect);
        }
    }

    double __animation1(double t, double b, double c) {
        // t: Time process in (0,1)
        // b: Begin value
        // c: total Change in value
        // d: total motion Duration
        double ts = t * t; double tc = ts * t;
        return b+c*(2*ts*ts + 2*tc + (-3)*ts);
    }

    double __animation2(double t, double b, double c) {
        double ts = t * t; double tc = ts * t;
        return b+c*(-18.0975*tc*ts + 29.3975*ts*ts + -7.3*tc + -3*ts);
    }

    void drawPlayer(SDL_Renderer *r, Player &p, int drawR) {
        bx = p.x - TK::WWIDTH / 2, by = p.y - TK::WHEIGHT / 2;
        truncate(bx, 0, TK::MWIDTH - TK::WWIDTH);
        truncate(by, 0, TK::MHEIGHT - TK::WHEIGHT);
        SDL_Rect srcrect = { bx, by, TK::WWIDTH, TK::WHEIGHT };
        SDL_RenderCopy(r, background, &srcrect, NULL);
        SDL_Rect prect = { (int)p.x - 32 - bx, (int)p.y - 32 - by, 64, 64 };
        SDL_RenderCopyEx(r, p.tex[2],
                NULL, &prect,
                (TK::Pi/2 - p.phi) / TK::Pi * 180, NULL, SDL_FLIP_NONE);

        if (drawR != -1) {
            // drawr = judging 从 150 开始，每帧减少 2
            double ksi;
            double radius;
            Uint32 color;
            if (drawR > 0) {
                ksi = drawR / 150.00;
                radius = __animation2(1.00 - ksi, 100, -85);
                //               AABBGGRR
                color = 0x4f898aff
                    + ((int)((1-ksi)*0xaf) << 24)       // alpha
                    + ((int)((1-ksi)*0x83) << 16);      // blue
                // will overflow to create orange
                // end color = 0xff0c8aff, orange
            } else {
                ksi = -drawR / 40.00;
                radius = 15;
                //               AABBGGRR
                color = 0xff0c8aff
                    - ((int)(ksi*0xaf) << 24)           // alpha
                    - ((int)(ksi*0x78) <<  0)           // red
                    - ((int)(ksi*0x03) <<  8)           // green
                    + ((int)(ksi*0x7b) << 16);          // blue
                // end color = 0xff878787, gray
            }

            circleColor(r, p.x - bx, p.y - by, radius + 1.5, color);
            circleColor(r, p.x - bx, p.y - by, radius + 0.5, color);
            circleColor(r, p.x - bx, p.y - by, radius - 0.5, color);
        }
    }

    void drawMonster(SDL_Renderer *r, Monster &p) {
        SDL_Rect rect = {
            (int)(p.x - 32 - bx), (int)(p.y - 32 - by), int(64), int(64) };
        SDL_RenderCopy(r, p.tex, NULL, &rect);
    }

    void drawBoss(SDL_Renderer *r, Boss &p){
        SDL_Rect rect = {
            (int)(p.x - 32 - bx), (int)(p.y - 32 - by), int(64), int(64) };
        SDL_RenderCopy(r, p.tex, NULL, &rect);
    }

    void drawBullet(SDL_Renderer *r, Bullet &p) {
        SDL_Rect rect = { (int)(p.x-7-bx), (int)(p.y-7-by), 14, 14 };
        SDL_RenderCopyEx(r, p.tex,
                NULL, &rect,
                (TK::Pi/2 - p.phi) / TK::Pi * 180, NULL, SDL_FLIP_NONE);

    }

    void drawmBullet(SDL_Renderer *r, mBullet &p) {
        SDL_Rect rect = { (int)(p.x-12-bx), (int)(p.y-12-by), 24, 24 };
        SDL_RenderCopyEx(r, p.tex,
                NULL, &rect,
                (TK::Pi/2 - p.phi) / TK::Pi * 180, NULL, SDL_FLIP_NONE);

    }

    void drawMonsterBullet(SDL_Renderer *r, Monster::Bullet &p) {
        SDL_Rect rect = { (int)(p.x-50-bx), (int)(p.y-50-by), 100, 100 };
        SDL_RenderCopyEx(r, p.tex,
                NULL, &rect,
                (TK::Pi/2 - p.phi) / TK::Pi * 180, NULL, SDL_FLIP_NONE);

    }
}

