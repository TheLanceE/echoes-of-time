#ifndef ENEMY_H
#define ENEMY_H

#include <SDL/SDL.h>

#define FRAME_WIDTH 100
#define FRAME_HEIGHT 135

#define DIRECTION_RIGHT 0
#define DIRECTION_LEFT 1

#define STATE_WALKING 0
#define STATE_ATTACKING 1
#define STATE_DEAD 2

#define MAX_FRAMES 12
#define MAX_ROWS 6

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

void initializeEnemy(Enemy *e, const char *imagePath);
void animateEnemy(Enemy *e);
void blitEnemy(SDL_Surface *screen, Enemy *e);
void deplacerEnemy(Enemy *e, int posMin, int posMax);
int checkCollision(SDL_Rect a, SDL_Rect b);
void drawHealthBar(SDL_Surface *screen, Enemy *e);

#endif

