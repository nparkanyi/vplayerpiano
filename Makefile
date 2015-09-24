all: vplayerpiano

vplayerpiano: vplayerpiano.c
	${CC} -std=c99 -o vplayerpiano vplayerpiano.c libmidi/midi.c `pkg-config --cflags sdl2` `pkg-config --cflags glib-2.0` -lSDL2

clean:
	rm vplayerpiano
