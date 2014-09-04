#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nes.h"
#include "cpu.h"
#include "rom.h"

unsigned char PRG;
unsigned char CHR;
unsigned char MAPPER;
int OS_MIRROR = 0;
int FS_MIRROR;
int SRAM;
int MIRRORING;
static char title[128];
static int TRAINER;

int analyze_header(char *romfn)
{

    unsigned char header[15];
    FILE *romfp = fopen(romfn,"rb");
    int rcb;
    int i;

    if (!romfp)
        return 1;

    fseek(romfp, 0, 2);

    romlen = ftell(romfp);

    fseek(romfp, 0, SEEK_SET);
    fread(&header[0], 1, 15, romfp);
    fclose(romfp);

    if ((header[0] != 'N') || (header[1] != 'E') || (header[2] != 'S') || (header[3] != 0x1A))
        return 1;

    for (i = 8; i < 15; i++)
    {

        if ((header[i] != 0x00) && (header[i] != 0xFF))
        {

        }

    }

    PRG = header[4];

    if (header[5] == 0x00)
    {

    }

    CHR = header[5];
    MAPPER = (header[6] >> 4);
    MAPPER |= (header[7] & 0xf0);
    rcb = (header[6] - ((header[6] >> 4) << 4));
    MIRRORING = (rcb & 1) ? 1 : 0;
    SRAM = (rcb & 2) ? 1 : 0;
    TRAINER = (rcb & 4) ? 1 : 0;
    FS_MIRROR = (rcb & 8) ? 1 : 0;

    return 0;

}

int load_rom(char *romfn)
{

    FILE *romfp = fopen(romfn, "rb");

    if (!romfp)
        return 1;

    fread(&romcache[0x0000],1, romlen, romfp);
    fclose(romfp);

    if (PRG == 0x01)
    {

        memcpy(memory + 0x8000, romcache + 16, 16384);
        memcpy(memory + 0xC000, romcache + 16, 16384);

    }

    else
    {

        memcpy(memory + 0x8000, romcache + 16, 16384);
        memcpy(memory + 0xC000, romcache + 16 + ((PRG - 1) * 16384), 16384);

    }

    if (CHR != 0x00)
    {

        memcpy(ppu_memory, romcache + 16 + (PRG * 16384), 8192);
        memcpy(title, romcache + 16 + (PRG * 16384) + 8192, 128);

    }

    return 0;

}

