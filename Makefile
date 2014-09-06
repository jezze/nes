BIN = nes
SRC = nes.c cpu.c rom.c sdl.c
OBJ = nes.o cpu.o rom.o sdl.o
CFLAGS = -Wall -O2 -fomit-frame-pointer

.c.o:
	$(CC) -o $@ -c $(CFLAGS) $<

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -L/usr/X11R6/lib -lSDL -pthread

clean: $(SRC)
	rm -f $(BIN) $(OBJ)
