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

extern unsigned char PRG;
extern unsigned char CHR;
extern int OS_MIRROR;
extern int FS_MIRROR;
extern int SRAM;
extern int MIRRORING;
