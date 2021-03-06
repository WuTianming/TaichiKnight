#include "player.h"
#include "render.h"

// Player
namespace TK {
    Player::Player() : hp(100), mp(0), x(MWIDTH / 2), y(MHEIGHT / 2), phi(Pi/2), v(2.5) {}

    double Player::getphi() {
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        int dx = mx - (x - bx), dy = (y - by) - my;
        double dl = sqrt(dx * dx + dy * dy);
        if (dy >= 0) return acos(dx / dl);
        else return -acos(dx / dl);
    }
}

// Bullet
namespace TK {
    Bullet::Bullet(double x, double y, double v, double phi, int damage, SDL_Texture *t)
        : x(x), y(y), v(v), phi(phi), damage(damage), tex(t) {}

    void Bullet::updatePosition() { x += v * cos(phi); y -= v * sin(phi); }

    bool Bullet::out() { return (x < -20 || x > MWIDTH + 20 || y < -20 || y > MHEIGHT + 20); }
}
//mBullet
namespace TK {
    mBullet::mBullet(double x, double y, double v, double phi, SDL_Texture *t)
        : x(x), y(y), v(v), phi(phi), tex(t) {}

    void mBullet::updatePosition() {
        x += v * cos(phi); y -= v * sin(phi);
    }
    bool mBullet::out() { return (x < -20 || x > 20+TK::MWIDTH || y < -20 || y > 20+TK::MHEIGHT); }
}
//monster's bullet
namespace TK {
    Monster::Bullet::Bullet(double x, double y, double v, double phi, SDL_Texture *t)
        : x(x), y(y), v(v), phi(phi), tex(t) {}

    void Monster::Bullet::updatePosition() {
        x += v * cos(phi); y -= v * sin(phi);
    }
    bool Monster::Bullet::out() { return (x < -20 || x > 20+TK::MWIDTH || y < -20 || y > 20+TK::MHEIGHT); }
}
// Weapon
namespace TK {
    Weapon::Weapon() : delay(70), damage(20), theta(0), swing(false) {}

    void setSwing(Weapon &p, Uint32 delay) {
        p.swing = true;
        SDL_AddTimer(delay, cancelSwing, &p);
    }

    Uint32 cancelSwing(Uint32 interval, void *weapon) {
        ((Weapon *)weapon)->swing = false; return 0;
    }
}

