#include <stdlib.h>
#include <string.h>
#include "nes.h"
#include "cpu.h"
#include "rom.h"
#include "backend.h"

static struct nes_header header;
static unsigned char memory[65536];
static int pad1_state[8];
static int pad1_readcount = 0;
static int running = 1;
static char *romfn;
static int OS_MIRROR = 0;
static int SRAM;
static int MIRRORING;

struct mapper
{

    void (*reset)();
    void (*access)(unsigned int address, unsigned char data);

};

#include "ppu.c"
#include "apu.c"
#include "mmc1.c"
#include "unrom.c"
#include "cnrom.c"
#include "mmc3.c"

struct mapper mappers[256] = {
    {0 , 0},
    {mmc1_reset, mmc1_access},
    {unrom_reset, unrom_access},
    {cnrom_reset, cnrom_access},
    {mmc3_reset, mmc3_access}
};

struct mapper *mapper = &mappers[0];

void input_set(int key)
{

    pad1_state[key] = 0x01;

}

void input_clear(int key)
{

    pad1_state[key] = 0x40;

}

void input_reset()
{

    input_clear(0);
    input_clear(1);
    input_clear(2);
    input_clear(3);
    input_clear(4);
    input_clear(5);
    input_clear(6);
    input_clear(7);

}

unsigned int input_memread(unsigned int address)
{

    address = pad1_state[pad1_readcount];

    if (pad1_readcount == 7)
        pad1_readcount = 0;
    else
        pad1_readcount++;

    return address;

}

unsigned char ram_read(unsigned int address)
{

    if ((address >= 0x2000 && address < 0x4000) || address == 0x4014)
        return ppu_memread(address);

    if ((address >= 0x4000 && address < 0x4014) || address == 0x4015 || address == 0x4017)
        return apu_memread(address);

    if (address == 0x4016)
        return input_memread(address);

    return memory[address];

}

void ram_write(unsigned int address, unsigned char data)
{

    if (address < 0x2000)
    {

        memory[address] = data;
        memory[address + 2048] = data;
        memory[address + 4096] = data;
        memory[address + 6144] = data;

        return;

    }

    if ((address >= 0x2000 && address < 0x4000) || address == 0x4014)
    {

        memory[address] = ppu_memwrite(address, data);

        return;

    }

    if ((address >= 0x4000 && address < 0x4014) || address == 0x4015 || address == 0x4017)
    {

        memory[address] = apu_memwrite(address, data);

        return;

    }

    if (address == 0x4016)
    {

        memory[address] = 0x40;

        return;

    }

    if (address >= 0x6000 && address < 0x8000)
    {

        if (SRAM == 1)
            backend_writesavefile("game.sav", memory);

        memory[address] = data;

        return;

    }

    if (mapper->access)
        mapper->access(address, data);

    memory[address] = data;

}

static void run(int width, int height, int start_int, int vblank_int, int vblank_timeout, int scanline_refresh)
{

    int counter = 0;
    int scanline = 0;
    unsigned int currenttime;
    unsigned int starttime;
    int deltatime;

    while (running)
    {

        starttime = backend_getticks();

        cpu_execute(start_int, memory);

        memory[PPUSTATUS] |= 0x80;

        counter += cpu_execute(12, memory);

        if (PPUCTRL_VBLANKNMI)
            counter += cpu_nmi(counter, memory);

        counter += cpu_execute(vblank_timeout, memory);
        memory[PPUSTATUS] &= 0x3F;

        ppu_register_v = ppu_register_t;

        backend_lock();

        for (scanline = 0; scanline < height; scanline++)
        {

            if (!PPUSTATUS_SPRITE0HIT)
                ppu_checkspritehit(width, scanline);

            ppu_renderbackground(scanline);

            counter += cpu_execute(scanline_refresh, memory);

            if (mmc3_irq_enable == 1)
            {

                if (scanline == mmc3_irq_counter)
                {

                    cpu_irq(counter, memory);
                    mmc3_irq_counter--;

                }

            }

        }

        ppu_rendersprites();
        backend_unlock();
        backend_update();

        currenttime = backend_getticks();
        deltatime = 16 - (currenttime - starttime);

        if (deltatime > 0)
            backend_delay(deltatime);

        backend_event();

    }

}

void halt()
{

    running = 0;

}

int main(int argc, char **argv)
{

    int pal_speed = 1773447;
    int pal_start_int = 341;
    int pal_vblank_int = pal_speed / 50;
    int pal_vblank_timeout = (313 - 240) * pal_vblank_int / 313;
    int pal_scanline_refresh = pal_vblank_int / 313;
    int width = 256;
    int height = 240;
    int rcb;

    if (argc < 2)
        return 1;

    romfn = argv[1];

    if (rom_load(romfn, &header, memory, ppu_memory) == 1)
        return 1;

    rcb = (header.flags6 - ((header.flags6 >> 4) << 4));
    MIRRORING = (rcb & 1) ? 1 : 0;
    SRAM = (rcb & 2) ? 1 : 0;

    mapper = &mappers[(header.flags6 >> 4) | (header.flags7 & 0xF0)];

    if (mapper->reset)
        mapper->reset();

    if (SRAM == 1)
        backend_readsavefile("game.sav", memory);

    backend_init(width, height);
    cpu_reset(memory);
    input_reset();
    run(width, height, pal_start_int, pal_vblank_int, pal_vblank_timeout, pal_scanline_refresh);

    return 0;

}

