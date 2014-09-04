SRC = nes.c cpu.c ppu.c rom.c sdl.c
OBJ_FLAG = -O3 -fomit-frame-pointer -Wall -I/usr/local/include -L/usr/local/lib -L/usr/X11R6/lib -lSDL -pthread;

nes: $(SRC)
	$(CC) -o $@ $(SRC) $(OBJ_FLAG)

clean: $(SRC)
	rm -f nes
