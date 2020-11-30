#ifndef PLAYER_H
#define PLAYER_H

#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace TK {
    const double Pi = 3.1415926535897932384626;

    struct Bullet {
        Bullet(double x = 0, double y = 0, double v = 0, double phi = 0);

        void updatePosition();
        bool out();

        double x, y, v;
        double phi;

        SDL_Texture *tex;
    };

    struct Weapon {
        Weapon();

        int delay;      // 连按射击的后摇（时间间隔，ms）
        // 实现连击计时：射击一次之后，将swing状态置为1，并设置Timer。让Timer在delay结束后将swing置为0。swing期间不能发射子弹
        int damage;     // 单个子弹的射击伤害
        double theta;      // 半张角

        bool swing;
    };

    void setSwing(Weapon &p);
    Uint32 cancelSwing(Uint32 interval, void *weapon);

    struct Player {
        Player();

        double getphi();

        int hp;         // health point
        int mp;         // magic point，通过特殊操作积累

        double x, y;    // 坐标，显示的时候再转换为int

        double v;       // 每帧移动的距离。帧率预计为60帧/秒
        double phi;     // 指向角，与极坐标的定义一致。phi=0指向正右，左键加，右键减
                        // dx/dt = v * cos(phi), dy/dt = -v * sin(phi)，注意y的坐标轴是向下增加

        Weapon weapon;

        SDL_Texture *tex[10];
    };
}

#endif
