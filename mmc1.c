int mmc1_PRGROM_area_switch;
int mmc1_PRGROM_bank_switch;
int mmc1_CHRROM_bank_switch;
int mmc1_reg0_data = 0;
int mmc1_reg1_data = 0;
int mmc1_reg2_data = 0;
int mmc1_reg3_data = 0;
int mmc1_reg0_bitcount = 0;
int mmc1_reg1_bitcount = 0;
int mmc1_reg2_bitcount = 0;
int mmc1_reg3_bitcount = 0;

void mmc1_switch_prg(int bank, int pagesize, int area)
{

    int prg_size;
    unsigned int address;

    if (pagesize == 0)
    {

        prg_size = 32768;
        address = 0x8000;

    }
    
    else if (pagesize == 1)
    {

        prg_size = 16384;

        if (area == 0)
        {

            address = 0xc000;

        }
        
        else if (area == 1)
        {

            address = 0x8000;

        }

        else
        {

            fprintf(stdout,"[!] error prg area selection incorrect!\n");
            exit(1);

        }

    }
    
    else
    {

        fprintf(stdout,"[!] error prg pagesize incorrect!\n");
        exit(1);

    }

    memcpy(memory + address, romcache + 16 + (bank * prg_size), prg_size); 

}

void mmc1_switch_chr(int bank, int pagesize, int area)
{

    int prg_size = 16384;
    int chr_start = prg_size * PRG;
    int chr_size;
    unsigned int address;

    if (pagesize == 0)
    {

        chr_size = 8192;
        address = 0x0000;

    }
    
    else if (pagesize == 1)
    {

        chr_size = 4096;

        if (area == 0)
        {

            address = 0x0000;

        }
        
        else if (area == 1)
        {

            address = 0x1000;

        }
        
        else
        {

            fprintf(stdout,"[!] error chr area selection incorrect!\n");
            exit(1);

        }

    }
    
    else
    {

        fprintf(stdout,"[!] error chr pages incorrect!\n");
        exit(1);

    }

    memcpy(ppu_memory + address, romcache + 16 + chr_start + (bank * chr_size), chr_size);

}

void mmc1_access(unsigned int address,unsigned char data)
{

    if (address > 0x7fff && address < 0xa000)
    {

        if (data & 0x80)
        {

            mmc1_reg0_data = 0;
            mmc1_reg0_bitcount = 0;

        }
        
        else
        {

            mmc1_reg0_data |= data;
            mmc1_reg0_bitcount++;

        }

        if (mmc1_reg0_bitcount == 5)
        {

            if (mmc1_reg0_data & 0x01)
                MIRRORING = 0;
            else
                MIRRORING = 1;

            if (mmc1_reg0_data & 0x02)
                OS_MIRROR = 1;
            else
                OS_MIRROR = 0;

            if (mmc1_reg0_data & 0x04)
                mmc1_PRGROM_area_switch = 1;
            else
                mmc1_PRGROM_area_switch = 0;

            if (mmc1_reg0_data & 0x08)
                mmc1_PRGROM_bank_switch = 1;
            else
                mmc1_PRGROM_bank_switch = 0;

            if (mmc1_reg0_data & 0x10)
                mmc1_CHRROM_bank_switch = 1;
            else
                mmc1_CHRROM_bank_switch = 0;

            mmc1_reg0_data = 0;
            mmc1_reg0_bitcount = 0;

        }

    }

    if (address > 0x9fff && address < 0xc000)
    {

        if (data & 0x80)
        {

            mmc1_reg1_data = 0;
            mmc1_reg1_bitcount = 0;

        }
        
        else
        {

            mmc1_reg1_bitcount++;
            mmc1_reg1_data |= (data & 0x01) << mmc1_reg1_bitcount;

        }

        if (mmc1_reg1_bitcount == 5)
        {

            if (mmc1_reg1_data != 0x00)
                mmc1_switch_chr(mmc1_reg1_data >> 1, mmc1_CHRROM_bank_switch, 0);

            mmc1_reg1_data = 0;
            mmc1_reg1_bitcount = 0;

        }

    }

    if (address > 0xbfff && address < 0xe000)
    {

        if (data & 0x80)
        {

            mmc1_reg2_data = 0;
            mmc1_reg2_bitcount = 0;

        }
        
        else
        {

            mmc1_reg2_bitcount++;
            mmc1_reg2_data |= (data & 0x01) << mmc1_reg2_bitcount;

        }

        if (mmc1_reg2_bitcount == 5)
        {

            if (mmc1_reg2_data != 0x00)
                mmc1_switch_chr(mmc1_reg2_data >> 1, mmc1_CHRROM_bank_switch, 1);

            mmc1_reg2_data = 0;
            mmc1_reg2_bitcount = 0;

        }

    }

    if (address > 0xdfff && address < 0x10000)
    {

        if (data & 0x80)
        {

            mmc1_reg3_data = 0;
            mmc1_reg3_bitcount = 0;

        }
        
        else
        {

            mmc1_reg3_bitcount++;
            mmc1_reg3_data |= (data & 0x01) << mmc1_reg3_bitcount;

        }

        if (mmc1_reg3_bitcount == 5)
        {

            mmc1_switch_prg(mmc1_reg3_data >> 1, mmc1_PRGROM_bank_switch, mmc1_PRGROM_area_switch);

            mmc1_reg3_data = 0;
            mmc1_reg3_bitcount = 0;

        }

    }

}

