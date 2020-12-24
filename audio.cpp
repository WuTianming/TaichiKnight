#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <algorithm>
using namespace std;

#include "MidiFile.h"

#include "audio.h"

namespace TK {
    smf::MidiFile   BGM;
    Mix_Chunk       *BGM_Play;
    double          BPM;
    double          Length;
    int             run = 0;

    void MidiInit(const char *filename) {
        // timing: unstable

        BGM.read(filename);
        BGM.doTimeAnalysis();
        BGM.linkNotePairs();

        int tracks = BGM.getTrackCount();
        for (int track = 0; track < tracks; ++track) {
            for (int event = 0; event < BGM[track].size(); ++event) {
                if (BGM[track][event].isTempo()) {
                    BPM = BGM[track][event].getTempoMicro() / 1000;
                }
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
        if (id < run) return 0;
        // fprintf(stdout, "beat %u as multiplied %d\n", SDL_GetTicks(), id);
        SDL_Event event;
        event.type = SDL_USEREVENT;
        SDL_PushEvent(&event);
        return 0;
    }

    void MidiSetCallbacks(int t0, int track = -1) {
        ++run;
        // 一开始帧率不稳，要等个几秒再设置callback
        int tcnt = BGM.getTrackCount();
        int L = 0, R = tcnt - 1;
        if (track != -1) L = R = track;
        /* for (int track = L; track <= R; track++) {
            double prevt = -1.00;
            int cntr = 1;
            for (int event = 0; event < BGM[track].size(); event++) {
                if (BGM[track][event].isNoteOn()) {
                    // printf("%lf\n", midifile[track][event].seconds);
                    double t = BGM[track][event].seconds;
                    if (t - prevt > 0.002) {
                        printf("add timer t=%lf, tick=%u, start=%u\n", t, SDL_GetTicks(), t0);
                        SDL_AddTimer(t * 1000.00 - (SDL_GetTicks() - t0), callback_beat, reinterpret_cast<void *>(cntr));
                        cntr = 1;
                        prevt = t;
                    } else ++cntr;
                }
            }
        }*/
        Length = 0.00;
        for (int tr = 0; tr < tcnt; tr++) {
            Length = max(Length, BGM[tr].back().seconds);
        }
        printf("bpm=%f\n", BPM);
        for (int t = 8 * BPM; t / 1000.00 <= Length; t += 8 * BPM) {
            SDL_AddTimer(t - (SDL_GetTicks() - t0), callback_beat, reinterpret_cast<void *>(run));
            printf("add %d\n", t);
        }
    }

    void MidiStart()  {
        Mix_PlayChannel(0, BGM_Play, 0);
    }

    void countDown3SecsWrapper(const char *filename) {
        Uint32 time0 = SDL_GetTicks() + 200;

        MidiInit(filename);
        MidiSetCallbacks(time0 + 200, 0);

        while (SDL_GetTicks() < time0 + 200) SDL_Delay(1);
        SDL_Delay(1250);
        MidiStart();
    }

    // !!! 由于SDL的bug，Pause之后并不能Resume
    void MidiPause()  { Mix_Pause(0); }
    void MidiResume() { Mix_Resume(0); }

    int MidiPlaying() { return Mix_Playing(0); }

    // TODO Mix_ChannelFinished --> custom callback
}

