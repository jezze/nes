static void cnrom_switchchr(int bank)
{

    int size = 8192;

    backend_read(romfn, 16 + (16384 * PRG) + (bank * size), size, ppu_memory);

}

static void cnrom_access(unsigned int address, unsigned char data)
{

    if (address > 0x7fff && address < 0x10000)
        cnrom_switchchr(data & (CHR - 1));

}

