#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "nes.h"
#include "cpu.h"
#include "ppu.h"
#include "rom.h"
#include "input.h"
#include "sdl.h"

#include "mmc1.c"
#include "unrom.c"
#include "cnrom.c"
#include "mmc3.c"

char romfn[256];
unsigned char *romcache;
unsigned char *ppu_memory;
int pad1_readcount = 0;
int start_int;
int vblank_int;
int vblank_cycle_timeout;
int scanline_refresh;
int CPU_is_running = 1;
int height;
int width;
int sdl_screen_height;
int sdl_screen_width;
char *savfile = "game.sav";
long romlen;
FILE *sav_fp;

static void read_sav()
{

    sav_fp = fopen(savfile, "rb");

    if (sav_fp)
    {

        fseek(sav_fp, 0, SEEK_SET);
        fread(&memory[0x6000], 1, 8192, sav_fp);
        fclose(sav_fp);

    }

}

static void write_sav()
{

    sav_fp = fopen(savfile, "wb");

    fwrite(&memory[0x6000], 1, 8192, sav_fp);
    fclose(sav_fp);

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

        switch (pad1_readcount)
        {

        case 0:
            memory[address] = pad1_A;
            pad1_readcount++;

            break;

        case 1:
            memory[address] = pad1_B;
            pad1_readcount++;

            break;

        case 2:
            memory[address] = pad1_SELECT;
            pad1_readcount++;

            break;

        case 3:
            memory[address] = pad1_START;
            pad1_readcount++;

            break;

        case 4:
            memory[address] = pad1_UP;
            pad1_readcount++;

            break;

        case 5:
            memory[address] = pad1_DOWN;
            pad1_readcount++;

            break;

        case 6:
            memory[address] = pad1_LEFT;
            pad1_readcount++;

            break;

        case 7:
            memory[address] = pad1_RIGHT;
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

        write_ppu_memory(address,data);

        return;

    }

    if (address == 0x4014)
    {

        write_ppu_memory(address,data);

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
            write_sav();

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

        screen_lock();

        for (scanline = 0; scanline < 240; scanline++)
        {

            if (!sprite_zero)
                check_sprite_hit(scanline);

            render_background(scanline);

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

        render_sprites();
        screen_unlock();
        update_screen();
        check_SDL_event();

    }

}

void reset_emulation()
{

    if (load_rom(romfn) == 1)
    {

        free(ppu_memory);
        free(memory);
        free(romcache);

        exit(1);

    }

    if (MAPPER == 4)
        mmc3_reset();

    cpu_reset();
    reset_input();
    start_emulation();

}

void quit_emulation()
{

    free(ppu_memory);
    free(memory);
    free(romcache);
    exit(0);

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

            /* do nothing */

        }
        
        else if (i == argc - 1)
        {

            snprintf(romfn, sizeof(romfn), "%s", argv[i]);

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
        read_sav();

    height = 240;
    width = 256;
    sdl_screen_height = height;
    sdl_screen_width = width;

    printf("[*] PAL_SPEED: %d\n",PAL_SPEED);
    printf("[*] PAL_VBLANK_INT: %d\n",PAL_VBLANK_INT);
    printf("[*] PAL_SCANLINE_REFRESH: %d\n",PAL_SCANLINE_REFRESH);
    printf("[*] PAL_VBLANK_CYCLE_TIMEOUT: %d\n",PAL_VBLANK_CYCLE_TIMEOUT);
    printf("[*] height * PAL_SCANLINE_REFRESH: %d\n",(height * PAL_SCANLINE_REFRESH) + PAL_VBLANK_CYCLE_TIMEOUT + 341);

    init_SDL(0);

    cpu_reset();
    reset_input();

    start_int = 341;
    vblank_int = PAL_VBLANK_INT;
    vblank_cycle_timeout = PAL_VBLANK_CYCLE_TIMEOUT;
    scanline_refresh = PAL_SCANLINE_REFRESH;

    start_emulation();

    return 0;

}

