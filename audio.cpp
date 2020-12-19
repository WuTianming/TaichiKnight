#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "MidiFile.h"

#include "audio.h"

namespace TK {
    smf::MidiFile   BGM;
    Mix_Chunk       *BGM_Play;

    void MidiInit(const char *filename) {
        BGM.read(filename);
        BGM.doTimeAnalysis();
        BGM.linkNotePairs();

        int tracks = BGM.getTrackCount();
        for (int track = 0; track < tracks; ++track) {
            for (int event = 0; event < BGM[track].size(); ++event) {
                // 遍历每一个音轨的每一个音符。可以在这里设置事件
                // 成员函数包含：
                // BGM[track][event].isNoteOn(); isNoteOff();
                // .getDurationInSeconds();
            }
        }

        // 由于播放器有bug，需要将音频文件稍作处理再播放，不然无法同步时间
        int dummytrack = BGM.addTrack();
        BGM.addNoteOn(dummytrack, 0, 0, 1, 0);
        BGM.write(".tmp.mid");

        BGM_Play = Mix_LoadWAV(".tmp.mid");
    }

    Uint32 callback_beat(Uint32 interval, void *param) {
        int id = reinterpret_cast<long long>(param);
        // fprintf(stdout, "beat %u as multiplied %d\n", SDL_GetTicks(), id);
        SDL_Event event;
        event.type = SDL_USEREVENT;
        SDL_PushEvent(&event);
        return 0;
    }

    void MidiStart()  {
        int starttime = SDL_GetTicks() + 50;
        int tcnt = BGM.getTrackCount();
        for (int track = 0; track < tcnt; track++) {
            // int track = tcnt - 7;
            // int track = 1;
            double prevt = -1.00;
            int cntr = 1;
            for (int event = 0; event < BGM[track].size(); event++) {
                if (BGM[track][event].isNoteOn()) {
                    // printf("%lf\n", midifile[track][event].seconds);
                    double t = BGM[track][event].seconds;
                    if (t - prevt > 0.002) {
                        printf("add timer t=%lf, tick=%u, start=%u\n", t, SDL_GetTicks(), starttime);
                        SDL_AddTimer(t * 1000.00 - (SDL_GetTicks() - starttime), callback_beat, reinterpret_cast<void *>(cntr));
                        cntr = 1;
                        prevt = t;
                    } else ++cntr;
                }
            }
        }
        while (SDL_GetTicks() < starttime) SDL_Delay(1);
        SDL_Delay(1250);
        Mix_PlayChannel(0, BGM_Play, 0);
    }
    // !!! 由于SDL的bug，Pause之后并不能Resume
    void MidiPause()  { Mix_Pause(0); }
    void MidiResume() { Mix_Resume(0); }

    int MidiPlaying() { return Mix_Playing(0); }

    // TODO Mix_ChannelFinished --> custom callback
}

