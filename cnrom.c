static void cnrom_switchchr(int bank)
{

    unsigned int address = 0x0000;
    int prg_size = 16384;
    int chr_size = 8192;
    int chr_start = prg_size * PRG;

    memcpy(ppu_memory + address, romcache + 16 + chr_start + (bank * chr_size), chr_size);

}

void cnrom_access(unsigned int address, unsigned char data)
{

    if (address > 0x7fff && address < 0x10000)
        cnrom_switchchr(data & (CHR - 1));

}

