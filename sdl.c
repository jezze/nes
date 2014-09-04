#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL/SDL.h>
#include "nes.h"

SDL_Surface *screen;
SDL_Event event;

static struct
{

    char r;
    char g;
    char b;

} palette[64] = {
    {0x80, 0x80, 0x80}, {0x00, 0x3D, 0xA6}, {0x00, 0x12, 0xB0}, {0x44, 0x00, 0x96},
    {0xA1, 0x00, 0x5E}, {0xC7, 0x00, 0x28}, {0xBA, 0x06, 0x00}, {0x8C, 0x17, 0x00},
    {0x5C, 0x2F, 0x00}, {0x10, 0x45, 0x00}, {0x05, 0x4A, 0x00}, {0x00, 0x47, 0x2E},
    {0x00, 0x41, 0x66}, {0x00, 0x00, 0x00}, {0x05, 0x05, 0x05}, {0x05, 0x05, 0x05},
    {0xC7, 0xC7, 0xC7}, {0x00, 0x77, 0xFF}, {0x21, 0x55, 0xFF}, {0x82, 0x37, 0xFA},
    {0xEB, 0x2F, 0xB5}, {0xFF, 0x29, 0x50}, {0xFF, 0x22, 0x00}, {0xD6, 0x32, 0x00},
    {0xC4, 0x62, 0x00}, {0x35, 0x80, 0x00}, {0x05, 0x8F, 0x00}, {0x00, 0x8A, 0x55},
    {0x00, 0x99, 0xCC}, {0x21, 0x21, 0x21}, {0x09, 0x09, 0x09}, {0x09, 0x09, 0x09},
    {0xFF, 0xFF, 0xFF}, {0x0F, 0xD7, 0xFF}, {0x69, 0xA2, 0xFF}, {0xD4, 0x80, 0xFF},
    {0xFF, 0x45, 0xF3}, {0xFF, 0x61, 0x8B}, {0xFF, 0x88, 0x33}, {0xFF, 0x9C, 0x12},
    {0xFA, 0xBC, 0x20}, {0x9F, 0xE3, 0x0E}, {0x2B, 0xF0, 0x35}, {0x0C, 0xF0, 0xA4},
    {0x05, 0xFB, 0xFF}, {0x5E, 0x5E, 0x5E}, {0x0D, 0x0D, 0x0D}, {0x0D, 0x0D, 0x0D},
    {0xFF, 0xFF, 0xFF}, {0xA6, 0xFC, 0xFF}, {0xB3, 0xEC, 0xFF}, {0xDA, 0xAB, 0xEB},
    {0xFF, 0xA8, 0xF9}, {0xFF, 0xAB, 0xB3}, {0xFF, 0xD2, 0xB0}, {0xFF, 0xEF, 0xA6},
    {0xFF, 0xF7, 0x9C}, {0xD7, 0xE8, 0x95}, {0xA6, 0xED, 0xAF}, {0xA2, 0xF2, 0xDA},
    {0x99, 0xFF, 0xFC}, {0xDD, 0xDD, 0xDD}, {0x11, 0x11, 0x11}, {0x11, 0x11, 0x11}
};

void video_drawpixel(int x, int y, int nescolor)
{

    if ((x >= sdl_screen_width) || (x < 0))
        return;

    if ((y >= sdl_screen_height) || (y < 0))
        return;

    if (!nescolor)
        return;

    Uint32 *bufp = (Uint32 *)screen->pixels + y * screen->w + x;

    *bufp = SDL_MapRGB(screen->format, palette[nescolor].r, palette[nescolor].g, palette[nescolor].b);

}

void video_clear()
{

    int nescolor = ppu_memory[0x3f00];

    SDL_Flip(screen);
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, palette[nescolor].r, palette[nescolor].g, palette[nescolor].b));

}

void video_lock()
{

    if (SDL_MUSTLOCK(screen))
    {

        if (SDL_LockSurface(screen) < 0)
            return;
    }

}

void video_unlock()
{

    if (SDL_MUSTLOCK(screen))
        SDL_UnlockSurface(screen);

}

void video_init()
{

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        exit(1);

    atexit(SDL_Quit);

    screen = SDL_SetVideoMode(sdl_screen_width, sdl_screen_height, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);

    if (screen == NULL)
        exit(1);

}

void video_event()
{

    while (SDL_PollEvent(&event))
    {

        if (event.type == SDL_KEYDOWN)
        {

            switch (event.key.keysym.sym)
            {

            case SDLK_x:
                set_input(0);

                break;

            case SDLK_z:
                set_input(1);

                break;

            case SDLK_LSHIFT:
                set_input(2);

                break;

            case SDLK_LCTRL:
                set_input(3);

                break;

            case SDLK_UP:
                set_input(4);

                break;

            case SDLK_DOWN:
                set_input(5);

                break;

            case SDLK_LEFT:
                set_input(6);

                break;

            case SDLK_RIGHT:
                set_input(7);

                break;

            case SDLK_ESCAPE:
                CPU_is_running = 0;

                break;

            default:
                break;

            }

        }

        if (event.type == SDL_KEYUP)
        {

            switch (event.key.keysym.sym)
            {

            case SDLK_x:
                clear_input(0);

                break;

            case SDLK_z:
                clear_input(1);

                break;

            case SDLK_LSHIFT:
                clear_input(2);

                break;

            case SDLK_LCTRL:
                clear_input(3);

                break;

            case SDLK_UP:
                clear_input(4);

                break;

            case SDLK_DOWN:
                clear_input(5);

                break;

            case SDLK_LEFT:
                clear_input(6);

                break;

            case SDLK_RIGHT:
                clear_input(7);

                break;

            default:
                break;

            }

        }

    }

}

void video_readsavefile(char *name)
{

    FILE *fp = fopen(name, "rb");

    if (fp)
    {

        fseek(fp, 0, SEEK_SET);
        fread(&memory[0x6000], 1, 8192, fp);
        fclose(fp);

    }

}

void video_writesavefile(char *name)
{

    FILE *fp = fopen(name, "wb");

    fwrite(&memory[0x6000], 1, 8192, fp);
    fclose(fp);

}

