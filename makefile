CC = g++
CFLAGS = --std=c++11 -O2 -I. -I./include
LIBS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -L./lib -lmidifile
OBJS = audio.o main.o mon.o player.o render.o
EXEC = main

all: $(EXEC)

$(EXEC): $(OBJS) LIB
	$(CC) -o $@ $(OBJS) $(CFLAGS) $(LIBS)

%.o: %.cpp
	$(CC) $^ $(CFLAGS) -c

LIB:
	$(MAKE) -f makefile_lib

clean:
	rm -f $(EXEC) $(OBJS)
