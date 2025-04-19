#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>

// Screen dimensions
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

// Minimap dimensions and position (bottom-right corner)
#define MINIMAP_WIDTH 320   // 20% of 1600
#define MINIMAP_HEIGHT 180  // 20% of 900
#define MINIMAP_X 1280      // SCREEN_WIDTH - MINIMAP_WIDTH
#define MINIMAP_Y 720       // SCREEN_HEIGHT - MINIMAP_HEIGHT

// Level definitions
#define LEVEL1 1
#define LEVEL2 2

// Scale factor for minimap (full map to minimap ratio)
#define SCALE_FACTOR 5      // Minimap is 1/5th of full map size

// Structure for buttons in the options menu
typedef struct {
    SDL_Rect pos;           // Fixed position on screen
    SDL_Surface *image;     // Normal state image
    SDL_Surface *image_pressed; // Pressed state image
    int is_pressed;         // State flag
} Button;

// Structure for the options menu
typedef struct {
    Button volume_up;       // Button to increase volume
    Button volume_down;     // Button to decrease volume
    Button toggle_fullscreen; // Button to toggle fullscreen
    Button back;            // Button to return to game
    int volume;             // Current volume level (0-100)
    int is_fullscreen;      // Fullscreen state (0 or 1)
} OptionsMenu;

// Structure for the minimap
typedef struct {
    SDL_Surface *thumbnail; // Minimap background image
    SDL_Rect position_minimap; // Fixed position on screen
    SDL_Surface *man_image; // Player marker image
    SDL_Rect position_man;  // Player position on minimap
    int level;              // Current level (LEVEL1 or LEVEL2)
    int is_colliding;       // Collision state flag
} minimap;

// Function prototypes
// Options Menu
void init_options_menu(OptionsMenu *menu);
void display_options_menu(OptionsMenu menu, SDL_Surface *screen);
void handle_options_menu_events(OptionsMenu *menu, SDL_Event event);
void free_options_menu(OptionsMenu *menu);

// Minimap
void init_minimap(minimap *m, int level);
void MAJMinimap(SDL_Rect posJoueur, minimap *m, SDL_Rect camera);
void display_minimap(minimap m, SDL_Surface *screen);
void Liberer_minimap(minimap *m);
void background_animation_after_collision(minimap *m, SDL_Surface *screen);

// Collision
int perfect_pixel_collision(minimap *m, SDL_Rect platform, SDL_Surface *mask);
int bb_collision(minimap *m, SDL_Rect platform, SDL_Surface *mask);

#endif