CC = g++
CFLAGS = --std=c++11 -O2 -I. -I./include
INCLUDE_WIN = -I "D:\Program Files (x86)\Dev-Cpp\SDL2\include" -I "D:\Program Files (x86)\Dev-Cpp\SDL2_image\include" -I "D:\Program Files (x86)\Dev-Cpp\SDL2_mixer\include" -I "D:\Program Files (x86)\Dev-Cpp\SDL2\include\SDL2" -I "D:\Program Files (x86)\Dev-Cpp\SDL2_image\include\SDL2" -I "D:\Program Files (x86)\Dev-Cpp\SDL2_mixer\include\SDL2"
LIBS_NIX = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer
LIBS_WIN = -L "D:\Program Files (x86)\Dev-Cpp\SDL2\lib" -L "D:\Program Files (x86)\Dev-Cpp\SDL2_image\lib" -L "D:\Program Files (x86)\Dev-Cpp\SDL2_mixer\lib" -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer
OBJS_WIN = audio.o main.o mon.o player.o render.o src/Binasc.o src/MidiEvent.o src/MidiEventList.o src/MidiFile.o src/MidiMessage.o src/Options.o
EXEC = main.exe

ifeq ($(OS),Windows_NT)
	LIBS = $(LIBS_WIN)
else
	LIBS = $(LIBS_NIX)
endif

all: $(EXEC)

$(EXEC): $(OBJS_WIN)
		$(CC) -o $@ $(OBJS_WIN) $(CFLAGS) $(LIBS)

%.o: %.cpp
	$(CC) -o $@ $^ $(CFLAGS) $(INCLUDE_WIN) -c

clean:
	rm -f $(EXEC) $(OBJS_WIN)
