#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace TK {
    const double Pi = 3.1415926535897932384626;

    struct Bullet {
        Bullet();

        double x, y, v;
        double phi;

        SDL_Texture *tex;
    };

    struct Weapon {
        int delay;      // 连按射击的后摇（时间间隔，ms）
        // 实现连击计时：射击一次之后，将swing状态置为1，并设置Timer。让Timer在delay结束后将swing置为0。swing期间不能发射子弹
        int damage;     // 单个子弹的射击伤害
        int theta;      // 半张角
    };

    struct Player {
        Player();

        int hp;         // health point
        int mp;         // magic point，通过特殊操作积累

        double x, y;    // 坐标，显示的时候再转换为int

        double v;       // 每帧移动的距离。帧率预计为60帧/秒
        double phi;     // 指向角，phi=0指向正右，左键减，右键加
                        // dx/dt = v * cos(phi), dy/dt = v * sin(phi)

        SDL_Texture *tex[10];
    };
}

#endif
