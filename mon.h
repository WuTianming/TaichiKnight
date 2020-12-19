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
        bool out();

        int hp;
        int mp;

        double x, y;   

        double v;       
        double phi;     


        SDL_Texture *tex;
        // 属性
    };

    struct Boss {
        Boss(double x = 0, double y = 0, double v = 0, double phi = 0, SDL_Texture *t = NULL);
        //Boss();

        double getghi();

        void updatePosition(int k);
        bool out();

        int hp=50;
        int mp;

        double x, y;   

        double v;       
        double phi;     


        SDL_Texture *tex;
        // 属性
    };
}

#endif
