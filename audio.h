#ifndef AUDIO_H
#define AUDIO_H

#include "MidiFile.h"

namespace TK {
    // 这个文件中的函数可以考虑全部以Midi开头，方便辨识

    extern smf::MidiFile    BGM;
    extern Mix_Chunk        *BGM_Play;
    extern double           BPM;
    extern int              run;

    // 加载背景音乐midi文件
    void MidiInit(const char *);

    void countDown3SecsWrapper(const char *filename);
    void MidiStart();
    void MidiPause();
    void MidiResume();
    int MidiPlaying();
}

#endif
