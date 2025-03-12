#ifndef OPTIONS_H
#define OPTIONS_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

typedef struct {
    SDL_Surface *screen;           // Pointer to the screen surface
    SDL_Surface *background;       // Background image (background.png, 1600x1075)
    Mix_Music *music;             // Background music (optionsmusic.mp3)
    SDL_Surface *box;              // box.png (700x700)
    SDL_Surface *btnIncrease;      // buttonincrease.png (80x82)
    SDL_Surface *btnIncreasePressed; // buttonincreasepressed.png (80x85)
    SDL_Surface *btnDecrease;      // buttondecrease.png (80x82)
    SDL_Surface *btnDecreasePressed; // buttondecreasepressed.png (80x85)
    SDL_Surface *btnMuteOn;        // mutebutton.png (80x82)
    SDL_Surface *btnMuteOff;       // mutebuttonpressed.png (80x85)
    SDL_Surface *btnFullscreen;    // fulldisplaybutton.png (288x160)
    SDL_Surface *btnFullscreenPressed; // fulldisplaybuttonpressed.png (292x160)
    SDL_Surface *btnWindowed;      // smalldisplaybutton.png (288x160)
    SDL_Surface *btnWindowedPressed; // smalldisplaybuttonpressed.png (292x160)
    SDL_Surface *btnBack;          // back.png (272x159)
    SDL_Surface *volumeText;       // "Volume" text
    SDL_Surface *displayText;      // "Display" text
    SDL_Rect boxPos;
    SDL_Rect volumePos;
    SDL_Rect displayPos;
    SDL_Rect backPos;
    SDL_Rect volumeTextPos;
    SDL_Rect displayTextPos;
    int hoverIncrease;
    int hoverDecrease;
    int hoverMute;
    int hoverFullscreen;
    int hoverWindowed;
    int hoverBack;
    int isMuted;
    int isFullscreen;
    int currentVolume;
    float scale_x;                 // Horizontal scaling factor
    float scale_y;                 // Vertical scaling factor
} OptionsMenu;

#ifdef __cplusplus
extern "C" {
#endif

void initOptions(OptionsMenu *options, SDL_Surface *screen);
void handleOptionsEvents(OptionsMenu *options, SDL_Event *event, int *currentMenu);
void drawOptions(OptionsMenu *options);
void cleanupOptions(OptionsMenu *options);

#ifdef __cplusplus
}
#endif

#endif
