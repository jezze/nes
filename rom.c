#include <stdlib.h>
#include <string.h>
#include "nes.h"
#include "cpu.h"
#include "rom.h"
#include "backend.h"

int rom_load(char *romfn, struct nes_header *header, unsigned char *ram_mem, unsigned char *ppu_mem)
{

    char identity[4] = {'N', 'E', 'S', 0x1A};

    backend_read(romfn, 0, 16, header);

    if (memcmp(header->identity, identity, 4))
        return 1;

    backend_read(romfn, 16, 16384, ram_mem + 0x8000);
    backend_read(romfn, 16 + ((header->prgromsize - 1) * 16384), 16384, ram_mem + 0xC000);

    if (header->chrromsize)
        backend_read(romfn, 16 + (header->prgromsize * 16384), 8192, ppu_mem);

    return 0;

}

