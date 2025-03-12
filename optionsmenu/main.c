#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include "options.h"  // Assumes this header contains the pre-coded options menu functions

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

// Function to render text centered on the screen
void renderTextCentered(SDL_Surface *screen, TTF_Font *font, const char *text, int y) {
    SDL_Color color = {255, 255, 255};  // White text
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color);
    if (textSurface) {
        SDL_Rect rect = {
            (SCREEN_WIDTH - textSurface->w) / 2,  // Center horizontally
            y,                                   // Vertical position
            textSurface->w,
            textSurface->h
        };
        SDL_BlitSurface(textSurface, NULL, screen, &rect);
        SDL_FreeSurface(textSurface);
    }
}

// Function to check if a point is within a rectangle
int isPointInRect(int x, int y, SDL_Rect rect) {
    return (x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h);
}

int main(int argc, char *argv[]) {
    SDL_Surface *screen;
    SDL_Event event;
    OptionsMenu options = {0};  // Structure for the options menu (assumed from options.h)
    int currentMenu = 0;        // 0 = main menu, 1 = options menu
    int running = 1;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL Init failed: %s\n", SDL_GetError());
        return 1;
    }

    // Set up the screen
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);
    if (!screen) {
        printf("Video mode set failed: %s\n", SDL_GetError());
        return 1;
    }
    SDL_WM_SetCaption("Pixel-Art Game", NULL);

    // Initialize SDL_ttf for font rendering
    if (TTF_Init() < 0) {
        printf("TTF Init failed: %s\n", TTF_GetError());
        return 1;
    }
    TTF_Font *font = TTF_OpenFont("alagard.ttf", 48);  // Load font (adjust path if needed)
    if (!font) {
        printf("Error loading font: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // Main game loop
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;  // Exit on window close
            } else if (currentMenu == 0) {  // Main menu state
                if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;
                    // Define clickable area for "options" text (approximate size)
                    SDL_Rect optionsRect = {
                        (SCREEN_WIDTH - 200) / 2,  // Rough width of "options" text
                        SCREEN_HEIGHT / 2 - 24,    // Center vertically based on font size
                        200,                       // Approximate width
                        48                         // Font height
                    };
                    if (isPointInRect(mouseX, mouseY, optionsRect)) {
                        initOptions(&options, screen);  // Initialize options menu
                        currentMenu = 1;                // Switch to options menu
                    }
                }
            } else if (currentMenu == 1) {  // Options menu state
                handleOptionsEvents(&options, &event, &currentMenu);  // Pass event to options menu
            }
        }

        // Render the current state
        if (currentMenu == 0) {
            // Main menu: Display "options" in the center
            SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));  // Black background
            renderTextCentered(screen, font, "options", SCREEN_HEIGHT / 2 - 24);
            SDL_Flip(screen);  // Update the screen
        } else if (currentMenu == 1) {
            // Options menu: Draw pre-coded options interface
            drawOptions(&options);
        }
    }

    // Cleanup resources
    if (currentMenu == 1) {
        cleanupOptions(&options);  // Free options menu resources if active
    }
    TTF_CloseFont(font);  // Close the font
    TTF_Quit();           // Quit SDL_ttf
    SDL_Quit();           // Quit SDL

    return 0;
}