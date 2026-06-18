CFLAGS = -std=c17 -Wall -Wextra -Werror
SDL_FLAGS = $(shell sdl2-config --cflags --libs)

all:
	gcc chip8.c -o chip8 $(CFLAGS) $(SDL_FLAGS)

clean:
	rm -f chip8