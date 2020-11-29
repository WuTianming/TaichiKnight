#define SDL_MAIN_HANDLED

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

bool GameLoop(SDL_Renderer *renderer) {
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
                break;
            case SDL_USEREVENT:
                // 激活特殊操作
                break;
        }
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    // 根据键盘操作来改变角色属性
    // state[SDL_SCANCODE_...]

    // 渲染显示内容

    return true;
}

void TEST() {
    TK::MidiInit("a.mid");
    TK::MidiStart();
}

int main() {
    // 初始化
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    InitSDL("Taichi Knight", window, renderer);

    // 程序没写完之前，所有乱七八糟的测试代码都往TEST函数里面塞吧，main干净一点
    TEST();

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

