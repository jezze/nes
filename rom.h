int rom_parse(char *romfn);
int rom_load(char *romfn, unsigned char *ram_mem, unsigned char *ppu_mem);

extern unsigned char PRG;
extern unsigned char CHR;
extern unsigned char MAPPER;
extern int OS_MIRROR;
extern int FS_MIRROR;
extern int SRAM;
extern int MIRRORING;
