SRC = nes.c cpu.c ppu.c rom.c input.c sdl.c
OBJ_FLAG = -O3 -fomit-frame-pointer -Wall -I/usr/local/include -L/usr/local/lib -L/usr/X11R6/lib -lSDL -pthread;

nes: $(SRC)
	$(CC) $(SRC) $(OBJ_FLAG)
	strip a.out
	mv a.out nes

clean: $(SRC)
	rm -f nes
