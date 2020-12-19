#define SDL_MAIN_HANDLED

#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>

#include "MidiFile.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL2_gfxPrimitives.h>

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
                TK::WWIDTH, TK::WHEIGHT, 0);
    renderer =
        SDL_CreateRenderer(
                window, -1,
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
}

TK::Player player;
vector<TK::Bullet> bullets;

int judging = -1;
Uint32 hitTick = 0, accTick = 0;

// Uint32 CALLBACK(Uint32 intv, void *n) { printf("THIS IS CALLBACK\n"); return 0; }

void TEST(SDL_Renderer *renderer) {
    // SDL_AddTimer(5000, CALLBACK, NULL);
}

bool StartMenu(SDL_Renderer *renderer) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_WINDOWEVENT_CLOSE:
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                return false;
                break;
        }
    }
    SDL_RenderPresent(renderer);
    return true;
}

bool EndLoop(SDL_Renderer *renderer) {
    static bool End = true;
    SDL_Event event;
    if (End) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_WINDOWEVENT_CLOSE:
                case SDL_QUIT:
                case SDL_KEYDOWN:
                    End = false;
                    TEST(renderer);
                    return false;
                    break;
            }
        }
        SDL_RenderPresent(renderer);
    }
    return End;
}

bool GameLoop(SDL_Renderer *renderer) {
    Uint32 tickNow = SDL_GetTicks();
    // countFPS();

    // 游戏结束？
    if (player.hp <= 0) {
        EndLoop(renderer);
        return false;
    }
    // if (EndLoop(renderer)) return true;

    ////////////////////////////////////////// 处理 系统事件（退出游戏） 和 用户事件（特殊操作）
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_WINDOWEVENT_CLOSE:
            case SDL_QUIT:
                return false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    return false;
                // experimental
                if (event.key.keysym.sym == SDLK_f) { TK::magnify += 1; }

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
                            player.mp += 200;
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
                // 激活特殊操作
                printf("EVENT\n");
                if (judging == -1) {
                    judging = 30 + 120;     // 15 frames ~= 1/4 second
                    accTick = SDL_GetTicks() + 1250;
                }
                break;
        }
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    ////////////////////////////////////////// 根据键盘操作来改变角色属性
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

    if (state[SDL_SCANCODE_LEFT])  { player.hp -= 2; }
    if (state[SDL_SCANCODE_RIGHT]) { player.hp += 2; }   // truncate!!
    if (state[SDL_SCANCODE_DOWN])  { player.mp -= 20; }
    if (state[SDL_SCANCODE_UP])    { player.mp += 20; }  // truncate!!

    player.mp -= 1;

    TK::truncate(player.mp, 0, 1100);

    if (state[SDL_SCANCODE_SPACE] && !player.weapon.swing) {
        double phi0 = player.getphi();
        int n0 = player.weapon.theta;
        for (int p = -n0; p <= n0; p++) {
            double phi = p * TK::Pi / 18;
            bullets.emplace_back(
                    player.x, player.y,
                    player.v * 1.5,
                    phi0 + phi,
                    player.tex[1]);
        }
        TK::setSwing(player.weapon, player.weapon.delay * (n0*2+1));
    }
    ////////////////////////////////////////// experimental
    if (state[SDL_SCANCODE_SPACE] && state[SDL_SCANCODE_LSHIFT] ) {
        for (double phi = 0.00; phi <= 2 * TK::Pi; phi += 0.02) {
            bullets.emplace_back(
                    player.x, player.y,
                    player.v * 3,
                    phi,
                    player.tex[1]);
        }
    }

    for (auto i = bullets.begin(); i != bullets.end(); ++i)
        i->updatePosition();
    for (auto i = bullets.begin(); i != bullets.end(); ) {
        if (i->out()) i = bullets.erase(i);
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

    for (auto i = bullets.begin(); i != bullets.end(); ++i)
        TK::drawBullet(renderer, *i);

    TK::drawHUD(renderer, player);

    SDL_RenderPresent(renderer);

    return true;
}

void GameLoopWrapper() { GameLoop(renderer); }

int main() {
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

    TK::background = IMG_LoadTexture(renderer, "res/pic/back.png");
    SDL_SetTextureColorMod(TK::background, 128, 128, 128);         // darken



    while (StartMenu(renderer));
    // game start splash menu
    // choose midi file



    // 3 2 1 ready go
    // 3 2 1 要卡上midistart的自带延迟
    TK::MidiInit("res/midi/un.mid");
    TK::MidiStart();

    // 游戏循环
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(GameLoopWrapper, 0, 1);
#else
    while (GameLoop(renderer));
#endif

    // 退出游戏
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}

