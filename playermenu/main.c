#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include "players.h"

int main(int argc, char *argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return -1;
    }

    // Initialize SDL_image, SDL_mixer, and SDL_ttf
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        printf("IMG_Init failed: %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    if (Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096) == -1) {
        printf("Mix_OpenAudio failed: %s\n", Mix_GetError());
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    if (TTF_Init() == -1) {
        printf("TTF_Init failed: %s\n", TTF_GetError());
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Create the main SDL surface
    SDL_Surface *screen = SDL_SetVideoMode(1600, 900, 32, SDL_SWSURFACE);
    if (!screen) {
        printf("SDL_SetVideoMode failed: %s\n", SDL_GetError());
        TTF_Quit();
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Initialize the Player Menu
    PlayerMenu playerMenu;
    initPlayerMenu(&playerMenu,screen);

    // Show the Player Menu
    showPlayerMenu(&playerMenu);

    // Clean up the Player Menu
    cleanupPlayerMenu(&playerMenu);

    // Clean up SDL
    TTF_Quit();
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();

    return 0;
}

