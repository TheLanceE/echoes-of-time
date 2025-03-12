#ifndef HEADER_H
#define HEADER_H
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

// Screen dimensions
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900
#define MINIMAP_WIDTH 400
#define MINIMAP_HEIGHT 225

#define LEVEL1 1
#define LEVEL2 2

//image struct
typedef struct {
    SDL_Rect pos;
    SDL_Rect ipos;
    SDL_Surface *image;
} Image;

//text struct
typedef struct {
    SDL_Rect pos;
    TTF_Font *font;
    SDL_Surface *text;
    SDL_Color color;
    char txt[50];
} Text;

//minimap struct
typedef struct {
    Image map_image;
    int level;
    SDL_Rect player_pos;
    int scale;
    int is_colliding;
} MiniMap;

//image handling
void init_image(Image *img, char path[], int x, int y, int w, int h);
void show_image(Image img, SDL_Surface *screen);
void free_image(Image *img);

//text handling
void init_text(Text *t, int x, int y, int r, int g, int b, char font[], int size, char txt[]);
void show_text(Text t, SDL_Surface *screen);
void free_text(Text *t);

//image collision detection
int image_collision(Image img, float x, float y);

// Function prototypes for minimap handling
void initialize_and_display_minimap(MiniMap *minimap, int level, int x, int y);
void update_minimap(MiniMap *minimap, SDL_Rect player_position);
int perfect_pixel_collision(MiniMap *minimap, SDL_Rect platform);
int bb_collision(MiniMap *minimap, SDL_Rect platform);
void background_animation_after_collision(MiniMap *minimap, SDL_Surface *screen);

// Function prototypes for music handling (kept for potential reuse)
void init_music(Mix_Music **music, char path[]);
void play_music(Mix_Music *music);
void free_music(Mix_Music *music);

#endif
