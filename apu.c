static unsigned char apu_memread(unsigned int address)
{

    /*
    if (address == 0x4015)
    {

    }
    */

    return memory[address];

}

static unsigned char apu_memwrite(unsigned int address, unsigned char data)
{

    /*
    if (address == 0x4000 || address == 0x4004)
    {

        unsigned char duty = data & 0xC0;
        unsigned char lcounter = data & 0x20;
        unsigned char cvolume = data & 0x10;
        unsigned char volume = data & 0x0F;

    }

    if (address == 0x4001 || address == 0x4005)
    {

        unsigned char enabled = data & 0x80;
        unsigned char period = data & 0x70;
        unsigned char negate = data & 0x08;
        unsigned char shift = data & 0x07;

    }

    if (address == 0x4002 || address == 0x4006)
    {

        unsigned char timerlo = data;

    }

    if (address == 0x4003 || address == 0x4007)
    {

        unsigned char lcounter = data & 0xF8;
        unsigned char timerhi = data & 0x07;

    }

    if (address == 0x4008)
    {

        unsigned char lcounterctrl = data & 0x80;
        unsigned char lcounterload = data & 0x7F;

    }

    if (address == 0x400A)
    {

        unsigned char timerlo = data;

    }

    if (address == 0x400B)
    {

        unsigned char lcounterload = data & 0xF7;
        unsigned char timerhi = data & 0x08;

    }

    if (address == 0x400C)
    {

        unsigned char lcounterhalt = data & 0x20;
        unsigned char cvolume = data & 0x10;
        unsigned char volume = data & 0x0F;

    }

    if (address == 0x400E)
    {

        unsigned char loopnoise = data & 0x80;
        unsigned char noiseperiod = data & 0x0F;

    }

    if (address == 0x400F)
    {

        unsigned char lcounterload = data & 0xF8;

    }

    if (address == 0x4010)
    {

        unsigned char irqenable = data & 0x80;
        unsigned char loop = data & 0x40;
        unsigned char frequency = data & 0x0F;

    }

    if (address == 0x4011)
    {

        unsigned char loadcounter = data & 0x7F;

    }

    if (address == 0x4012)
    {

        unsigned char sampleaddress = data;

    }

    if (address == 0x4013)
    {

        unsigned char samplelength = data;

    }

    if (address == 0x4015)
    {

        unsigned char enable = data & 0x10;
        unsigned char noise = data & 0x08;
        unsigned char triangle = data & 0x04;
        unsigned char pulse2 = data & 0x02;
        unsigned char pulse1 = data & 0x01;

    }

    if (address == 0x4017)
    {

        unsigned char mode = data & 0x80;
        unsigned char irqinhibit = data & 0x40;

    }
    */

    return memory[address];

}
