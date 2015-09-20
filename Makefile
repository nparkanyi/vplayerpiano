all: vplayerpiano

vplayerpiano: vplayerpiano.c
	${CC} -std=c99 -o vplayerpiano vplayerpiano.c `pkg-config --cflags sdl2` -lSDL2
  
clean: 
	rm vplayerpiano
