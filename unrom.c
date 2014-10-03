static void unrom_switchprg(int bank)
{

    int size = 16384;

    backend_read(romfn, 16 + (bank * size), size, memory + 0x8000);

}

static void unrom_access(unsigned int address, unsigned char data)
{

    if (address > 0x7fff && address < 0x10000)
        unrom_switchprg(data);

}

static void unrom_reset()
{

}

