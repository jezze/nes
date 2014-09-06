#define PPUCTRL                         0x2000
#define PPUMASK                         0x2001
#define PPUSTATUS                       0x2002
#define PPUOAMADDR                      0x2003
#define PPUOAMDATA                      0x2004
#define PPUSCROLL                       0x2005
#define PPUADDR                         0x2006
#define PPUDATA                         0x2007
#define PPUCTRL_NAMETABLE               (ppu_ctrl & 0x00)
#define PPUCTRL_INCREMENT               (ppu_ctrl & 0x04)
#define PPUCTRL_SPRITEHI                (ppu_ctrl & 0x08)
#define PPUCTRL_BACKGROUNDHI            (ppu_ctrl & 0x10)
#define PPUCTRL_SPRITE16                (ppu_ctrl & 0x20)
#define PPUCTRL_VBLANKNMI               (ppu_ctrl & 0x80)
#define PPUMASK_MONOCHROME              (ppu_mask & 0x01)
#define PPUMASK_BACKGROUNDCLIP          (ppu_mask & 0x02)
#define PPUMASK_SPRITECLIP              (ppu_mask & 0x04)
#define PPUMASK_BACKGROUNDSHOW          (ppu_mask & 0x08)
#define PPUMASK_SPRITESHOW              (ppu_mask & 0x10)
#define PPUSTATUS_VRAMWRITEFLAG         (ppu_status & 0x10)
#define PPUSTATUS_SPRITEOVERFLOW        (ppu_status & 0x20)
#define PPUSTATUS_SPRITE0HIT            (ppu_status & 0x40)
#define PPUSTATUS_VBLANKON              (ppu_status & 0x80)

static unsigned char ppu_memory[16384];
static unsigned int ppu_ctrl = 0x00;
static unsigned int ppu_mask = 0x00;
static unsigned int ppu_addr_h = 0x00;
static unsigned int ppu_addr = 0x2000;
static unsigned int ppu_addr_tmp = 0x2000;
static unsigned int loopyT = 0x00;
static unsigned int loopyV = 0x00;
static unsigned int loopyX = 0x00;
static unsigned int ppu_status;
static unsigned int ppu_status_tmp = 0x00;
static unsigned int ppu_bgscr_f = 0x00;
static unsigned int sprite_address = 0x00;
static unsigned char sprite_memory[256];
static unsigned char bgcache[256 + 8][256 + 8];
static unsigned char sprcache[256 + 8][256 + 8];

static unsigned char ppu_memread(unsigned int address)
{

    if (address == PPUSTATUS)
    {

        ppu_status_tmp = ppu_status;
        ppu_status &= 0x7F;
        ram_write(PPUSTATUS, ppu_status);
        ppu_status &= 0x1F;
        ram_write(PPUSTATUS, ppu_status);
        ppu_bgscr_f = 0x00;
        ppu_addr_h = 0x00;

        return (ppu_status_tmp & 0xE0) | (ppu_addr_tmp & 0x1F);

    }

    if (address == PPUDATA)
    {

        unsigned int old = ppu_addr_tmp;

        ppu_addr_tmp = ppu_addr;

        if (!PPUCTRL_INCREMENT)
            ppu_addr++;
        else
            ppu_addr += 0x20;

        return ppu_memory[old];

    }

    return memory[address];

}

static unsigned char ppu_memwrite(unsigned int address, unsigned char data)
{

    if (address == PPUCTRL)
    {

        ppu_addr_tmp = data;
        ppu_ctrl = data;
        loopyT &= 0xf3ff;
        loopyT |= (data & 3) << 10;

        return data;

    }

    if (address == PPUMASK)
    {

        ppu_addr_tmp = data;
        ppu_mask = data;

        return data;

    }

    if (address == PPUSTATUS)
    {

        return data;

    }

    if (address == PPUOAMADDR)
    {

        ppu_addr_tmp = data;
        sprite_address = data;

        return data;

    }

    if (address == PPUOAMDATA)
    {

        ppu_addr_tmp = data;
        sprite_memory[sprite_address] = data;
        sprite_address++;

        return data;

    }

    if (address == PPUSCROLL)
    {

        ppu_addr_tmp = data;

        if (ppu_bgscr_f == 0x00)
        {

            loopyT &= 0xFFE0;
            loopyT |= (data & 0xF8) >> 3;
            loopyX = data & 0x07;
            ppu_bgscr_f = 0x01;

            return data;

        }

        if (ppu_bgscr_f == 0x01)
        {

            loopyT &= 0xFC1F;
            loopyT |= (data & 0xF8) << 2;
            loopyT &= 0x8FFF;
            loopyT |= (data & 0x07) << 12;
            ppu_bgscr_f = 0x00;

            return data;

        }

    }

    if (address == PPUADDR)
    {

        ppu_addr_tmp = data;

        if (ppu_addr_h == 0x00)
        {

            ppu_addr = (data << 8);
            loopyT &= 0x00FF;
            loopyT |= (data & 0x3F);
            ppu_addr_h = 0x01;

            return data;

        }

        if (ppu_addr_h == 0x01)
        {

            ppu_addr |= data;
            loopyT &= 0xFF00;
            loopyT |= data;
            loopyV = loopyT;
            ppu_addr_h = 0x00;

            return data;

        }

    }

    if (address == PPUDATA)
    {

        if (PPUSTATUS_VRAMWRITEFLAG)
            return data;

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

        if (!PPUCTRL_INCREMENT)
            ppu_addr++;
        else
            ppu_addr += 0x20;

        return data;

    }

    if (address == 0x4014)
    {

        unsigned int i;

        for (i = 0; i < 256; i++)
            sprite_memory[i] = memory[0x100 * data + i];

        return data;

    }

    return data;

}

static void ppu_checkspritehit(int width, int scanline)
{

    int i;

    for (i = 0; i < width; i++)
    {

        if ((bgcache[i][scanline - 1] > 0x00) && (sprcache[i][scanline - 1] > 0x00))
            ppu_status |= 0x40;

    }

}

static void ppu_renderbackground(int scanline)
{

    int tile_count;
    int i;
    int nt_addr;
    int at_addr;
    int x_scroll;
    int y_scroll;
    int attribs;
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
        int pt_addr = (ppu_memory[nt_addr] << 4) + ((loopyV & 0x7000) >> 12);
        char a1, a2;

        if (PPUCTRL_BACKGROUNDHI)
            pt_addr += 0x1000;

        a1 = ppu_memory[pt_addr];
        a2 = ppu_memory[pt_addr + 8];

        for (i = 0; i < 8; i++)
        {

            tile[i] = 0;

            if (a1 & (0x80 >> i))
                tile[i] += 1;

            if (a2 & (0x80 >> i))
                tile[i] += 2;

            if (tile[i])
                tile[i] += attribs;

        }

        if ((tile_count == 0) && (loopyX != 0))
        {

            for (i = 0; i < 8 - loopyX; i++)
            {

                bgcache[ttc + i][scanline] = tile[loopyX + i];

                if (PPUMASK_BACKGROUNDSHOW)
                    backend_drawpixel(ttc + i, scanline, ppu_memory[0x3f00 + (tile[loopyX + i])]);

            }

        }

        else if ((tile_count == 32) && (loopyX != 0))
        {

            for (i = 0; i < loopyX; i++)
            {

                bgcache[ttc + i - loopyX][scanline] = tile[i];

                if (PPUMASK_BACKGROUNDSHOW)
                    backend_drawpixel(ttc + i - loopyX, scanline, ppu_memory[0x3f00 + (tile[i])]);

            }

        }

        else
        {

            for (i = 0; i < 8; i++)
            {

                bgcache[ttc + i - loopyX][scanline] = tile[i];

                if (PPUMASK_BACKGROUNDSHOW)
                    backend_drawpixel(ttc + i - loopyX, scanline, ppu_memory[0x3f00 + (tile[i])]);

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

static void ppu_rendersprite(int y, int x, int pattern_number, int attribs, int spr_nr)
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

    sprite_pattern_table = (!PPUCTRL_SPRITE16 && PPUCTRL_SPRITEHI) ? 0x1000 : 0x0000;
    sprite_start = sprite_pattern_table + ((pattern_number << 3) << 1);
    imax = 8;
    jmax = (PPUCTRL_SPRITE16) ? 16 : 8;

    if ((!flip_spr_hor) && (!flip_spr_ver))
    {

        for (i = imax - 1; i >= 0; i--)
        {

            for (j = 0; j < jmax; j++)
            {

                bit1[(imax - 1) - i][j] = ((ppu_memory[sprite_start + j] >> i) & 0x01) ? 1 : 0;
                bit2[(imax - 1) - i][j] = ((ppu_memory[sprite_start + 8 + j] >> i) & 0x01) ? 1 : 0;

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

            if (spr_nr == 0)
                sprcache[x + i][y + j] = sprite[i][j];

            if (sprite[i][j] != 0)
            {

                if (!disp_spr_back)
                {

                    if (PPUMASK_BACKGROUNDSHOW)
                        backend_drawpixel(x + i, y + j, ppu_memory[0x3f10 + (sprite[i][j])]);

                }

                else
                {

                    if (bgcache[x + i][y + j] == 0)
                        if (PPUMASK_BACKGROUNDSHOW)
                            backend_drawpixel(x + i, y + j, ppu_memory[0x3f10 + (sprite[i][j])]);

                }

            }

        }

    }

}

static void ppu_rendersprites()
{

    int i = 0;

    memset(sprcache, 0x00, sizeof (sprcache));

    for (i = 63; i >= 0; i--)
        ppu_rendersprite(sprite_memory[i * 4], sprite_memory[i * 4 + 3], sprite_memory[i * 4 + 1], sprite_memory[i * 4 + 2], i);

}

