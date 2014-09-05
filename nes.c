#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nes.h"
#include "cpu.h"
#include "rom.h"
#include "sdl.h"

long romlen;
static unsigned char *romcache;
static unsigned char memory[65536];
static int height;
static int width;
static int pad1_state[8];
static int pad1_readcount = 0;
static int running = 1;
static char *romfn;

#include "ppu.c"
#include "mmc1.c"
#include "unrom.c"
#include "cnrom.c"
#include "mmc3.c"

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

unsigned char ram_read(unsigned int address)
{

    if (address < 0x2000 || address > 0x7FFF)
        return memory[address];

    if (address == 0x2002)
    {

        ppu_status_tmp = ppu_status;
        ppu_status &= 0x7F;
        ram_write(0x2002, ppu_status);
        ppu_status &= 0x1F;
        ram_write(0x2002, ppu_status);
        ppu_bgscr_f = 0x00;
        ppu_addr_h = 0x00;

        return (ppu_status_tmp & 0xE0) | (ppu_addr_tmp & 0x1F);

    }

    if (address == 0x2007)
    {

        unsigned int old = ppu_addr_tmp;

        ppu_addr_tmp = ppu_addr;

        if (!increment_32)
            ppu_addr++;
        else
            ppu_addr += 0x20;

        return ppu_memory[old];

    }

    if (address == 0x4015)
    {

        return memory[address];

    }

    if (address == 0x4016)
    {

        memory[address] = pad1_state[pad1_readcount];

        switch (pad1_readcount)
        {

        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            pad1_readcount++;

            break;

        case 7:
            pad1_readcount = 0;

            break;

        }

        return memory[address];

    }

    if (address == 0x4017)
    {

        return memory[address];

    }

    return memory[address];

}

void ram_write(unsigned int address,unsigned char data)
{

    if (address == 0x2002)
    {

        memory[address] = data;

        return;

    }

    if (address > 0x1fff && address < 0x4000)
    {

        ppu_memwrite(address, data);

        return;

    }

    if (address == 0x4014)
    {

        ppu_memwrite(address, data);

        return;

    }

    if (address == 0x4016)
    {

        memory[address] = 0x40;

        return;

    }

    if (address == 0x4017)
    {

        memory[address] = 0x48;

        return;

    }

    if (address > 0x3fff && address < 0x4016)
    {

        memory[address] = data;

        return;

    }

    if (address > 0x5fff && address < 0x8000)
    {

        if (SRAM == 1)
            backend_writesavefile("game.sav", memory);

        memory[address] = data;

        return;

    }

    if (address < 0x2000)
    {

        memory[address] = data;
        memory[address + 2048] = data;
        memory[address + 4096] = data;
        memory[address + 6144] = data;

        return;

    }

    if (MAPPER == 1)
    {

        mmc1_access(address, data);

        return;

    }

    if (MAPPER == 2)
    {

        unrom_access(address, data);

        return;

    }

    if (MAPPER == 3)
    {

        cnrom_access(address, data);

        return;

    }

    if (MAPPER == 4)
    {

        mmc3_access(address, data);

        return;

    }

    memory[address] = data;

}

static void run(int start_int, int vblank_int, int vblank_timeout, int scanline_refresh)
{

    int counter = 0;
    int scanline = 0;

    while (running)
    {

        cpu_execute(start_int, memory);

        ppu_status |= 0x80;
        ram_write(0x2002, ppu_status);

        counter += cpu_execute(12, memory);

        if (exec_nmi_on_vblank)
            counter += cpu_nmi(counter, memory);

        counter += cpu_execute(vblank_timeout, memory);
        ppu_status &= 0x3F;

        ram_write(0x2002, ppu_status);

        loopyV = loopyT;

        backend_lock();

        for (scanline = 0; scanline < 240; scanline++)
        {

            if (!sprite_zero)
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
        backend_delay(6);
        backend_unlock();
        backend_clear(ppu_memory[0x3f00]);
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

    if (argc < 2)
        return 1;

    romfn = argv[1];

    if (rom_parse(romfn) == 1)
        return 1;

    romcache = (unsigned char *)malloc(romlen);

    if (rom_load(romfn, romcache, memory, ppu_memory) == 1)
    {

        free(romcache);

        return 1;

    }

    if (MAPPER == 4)
        mmc3_reset();

    if (SRAM == 1)
        backend_readsavefile("game.sav", memory);

    height = 240;
    width = 256;

    backend_init(width, height);
    cpu_reset(memory);
    input_reset();
    run(pal_start_int, pal_vblank_int, pal_vblank_timeout, pal_scanline_refresh);
    free(romcache);

    return 0;

}

