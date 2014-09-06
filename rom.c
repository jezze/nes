#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nes.h"
#include "cpu.h"
#include "rom.h"
#include "backend.h"

unsigned char PRG;
unsigned char CHR;
unsigned char MAPPER;
int OS_MIRROR = 0;
int FS_MIRROR;
int SRAM;
int MIRRORING;
static char title[128];
static int TRAINER;

struct nes_header
{

    char identity[4];
    char prgromsize;
    char chrromsize;
    char flags6;
    char flags7;
    char prgramsize;
    char flags9;
    char flags10;
    char zero[5];

};

int rom_load(char *romfn, unsigned char *ram_mem, unsigned char *ppu_mem)
{

    struct nes_header header;
    char identity[4] = {'N', 'E', 'S', 0x1A};
    int rcb;

    backend_read(romfn, 0, 16, &header);

    if (memcmp(header.identity, identity, 4))
        return 1;

    PRG = header.prgromsize;
    CHR = header.chrromsize;
    MAPPER = (header.flags6 >> 4) | (header.flags7 & 0xf0);
    rcb = (header.flags6 - ((header.flags6 >> 4) << 4));
    MIRRORING = (rcb & 1) ? 1 : 0;
    SRAM = (rcb & 2) ? 1 : 0;
    TRAINER = (rcb & 4) ? 1 : 0;
    FS_MIRROR = (rcb & 8) ? 1 : 0;

    backend_read(romfn, 16, 16384, ram_mem + 0x8000);

    if (PRG == 0x01)
        backend_read(romfn, 16, 16384, ram_mem + 0xC000);
    else
        backend_read(romfn, 16 + ((PRG - 1) * 16384), 16384, ram_mem + 0xC000);

    if (CHR != 0x00)
    {

        backend_read(romfn, 16 + (PRG * 16384), 8192, ppu_mem);
        backend_read(romfn, 16 + (PRG * 16384), 128, title);

    }

    return 0;

}

