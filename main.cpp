#define SDL_MAIN_HANDLED

#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <time.h>
#include <algorithm>
#include <stdlib.h>

#include "MidiFile.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "SDL2_gfxPrimitives.h"

#include "audio.h"
#include "render.h"
#include "player.h"
#include "mon.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using namespace std;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
double scaleFactorX = 1.00, scaleFactorY = 1.00;

double countFPS() {
    static double timeElapsed = 0.00;
    double tmp = timeElapsed; timeElapsed = SDL_GetTicks();
    double ans = 1000.00 / (timeElapsed - tmp);
    if (ans < 55) printf("FPS %lf\n", ans);
    return ans;
}

void InitSDL(const char *Name, SDL_Window *&window, SDL_Renderer *&renderer) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    window =
        SDL_CreateWindow(
                Name,
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                TK::WWIDTH, TK::WHEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    int xpix, ypix;
    SDL_GL_GetDrawableSize(window, &xpix, &ypix);
    scaleFactorX = 1.00 * xpix / TK::WWIDTH;
    scaleFactorY = 1.00 * ypix / TK::WHEIGHT;

    renderer =
        SDL_CreateRenderer(
                window, -1,
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetScale(renderer, scaleFactorX, scaleFactorX);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
}

void ExitSDL() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
    exit(0);
}

TK::Player player;
vector<TK::Monster> monster;
vector<TK::Boss> boss;
vector<TK::Bullet> bullets;
vector<TK::mBullet> mbullets;

int judging = -1;
Uint32 hitTick = 0, accTick = 0;

// Uint32 CALLBACK(Uint32 intv, void *n) { printf("THIS IS CALLBACK\n"); return 0; }

void TEST(SDL_Renderer *renderer) {
    // SDL_AddTimer(5000, CALLBACK, NULL);
}

struct Button {
#define NORM  0
#define HOVER 1
#define DOWN  2
    int k;
    SDL_Rect rect;
    bool isInside(int x, int y) {
        return x >= rect.x && y >= rect.y
            && x <= rect.x + rect.w && y <= rect.y + rect.h;
    }
    void process(int x, int y, int mflag) {
        bool In = isInside(x, y);
        if (In) {
            if (mflag == 2) k = DOWN;
            else if (mflag == 3) k = HOVER;
            else if (k == NORM) k = HOVER;
        } else {
            k = NORM;
        }
    }
} buttons[3];

int StartMenu() {
    int GameLoop(void);

    SDL_Event event;
    int mflag = 1;
    int mx, my;
    SDL_GetMouseState(&mx, &my);

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_WINDOWEVENT_CLOSE:
            case SDL_QUIT:
                ExitSDL();
            case SDL_MOUSEBUTTONDOWN:
                mflag = 2;
                printf("down\n");
                break;
            case SDL_MOUSEBUTTONUP:
                mflag = 3;
                printf("up\n");
                break;
        }
    }

    int texid = 0;
    for (int i = 0; i <= 2; i++) {
        bool isDown = (buttons[i].k == DOWN);
        buttons[i].process(mx, my, mflag);
        if (buttons[i].k == DOWN) {
            texid = 4 + i;
        } else if (buttons[i].k == HOVER) {
            texid = 1 + i;
            if (isDown && mflag == 3) {
                if (i == 0) {
                    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
#ifdef __EMSCRIPTEN__
                    TK::countDown3SecsWrapper((void *)"res/midi/Rydeen.mid");
#else
                    SDL_Thread *pthread;
                    pthread = SDL_CreateThread(TK::countDown3SecsWrapper, "ptr", (void *)"res/midi/Rydeen.mid");
#endif
                    // while (GameLoop());
                    return 2;
                    // TK::MidiPause();
                } else if (i == 1) {
                    return 3;
                } else if (i == 2) {
                    return 0;
                }
            }
        }
    }
    SDL_RenderCopy(renderer, TK::mainmenu[texid], NULL, NULL);

    SDL_RenderPresent(renderer);
    return 1;
}

void StartMenuWrapper() {
    int GameLoop(void);
    static int State = 1;
    static Uint32 deathTime = 0;
    int t = false;
    switch (State) {
        case 1:
            t = StartMenu();
            if (t == 2) {
                // game start
                State = 2;
            } else if (t == 3) {
                // help
                printf("help\n");
                State = 3;
            } else if (t == 0) {
                // quit
                ExitSDL();
            }
            break;
        case 2:
            t = GameLoop();
            if (t == 0) {
                // death
                TK::MidiPause();
                State = 0;
                deathTime = SDL_GetTicks();
            } else if (t == 1) {
                // next frame
            } else if (t == 2) {
                TK::MidiPause();
                State = 1;
            }
            break;
        case 3:
        case 0:
            int id = (State == 3 ? 1 : 7);
            SDL_RenderCopy(renderer, TK::mainmenu[id], NULL, NULL);
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_WINDOWEVENT_CLOSE:
                    case SDL_QUIT:
                    case SDL_KEYDOWN:
                        if (SDL_GetTicks() - deathTime >= 500) {
                            State = 1;
                            return;
                        }
                        break;
                }
            }
            SDL_RenderPresent(renderer);
            break;
    }
}

int fb,kb,ff=1;
int monstern,monsters;
int t1, t2;

int GameLoop() {
    // RETURNS:
    // 1 for continue to next frame
    // 0 for death
    // 2 for quit

    Uint32 tickNow = SDL_GetTicks();
    // countFPS();

    // 游戏结束？
    if (player.hp <= 0) { return 0; }

    ////////////////////////////////////////// 处理 系统事件（退出游戏） 和 用户事件（特殊操作）
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_WINDOWEVENT_CLOSE:
            case SDL_QUIT:
                ExitSDL();
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    TK::MidiPause();
                    return 2;
                }

                if (event.key.keysym.sym == SDLK_SPACE && event.key.repeat == 0) {
                    hitTick = SDL_GetTicks();
                    if (judging != -1) {
                        // 敲击的时候恰好处于判定期
                        int delta = accTick - hitTick;
                        printf("delta = %+dms, ", delta);
                        // threshold: 85, 140
                        if (abs(delta) <= 86) {
                            printf("PERFECT\n");
                            player.buffs.push_back(TK::Buff{ tickNow + 3000, 3 });
                            player.mp += 300;
                        } else if (abs(delta) <= 200) {
                            printf("good\n"); 
                            player.buffs.push_back(TK::Buff{ tickNow + 3000, 4 });
                            player.mp += 100;
                        } else {
                            printf("bad\n");
                            player.buffs.push_back(TK::Buff{ tickNow + 3000, 5 });
                        }
                        judging = -1;
                    }
                }

                if (event.key.keysym.sym == SDLK_z) { player.weapon.theta--; }
                if (event.key.keysym.sym == SDLK_x) { player.weapon.theta++; }
                TK::truncate(player.weapon.theta, 0, 2);
                break;
            case SDL_USEREVENT:
                // BEAT
                printf("EVENT\n");
                if (judging == -1) {
                    judging = 30 + 120;     // 15 frames ~= 1/4 second
                    accTick = SDL_GetTicks() + 1250;
                }
                break;
        }
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    // 根据键盘操作来改变角色属性
    if(ff==1){
        ff=0;
        TK::Boss New(900,250,2.5,TK::Pi/2,IMG_LoadTexture(renderer, "res/pic/shiteater.png")); 
        boss.push_back(New);
        boss.begin()->hp=300;
    }
    if(monstern<=1){
        TK::Monster New(boss.begin()->x,boss.begin()->y,2.5,TK::Pi/2,IMG_LoadTexture(renderer, "res/pic/caixukun.png")); 
        monster.push_back(New);
        monster.back().fm=0; monster.back().km=0;
        monster.back().tex=IMG_LoadTexture(renderer, "res/pic/laoba.png");
        monster.back().btex=IMG_LoadTexture(renderer, "res/pic/bianbian1.png");
        monster.push_back(New);
        monster.back().fm=0; monster.back().km=0;
        monster.back().tex=IMG_LoadTexture(renderer, "res/pic/caixukun.png");
        monster.back().btex=player.tex[7];
        monster.push_back(New);
        monster.back().fm=0; monster.back().km=0;
        monster.back().btex=IMG_LoadTexture(renderer, "res/pic/basketball.png");
        monstern+=3;
    }//增加monster
    if (state[SDL_SCANCODE_W] ) {
        player.x += player.v * cos(player.phi);
        player.y += -player.v * sin(player.phi);
    }
    if (state[SDL_SCANCODE_S]) {
        player.x -= player.v * cos(player.phi);
        player.y -= -player.v * sin(player.phi);
    }
    TK::truncate(player.x, 0.00, (double)TK::MWIDTH);
    TK::truncate(player.y, 0.00, (double)TK::MHEIGHT);

    if (state[SDL_SCANCODE_A]) { player.phi += TK::Pi / 60; }
    if (state[SDL_SCANCODE_D]) { player.phi -= TK::Pi / 60; }

    /*
    if (state[SDL_SCANCODE_LEFT])  { player.hp -= 2; }
    if (state[SDL_SCANCODE_RIGHT]) { player.hp += 2; }   // truncate!!
    if (state[SDL_SCANCODE_DOWN])  { player.mp -= 20; }
    if (state[SDL_SCANCODE_UP])    { player.mp += 20; }  // truncate!!
    */

    if (player.mp >= 500) ;
    else player.mp -= 1;

    TK::truncate(player.mp, 0, 600);

    if (state[SDL_SCANCODE_SPACE] && !player.weapon.swing) {
        double phi0 = player.getphi();
        int n0 = player.weapon.theta;
        for (int p = -n0; p <= n0; p++) {
            double phi = p * TK::Pi / 18;
            bullets.emplace_back(
                    player.x, player.y,
                    player.v * 1.5,
                    phi0 + phi,
                    5,
                    player.tex[1]);
        }
        TK::setSwing(player.weapon, player.weapon.delay * (n0*2+1));
    }
    ////////////////////////////////////////// experimental
    if (state[SDL_SCANCODE_SPACE] && state[SDL_SCANCODE_LSHIFT] && player.mp >= 500) {
        for (double phi = 0.00; phi <= 2 * TK::Pi; phi += 0.02) {
            bullets.emplace_back(
                    player.x, player.y,
                    player.v * 2.5,
                    phi,
                    1,
                    player.tex[1]);
            bullets.emplace_back(
                    player.x, player.y,
                    player.v * 2,
                    phi + 0.01,
                    2,
                    player.tex[1]);
        }
        player.mp -= 500;
    }
    // printf("%d\n",monstern);
    if ((tickNow-t1)>=5000) {
        t1=tickNow;
        for(auto i=monster.begin();i!=monster.end();i++){
            for (double phi = 0.00; phi <= 2 * TK::Pi; phi += 1) {
                int x=i->x;
                int y=i->y;
                i->bullets.emplace_back(
                        x, y,
                        2.5,
                        phi,
                        i->btex);
            }
        }
    }
    if ((tickNow-t2) >= 7000) {
        t2 = tickNow;
        for (double phi = 0.00; phi <= 2 * TK::Pi; phi += 0.5) {
            int x=boss.begin()->x;
            int y=boss.begin()->y;
            mbullets.emplace_back(
                    x, y,
                    1,
                    phi,
                    player.tex[7]);
        }
    }

    for (auto i = bullets.begin(); i != bullets.end(); ++i)
        i->updatePosition();
    for (auto i = bullets.begin(); i != bullets.end(); ) {
        if (i->out()) i = bullets.erase(i);
        else ++i;
    }
    for(auto i=monster.begin();i!=monster.end();i++){
        for(auto j=i->bullets.begin();j!=i->bullets.end();j++){
            j->updatePosition();
        }
    }
    for(auto i=monster.begin();i!=monster.end();i++){
        for(auto j=i->bullets.begin();j!=i->bullets.end();){
            if(j->out()) j=i->bullets.erase(j);
            else j++;
        }
    }
    for (auto i = mbullets.begin(); i != mbullets.end(); ++i)
        i->updatePosition();
    for (auto i = mbullets.begin(); i != mbullets.end(); ) {
        if (i->out()) i = mbullets.erase(i);
        else ++i;
    }

    for (auto i = player.buffs.begin(); i != player.buffs.end(); ) {
        if (i->endtick < tickNow) i = player.buffs.erase(i);
        else ++i;
    }

    ////////////////////////////////////////// 渲染显示内容
    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // SDL_RenderClear(renderer);

    TK::drawPlayer(renderer, player, judging);

    if (judging != -1) {
        judging -= 2;
        if (judging < -40) {
            printf("miss\n");
            player.buffs.push_back(TK::Buff{ tickNow + 3000, 6 });
            judging = -1;
        }
    }

    //monster&boss 's move
    for (auto i = monster.begin(); i != monster.end(); ++i){
        if(i->fm>0){
            i->fm--;
            i->updatePosition(i->km);
        }
        else {
            int monster_move=rand()%6;
            if(monster_move>=0&&monster_move<=3){
                i->fm=32;
                i->km=monster_move;
            }
            i->updatePosition(monster_move);
        }
        int x=i->x;
        int y=i->y;
        if(x < -20 || x > 20+TK::MWIDTH || y < -20 || y > 20+TK::MHEIGHT){
            i->fm=48;
        }
        if(x<-20){
            i->km=6;
        }
        if(x> 20+TK::MWIDTH){
            i->km=7; 
        }
        if(y<-20){
            i->km=8;
        }
        if(y>20+TK::MHEIGHT){
            i->km=9;
        }
    }

    for (auto i = boss.begin(); i != boss.end(); i++) {
        if((abs(player.x-(i->x))<=50)&&(abs(player.y-(i->y))<=50)){
            int dx = player.x - (i->x), dy = (i->y) - player.y;
            double dl = sqrt(dx * dx + dy * dy);
            double phi1;
            if (dy >= 0) phi1=acos(dx / dl);
            phi1=-acos(dx / dl);
            i->x -=48 * cos(phi1); i->y += 48 * sin(phi1);
        }
        if(fb>0){
            fb--;
            i->updatePosition(kb);
        }
        else {
            int boss_move=rand()%6;
            if(boss_move>=0&&boss_move<=3){
                fb=32;
                kb=boss_move;
            }
            i->updatePosition(boss_move);
        }
        int x=i->x;
        int y=i->y;
        if(x < -20 || x > 20+TK::MWIDTH || y < -20 || y > 20+TK::MHEIGHT) fb=48;
        if(x<-20){
            kb=6;
        }
        if(x>20+TK::MWIDTH){
            kb=7;
        }
        if(y<-20){
            kb=8;
        }
        if(y>20+TK::MHEIGHT){
            kb=9;
        }
    }

    //玩家是否被击中
    for(auto j = mbullets.begin(); j != mbullets.end();){
        if (abs((j->x)-(player.x))<=10&&abs((j->y)-(player.y))<=10) {j = mbullets.erase(j);player.hp-=20;}
        else ++j;
    }
    for(auto i=monster.begin();i!=monster.end();i++){
        for(auto j=i->bullets.begin();j!=i->bullets.end();){
            if (abs((j->x)-(player.x))<=10&&abs((j->y)-(player.y))<=10) {j=i->bullets.erase(j);player.hp-=10;}
            else ++j;
        }
    }
    //判断moster&boss是否被子弹击中
    for(auto j = bullets.begin(); j != bullets.end(); j++){
        for (auto i = monster.begin(); i != monster.end();) {
            if (abs((j->x)-(i->x))<=7&&abs((j->y)-(i->y))<=7) {i = monster.erase(i);monstern--;}
            else ++i;
        }
    }

    if(!boss.empty() && boss.begin()->hp>0)
        for(auto j = bullets.begin(); j != bullets.end();){
            auto i = boss.begin();
            if (abs((j->x)-(i->x))<=10&&abs((j->y)-(i->y))<=10){
                i->hp -= j->damage;
                j = bullets.erase(j);
                printf("hp=%d\n",i->hp);
                if(i->hp<=0){
                    i = boss.erase(i);
                    break;
                }
            }
            else j++;
        }


    //画出：子弹 怪物 boss
    for (auto i = monster.begin(); i != monster.end(); ++i){
        TK::drawMonster(renderer, *i);
    }

    for (auto i = boss.begin(); i != boss.end(); ++i)
        TK::drawBoss(renderer, *i);
    for (auto i = bullets.begin(); i != bullets.end(); ++i)
        TK::drawBullet(renderer, *i);
    for (auto i = mbullets.begin(); i != mbullets.end(); ++i)
        TK::drawmBullet(renderer, *i);
    for(auto i=monster.begin();i!=monster.end();i++){
        for(auto j=i->bullets.begin();j!=i->bullets.end();j++){
            TK::drawMonsterBullet(renderer, *j);
        }
    }


    TK::drawHUD(renderer, player);

    SDL_RenderPresent(renderer);

    return true;
}

int main() {
    srand(time(0));
    // 初始化
    InitSDL("Taichi Knight", window, renderer);

    // 程序没写完之前，所有乱七八糟的测试代码都往TEST函数里面塞吧，main干净一点
    TEST(renderer);

    // 加载贴图
    player.tex[0] = IMG_LoadTexture(renderer, "res/pic/taichi.png");
    player.tex[1] = IMG_LoadTexture(renderer, "res/pic/bullet.png");
    player.tex[2] = IMG_LoadTexture(renderer, "res/pic/taichi_cursor.png");

    player.tex[3] = IMG_LoadTexture(renderer, "res/pic/perfect.png");
    player.tex[4] = IMG_LoadTexture(renderer, "res/pic/good.png");
    player.tex[5] = IMG_LoadTexture(renderer, "res/pic/bad.png");
    player.tex[6] = IMG_LoadTexture(renderer, "res/pic/miss.png");

    player.tex[7] = scaleFactorX > 1.01 ?
        IMG_LoadTexture(renderer, "res/pic/basketball_80.png") :
        IMG_LoadTexture(renderer, "res/pic/basketball_40.png");

    TK::background  = IMG_LoadTexture(renderer, "res/pic/back.png");
    SDL_SetTextureColorMod(TK::background, 128, 128, 128);         // darken

    buttons[0].rect = { 144, 90,  350, 70 };
    buttons[1].rect = { 144, 220, 350, 70 };
    buttons[2].rect = { 144, 315, 350, 70 };
    TK::mainmenu[0] = IMG_LoadTexture(renderer, "res/mm/1.png");
    TK::mainmenu[1] = IMG_LoadTexture(renderer, "res/mm/2.png");
    TK::mainmenu[2] = IMG_LoadTexture(renderer, "res/mm/3.png");
    TK::mainmenu[3] = IMG_LoadTexture(renderer, "res/mm/4.png");
    TK::mainmenu[4] = IMG_LoadTexture(renderer, "res/mm/5.png");
    TK::mainmenu[5] = IMG_LoadTexture(renderer, "res/mm/6.png");
    TK::mainmenu[6] = IMG_LoadTexture(renderer, "res/mm/7.png");
    TK::mainmenu[7] = IMG_LoadTexture(renderer, "res/mm/8.png");



#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(StartMenuWrapper, 0, 1);
#else
    while (1) StartMenuWrapper();
#endif

    // game start splash menu
    // choose midi file



    // 3 2 1 ready go
    // 3 2 1 要卡上midistart的自带延迟
    // midistart 延迟极大
    // TK::MidiInit("res/midi/un.mid");        // unstable time <= 1s
    // TK::MidiStart();                        // 1300 ms
    // TK::countDown3SecsWrapper("res/midi/un.mid");

    // while (GameLoop());

    // 退出游戏
    ExitSDL();
    return 0;
}

