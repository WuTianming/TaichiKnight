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

    void MidiStart()  { Mix_PlayChannel(0, BGM_Play, 0); }
    // !!! 由于SDL的bug，Pause之后并不能Resume
    void MidiPause()  { Mix_Pause(0); }
    void MidiResume() { Mix_Resume(0); }

    int MidiPlaying() { return Mix_Playing(0); }

    // TODO Mix_ChannelFinished --> custom callback
}

