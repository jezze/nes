#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "nes.h"
#include "input.h"

SDL_Surface *screen;
SDL_Event event;

void screen_lock()
{

    if (SDL_MUSTLOCK(screen))
    {

        if (SDL_LockSurface(screen) < 0)
            return;
    }

}

void screen_unlock()
{

    if (SDL_MUSTLOCK(screen))
        SDL_UnlockSurface(screen);

}

void init_SDL(int type)
{

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {

        printf("error: unable to init SDL %s\n", SDL_GetError());
        exit(1);

    }

    atexit(SDL_Quit);

    screen = SDL_SetVideoMode(sdl_screen_width, sdl_screen_height, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);

    if (screen == NULL)
    {

        printf("error: unable to set video %s\n", SDL_GetError());
        exit(1);

    }

}

void check_SDL_event()
{

    while (SDL_PollEvent(&event))
    {

        if (event.type == SDL_KEYDOWN)
        {

            switch (event.key.keysym.sym)
            {

                case SDLK_DOWN:
                set_input((char *) 1);
                break;

                case SDLK_UP:
                set_input((char *) 2);
                break;

                case SDLK_LEFT:
                set_input((char *) 3);
                break;

                case SDLK_RIGHT:
                set_input((char *) 4);
                break;

                case SDLK_LCTRL:
                set_input((char *) 5);
                break;

                case SDLK_LSHIFT:
                set_input((char *) 6);
                break;

                case SDLK_x:
                set_input((char *) 7);
                break;

                case SDLK_z:
                set_input((char *) 8);
                break;

                case SDLK_q:
                quit_emulation();
                break;

                case SDLK_ESCAPE:
                CPU_is_running = 0;
                break;

                case SDLK_F1:
                reset_emulation();
                break;

                default:
                break;

            }

        }

        if (event.type == SDL_KEYUP)
        {

            switch (event.key.keysym.sym)
            {

                case SDLK_DOWN:
                clear_input((char *) 1);
                break;

                case SDLK_UP:
                clear_input((char *) 2);
                break;

                case SDLK_LEFT:
                clear_input((char *) 3);
                break;

                case SDLK_RIGHT:
                clear_input((char *) 4);
                break;

                case SDLK_LCTRL:
                clear_input((char *) 5);
                break;

                case SDLK_LSHIFT:
                clear_input((char *) 6);
                break;

                case SDLK_x:
                clear_input((char *) 7);
                break;

                case SDLK_z:
                clear_input((char *) 8);
                break;

                default:
                break;

            }

        }

    }

}

