#include <stdio.h>
#include "nes.h"

int pad1_DOWN;
int pad1_UP;
int pad1_LEFT;
int pad1_RIGHT;
int pad1_START;
int pad1_SELECT;
int pad1_A;
int pad1_B;

void set_input(int pad_key)
{

    switch (pad_key)
    {

    case 1:
        pad1_DOWN = 0x01;
        break;

    case 2:
        pad1_UP = 0x01;
        break;

    case 3:
        pad1_LEFT = 0x01;
        break;

    case 4:
        pad1_RIGHT = 0x01;
        break;

    case 5:
        pad1_START = 0x01;
        break;

    case 6:
        pad1_SELECT = 0x01;
        break;

    case 7:
        pad1_A = 0x01;
        break;

    case 8:
        pad1_B = 0x01;
        break;

    default:
        break;

    }

}

void clear_input(int pad_key)
{

    switch (pad_key)
    {

    case 1:
        pad1_DOWN = 0x40;
        break;

    case 2:
        pad1_UP = 0x40;
        break;

    case 3:
        pad1_LEFT = 0x40;
        break;

    case 4:
        pad1_RIGHT = 0x40;
        break;

    case 5:
        pad1_START = 0x40;
        break;

    case 6:
        pad1_SELECT = 0x40;
        break;

    case 7:
        pad1_A = 0x40;
        break;

    case 8:
        pad1_B = 0x40;
        break;

    default:
        break;

    }

}

void reset_input()
{

    pad1_DOWN = 0x40;
    pad1_UP = 0x40;
    pad1_LEFT = 0x40;
    pad1_RIGHT = 0x40;
    pad1_START = 0x40;
    pad1_SELECT = 0x40;
    pad1_A = 0x40;
    pad1_B = 0x40;

}

