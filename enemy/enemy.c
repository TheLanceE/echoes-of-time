#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include "enemy.h"

void initializeEnemy(Enemy *e, const char *imagePath) {
    e->sprite = IMG_Load(imagePath);
    if (!e->sprite) {
        fprintf(stderr, "Failed to load sprite: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    e->direction = DIRECTION_RIGHT;
    e->state = STATE_WALKING;
    e->currentFrame = 0;
    e->pixelsMoved = 0;
    e->isDead = 0;
    e->deathRow = 4;
    e->animTickCounter = 0;

    e->totalFramesPerRow[0] = 12; // walk right
    e->totalFramesPerRow[1] = 12; // walk left
    e->totalFramesPerRow[2] = 12; // attack
    e->totalFramesPerRow[4] = 12; // death right
    e->totalFramesPerRow[5] = 12; // death left (not reversed visually!)

    e->maxHealth = 100;
    e->health = 100;

    e->posScreen.x = (1600 - FRAME_WIDTH) / 2;
    e->posScreen.y = (900 - FRAME_HEIGHT) / 2;

    e->posSprite.x = 0;
    e->posSprite.y = 0;
    e->posSprite.w = FRAME_WIDTH;
    e->posSprite.h = FRAME_HEIGHT;
}

void animateEnemy(Enemy *e) {
    int row = 0;

    if (e->state == STATE_ATTACKING)
        row = 2;
    else if (e->state == STATE_DEAD)
        row = e->deathRow;
    else
        row = e->direction;

    e->posSprite.y = row * FRAME_HEIGHT;

    if (e->state == STATE_DEAD) {
        e->animTickCounter++;
        if (e->animTickCounter < 4) return;
        e->animTickCounter = 0;
    }

    if (e->state == STATE_DEAD) {
        if (e->currentFrame < e->totalFramesPerRow[row] - 1)
            e->currentFrame++;
    } else {
        e->currentFrame++;
        if (e->currentFrame >= e->totalFramesPerRow[row]) {
            e->currentFrame = 0;
            if (e->state == STATE_ATTACKING)
                e->state = STATE_WALKING;
        }
    }

    // ✅ Death-left plays in reverse
    if (e->state == STATE_DEAD && e->deathRow == 5) {
        e->posSprite.x = (e->totalFramesPerRow[5] - 1 - e->currentFrame) * FRAME_WIDTH;
    } else {
        e->posSprite.x = e->currentFrame * FRAME_WIDTH;
    }
}

void blitEnemy(SDL_Surface *screen, Enemy *e) {
    SDL_BlitSurface(e->sprite, &e->posSprite, screen, &e->posScreen);
}

void deplacerEnemy(Enemy *e, int posMin, int posMax) {
    if (e->state != STATE_WALKING || e->isDead) return;

    int speed = 2;

    if (e->posScreen.x <= posMin)
        e->direction = DIRECTION_RIGHT;
    else if (e->posScreen.x >= posMax)
        e->direction = DIRECTION_LEFT;

    e->posScreen.x += (e->direction == DIRECTION_RIGHT) ? speed : -speed;

    e->pixelsMoved += speed;
    if (e->pixelsMoved >= 16) {
        animateEnemy(e);
        e->pixelsMoved = 0;
    }
}

int checkCollision(SDL_Rect a, SDL_Rect b) {
    return (
        a.x < b.x + b.w &&
        a.x + a.w > b.x &&
        a.y < b.y + b.h &&
        a.y + a.h > b.y
    );
}

void drawHealthBar(SDL_Surface *screen, Enemy *e) {
    if (e->isDead) return;

    int barWidth = 60;
    int barHeight = 8;
    int barX = e->posScreen.x + (FRAME_WIDTH - barWidth) / 2;
    int barY = e->posScreen.y - barHeight - 5;

    SDL_Rect bg = { barX, barY, barWidth, barHeight };
    SDL_FillRect(screen, &bg, SDL_MapRGB(screen->format, 60, 60, 60));

    int healthWidth = (e->health * barWidth) / e->maxHealth;
    SDL_Rect fg = { barX, barY, healthWidth, barHeight };

    Uint32 color = SDL_MapRGB(screen->format,
        (e->health > 60) ? 0 : (e->health > 30) ? 255 : 255,
        (e->health > 60) ? 255 : (e->health > 30) ? 165 : 0,
        (e->health > 60) ? 0 : 0
    );

    SDL_FillRect(screen, &fg, color);
}

