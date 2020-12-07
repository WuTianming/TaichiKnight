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

#include "audio.h"
#include "render.h"
#include "player.h"
#include "mon.h"

using namespace std;

double countFPS() {
    static double timeElapsed = 0.00;
    double tmp = timeElapsed; timeElapsed = SDL_GetTicks();
    return 1000.00 / (timeElapsed - tmp);
}

void InitSDL(const char *Name, SDL_Window *&window, SDL_Renderer *&renderer) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    window =
        SDL_CreateWindow(
                Name,
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                640, 480, 0);
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

template <class T>
T &truncate(T &x, T Mn, T Mx) { if (x > Mx) x = Mx; if (x < Mn) x = Mn; return x; }

void EndLoop(SDL_Renderer *renderer) {
    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_WINDOWEVENT_CLOSE:
                case SDL_QUIT:
                // case SDL_KEYDOWN:
                    return;
                    break;
            }
        }
        SDL_RenderPresent(renderer);
    }
}

bool GameLoop(SDL_Renderer *renderer) {
    printf("%lf fps\n", countFPS());

    // 游戏结束？
    if (player.hp <= 0) {
        EndLoop(renderer);
        return false;
    }

    // 处理 系统事件（退出游戏） 和 用户事件（特殊操作）
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
                if (event.key.keysym.sym == SDLK_f) {
                    TK::magnify += 1;
                }
                break;
            case SDL_USEREVENT:
                // 激活特殊操作
                break;
        }
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    // 根据键盘操作来改变角色属性
    if (state[SDL_SCANCODE_W] ) {
        player.x += player.v * cos(player.phi);
        player.y += -player.v * sin(player.phi);
    }
    if (state[SDL_SCANCODE_S]) {
        player.x -= player.v * cos(player.phi);
        player.y -= -player.v * sin(player.phi);
    }
    if (state[SDL_SCANCODE_A]) { player.phi += TK::Pi / 60; }
    if (state[SDL_SCANCODE_D]) { player.phi -= TK::Pi / 60; }

    if (state[SDL_SCANCODE_DOWN])  { player.hp -= 2; }
    if (state[SDL_SCANCODE_UP])    { player.hp += 2; }   // truncate!!

    truncate(player.x, 0.00, 640.00);
    truncate(player.y, 0.00, 480.00);

    if (state[SDL_SCANCODE_SPACE] && !player.weapon.swing) {
        bullets.emplace_back(
                player.x, player.y,
                player.v * 1.5,
                player.getphi(),
                player.tex[1]);
        TK::setSwing(player.weapon);
    }
    // experimental
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

    // 渲染显示内容
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    TK::drawPlayer(renderer, player);

    for (auto i = bullets.begin(); i != bullets.end(); ++i)
        TK::drawBullet(renderer, *i);

    TK::drawHUD(renderer, player);

    SDL_RenderPresent(renderer);

    return true;
}

void TEST(SDL_Renderer *renderer) {
    TK::MidiInit("res/a.mid");
    TK::MidiStart();
    player.tex[0] = IMG_LoadTexture(renderer, "res/taichi.png");
    player.tex[1] = IMG_LoadTexture(renderer, "res/bullet.png");
    player.tex[2] = IMG_LoadTexture(renderer, "res/taichi_cursor.png");
    // player.tex[2] = IMG_LoadTexture(renderer, "res/mbg.png");
}

int main() {
    // 初始化
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    InitSDL("Taichi Knight", window, renderer);

    // 程序没写完之前，所有乱七八糟的测试代码都往TEST函数里面塞吧，main干净一点
    TEST(renderer);

    // 游戏循环
    while (GameLoop(renderer));

    // 退出游戏
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}

