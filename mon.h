#ifndef MON_H
#define MON_H

#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "player.h"

namespace TK {

    struct Monster {
        Monster(double x = 0, double y = 0, double v = 0, double phi = 0, SDL_Texture *t = NULL);
        //Monster();

        double getghi();

        void updatePosition(int k);

        int hp;
        int mp;

        double x, y;   

        double v;       
        double phi;     

        int fm=0;
        int km=0;

        SDL_Texture *tex;
        SDL_Texture *btex;

        struct Bullet {
        Bullet(double x = 0, double y = 0, double v = 0, double phi = 0, SDL_Texture *t = NULL);
        void updatePosition();
        bool out();

        double x, y, v;
        double phi;

        SDL_Texture *tex;
        };

        vector<TK::Monster::Bullet> bullets;
        // 属性
    };

    struct Boss {
        Boss(double x = 0, double y = 0, double v = 0, double phi = 0, SDL_Texture *t = NULL, SDL_Texture *t2 = NULL);
        //Boss();

        double getghi();

        void updatePosition(int k);
        bool out();
        bool fout();

        int hp=300;
        int mp;

        double x, y;   

        double v;       
        double phi;     


        SDL_Texture *tex, *t1;
        // 属性
    };
}

#endif
