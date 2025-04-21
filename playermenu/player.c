#include "players.h"

// Function to load button with images and sounds
void loadButton(Button *btn, const char *path, const char *hoverPath, int x, int y, int width, int height) {
    // Load the original image
    btn->image = IMG_Load(path);
    btn->hoverImage = IMG_Load(hoverPath);

    if (!btn->image || !btn->hoverImage) {
        printf("Error loading button images: %s\n", IMG_GetError());
        return;
    }

    // Set button position
    btn->position.x = x;
    btn->position.y = y;

    // Resize images if they are larger than the specified size
    if (btn->image->w != width || btn->image->h != height) {
        SDL_Surface *resized = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 
                                                    btn->image->format->BitsPerPixel,
                                                    btn->image->format->Rmask,
                                                    btn->image->format->Gmask,
                                                    btn->image->format->Bmask,
                                                    btn->image->format->Amask);
        
        SDL_Surface *resized_hover = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 
                                                          btn->hoverImage->format->BitsPerPixel,
                                                          btn->hoverImage->format->Rmask,
                                                          btn->hoverImage->format->Gmask,
                                                          btn->hoverImage->format->Bmask,
                                                          btn->hoverImage->format->Amask);
        
        SDL_Rect src_rect = { 0, 0, btn->image->w, btn->image->h };
        SDL_Rect dest_rect = { 0, 0, width, height };

        SDL_SoftStretch(btn->image, &src_rect, resized, &dest_rect);
        SDL_SoftStretch(btn->hoverImage, &src_rect, resized_hover, &dest_rect);

        SDL_FreeSurface(btn->image);
        SDL_FreeSurface(btn->hoverImage);

        btn->image = resized;
        btn->hoverImage = resized_hover;
    }

    btn->hover = 0; // Initialize hover state
}

// Function to check if a button is clicked
int isButtonClicked(Button btn, int x, int y) {
    return (x > btn.position.x && x < (btn.position.x + btn.image->w) &&
            y > btn.position.y && y < (btn.position.y + btn.image->h));
}

// Function to check if the mouse is over a button
int isMouseOver(Button btn, int x, int y) {
    return isButtonClicked(btn, x, y);
}

// Render text function
void renderText(SDL_Surface *screen, const char *text, int x, int y, SDL_Color color, TTF_Font *font) {
    if (!font) return;
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color);
    if (textSurface) {
        SDL_Rect textPosition = {x, y, 0, 0};
        SDL_BlitSurface(textSurface, NULL, screen, &textPosition);
        SDL_FreeSurface(textSurface);
    }
}

void initPlayerMenu(PlayerMenu *menu, SDL_Surface *screen) {
    menu->screen = screen;
    menu->bg = IMG_Load("background.png");
    if (!menu->bg) {
        printf("Error loading background: %s\n", IMG_GetError());
    }

    // Load buttons
    loadButton(&menu->btn_singlePlayer, "singleplayer.png", "singleplayer_hover.png", 600, 450, 400, 200);
    loadButton(&menu->btn_multiPlayer, "multiplayer.png", "multiplayer_hover.png", 600, 700, 400, 200);
    loadButton(&menu->btn_back, "back.png", "back_hover.png", 50, 800, 150, 75);

    // Initialize audio with better quality
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Error initializing audio: %s\n", Mix_GetError());
    }

    // Load sounds with error checking - using WAV files instead of MP3
    menu->hoverSound = Mix_LoadWAV("hover.wav");
    if (!menu->hoverSound) {
        printf("Error loading hover sound: %s\n", Mix_GetError());
    }

    menu->clickSound = Mix_LoadWAV("click.wav");
    if (!menu->clickSound) {
        printf("Error loading click sound: %s\n", Mix_GetError());
    }

    // For background music, MP3 is supported but needs SDL_mixer with MP3 support
    menu->backgroundMusic = Mix_LoadMUS("playermusic.mp3");
    if (!menu->backgroundMusic) {
        // Try WAV format if MP3 fails
        menu->backgroundMusic = Mix_LoadMUS("playermusic.wav");
        if (!menu->backgroundMusic) {
            printf("Error loading background music: %s\n", Mix_GetError());
        }
    }

    if (menu->backgroundMusic) {
        Mix_VolumeMusic(MIX_MAX_VOLUME / 2); // Set music to 50% volume
        Mix_PlayMusic(menu->backgroundMusic, -1); // Start playing music on loop
    }
}

void showPlayerMenu(PlayerMenu *menu) {
    int running = 1;
    SDL_Event event;
    int mouseX, mouseY;
    int previousHoverState[3] = {0, 0, 0}; // Track previous hover states

    menu->font = TTF_OpenFont("alagard.ttf", 100);
    if (!menu->font) {
        printf("Error loading font: %s\n", TTF_GetError());
        return;
    }

    // Ensure background music is playing
    if (!Mix_PlayingMusic() && menu->backgroundMusic) {
        Mix_PlayMusic(menu->backgroundMusic, -1);
    }

    while (running) {
        SDL_BlitSurface(menu->bg, NULL, menu->screen, NULL);
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_Color textColor = {255, 255, 255};
        renderText(menu->screen, "Player Menu", 500, 30, textColor, menu->font);

        Button *buttons[] = {&menu->btn_singlePlayer, &menu->btn_multiPlayer, &menu->btn_back};

        // Handle button hover states and sounds
        for (int i = 0; i < 3; i++) {
            int currentlyHovering = isMouseOver(*buttons[i], mouseX, mouseY);
            
            // Play hover sound when mouse first enters button area
            if (currentlyHovering && !previousHoverState[i]) {
                if (menu->hoverSound) {
                    Mix_PlayChannel(-1, menu->hoverSound, 0);
                }
                buttons[i]->hover = 1;
            } else if (!currentlyHovering) {
                buttons[i]->hover = 0;
            }
            
            previousHoverState[i] = currentlyHovering;

            // Render appropriate button image
            SDL_BlitSurface(
                buttons[i]->hover ? buttons[i]->hoverImage : buttons[i]->image,
                NULL,
                menu->screen,
                &buttons[i]->position
            );
        }

        SDL_Flip(menu->screen);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // Play click sound for any button click
                    if (menu->clickSound) {
                        Mix_PlayChannel(-1, menu->clickSound, 0);
                    }

                    if (isButtonClicked(menu->btn_singlePlayer, event.button.x, event.button.y)) {
                        printf("Single Player Selected\n");
                        AvatarMenu avatarMenu;
                        initAvatarMenu(&avatarMenu, menu->screen);
                        showAvatarMenu(&avatarMenu);
                        cleanupAvatarMenu(&avatarMenu);
                        
                        // Restore background music after returning from avatar menu
                        if (menu->backgroundMusic) {
                            Mix_PlayMusic(menu->backgroundMusic, -1);
                        }
                    }
                    if (isButtonClicked(menu->btn_multiPlayer, event.button.x, event.button.y)) {
                        printf("Multiplayer Selected\n");
                        AvatarMenu avatarMenu;
                        initAvatarMenu(&avatarMenu, menu->screen);
                        showAvatarMenu(&avatarMenu);
                        cleanupAvatarMenu(&avatarMenu);
                        
                        // Restore background music after returning from avatar menu
                        if (menu->backgroundMusic) {
                            Mix_PlayMusic(menu->backgroundMusic, -1);
                        }
                    }
                    if (isButtonClicked(menu->btn_back, event.button.x, event.button.y)) {
                        printf("Returning to Main Menu\n");
                        running = 0;
                    }
                    break;
            }
        }

        SDL_Delay(16); // Cap frame rate
    }
}

void cleanupPlayerMenu(PlayerMenu *menu) {
    if (menu->backgroundMusic) Mix_FreeMusic(menu->backgroundMusic);
    if (menu->hoverSound) Mix_FreeChunk(menu->hoverSound);
    if (menu->clickSound) Mix_FreeChunk(menu->clickSound);
    
    Mix_HaltMusic();
    Mix_CloseAudio();
    Mix_Quit();  // Add this line to properly quit SDL_mixer

    TTF_CloseFont(menu->font);
    TTF_Quit();

    SDL_FreeSurface(menu->bg);
    SDL_FreeSurface(menu->btn_singlePlayer.image);
    SDL_FreeSurface(menu->btn_singlePlayer.hoverImage);
    SDL_FreeSurface(menu->btn_multiPlayer.image);
    SDL_FreeSurface(menu->btn_multiPlayer.hoverImage);
    SDL_FreeSurface(menu->btn_back.image);
    SDL_FreeSurface(menu->btn_back.hoverImage);
}


void initAvatarMenu(AvatarMenu *menu, SDL_Surface *screen) {
    menu->screen = screen;
    menu->bg = IMG_Load("background.png");
    if (!menu->bg) {
        printf("Error loading background: %s\n", IMG_GetError());
    }

    loadButton(&menu->avatar1, "avatar1.png", "avatar1_hover.png", 350, 200, 300, 150);
    loadButton(&menu->avatar2, "avatar2.png", "avatar2_hover.png", 950, 200, 300, 150);
    loadButton(&menu->input1, "input1.png", "input1_hover.png", 350, 400, 300, 150);
    loadButton(&menu->input2, "input2.png", "input2_hover.png", 950, 400, 300, 150);
    loadButton(&menu->btn_validate, "validate.png", "validate_hover.png", 650, 600, 300, 150);
    loadButton(&menu->btn_back, "back.png", "back_hover.png", 1300, 750, 300, 150);

    menu->backgroundMusic = NULL;

    // Load sounds with WAV format
    menu->clickSound = Mix_LoadWAV("click.wav");
    if (!menu->clickSound) {
        printf("Error loading click sound: %s\n", Mix_GetError());
    }

    menu->hoverSound = Mix_LoadWAV("hover.wav");
    if (!menu->hoverSound) {
        printf("Error loading hover sound: %s\n", Mix_GetError());
    }
}

void showAvatarMenu(AvatarMenu *menu) {
    int menuRunning = 1;
    SDL_Event event;
    int mouseX, mouseY;
    int previousHoverState[4] = {0}; // Initialize hover state tracking
    
    Button *buttons[] = {&menu->input1, &menu->input2, &menu->btn_validate, &menu->btn_back};
    menu->font = TTF_OpenFont("alagard.ttf", 100);
    if (!menu->font) {
        printf("Error loading font: %s\n", TTF_GetError());
        return;
    }
    
    while (menuRunning) {
        SDL_BlitSurface(menu->bg, NULL, menu->screen, NULL);
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_Color textColor = {255, 255, 255};
        renderText(menu->screen, "Player Menu", 550, 60, textColor, menu->font);

        // Handle button hover states and sounds
        for (int i = 0; i < 4; i++) {
            int currentlyHovering = isMouseOver(*buttons[i], mouseX, mouseY);
            
            if (currentlyHovering && !previousHoverState[i]) {
                if (menu->hoverSound) {
                    Mix_PlayChannel(-1, menu->hoverSound, 0);
                }
                buttons[i]->hover = 1;
            } else if (!currentlyHovering) {
                buttons[i]->hover = 0;
            }
            
            previousHoverState[i] = currentlyHovering;

            SDL_BlitSurface(
                buttons[i]->hover ? buttons[i]->hoverImage : buttons[i]->image,
                NULL,
                menu->screen,
                &buttons[i]->position
            );
        }

        SDL_Flip(menu->screen);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    menuRunning = 0;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (menu->clickSound) {
                        Mix_PlayChannel(-1, menu->clickSound, 0);
                    }
                    if (isButtonClicked(menu->input1, event.button.x, event.button.y)) {
                        printf("Input 1 Selected\n");
                    } else if (isButtonClicked(menu->input2, event.button.x, event.button.y)) {
                        printf("Input 2 Selected\n");
                    }
                    if (isButtonClicked(menu->btn_validate, event.button.x, event.button.y)) {
                        printf("Moving to Highscore Menu\n");
                        menuRunning = 0;
                    }
                    if (isButtonClicked(menu->btn_back, event.button.x, event.button.y)) {
                        printf("Returning to Player Menu\n");
                        menuRunning = 0;
                    }
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        printf("Enter pressed: move to highscore Menu\n");
                        menuRunning = 0;
                    }
                    break;
            }
        }
    }
}

void cleanupAvatarMenu(AvatarMenu *menu) {
    Mix_HaltMusic();
    if (menu->backgroundMusic) Mix_FreeMusic(menu->backgroundMusic);
    if (menu->clickSound) Mix_FreeChunk(menu->clickSound);
    if (menu->hoverSound) Mix_FreeChunk(menu->hoverSound);
    
    Mix_CloseAudio();
    Mix_Quit();  // Add this line to properly quit SDL_mixer

    SDL_FreeSurface(menu->bg);
    SDL_FreeSurface(menu->avatar1.image);
    SDL_FreeSurface(menu->avatar1.hoverImage);
    SDL_FreeSurface(menu->avatar2.image);
    SDL_FreeSurface(menu->avatar2.hoverImage);
    SDL_FreeSurface(menu->input1.image);
    SDL_FreeSurface(menu->input1.hoverImage);
    SDL_FreeSurface(menu->input2.image);
    SDL_FreeSurface(menu->input2.hoverImage);
    SDL_FreeSurface(menu->btn_validate.image);
    SDL_FreeSurface(menu->btn_validate.hoverImage);
    SDL_FreeSurface(menu->btn_back.image);
    SDL_FreeSurface(menu->btn_back.hoverImage);
    
    if (menu->font) {
        TTF_CloseFont(menu->font);
    }
}
