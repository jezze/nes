#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL/SDL.h>
#include "nes.h"
#include "cpu.h"
#include "ppu.h"
#include "rom.h"
#include "sdl.h"

struct
{

    char r;
    char g;
    char b;

} palette[64] = {
    {0x80,0x80,0x80},{0x00,0x3D,0xA6},{0x00,0x12,0xB0},{0x44,0x00,0x96},
    {0xA1,0x00,0x5E},{0xC7,0x00,0x28},{0xBA,0x06,0x00},{0x8C,0x17,0x00},
    {0x5C,0x2F,0x00},{0x10,0x45,0x00},{0x05,0x4A,0x00},{0x00,0x47,0x2E},
    {0x00,0x41,0x66},{0x00,0x00,0x00},{0x05,0x05,0x05},{0x05,0x05,0x05},
    {0xC7,0xC7,0xC7},{0x00,0x77,0xFF},{0x21,0x55,0xFF},{0x82,0x37,0xFA},
    {0xEB,0x2F,0xB5},{0xFF,0x29,0x50},{0xFF,0x22,0x00},{0xD6,0x32,0x00},
    {0xC4,0x62,0x00},{0x35,0x80,0x00},{0x05,0x8F,0x00},{0x00,0x8A,0x55},
    {0x00,0x99,0xCC},{0x21,0x21,0x21},{0x09,0x09,0x09},{0x09,0x09,0x09},
    {0xFF,0xFF,0xFF},{0x0F,0xD7,0xFF},{0x69,0xA2,0xFF},{0xD4,0x80,0xFF},
    {0xFF,0x45,0xF3},{0xFF,0x61,0x8B},{0xFF,0x88,0x33},{0xFF,0x9C,0x12},
    {0xFA,0xBC,0x20},{0x9F,0xE3,0x0E},{0x2B,0xF0,0x35},{0x0C,0xF0,0xA4},
    {0x05,0xFB,0xFF},{0x5E,0x5E,0x5E},{0x0D,0x0D,0x0D},{0x0D,0x0D,0x0D},
    {0xFF,0xFF,0xFF},{0xA6,0xFC,0xFF},{0xB3,0xEC,0xFF},{0xDA,0xAB,0xEB},
    {0xFF,0xA8,0xF9},{0xFF,0xAB,0xB3},{0xFF,0xD2,0xB0},{0xFF,0xEF,0xA6},
    {0xFF,0xF7,0x9C},{0xD7,0xE8,0x95},{0xA6,0xED,0xAF},{0xA2,0xF2,0xDA},
    {0x99,0xFF,0xFC},{0xDD,0xDD,0xDD},{0x11,0x11,0x11},{0x11,0x11,0x11}
};

unsigned int ppu_control1 = 0x00;
unsigned int ppu_control2 = 0x00;
unsigned int ppu_addr_h = 0x00;
unsigned int ppu_addr = 0x2000;
unsigned int ppu_addr_tmp = 0x2000;
unsigned int loopyT = 0x00;
unsigned int loopyV = 0x00;
unsigned int loopyX = 0x00;
unsigned int ppu_status;
unsigned int ppu_status_tmp = 0x00;
unsigned int ppu_bgscr_f = 0x00;
static unsigned int sprite_address = 0x00;
static unsigned char sprite_memory[256];
static unsigned char bgcache[256 + 8][256 + 8];
static unsigned char sprcache[256 + 8][256 + 8];

void write_ppu_memory(unsigned int address, unsigned char data)
{

    int i;

    if (address == 0x2000)
    {

        ppu_addr_tmp = data;
        ppu_control1 = data;
        memory[address] = data;
        loopyT &= 0xf3ff;
        loopyT |= (data & 3) << 10;

        return;

    }

    if (address == 0x2001)
    {

        ppu_addr_tmp = data;
        ppu_control2 = data;
        memory[address] = data;

        return;

    }

    if (address == 0x2003)
    {

        ppu_addr_tmp = data;
        sprite_address = data;
        memory[address] = data;

        return;

    }

    if (address == 0x2004)
    {

        ppu_addr_tmp = data;
        sprite_memory[sprite_address] = data;
        sprite_address++;
        memory[address] = data;

        return;

    }

    if (address == 0x2005)
    {

        ppu_addr_tmp = data;

        if (ppu_bgscr_f == 0x00)
        {

            loopyT &= 0xFFE0;
            loopyT |= (data & 0xF8) >> 3;
            loopyX = data & 0x07;
            ppu_bgscr_f = 0x01;
            memory[address] = data;

            return;

        }

        if (ppu_bgscr_f == 0x01)
        {

            loopyT &= 0xFC1F;
            loopyT |= (data & 0xF8) << 2;
            loopyT &= 0x8FFF;
            loopyT |= (data & 0x07) << 12;
            ppu_bgscr_f = 0x00;
            memory[address] = data;

            return;

        }

    }

    if (address == 0x2006)
    {

        ppu_addr_tmp = data;

        if (ppu_addr_h == 0x00)
        {

            ppu_addr = (data << 8);
            loopyT &= 0x00FF;
            loopyT |= (data & 0x3F);
            ppu_addr_h = 0x01;
            memory[address] = data;

            return;

        }

        if (ppu_addr_h == 0x01)
        {

            ppu_addr |= data;
            loopyT &= 0xFF00;
            loopyT |= data;
            loopyV = loopyT;
            ppu_addr_h = 0x00;
            memory[address] = data;

            return;

        }

    }

    if (address == 0x2007)
    {

        if (vram_write_flag)
            return;

        ppu_addr_tmp = data;
        ppu_memory[ppu_addr] = data;

        if ((ppu_addr > 0x1999) && (ppu_addr < 0x3000))
        {

            if (OS_MIRROR == 1)
            {

                ppu_memory[ppu_addr + 0x400] = data;
                ppu_memory[ppu_addr + 0x800] = data;
                ppu_memory[ppu_addr + 0x1200] = data;

            }
            
            else if (FS_MIRROR == 1)
            {

                printf("FS_MIRRORING detected! do nothing\n");

            }
            
            else
            {

                if (MIRRORING == 0)
                    ppu_memory[ppu_addr + 0x400] = data;
                else
                    ppu_memory[ppu_addr + 0x800] = data;

            }

        }

        if (ppu_addr == 0x3f10)
            ppu_memory[0x3f00] = data;

        ppu_addr_tmp = ppu_addr;

        if (!increment_32)
            ppu_addr++;
        else
            ppu_addr += 0x20;

        memory[address] = data;

        return;

    }

    if (address == 0x4014)
    {

        for (i = 0; i < 256; i++)
            sprite_memory[i] = memory[0x100 * data + i];

    }

}

void draw_pixel(int x, int y, int nescolor)
{

    if ((x >= sdl_screen_width) || (x < 0))
        return;

    if ((y >= sdl_screen_height) || (y < 0))
        return;

    if (!nescolor)
        return;

    Uint32 *bufp = (Uint32 *)screen->pixels + y * screen->w + x;

    *bufp = SDL_MapRGB(screen->format, palette[nescolor].r, palette[nescolor].g, palette[nescolor].b);

}

void render_background(int scanline)
{

    int tile_count;
    int i;
    int nt_addr;
    int at_addr;
    int x_scroll;
    int y_scroll;
    int pt_addr;
    int attribs;
    unsigned char bit1[8];
    unsigned char bit2[8];
    unsigned char tile[8];

    loopyV &= 0xfbe0;
    loopyV |= (loopyT & 0x041f);
    x_scroll = (loopyV & 0x1f);
    y_scroll = (loopyV & 0x03e0) >> 5;
    nt_addr = 0x2000 + (loopyV & 0x0fff);
    at_addr = 0x2000 + (loopyV & 0x0c00) + 0x03c0 + ((y_scroll & 0xfffc) << 1) + (x_scroll >> 2);

    if ((y_scroll & 0x0002) == 0)
    {

        if ((x_scroll & 0x0002) == 0)
            attribs = (ppu_memory[at_addr] & 0x03) << 2;
        else
            attribs = (ppu_memory[at_addr] & 0x0C);

    }
    
    else
    {

        if ((x_scroll & 0x0002) == 0)
            attribs = (ppu_memory[at_addr] & 0x30) >> 2;
        else
            attribs = (ppu_memory[at_addr] & 0xC0) >> 4;

    }

    for (tile_count = 0; tile_count < 33; tile_count++)
    {

        int ttc = tile_count << 3;

        pt_addr = (ppu_memory[nt_addr] << 4) + ((loopyV & 0x7000) >> 12);

        if (background_addr_hi)
            pt_addr += 0x1000;

        for (i = 7; i >= 0; i--)
        {

            bit1[7 - i] = ((ppu_memory[pt_addr] >> i) & 0x01) ? 1 : 0;
            bit2[7 - i] = ((ppu_memory[pt_addr + 8] >> i) & 0x01) ? 1 : 0;

        }

        for (i = 0; i < 8; i++)
        {

            if ((bit1[i] == 0) && (bit2[i] == 0))
                tile[i] = 0;
            else if ((bit1[i] == 1) && (bit2[i] == 0))
                tile[i] = (1 + attribs);
            else if ((bit1[i] == 0) && (bit2[i] == 1))
                tile[i] = (2 + attribs);
            else if ((bit1[i] == 1) && (bit2[i] == 1))
                tile[i] = (3 + attribs);

        }

        if ((tile_count == 0) && (loopyX != 0))
        {

            for (i = 0; i < 8 - loopyX; i++)
            {

                bgcache[ttc + i][scanline] = tile[loopyX + i];
                draw_pixel(ttc + i, scanline, ppu_memory[0x3f00 + (tile[loopyX + i])]);

            }

        }

        else if ((tile_count == 32) && (loopyX != 0))
        {

            for (i = 0; i < loopyX; i++)
            {

                bgcache[ttc + i - loopyX][scanline] = tile[i];
                draw_pixel(ttc + i - loopyX, scanline, ppu_memory[0x3f00 + (tile[i])]);

            }

        }
        
        else
        {

            for (i = 0; i < 8; i++)
            {

                bgcache[ttc + i - loopyX][scanline] = tile[i];
                draw_pixel(ttc + i - loopyX, scanline, ppu_memory[0x3f00 + (tile[i])]);

            }

        }

        nt_addr++;
        x_scroll++;

        if ((x_scroll & 0x0001) == 0)
        {

            if ((x_scroll & 0x0003) == 0)
            {

                if ((x_scroll & 0x1F) == 0)
                {

                    nt_addr ^= 0x0400;
                    at_addr ^= 0x0400;
                    nt_addr -= 0x0020;
                    at_addr -= 0x0008;
                    x_scroll -= 0x0020;

                }

                at_addr++;

            }

            if ((y_scroll & 0x0002) == 0)
            {

                if ((x_scroll & 0x0002) == 0)
                    attribs = ((ppu_memory[at_addr]) & 0x03) << 2;
                else
                    attribs = ((ppu_memory[at_addr]) & 0x0C);

            }
            
            else
            {

                if ((x_scroll & 0x0002) == 0)
                    attribs = ((ppu_memory[at_addr]) & 0x30) >> 2;
                else
                    attribs = ((ppu_memory[at_addr]) & 0xC0) >> 4;

            }

        }

    }

    if ((loopyV & 0x7000) == 0x7000)
    {

        loopyV &= 0x8fff;

        if ((loopyV & 0x03e0) == 0x03a0)
        {

            loopyV ^= 0x0800;
            loopyV &= 0xfc1f;

        }
        
        else
        {

            if ((loopyV & 0x03e0) == 0x03e0)
                loopyV &= 0xfc1f;
            else
                loopyV += 0x0020;

        }

    }
    
    else
    {

        loopyV += 0x1000;

    }

}

void render_sprite(int y, int x, int pattern_number, int attribs, int spr_nr)
{

    int disp_spr_back = attribs & 0x20;
    int flip_spr_hor = attribs & 0x40;
    int flip_spr_ver = attribs & 0x80;
    int i, imax;
    int j, jmax;
    int sprite_start;
    int sprite_pattern_table;
    unsigned char bit1[8][16];
    unsigned char bit2[8][16];
    unsigned char sprite[8][16];

    sprite_pattern_table = (sprite_addr_hi) ? 0x1000 : 0x0000;
    sprite_start = sprite_pattern_table + ((pattern_number << 3) << 1);
    imax = 8;
    jmax = (sprite_16) ? 16 : 8;

    if ((!flip_spr_hor) && (!flip_spr_ver))
    {

        for (i = imax - 1; i >= 0; i--)
        {

            for (j = 0; j < jmax; j++)
            {

                bit1[(imax - 1) - i] [j] = ((ppu_memory[sprite_start + j] >> i) & 0x01) ? 1 : 0;
                bit2[(imax - 1) - i] [j] = ((ppu_memory[sprite_start + 8 + j] >> i) & 0x01) ? 1 : 0;

            }

        }

    }
 
    else if ((flip_spr_hor) && (!flip_spr_ver))
    {

        for (i = 0; i < imax; i++)
        {

            for (j = 0; j < jmax; j++)
            {

                bit1[i][j] = ((ppu_memory[sprite_start + j] >> i) & 0x01) ? 1 : 0;
                bit2[i][j] = ((ppu_memory[sprite_start + 8 + j] >> i) & 0x01) ? 1 : 0;

            }

        }

    }

    else if ((!flip_spr_hor) && (flip_spr_ver))
    {

        for (i = imax - 1; i >= 0; i--)
        {

            for (j = jmax - 1; j >= 0; j--)
            {

                bit1[(imax - 1) - i][(jmax - 1) - j] = ((ppu_memory[sprite_start + j] >> i) & 0x01) ? 1 : 0;
                bit2[(imax - 1) - i][(jmax - 1) - j] = ((ppu_memory[sprite_start + 8 + j] >> i) & 0x01) ? 1 : 0;

            }

        }

    }

    else if ((flip_spr_hor) && (flip_spr_ver))
    {

        for (i = 0; i < imax; i++)
        {

            for (j = (jmax - 1); j >= 0; j--)
            {

                bit1[i][(jmax - 1) - j] = ((ppu_memory[sprite_start + j] >> i) & 0x01) ? 1 : 0;
                bit2[i][(jmax - 1) - j] = ((ppu_memory[sprite_start + 8 + j] >> i) & 0x01) ? 1 : 0;

            }

        }

    }

    for (i = 0; i < imax; i++)
    {

        for (j = 0; j < jmax; j++)
        {

            if ((bit1[i][j] == 0) && (bit2[i][j] == 0))
                sprite[i][j] = 0;
            else if ((bit1[i][j] == 1) && (bit2[i][j] == 0))
                sprite[i][j] = 1 + ((attribs & 0x03) << 0x02);
            else if ((bit1[i][j] == 0) && (bit2[i][j] == 1))
                sprite[i][j] = 2 + ((attribs & 0x03) << 0x02);
            else if ((bit1[i][j] == 1) && (bit2[i][j] == 1))
                sprite[i][j] = 3 + ((attribs & 0x03) << 0x02);

        }

    }    

    for (i = 0; i < imax; i++)
    {

        for (j = 0; j < jmax; j++)
        {

            if (spr_nr == 0)
                sprcache[x + i][y + j] = sprite[i][j];

            if (sprite[i][j] != 0)
            {

                if (!disp_spr_back)
                {

                    draw_pixel(x + i, y + j, ppu_memory[0x3f10 + (sprite[i][j])]);

                }

                else
                {

                    if (bgcache[x + i][y + j] == 0)
                        draw_pixel(x + i, y + j, ppu_memory[0x3f10 + (sprite[i][j])]);

                }

            }

        }

    }

}

void check_sprite_hit(int scanline)
{

    int i;

    for (i = 0; i < width; i++)
    {

        if ((bgcache[i] [scanline - 1] > 0x00) && (sprcache[i][scanline - 1] > 0x00))
            ppu_status |= 0x40;

    }

}

void render_sprites()
{

    int i = 0;

    memset(sprcache, 0x00, sizeof (sprcache));

    for (i = 63; i >= 0; i--)
        render_sprite(sprite_memory[i * 4], sprite_memory[i * 4 + 3], sprite_memory[i * 4 + 1], sprite_memory[i * 4 + 2], i);

}

void update_screen()
{

    int nescolor = ppu_memory[0x3f00];

    SDL_Flip(screen);
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, palette[nescolor].r, palette[nescolor].g, palette[nescolor].b));

}

