#include <SDL/SDL.h>

extern SDL_Surface *screen;
extern void screen_lock();
extern void screen_unlock();
extern void init_SDL(int type);
extern void check_SDL_event();
