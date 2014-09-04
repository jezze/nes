#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nes.h"
#include "cpu.h"
#include "ppu.h"
#include "rom.h"
#include "sdl.h"

#include "mmc1.c"
#include "unrom.c"
#include "cnrom.c"
#include "mmc3.c"

static char *romfn;
unsigned char *romcache;
unsigned char *ppu_memory;
int start_int;
int vblank_int;
int vblank_cycle_timeout;
int scanline_refresh;
int CPU_is_running = 1;
int height;
int width;
int sdl_screen_height;
int sdl_screen_width;
long romlen;

static int pad1_state[8];
static int pad1_readcount = 0;

void set_input(int pad_key)
{

    pad1_state[pad_key] = 0x01;

}

void clear_input(int pad_key)
{

    pad1_state[pad_key] = 0x40;

}

void reset_input()
{

    clear_input(0);
    clear_input(1);
    clear_input(2);
    clear_input(3);
    clear_input(4);
    clear_input(5);
    clear_input(6);
    clear_input(7);

}

unsigned char memory_read(unsigned int address)
{

    if (address < 0x2000 || address > 0x7FFF)
        return memory[address];

    if (address == 0x2002)
    {

        ppu_status_tmp = ppu_status;
        ppu_status &= 0x7F;
        write_memory(0x2002,ppu_status);
        ppu_status &= 0x1F;
        write_memory(0x2002,ppu_status);
        ppu_bgscr_f = 0x00;
        ppu_addr_h = 0x00;

        return (ppu_status_tmp & 0xE0) | (ppu_addr_tmp & 0x1F);

    }

    if (address == 0x2007)
    {

        tmp = ppu_addr_tmp;
        ppu_addr_tmp = ppu_addr;

        if (!increment_32)
            ppu_addr++;
        else
            ppu_addr += 0x20;

        return ppu_memory[tmp];

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

void write_memory(unsigned int address,unsigned char data)
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
            video_writesavefile("game.sav");

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

static void start_emulation()
{

    int counter = 0;
    int scanline = 0;

    while (CPU_is_running)
    {

        cpu_execute(start_int);

        ppu_status |= 0x80;
        write_memory(0x2002, ppu_status);

        counter += cpu_execute(12);

        if (exec_nmi_on_vblank)
            counter += cpu_nmi(counter);

        counter += cpu_execute(vblank_cycle_timeout);
        ppu_status &= 0x3F;

        write_memory(0x2002,ppu_status);

        loopyV = loopyT;

        video_lock();

        for (scanline = 0; scanline < 240; scanline++)
        {

            if (!sprite_zero)
                ppu_checkspritehit(scanline);

            ppu_renderbackground(scanline);

            counter += cpu_execute(scanline_refresh);

            if (mmc3_irq_enable == 1)
            {

                if (scanline == mmc3_irq_counter)
                {

                    cpu_irq(counter);
                    mmc3_irq_counter--;

                }

            }

        }

        ppu_rendersprites();
        video_unlock();
        video_clear();
        video_event();

    }

}

int main(int argc, char **argv)
{

    int PAL_SPEED = 1773447;
    int PAL_VBLANK_INT = PAL_SPEED / 50;
    int PAL_SCANLINE_REFRESH = PAL_VBLANK_INT / 313;
    int PAL_VBLANK_CYCLE_TIMEOUT = (313 - 240) * PAL_VBLANK_INT / 313;
    int i;

    if (argc < 2)
    {

        return 1;

    }

    for (i = 0; i < argc; i++)
    {

        if (i == 0)
        {

        }

        else if (i == argc - 1)
        {

            romfn = argv[i];

        }

    }

    memory = (unsigned char *)malloc(65536);
    ppu_memory = (unsigned char *)malloc(16384);

    if (analyze_header(romfn) == 1)
    {

        free(ppu_memory);
        free(memory);
        exit(1);

    }

    romcache = (unsigned char *)malloc(romlen);

    if (load_rom(romfn) == 1)
    {

        free(ppu_memory);
        free(memory);
        free(romcache);
        exit(1);

    }

    if (MAPPER == 4)
        mmc3_reset();

    if (SRAM == 1)
        video_readsavefile("game.sav");

    height = 240;
    width = 256;
    sdl_screen_height = height;
    sdl_screen_width = width;

    video_init();
    cpu_reset();
    reset_input();

    start_int = 341;
    vblank_int = PAL_VBLANK_INT;
    vblank_cycle_timeout = PAL_VBLANK_CYCLE_TIMEOUT;
    scanline_refresh = PAL_SCANLINE_REFRESH;

    start_emulation();
    free(ppu_memory);
    free(memory);
    free(romcache);

    return 0;

}

