#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

typedef struct {
    SDL_Rect pos;
    SDL_Rect ipos;
    SDL_Surface *image;
} Image;

typedef struct {
    SDL_Rect pos;
    TTF_Font *font;
    SDL_Surface *text;
    SDL_Color color;
    char txt[50];
} Text;

// Fonctions pour les images
void init_image(Image *img, const char *path, int x, int y, int w, int h);
void show_image(Image img, SDL_Surface *screen);
void free_image(Image *img);

// Fonctions pour le texte
void init_text(Text *t, int x, int y, int r, int g, int b, const char *font, int size, const char *txt);
void show_text(Text t, SDL_Surface *screen);
void free_text(Text *t);

// Détection de collision
int image_collision(Image img, int x, int y);

#endif
