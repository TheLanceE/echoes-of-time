#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "enemy.h"

int main(int argc, char *argv[]) {
    SDL_Surface *screen;
    int running = 1;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    screen = SDL_SetVideoMode(1600, 900, 32, SDL_SWSURFACE);
    if (!screen) {
        fprintf(stderr, "SDL_SetVideoMode Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    Enemy enemy;
    initializeEnemy(&enemy, "walk_sheet_6rows_death_final.png");

    SDL_Rect player = { 1000, 400, 100, 135 };
    int posMin = (1600 - FRAME_WIDTH) / 2;
    int posMax = 1600 - FRAME_WIDTH - 100;
    int alreadyAttacked = 0;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = 0;

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                if (!enemy.isDead && enemy.health > 0) {
                    enemy.health -= 5;
                    if (enemy.health <= 0) {
                        enemy.health = 0;
                        enemy.deathRow = (enemy.direction == DIRECTION_RIGHT) ? 4 : 5;
                        enemy.state = STATE_DEAD;
                        enemy.currentFrame = 0;
                        enemy.animTickCounter = 0;
                        enemy.isDead = 1;
                    }
                }
            }
        }

        if (!enemy.isDead && checkCollision(enemy.posScreen, player)) {
            if (enemy.state == STATE_WALKING && !alreadyAttacked) {
                enemy.state = STATE_ATTACKING;
                enemy.currentFrame = 0;
                alreadyAttacked = 1;
            }
        } else {
            alreadyAttacked = 0;
        }

        if (enemy.state == STATE_WALKING) {
            deplacerEnemy(&enemy, posMin, posMax);
        } else if (enemy.state == STATE_ATTACKING || enemy.state == STATE_DEAD) {
            animateEnemy(&enemy);
        }

        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0)); // black background
        SDL_FillRect(screen, &player, SDL_MapRGB(screen->format, 255, 255, 255)); // mock white player
        blitEnemy(screen, &enemy);
        drawHealthBar(screen, &enemy);
        SDL_Flip(screen);
        SDL_Delay(16);
    }

    SDL_FreeSurface(enemy.sprite);
    SDL_Quit();
    return 0;
}

