#ifndef back_H_INCLUDED
#define back_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>

typedef struct background
	{
        SDL_Surface *image,*sprite;
        SDL_Rect camera;
        Mix_Music *music;
        SDL_Surface *imageLives;
        SDL_Surface *imageScore;
        SDL_Surface *imageStage ; 
        SDL_Rect   posLives, posScore, posStage;
        TTF_Font * font ; 
        SDL_Color color ; 
        int score ; 
        int stage ;
        SDL_Rect pos_anim;
        SDL_Rect pos_sprite;
      
    }background;

void initBack(background * b, int x,int y);
void displayBack(background b, SDL_Surface * screen,int x,int y);
void animation(background *b, SDL_Surface *screen);
void scrolling(background * b, int direction, int stepProgress);
void update_score(background * b);




#endif 
