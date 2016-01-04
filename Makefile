all: vplayerpiano

vplayerpiano: vplayerpiano.cc
	${CC} -std=c99 libmidi/midi.c -c `pkg-config --cflags glib-2.0`
	${CXX} -std=c++11 -g -o vplayerpiano vplayerpiano.cc midi.o `pkg-config --cflags sdl2` `pkg-config --cflags glib-2.0` -lSDL2

clean:
	rm vplayerpiano
