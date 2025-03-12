#include "options.h"
#include <stdio.h>

void initOptions(OptionsMenu *options, SDL_Surface *screen) {
    options->screen = screen;

    // Calculate scaling factors based on original resolution (1920x1080) to 1600x900
    options->scale_x = (float)screen->w / 1920.0f;
    options->scale_y = (float)screen->h / 1080.0f;

    // Load pre-scaled background image
    options->background = IMG_Load("background.jpeg");
    if (!options->background) {
        printf("Error loading background image: %s\n", IMG_GetError());
        return;
    }

    // Load pre-scaled images
    options->box = IMG_Load("box.jpg");
    options->btnIncrease = IMG_Load("buttonincrease.png");
    options->btnIncreasePressed = IMG_Load("buttonincreasepressed.png");
    options->btnDecrease = IMG_Load("buttondecrease.png");
    options->btnDecreasePressed = IMG_Load("buttondecreasepressed.png");
    options->btnMuteOn = IMG_Load("mutebutton.png");  // Muted state
    options->btnMuteOff = IMG_Load("mutebuttonpressed.png");  // Unmuted state
    options->btnFullscreen = IMG_Load("fulldisplaybutton.png");
    options->btnFullscreenPressed = IMG_Load("fulldisplaybuttonpressed.png");
    options->btnWindowed = IMG_Load("smalldisplaybutton.png");
    options->btnWindowedPressed = IMG_Load("smalldisplaybuttonpressed.png");
    options->btnBack = IMG_Load("back.png");

    // Check for loading errors
    if (!options->box || !options->btnIncrease) {
        printf("Error loading images: %s\n", IMG_GetError());
        return;
    }

    // Load and play background music
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Mixer Init failed: %s\n", Mix_GetError());
        return;
    }
    options->music = Mix_LoadMUS("optionsmusic.mp3");
    if (!options->music) {
        printf("Error loading music: %s\n", Mix_GetError());
    }
    Mix_PlayMusic(options->music, -1);  // Loop indefinitely

    // Initialize TTF and render text (maintaining font size 32 for larger text)
    if (TTF_Init() < 0) {
        printf("TTF Init failed: %s\n", TTF_GetError());
        return;
    }
    TTF_Font *font = TTF_OpenFont("alagard.ttf", 32); // Keeping font size at 32 for larger text
    if (!font) {
        printf("Error loading font: %s\n", TTF_GetError());
        return;
    }
    SDL_Color white = {255, 255, 255};
    options->volumeText = TTF_RenderText_Solid(font, "Volume", white);
    options->displayText = TTF_RenderText_Solid(font, "Display", white);
    TTF_CloseFont(font);

    // Position the box (centered at 1600x900)
    options->boxPos.x = (screen->w - options->box->w) / 2;
    options->boxPos.y = (screen->h - options->box->h) / 2;

    // Volume section position (relative to box, scaled, adjusted right for decrease/increase, revert mute slightly)
    options->volumePos.x = options->boxPos.x + (int)(150 * options->scale_x);  // Moved "-" slightly right (from 100 to 150)
    options->volumePos.y = options->boxPos.y + (int)(200 * options->scale_y);

    // Display section position (relative to box, scaled)
    options->displayPos.x = options->boxPos.x + (int)(150 * options->scale_x);
    options->displayPos.y = options->boxPos.y + (int)(350 * options->scale_y);

    // Back button position (bottom-right corner with scaled margin)
    options->backPos.x = screen->w - options->btnBack->w - (int)(50 * options->scale_x);
    options->backPos.y = screen->h - options->btnBack->h - (int)(50 * options->scale_x);

    // Text positions (relative to their buttons, scaled, above settings)
    options->volumeTextPos.x = options->volumePos.x - (int)(20 * options->scale_x);  // Left margin, aligned above decrease/increase buttons
    options->volumeTextPos.y = options->volumePos.y - 50;  // Positioned 50 pixels above volume buttons (above settings)

    options->displayTextPos.x = options->displayPos.x - (int)(20 * options->scale_x);  // Left margin, aligned above fullscreen button
    options->displayTextPos.y = options->displayPos.y - 50;  // Positioned 50 pixels above fullscreen button (above settings)

    // Initialize states
    options->hoverIncrease = 0;
    options->hoverDecrease = 0;
    options->hoverMute = 0;
    options->hoverFullscreen = 0;
    options->hoverWindowed = 0;
    options->hoverBack = 0;
    options->isMuted = 0;
    options->isFullscreen = 0;
    options->currentVolume = MIX_MAX_VOLUME / 2; // Default to half volume
}

void handleOptionsEvents(OptionsMenu *options, SDL_Event *event, int *currentMenu) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Define hover areas using actual button sizes
    options->hoverDecrease = (mouseX >= options->volumePos.x &&
                              mouseX < options->volumePos.x + options->btnDecrease->w &&
                              mouseY >= options->volumePos.y &&
                              mouseY < options->volumePos.y + options->btnDecrease->h);

    int increase_x = options->volumePos.x + (int)(500 * options->scale_x);  // Moved "+" further right
    options->hoverIncrease = (mouseX >= increase_x &&
                              mouseX < increase_x + options->btnIncrease->w &&
                              mouseY >= options->volumePos.y &&
                              mouseY < options->volumePos.y + options->btnIncrease->h);

    int mute_x = options->volumePos.x + (int)(550 * options->scale_x);  // Pushed mute slightly right (from 450 to 550)
    options->hoverMute = (mouseX >= mute_x &&
                          mouseX < mute_x + options->btnMuteOn->w &&
                          mouseY >= options->volumePos.y &&
                          mouseY < options->volumePos.y + options->btnMuteOn->h);

    options->hoverFullscreen = (mouseX >= options->displayPos.x &&
                                mouseX < options->displayPos.x + options->btnFullscreen->w &&
                                mouseY >= options->displayPos.y &&
                                mouseY < options->displayPos.y + options->btnFullscreen->h);

    int windowed_y = options->displayPos.y + (int)(200 * options->scale_y);  // Pushed Windowed button slightly up (from 250 to 200)
    options->hoverWindowed = (mouseX >= options->displayPos.x &&
                              mouseX < options->displayPos.x + options->btnWindowed->w &&
                              mouseY >= windowed_y &&
                              mouseY < windowed_y + options->btnWindowed->h);

    options->hoverBack = (mouseX >= options->backPos.x &&
                          mouseX < options->backPos.x + options->btnBack->w &&
                          mouseY >= options->backPos.y &&
                          mouseY < options->backPos.y + options->btnBack->h);

    // Play hover sound if mouse enters a button area
    static int prevHoverState = 0;
    int currentHoverState = options->hoverIncrease || options->hoverDecrease || options->hoverMute ||
                            options->hoverFullscreen || options->hoverWindowed || options->hoverBack;
    if (currentHoverState && !prevHoverState) {
        Mix_Chunk *hoverSound = Mix_LoadWAV("hover.mp3");
        Mix_PlayChannel(-1, hoverSound, 0);
        Mix_FreeChunk(hoverSound);
    }
    prevHoverState = currentHoverState;

    // Handle keyboard input
    if (event->type == SDL_KEYDOWN) {
        Mix_Chunk *clickSound = Mix_LoadWAV("click.mp3");
        switch (event->key.keysym.sym) {
            case SDLK_PLUS:  // Increase volume (raises audio by one "tower" or level)
            case SDLK_KP_PLUS:  // NumPad +
                if (!options->isMuted && options->currentVolume < MIX_MAX_VOLUME) {
                    options->currentVolume += MIX_MAX_VOLUME / 5;
                    if (options->currentVolume > MIX_MAX_VOLUME) {
                        options->currentVolume = MIX_MAX_VOLUME;
                    }
                    Mix_VolumeMusic(options->currentVolume);  // Corrected
                    Mix_PlayChannel(-1, clickSound, 0);
                }
                break;

            case SDLK_MINUS:  // Decrease volume (lowers audio by one "tower" or level)
            case SDLK_KP_MINUS:  // NumPad -
                if (!options->isMuted && options->currentVolume > 0) {
                    options->currentVolume -= MIX_MAX_VOLUME / 5;
                    if (options->currentVolume < 0) {
                        options->currentVolume = 0;
                    }
                    Mix_VolumeMusic(options->currentVolume);  // Corrected
                    Mix_PlayChannel(-1, clickSound, 0);
                }
                break;

            case SDLK_ESCAPE:  // Back button function (return to main menu)
                *currentMenu = 0; // Exit options menu
                Mix_HaltMusic();  // Corrected
                Mix_PlayChannel(-1, clickSound, 0);
                break;
        }
        Mix_FreeChunk(clickSound);
    }

    // Handle mouse clicks
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        Mix_Chunk *clickSound = Mix_LoadWAV("click.mp3");
        if (options->hoverDecrease && options->currentVolume > 0) {
            options->currentVolume -= MIX_MAX_VOLUME / 5;
            if (options->currentVolume < 0) {
                options->currentVolume = 0;
            }
            Mix_VolumeMusic(options->currentVolume);  // Corrected
            Mix_PlayChannel(-1, clickSound, 0);
        }
        if (options->hoverIncrease && options->currentVolume < MIX_MAX_VOLUME) {
            options->currentVolume += MIX_MAX_VOLUME / 5;
            if (options->currentVolume > MIX_MAX_VOLUME) {
                options->currentVolume = MIX_MAX_VOLUME;
            }
            Mix_VolumeMusic(options->currentVolume);  // Corrected
            Mix_PlayChannel(-1, clickSound, 0);
        }
        if (options->hoverMute) {
            options->isMuted = !options->isMuted;
            Mix_VolumeMusic(options->isMuted ? 0 : options->currentVolume);  // Corrected
            Mix_PlayChannel(-1, clickSound, 0);
        }
        if (options->hoverFullscreen && !options->isFullscreen) {
            options->isFullscreen = 1;
            options->screen = SDL_SetVideoMode(1600, 900, 32, SDL_SWSURFACE | SDL_FULLSCREEN);
            Mix_PlayChannel(-1, clickSound, 0);
        }
        if (options->hoverWindowed && options->isFullscreen) {
            options->isFullscreen = 0;
            options->screen = SDL_SetVideoMode(1600, 900, 32, SDL_SWSURFACE);
            Mix_PlayChannel(-1, clickSound, 0);
        }
        if (options->hoverBack) {
            *currentMenu = 0; // Return to main menu
            Mix_HaltMusic();  // Corrected
            Mix_PlayChannel(-1, clickSound, 0);
        }
        Mix_FreeChunk(clickSound);
    }
}

void drawOptions(OptionsMenu *options) {
    SDL_BlitSurface(options->background, NULL, options->screen, NULL);  // Use background image (JPEG)
    SDL_BlitSurface(options->box, NULL, options->screen, &options->boxPos);

    // Volume section (shifted right for decrease/increase, revert mute slightly)
    SDL_BlitSurface(options->hoverDecrease ? options->btnDecreasePressed : options->btnDecrease,
                    NULL, options->screen, &options->volumePos);
    SDL_Rect increasePos = {options->volumePos.x + (int)(500 * options->scale_x), options->volumePos.y, 0, 0};  // Moved "+" further right
    SDL_BlitSurface(options->hoverIncrease ? options->btnIncreasePressed : options->btnIncrease,
                    NULL, options->screen, &increasePos);
    SDL_Rect mutePos = {options->volumePos.x + (int)(550 * options->scale_x), options->volumePos.y, 0, 0};  // Pushed mute slightly right
    SDL_BlitSurface(options->isMuted ? options->btnMuteOn : options->btnMuteOff,
                    NULL, options->screen, &mutePos);

    // Volume bars (5 bars, orange, scaled size, moved slightly right)
    int level = options->currentVolume / (MIX_MAX_VOLUME / 5);
    int bar_start_x = options->volumePos.x + (int)(100 * options->scale_x);  // Moved slightly right (unchanged)
    int bar_y = options->volumePos.y + (int)(70 * options->scale_y);
    int bar_width = (int)(50 * options->scale_x);
    int bar_height = (int)(20 * options->scale_y);
    int bar_spacing = (int)(60 * options->scale_x);
    for (int i = 0; i < 5; i++) {
        SDL_Rect bar = {bar_start_x + (i * bar_spacing), bar_y, bar_width, bar_height};
        if (i < level) {
            SDL_FillRect(options->screen, &bar, SDL_MapRGB(options->screen->format, 255, 165, 0));  // Orange (RGB: 255, 165, 0)
        }
    }

    // Display section (stack vertically, push Windowed button slightly up)
    SDL_BlitSurface(options->hoverFullscreen ? options->btnFullscreenPressed : options->btnFullscreen,
                    NULL, options->screen, &options->displayPos);
    SDL_Rect windowedPos = {options->displayPos.x, options->displayPos.y + (int)(150 * options->scale_y), 0, 0};  // Pushed Windowed slightly up (from 200 to 150)
    SDL_BlitSurface(options->hoverWindowed ? options->btnWindowedPressed : options->btnWindowed,
                    NULL, options->screen, &windowedPos);

    // Back button
    SDL_BlitSurface(options->hoverBack ? options->btnBack : options->btnBack, // No pressed variant for back
                    NULL, options->screen, &options->backPos);

    // Text (positioned above settings, font size 32)
    SDL_BlitSurface(options->volumeText, NULL, options->screen, &options->volumeTextPos);
    SDL_BlitSurface(options->displayText, NULL, options->screen, &options->displayTextPos);

    SDL_Flip(options->screen);
}

void cleanupOptions(OptionsMenu *options) {
    SDL_FreeSurface(options->background);
    SDL_FreeSurface(options->box);
    SDL_FreeSurface(options->btnIncrease);
    SDL_FreeSurface(options->btnIncreasePressed);
    SDL_FreeSurface(options->btnDecrease);
    SDL_FreeSurface(options->btnDecreasePressed);
    SDL_FreeSurface(options->btnMuteOn);
    SDL_FreeSurface(options->btnMuteOff);
    SDL_FreeSurface(options->btnFullscreen);
    SDL_FreeSurface(options->btnFullscreenPressed);
    SDL_FreeSurface(options->btnWindowed);
    SDL_FreeSurface(options->btnWindowedPressed);
    SDL_FreeSurface(options->btnBack);
    SDL_FreeSurface(options->volumeText);
    SDL_FreeSurface(options->displayText);
    Mix_FreeMusic(options->music);
    Mix_CloseAudio();
    TTF_Quit();
}
