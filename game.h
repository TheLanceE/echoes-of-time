#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>

// Constants
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

#define MINIMAP_WIDTH 320
#define MINIMAP_HEIGHT 180
#define MINIMAP_X 1280
#define MINIMAP_Y 720

#define LEVEL1 1
#define LEVEL2 2
#define SCALE_FACTOR 5

#define FRAME_WIDTH 100
#define FRAME_HEIGHT 135
#define DIRECTION_RIGHT 0
#define DIRECTION_LEFT 1
#define STATE_WALKING 0
#define STATE_ATTACKING 1
#define STATE_DEAD 2
#define MAX_FRAMES 12
#define MAX_ROWS 6

// Structures
typedef struct {
    SDL_Rect pos;
    SDL_Surface *image;
    SDL_Surface *image_pressed;
    int is_pressed;
} Button;

typedef struct {
    Button volume_up;
    Button volume_down;
    Button toggle_fullscreen;
    Button back;
    Button mute_button;
    int volume;
    int is_fullscreen;
    int is_muted;
    SDL_Surface *background;
    SDL_Surface *box;
    SDL_Rect box_pos;
    SDL_Surface *volume_text;
    SDL_Surface *display_text;
    SDL_Rect volume_text_pos;
    SDL_Rect display_text_pos;
    Mix_Music *music;
    Mix_Chunk *hover_sound;
    Mix_Chunk *click_sound;
} OptionsMenu;

typedef struct {
    SDL_Surface *thumbnail;
    SDL_Rect position_minimap;
    SDL_Surface *man_image;
    SDL_Rect position_man;
    int level;
    int is_colliding;
} minimap;

typedef struct {
    int x;
    int y;
    int w;
    int h;
} collision;

typedef struct {
    SDL_Surface *image;
    SDL_Rect pos;
} background;

typedef struct {
    Uint32 start_time;
    Uint32 elapsed_time;
} GameTime;

typedef struct {
    SDL_Surface *sprite;
    SDL_Rect posScreen;
    SDL_Rect posSprite;
    int direction;
    int currentFrame;
    int totalFramesPerRow[MAX_ROWS];
    int pixelsMoved;
    int state;
    int health;
    int maxHealth;
    int isDead;
    int deathRow;
    int animTickCounter;
} Enemy;

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

// Background
void initback(background *bg, Mix_Music *musique);
void initpartage(background *bg1, background *bg2, Mix_Music *musique);
void afficher_back(background bg, SDL_Surface *ecran);
void scrolling(background *bg, SDL_Rect *pos, int vitesse, int d);
void clamp_player(SDL_Rect *pos, background *bg, SDL_Surface *player_surf, int view_w, int view_h);
void afficher_temps(SDL_Surface *ecran, TTF_Font *font, GameTime *game_time, SDL_Color color);

// Enemy
void initializeEnemy(Enemy *e, const char *imagePath);
void animateEnemy(Enemy *e);
void blitEnemy(SDL_Surface *screen, Enemy *e);
void deplacerEnemy(Enemy *e, int posMin, int posMax);
int checkCollision(SDL_Rect a, SDL_Rect b);
void drawHealthBar(SDL_Surface *screen, Enemy *e);

// Collision
collision minimap_to_collision(minimap *m);
collision rect_to_collision(SDL_Rect rect);
SDL_Color Get_Pixel(SDL_Surface *surface, int x, int y);
int Perfect_Collision(collision entity, SDL_Surface *mask, int points);
int Directional_Collision(collision entity, SDL_Surface *mask, SDLKey direction);
int AABBcollision(collision charac, collision object);
int AABB_CollisionWithSide(collision entity, collision fix, int *side);
int is_pixel_non_transparent(SDL_Surface *surface, int x, int y);
int perfect_pixel_collision(minimap *m, SDL_Rect platform, SDL_Surface *mask);
int bb_collision(minimap *m, SDL_Rect platform, SDL_Surface *mask);

#endif