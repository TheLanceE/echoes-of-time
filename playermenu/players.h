#ifndef PLAYER_H
#define PLAYER_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

// Button structure
typedef struct {
    SDL_Surface *image;
    SDL_Surface *hoverImage;
    SDL_Rect position;
    int hover;
} Button;

// PlayerMenu structure
typedef struct {
    SDL_Surface *screen; 
    SDL_Surface *bg;
    Mix_Music *backgroundMusic;
    Mix_Chunk *clickSound;
    Mix_Chunk *hoverSound;
    TTF_Font *font;
    Button btn_singlePlayer;
    Button btn_multiPlayer;
    Button btn_back;
} PlayerMenu;

// AvatarMenu structure
typedef struct {
    SDL_Surface *screen;
    SDL_Surface *bg;
    Mix_Music *backgroundMusic;
    Mix_Chunk *clickSound;
    Mix_Chunk *hoverSound; 
    TTF_Font *font;
    Button avatar1,avatar2,input1,input2,btn_validate,btn_back;
} AvatarMenu;

// Function prototypes
void loadButton(Button *btn, const char *path, const char *hoverPath, int x, int y, int width, int height);
void renderText(SDL_Surface *screen, const char *text, int x, int y, SDL_Color color, TTF_Font *font);
int isButtonClicked(Button btn, int x, int y);
int isMouseOver(Button btn, int x, int y);
void initPlayerMenu(PlayerMenu *menu,SDL_Surface *screen);
void showPlayerMenu(PlayerMenu *menu);
void cleanupPlayerMenu(PlayerMenu *menu);
void initAvatarMenu(AvatarMenu *menu, SDL_Surface *screen);
void showAvatarMenu(AvatarMenu *menu);
void cleanupAvatarMenu(AvatarMenu *menu);

#endif

