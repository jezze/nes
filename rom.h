enum flags6
{

    FLAGS6_HMIRROR                      = (1 << 0),
    FLAGS6_SRAM                         = (1 << 1),
    FLAGS6_TRAINER                      = (1 << 2)

};

enum flags7
{

    FLAGS7_UNISYSTEM                    = (1 << 0),
    FLAGS7_PLAYCHOICE                   = (1 << 1),
    FLAGS7_FORMAT                       = (2 << 2)

};

enum flags9
{

    FLAGS9_PAL                          = (1 << 0)

};

enum flags10
{

    FLAGS10_PAL                         = (1 << 1),
    FLAGS10_SRAM                        = (1 << 4),
    FLAGS10_BUSCONFLICT                 = (1 << 5)

};

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

int rom_load(char *romfn, struct nes_header *header, unsigned char *ram_mem, unsigned char *ppu_mem);
