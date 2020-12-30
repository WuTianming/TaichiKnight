#include "mon.h"
#include "render.h"

// monster
namespace TK {
    //Monster::Monster() : hp(100), mp(0), x(320), y(240), phi(Pi/2), v(2.5) {}
     Monster::Monster(double x, double y, double v, double phi, SDL_Texture *t)
        : x(x), y(y), v(v), phi(phi), tex(t) {}
    void Monster::updatePosition(int k) {
        if(k==0){
            x += v * cos(phi); y += v * sin(phi);
        }
        if(k==1){
            x += v * cos(phi); y -= v * sin(phi);
        }
        if(k==2){
            x -= v * cos(phi); y -= v * sin(phi);
        }
        if(k==3){
            x -= v * cos(phi); y += v * sin(phi);
        }
        if(k==4){
            phi += TK::Pi / 3;
        }
        if(k==5){
            phi -= TK::Pi / 3;
        }
        if(k==6){
            x += v ;
        }
        if(k==7){
            x -= v ;
        }
        if(k==8){
            y += v ;
        }
        if(k==9){
            y -= v ;
        }
    }
}

// boss
namespace TK {
     Boss::Boss(double x, double y, double v, double phi, SDL_Texture *t, SDL_Texture *t2)
        : x(x), y(y), v(v), phi(phi), tex(t), t1(t2) {}
    void Boss::updatePosition(int k) {
        if(k==0){
            x += v * cos(phi); y += v * sin(phi);
        }
        if(k==1){
            x += v * cos(phi); y -= v * sin(phi);
        }
        if(k==2){
            x -= v * cos(phi); y -= v * sin(phi);
        }
        if(k==3){
            x -= v * cos(phi); y += v * sin(phi);
        }
        if(k==4){
            phi += TK::Pi / 3;
        }
        if(k==5){
            phi -= TK::Pi / 3;
        }
        if(k==6){
            x += v ;
        }
        if(k==7){
            x -= v ;
        }
        if(k==8){
            y += v ;
        }
        if(k==9){
            y -= v ;
        }
    }

    bool Boss::out() { return (x < -20 || x > 20+TK::MWIDTH || y < -20 || y > 20+TK::MHEIGHT); }
    bool Boss::fout() { return (x - bx < 0 || x - bx > TK::WWIDTH || y - by < 0 || y - by > TK::WHEIGHT); }
}


