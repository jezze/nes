static void unrom_switchprg(int bank)
{

    int prg_size = 16384;
    unsigned int address = 0x8000;

    backend_read(romfn, 16 + (bank * prg_size), prg_size, memory + address);

}

static void unrom_access(unsigned int address, unsigned char data)
{

    if (address > 0x7fff && address < 0x10000)
        unrom_switchprg(data);

}

