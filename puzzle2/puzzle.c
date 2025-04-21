#include "puzzle.h"


// Audio functions
// ==============

// Generate a simple square wave tone at the specified frequency
Mix_Chunk* generateTone(double frequency) {
    int samples = (SAMPLE_RATE * DURATION_MS) / 1000; // 1000ms = 1 second      
    Uint16* buffer = malloc(samples * sizeof(Uint16));  // unit = 16-bit for save sample sounds
    if (!buffer) return NULL;

    int period = SAMPLE_RATE / frequency;
    for (int i = 0; i < samples; ++i) {
        buffer[i] = (i % period < (period / 2)) ? AMPLITUDE : -AMPLITUDE;
    }

    Mix_Chunk* chunk = malloc(sizeof(Mix_Chunk));
    if (!chunk) {
        free(buffer);
        return NULL;
    }

    chunk->allocated = 1;
    chunk->alen = samples * sizeof(Uint16);
    chunk->abuf = (Uint8*)buffer;
    chunk->volume = MIX_MAX_VOLUME;

    return chunk;
}



// Generate a level start sound (ascending notes based on level)
Mix_Chunk* generateLevelStartSound(int level) {
    int samples = (SAMPLE_RATE * 800) / 1000;
    Uint16* buffer = malloc(samples * sizeof(Uint16));
    if (!buffer) return NULL;

    double baseFreq = red_btn;
    int numNotes = level + 1;
    int samplesPerNote = samples / numNotes;
    
    for (int note = 0; note < numNotes; note++) {
        double freq = baseFreq * (1.0 + (note * 0.25)); // كل نغمة ترددها أعلى من السابقة بنسبة 25%.
        int period = SAMPLE_RATE / freq; // period of the note
        for (int i = 0; i < samplesPerNote; ++i) {
            int sampleIndex = note * samplesPerNote + i;
            buffer[sampleIndex] = (i % period < (period / 2)) ? AMPLITUDE : -AMPLITUDE; // sound wave is a square wave
        }
    }

    Mix_Chunk* chunk = malloc(sizeof(Mix_Chunk));
    if (!chunk) {
        free(buffer);
        return NULL;
    }

    chunk->allocated = 1;
    chunk->alen = samples * sizeof(Uint16);
    chunk->abuf = (Uint8*)buffer;
    chunk->volume = MIX_MAX_VOLUME;

    return chunk;
}

// Generate a success sound (ascending major triad)
Mix_Chunk* generateSuccessSound() {
    int samples = (SAMPLE_RATE * 1000) / 1000;
    Uint16* buffer = malloc(samples * sizeof(Uint16));
    if (!buffer) return NULL;

    double frequencies[] = {blue_btn, yellow_btn, green_btn};
    int samplesPerNote = samples / 3;
    
    for (int note = 0; note < 3; note++) {
        int period = SAMPLE_RATE / frequencies[note];
        for (int i = 0; i < samplesPerNote; ++i) {
            int sampleIndex = note * samplesPerNote + i;
            buffer[sampleIndex] = (i % period < (period / 2)) ? AMPLITUDE : -AMPLITUDE;
        }
    }

    Mix_Chunk* chunk = malloc(sizeof(Mix_Chunk));
    if (!chunk) {
        free(buffer);
        return NULL;
    }

    chunk->allocated = 1;
    chunk->alen = samples * sizeof(Uint16);
    chunk->abuf = (Uint8*)buffer;
    chunk->volume = MIX_MAX_VOLUME;

    return chunk;
}

// Generate a failure sound (descending minor triad)
Mix_Chunk* generateFailureSound() {
    int samples = (SAMPLE_RATE * 1000) / 1000;
    Uint16* buffer = malloc(samples * sizeof(Uint16));
    if (!buffer) return NULL;

    double frequencies[] = {yellow_btn, blue_btn, green_btn}; // frequencies of the notes
    int samplesPerNote = samples / 3; // number of samples per note
    
    for (int note = 0; note < 3; note++) {
        int period = SAMPLE_RATE / frequencies[note]; // period of the note
        for (int i = 0; i < samplesPerNote; ++i) {
            int sampleIndex = note * samplesPerNote + i;
            buffer[sampleIndex] = (i % period < (period / 2)) ? AMPLITUDE : -AMPLITUDE;
        }
    }

    Mix_Chunk* chunk = malloc(sizeof(Mix_Chunk));
    if (!chunk) {
        free(buffer);
        return NULL;
    }

    chunk->allocated = 1;
    chunk->alen = samples * sizeof(Uint16);
    chunk->abuf = (Uint8*)buffer;
    chunk->volume = MIX_MAX_VOLUME;

    return chunk;
}



// Asset loading functions
// =====================

// Load a button with its images and sound
void loadButton(Button* btn, const char* path, const char* clickedPath, int x, int y, int width, int height) {
    // Load button images
    btn->image = IMG_Load(path);
    btn->clickedImage = IMG_Load(clickedPath);

    // Check if images loaded successfully
    if (!btn->image || !btn->clickedImage) {
        SDL_Color errorColor = { 255, 0, 0 };
        renderText(SDL_GetVideoSurface(), "Error loading button assets: ", 10, 10, errorColor, NULL);
        renderText(SDL_GetVideoSurface(), IMG_GetError(), 200, 10, errorColor, NULL);
        SDL_Flip(SDL_GetVideoSurface());
        SDL_Delay(3000);
        return;
    }

    // Set button position
    btn->position.x = x;
    btn->position.y = y;

    // Resize button images if needed
    if (btn->image->w != width || btn->image->h != height) {
        SDL_Surface* resized = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height,
            btn->image->format->BitsPerPixel,
            btn->image->format->Rmask,
            btn->image->format->Gmask,
            btn->image->format->Bmask,
            btn->image->format->Amask);

        SDL_Surface* resized_clicked = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height,
            btn->clickedImage->format->BitsPerPixel,
            btn->clickedImage->format->Rmask,
            btn->clickedImage->format->Gmask,
            btn->clickedImage->format->Bmask,
            btn->clickedImage->format->Amask);

        SDL_Rect src_rect = { 0, 0, btn->image->w, btn->image->h };
        SDL_Rect dest_rect = { 0, 0, width, height };

        SDL_SoftStretch(btn->image, &src_rect, resized, &dest_rect);
        SDL_SoftStretch(btn->clickedImage, &src_rect, resized_clicked, &dest_rect);

        SDL_FreeSurface(btn->image);
        SDL_FreeSurface(btn->clickedImage);

        btn->image = resized;
        btn->clickedImage = resized_clicked;
    }

    // Initialize button state
    btn->clicked = 0;

    // Assign sounds to buttons
    if (strcmp(path, "Red.png") == 0) {
        btn->clickSound = generateTone(red_btn); // Red button Sound
    } else if (strcmp(path, "green.png") == 0) {
        btn->clickSound = generateTone(green_btn); // sound Green Sound
    } else if (strcmp(path, "blue.png") == 0) {
        btn->clickSound = generateTone(blue_btn); // sound Blue Sound
    } else if (strcmp(path, "yellow.png") == 0) {
        btn->clickSound = generateTone(yellow_btn); // sound Yellow Sound
    }
}

// Load all game assets
void loadAssets(PuzzleGame* game) {
    // Load background image
    game->backgroundImage = IMG_Load("background.png");
    if (!game->backgroundImage) {
        SDL_Color errorColor = { 255, 0, 0 };
        renderText(SDL_GetVideoSurface(), "Error loading background image: ", 10, 10, errorColor, NULL);
        renderText(SDL_GetVideoSurface(), IMG_GetError(), 220, 10, errorColor, NULL);
        SDL_Flip(SDL_GetVideoSurface());
        SDL_Delay(3000);
        return;
    }

    // Button dimensions and spacing
    int buttonWidth = 200;
    int buttonHeight = 200;
    int buttonGap = 10;

    // Calculate the total width and height of the 2x2 grid
    int totalWidth = (2 * buttonWidth) + buttonGap;
    int totalHeight = (2 * buttonHeight) + buttonGap;

    // Calculate the starting X and Y positions to center the grid
    int startX = (SCREEN_WIDTH - totalWidth) / 2;
    int startY = (SCREEN_HEIGHT - totalHeight) / 2;

    // Load the buttons and set their positions for 2x2 grid
    loadButton(&game->buttons[0], "Red.png", "Red1.png", startX, startY, buttonWidth, buttonHeight);
    loadButton(&game->buttons[1], "green.png", "green1.png", startX + buttonWidth + buttonGap, startY, buttonWidth, buttonHeight);
    loadButton(&game->buttons[2], "blue.png", "blue1.png", startX, startY + buttonHeight + buttonGap, buttonWidth, buttonHeight);
    loadButton(&game->buttons[3], "yellow.png", "yellow1.png", startX + buttonWidth + buttonGap, startY + buttonHeight + buttonGap, buttonWidth, buttonHeight);
}
// Shuffle button colors and sounds while keeping positions
void shuffleButtons(Button buttons[4]) {
    // Create temporary copies of button properties
    SDL_Surface* tempImages[4];
    SDL_Surface* tempClickedImages[4];
    Mix_Chunk* tempSounds[4];
    
    // Store current properties
    for (int i = 0; i < 4; i++) {
        tempImages[i] = buttons[i].image;
        tempClickedImages[i] = buttons[i].clickedImage;
        tempSounds[i] = buttons[i].clickSound;
    }
    
    // Fisher-Yates shuffle algorithm
    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        
        // Swap properties
        SDL_Surface* tempImg = buttons[i].image;
        SDL_Surface* tempClickedImg = buttons[i].clickedImage;
        Mix_Chunk* tempSound = buttons[i].clickSound;
        
        buttons[i].image = buttons[j].image;
        buttons[i].clickedImage = buttons[j].clickedImage;
        buttons[i].clickSound = buttons[j].clickSound;
        
        buttons[j].image = tempImg;
        buttons[j].clickedImage = tempClickedImg;
        buttons[j].clickSound = tempSound;
    }
}

// Game initialization
// =================

// Initialize SDL and game state
void initSDL(PuzzleGame* game) {

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        exit(1);
    }
    
    // Initialize SDL_ttf
    if (TTF_Init() < 0) {
        printf("TTF initialization failed: %s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }
    
    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Mixer initialization failed: %s\n", Mix_GetError());
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }
    
    // Set up video mode
    game->screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);
    if (!game->screen) {
        printf("Video mode set failed: %s\n", SDL_GetError());
        Mix_CloseAudio();
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }
    
    // Set window title
    SDL_WM_SetCaption("Simon Game", NULL);
    
    // Load font
    game->font = TTF_OpenFont("alagard.ttf", 24);
    if (!game->font) {
        printf("Font loading failed: %s\n", TTF_GetError());
        Mix_CloseAudio();
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }
    
    // Initialize random seed
    srand(time(NULL));
    
    // Initialize game variables
    game->gameRunning = true;
    game->level = 1;
    game->currentSequenceLength = 0;
    game->userIndex = 0;
    game->playingSequence = false;
    game->startTime = SDL_GetTicks();
}



// Enhanced Rotozoom text rendering with better effects
void renderZoomText(SDL_Surface* screen, const char* text, int x, int y, 
                   SDL_Color color, TTF_Font* font, double zoom) {
    if (!font || !text) return;  // Check for null parameters

    // Create the basic text surface
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, color);
    if (!textSurface) return;

    // Calculate new dimensions based on zoom
    int newWidth = (int)(textSurface->w * zoom);
    int newHeight = (int)(textSurface->h * zoom);

    // Create zoomed surface
    SDL_Surface* zoomedSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 
                                                     newWidth, newHeight,
                                                     textSurface->format->BitsPerPixel,
                                                     textSurface->format->Rmask,
                                                     textSurface->format->Gmask,
                                                     textSurface->format->Bmask,
                                                     textSurface->format->Amask);

    if (!zoomedSurface) {
        SDL_FreeSurface(textSurface);
        return;
    }

    // Scale the text
    SDL_SoftStretch(textSurface, NULL, zoomedSurface, NULL);

    // Calculate position (center the text)
    SDL_Rect destRect = {
x - (newWidth / 2),  // Center horizontally
        y - (newHeight / 2), // Center vertically
        newWidth,
        newHeight
    };

    // Draw to screen
    SDL_BlitSurface(zoomedSurface, NULL, screen, &destRect);

    // Clean up
    SDL_FreeSurface(textSurface);
    SDL_FreeSurface(zoomedSurface);
}

// Enhanced animated message display
void showAnimatedMessage(PuzzleGame* game, const char* text, SDL_Color color) {
    // Setup basic parameters
    const int DURATION = 1500;        // Animation duration in milliseconds
    const int FRAME_DELAY = 16;       // Time between frames (60 FPS)
    const int CENTER_X = SCREEN_WIDTH/2;
    const int CENTER_Y = SCREEN_HEIGHT/2;
    
    // Load larger font for message
    TTF_Font* largeFont = TTF_OpenFont("alagard.ttf", 72);
    if (!largeFont) {
        largeFont = game->font; // Fallback to default font if loading fails
    }

    // Animation start time
    Uint32 startTime = SDL_GetTicks();
    Uint32 currentTime;

    // Main animation loop
    while ((currentTime = SDL_GetTicks()) - startTime < DURATION) {
        // Calculate animation progress (0.0 to 1.0)
        float progress = (currentTime - startTime) / (float)DURATION;
        
        // Simple zoom effect (1.0 to 1.3 and back)
        float zoom = 1.0f + 0.3f * sin(progress * M_PI);
        
         // Handle color effects
        SDL_Color messageColor =color;
        if (strcmp(text, "SUCCESS!") == 0) {
            // Bright green for success
            messageColor.r = 50;
            messageColor.g = 255;
            messageColor.b = 50;
        } 
        else if (strcmp(text, "FAILURE!") == 0) {
            // Bright red for failure
            messageColor.r = 255;
            messageColor.g = 50;
            messageColor.b = 50;
        }


        // Clear screen and draw background
        SDL_FillRect(game->screen, NULL, 0);
        if (game->backgroundImage) {
            SDL_BlitSurface(game->backgroundImage, NULL, game->screen, NULL);
        }

       
        
        // Render the text
        renderZoomText(game->screen, text, CENTER_X, CENTER_Y,messageColor, largeFont, zoom);
        
        // Update screen
        SDL_Flip(game->screen);
        
        // Control animation speed
        SDL_Delay(FRAME_DELAY);
    }
    
    // Cleanup
    if (largeFont != game->font) {
        TTF_CloseFont(largeFont);
    }
    
    // Restore normal screen
    SDL_FillRect(game->screen, NULL, 0);
    if (game->backgroundImage) {
        SDL_BlitSurface(game->backgroundImage, NULL, game->screen, NULL);
    }
    SDL_Flip(game->screen);
}

// Rendering functions
// =================
// Add this helper function for formatted time
void renderFormattedTime(SDL_Surface* screen, int minutes, int seconds, int x, int y, SDL_Color color, TTF_Font* font) {
    char timeStr[32];
    sprintf(timeStr, "%02d:%02d", minutes, seconds);
    renderText(screen, timeStr, x, y, color, font);
}
// Format time as MM:SS
void formatTime(SDL_Surface* screen, int totalSeconds, int x, int y, SDL_Color color, TTF_Font* font) {
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    renderFormattedTime(screen, minutes, seconds, x, y, color, font);
}

// Render text on the screen
void renderText(SDL_Surface* screen, const char* text, int x, int y, SDL_Color color, TTF_Font* font) {
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, color);
    if (textSurface != NULL) {
        // Render the text directly without a background
        SDL_Rect textRect = { x, y, textSurface->w, textSurface->h };
        SDL_BlitSurface(textSurface, NULL, screen, &textRect);
        SDL_FreeSurface(textSurface);
    }
}
// Add this helper function in puzzle.c
void renderNumber(SDL_Surface* screen, int number, int x, int y, SDL_Color color, TTF_Font* font) {
    char str[32];
    sprintf(str, "%d", number);
    renderText(screen, str, x, y, color, font);
}
// Render the timer
void renderTimer(PuzzleGame* game) {
    int width = 500;
    int height = 30;
    int timerY = SCREEN_HEIGHT - 70;
    int timerTextY = timerY - 40;
    
    SDL_Rect timerRect = { SCREEN_WIDTH / 2 - width / 2, timerY, width, height };
    
    // Calculate remaining time (always out of 30 seconds)
    int remainingTime = 30 - (SDL_GetTicks() - game->startTime) / 1000;
    
    // Render timer bar
    if (remainingTime > 0) {
        timerRect.w = (remainingTime * width) / 30;  // Always divide by 30 seconds
        
        // Change color based on remaining time
        Uint32 color;
        if (remainingTime > 15) {  // More than half time remaining
            color = SDL_MapRGB(game->screen->format, 0, 255, 0);  // Green
        } else if (remainingTime > 7) {  // More than quarter time remaining
            color = SDL_MapRGB(game->screen->format, 255, 255, 0);  // Yellow
        } else {
            color = SDL_MapRGB(game->screen->format, 255, 0, 0);  // Red
        }
        
        SDL_FillRect(game->screen, &timerRect, color);
        
        // Render time text
        SDL_Color textColor = {255, 255, 255};
        char timeText[32];
        sprintf(timeText, "%02d:%02d", remainingTime / 60, remainingTime % 60);
        renderText(game->screen, timeText, SCREEN_WIDTH/2 - 30, timerTextY, textColor, game->font);
    }
}

// Render the game state
void renderGame(PuzzleGame* game) {
    // Draw background
    SDL_BlitSurface(game->backgroundImage, NULL, game->screen, NULL);
    
    // Draw buttons
    for (int i = 0; i < 4; i++) {
        SDL_BlitSurface(
            game->buttons[i].clicked ? game->buttons[i].clickedImage : game->buttons[i].image,
            NULL,
            game->screen,
            &game->buttons[i].position
        );
    }
    
    // Draw timer
    renderTimer(game);
    
    // Define consistent text positions
    int leftMargin = 20;
    int topMargin = 20;
    int lineHeight = 30;
    
    // Draw level indicator
    SDL_Color color = { 255, 255, 255 };
    char levelText[32];
    snprintf(levelText, sizeof(levelText), "Level: %d", game->level);
    renderText(game->screen, levelText, leftMargin, topMargin, color, game->font);
    
    // Draw sequence length
    char seqText[32];
    snprintf(seqText, sizeof(seqText), "Sequence: %d/%d", game->currentSequenceLength, MAX_SEQUENCE_LENGTH);
    renderText(game->screen, seqText, leftMargin, topMargin + lineHeight, color, game->font);
    
    // Draw instructions based on game state
    if (game->playingSequence) {
        renderText(game->screen, "Watch the sequence!", SCREEN_WIDTH/2 - 100, topMargin, color, game->font);
    } else if (game->userIndex < game->currentSequenceLength) {
        renderText(game->screen, "Your turn! Repeat the sequence!", SCREEN_WIDTH/2 - 150, topMargin, color, game->font);
    }
    
    // Update the screen
    SDL_Flip(game->screen);
}

// Game logic functions
// ==================

// Generate a sequence of buttons
void generateSequence(PuzzleGame* game) {
    // For the first level, just generate one random button
    if (game->level == 1 && game->currentSequenceLength == 0) {
        game->sequence[0] = rand() % 4;
        game->currentSequenceLength = 1;
    } else {
        // For subsequent levels, add one new random button to the existing sequence
        game->sequence[game->currentSequenceLength] = rand() % 4;
        game->currentSequenceLength++;
    }
}

// Play the sequence for the player to remember
void playSequence(PuzzleGame* game) {
    game->playingSequence = 1;
    
    // Clear screen and show "Watch" message
    SDL_FillRect(game->screen, NULL, 0);
    SDL_Color color = {255, 255, 255};
    renderText(game->screen, "Watch the sequence...", SCREEN_WIDTH/2-100, 20, color, game->font);
    SDL_Flip(game->screen);
    SDL_Delay(1000);
    
    for (int i = 0; i < game->currentSequenceLength; i++) {
        int btnIndex = game->sequence[i];
        Button* btn = &game->buttons[btnIndex];
        
        // Visual feedback (show clicked image)
        SDL_BlitSurface(btn->clickedImage, NULL, game->screen, &btn->position);
        SDL_Flip(game->screen);
        
        // Audio feedback - use the pre-generated sound
        Mix_PlayChannel(-1, btn->clickSound, 0);
        SDL_Delay(500); // Show pressed state
        
        // Return to normal
        SDL_BlitSurface(btn->image, NULL, game->screen, &btn->position);
        SDL_Flip(game->screen);
        SDL_Delay(250); // Brief pause between buttons
    }
    
    game->playingSequence = 0;
    game->userIndex = 0;
    
    // Prompt user to repeat
    renderText(game->screen, "Your turn! Repeat the sequence", SCREEN_WIDTH/2-150, 20, color, game->font);
    SDL_Flip(game->screen);
}

// Handle mouse click events
void handleMouseClick(PuzzleGame* game, int x, int y) {
    // If the game is playing the sequence, ignore clicks
    if (game->playingSequence) return;
    
    // Define consistent text positions
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;
    
    // Check if a button was clicked
    for (int i = 0; i < 4; i++) {
        if (x >= game->buttons[i].position.x && 
            x < game->buttons[i].position.x + game->buttons[i].position.w &&
            y >= game->buttons[i].position.y && 
            y < game->buttons[i].position.y + game->buttons[i].position.h) {
            
            // Highlight the button
            game->buttons[i].clicked = 1;
            renderGame(game);
            
            // Play the sound
            Mix_PlayChannel(-1, game->buttons[i].clickSound, 0);
            SDL_Delay(200);
            
            // Unhighlight the button
            game->buttons[i].clicked = 0;
            renderGame(game);
            
            // Check if the button matches the sequence
            if (i == game->sequence[game->userIndex]) {
                game->userIndex++;
                
                // If the player completed the sequence
                if (game->userIndex == game->currentSequenceLength) {
                    // Calculate time bonus
                    
                    
                    // Play success sound
                    Mix_Chunk* successSound = generateSuccessSound();
                    if (successSound) {
                        Mix_PlayChannel(-1, successSound, 0);
                        SDL_Delay(1000);
                        Mix_FreeChunk(successSound);
                    }
                    
                    // Move to the next level
                    nextLevel(game);
                }
            } else {
                // Player made a mistake - restart from Level 1
                // Play failure sound
                Mix_Chunk* failureSound = generateFailureSound();
                if (failureSound) {
                    Mix_PlayChannel(-1, failureSound, 0);
                    
                    // Display failure message with animation
                    SDL_Color failureRed = {255, 50, 50, 255};
                    showAnimatedMessage(game, "FAILURE!", failureRed);
                    
                    SDL_Delay(500);  // Reduced delay after animation
                    Mix_FreeChunk(failureSound);
                }
                
                // Display game over message
                SDL_Color color = { 255, 0, 0 };
                renderText(game->screen, "Game Over! Click to restart.", centerX - 150, centerY, color, game->font);
                SDL_Flip(game->screen);
                
                // Wait for a click to restart
                SDL_Event event;
                int waiting = 1;
                while (waiting) {
                    while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_MOUSEBUTTONDOWN) {
                            waiting = 0;
                        }
                    }
                }
                
                // Reset the game to Level 1 with a new sequence
                game->level = 1;
                game->currentSequenceLength = 0;
                generateSequence(game);
                playSequence(game);
            }
            
            break;
        }
    }
}

// Move to the next level
void nextLevel(PuzzleGame* game) {
    game->level++;
    
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;
    
    if (game->level > MAX_LEVELS) {
        SDL_Color congratsColor = { 0, 255, 0 };
        renderText(game->screen, "Congratulations! You've completed all levels!", 
                  centerX - 200, centerY - 50, congratsColor, game->font);
        SDL_Flip(game->screen);
        SDL_Delay(2000);
        game->gameRunning = false;
        return;
    }
    
    // Play success sound and show message
    Mix_Chunk* successSound = generateSuccessSound();
    if (successSound) {
        Mix_PlayChannel(-1, successSound, 0);
        SDL_Color successGreen = {50, 255, 50, 255};
        showAnimatedMessage(game, "SUCCESS!", successGreen);
        SDL_Delay(500);
        Mix_FreeChunk(successSound);
    }

    // Display level completion message
    SDL_Color color = {255, 255, 255};
    char levelText[32];
    sprintf(levelText, "Level %d Complete!", game->level - 1);
    renderText(game->screen, levelText, centerX - 100, centerY, color, game->font);
    SDL_Flip(game->screen);
    SDL_Delay(1000);

    // Next level announcement
    Mix_Chunk* levelStartSound = generateLevelStartSound(game->level);
    if (levelStartSound) {
        Mix_PlayChannel(-1, levelStartSound, 0);
        char nextLevelText[32];
        sprintf(nextLevelText, "Moving to Level %d", game->level);
        renderText(game->screen, nextLevelText, centerX - 100, centerY + 50, color, game->font);
        SDL_Flip(game->screen);
        SDL_Delay(1200);
        Mix_FreeChunk(levelStartSound);
    }

    // Generate and prepare new sequence
    generateSequence(game);
    SDL_Delay(500);
    
    // Reset timer for new level
    game->startTime = SDL_GetTicks();
    playSequence(game);
}



// Modify the main game loop to include pause menu handling:
void gameLoop(PuzzleGame* game) {
    if (!game || !game->screen) {
        printf("Invalid game state\n");
        return;
    }

    SDL_Event event;
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;
    bool startScreen = true;
// Load background
     game->startBackground = IMG_Load("background.png");
    // Load all sounds with error checking
    game->buttonClickSound = Mix_LoadWAV("click.wav");
    game->buttonHoverSound = Mix_LoadWAV("hover.wav");
    game->startSound = Mix_LoadWAV("startsound.wav");
    game->gameMusic = Mix_LoadWAV("gamemusic.wav");
    if (!game->startBackground) {
        printf("Warning: Couldn't load background image: %s\n", IMG_GetError());
        
        }
    if (!game->startSound) {
        printf("Warning: Failed to load start sound: %s\n", Mix_GetError());
    } else {
        Mix_PlayChannel(-1, game->startSound, 0);
    }

    if (!game->buttonClickSound || !game->buttonHoverSound) {
        printf("Warning: Failed to load button sounds: %s\n", Mix_GetError());
    }

    // Create and load start button using loadButton function - adjusted size and position
    
    loadButton(&game->startButton, "start.png", "darkstart.png",centerX - 150,centerY + 180,300,150);          

    // Updated title and instructions text
    const char* gameTitle = "SIMON GAME";
    const char* subtitle = "Test Your Memory!";
    const char* instructions[] = {
        "HOW TO PLAY:",
        "Watch the sequence of colored buttons",
        "Remember and repeat the pattern",
        "Each level adds one more step",
        "Complete all levels to win!"
    };
    
    SDL_Color titleColor = {255, 215, 0};    // Bright gold for title
    SDL_Color subtitleColor = {135, 206, 235}; // Sky blue for subtitle
    SDL_Color whiteColor = {255, 255, 255};    // White for instructions
    
    
    bool buttonHovered = false;
    bool wasHovered = false; 

    // Start screen loop
    while (startScreen && game->gameRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                game->gameRunning = false;
                startScreen = false;
                break;
            }
            
            // Handle mouse hover
            if (event.type == SDL_MOUSEMOTION) {
                int mouseX = event.motion.x;
                int mouseY = event.motion.y;
                bool isHovered = (mouseX >= game->startButton.position.x && 
                                mouseX <= game->startButton.position.x + game->startButton.position.w &&
                                mouseY >= game->startButton.position.y && 
                                mouseY <= game->startButton.position.y + game->startButton.position.h);
                
                if (!game->wasHovered && isHovered && game->buttonHoverSound) {
                    Mix_PlayChannel(-1, game->buttonHoverSound, 0);
                }
                
                game->wasHovered = isHovered;
                game->startButton.clicked = isHovered;
            }
            
            // Handle button click
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
                if (mouseX >= game->startButton.position.x && 
                    mouseX <= game->startButton.position.x + game->startButton.position.w &&
                    mouseY >= game->startButton.position.y && 
                    mouseY <= game->startButton.position.y + game->startButton.position.h) {
                    
                    if (game->buttonClickSound) {
                        Mix_PlayChannel(-1, game->buttonClickSound, 0);
                        SDL_Delay(100);
                    }
                    startScreen = false;
                }
            }
        }

        // Draw background
        if (game->startBackground && game->screen) {
            SDL_BlitSurface(game->startBackground, NULL, game->screen, NULL);
        }

        

        // Render main title with pulsing effect
        float titleScale = 2.0f + 0.1f * sin(SDL_GetTicks() / 500.0f);
        renderZoomText(game->screen, gameTitle, 
                      centerX, 100,
                      titleColor, game->font, titleScale);

        // Render subtitle
        renderZoomText(game->screen, subtitle,
                      centerX, 180,
                      subtitleColor, game->font, 1.2f);

        // Render instructions with better spacing
        int instructionY = 280;
        renderZoomText(game->screen, instructions[0],  // "HOW TO PLAY:" header
                      centerX, instructionY,
                      subtitleColor, game->font, 1.3f);
        
        instructionY += 60;  // Extra space after header
        for (int i = 1; i < 5; i++) {
            renderText(game->screen, instructions[i], 
                      centerX - 200, instructionY + (i-1) * 45,  // Increased line spacing
                      whiteColor, game->font);
        }

        // Draw the start button
        SDL_BlitSurface(
            game->startButton.clicked ? game->startButton.clickedImage : game->startButton.image,
            NULL,
            game->screen,
            &game->startButton.position
        );

        SDL_Flip(game->screen);
        SDL_Delay(16);
    }


    // If game is still running, start the actual game
    if (game->gameRunning) {
        // Initialize game state
        game->level = 1;
        game->currentSequenceLength = 0;
        generateSequence(game);
        
        // Play start sound
        Mix_Chunk* levelStartSound = generateLevelStartSound(1);
        if (levelStartSound) {
            Mix_PlayChannel(-1, levelStartSound, 0);
            Mix_FreeChunk(levelStartSound);
        }

        // Start the first sequence
        game->startTime = SDL_GetTicks();
        playSequence(game);

        // Main game loop
        while (game->gameRunning) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    game->gameRunning = false;
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    game->gameRunning = false;
                } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    handleMouseClick(game, x, y);
                }
            }

            renderGame(game);
            SDL_Delay(10);
        }
    }
}

// Clean up resources
void cleanup(PuzzleGame* game) {
    if (!game) return;
  // Free background image
    SDL_FreeSurface(game->backgroundImage);
    // Stop all sounds
    Mix_HaltChannel(-1);
    Mix_HaltMusic();

    // Cleanup sounds
    if (game->buttonClickSound) {
        Mix_FreeChunk(game->buttonClickSound);
        game->buttonClickSound = NULL;
    }
    if (game->buttonHoverSound) {
        Mix_FreeChunk(game->buttonHoverSound);
        game->buttonHoverSound = NULL;
    }
    if (game->startSound) {
        Mix_FreeChunk(game->startSound);
        game->startSound = NULL;
    }
    if (game->gameMusic) {
        Mix_FreeChunk(game->gameMusic);
        game->gameMusic = NULL;
    }
// Free button resources
    for (int i = 0; i < 4; i++) {
        Button* btn = &game->buttons[i];
        if (btn->clickSound) {
            Mix_FreeChunk(btn->clickSound);
        }
        SDL_FreeSurface(btn->image);
        SDL_FreeSurface(btn->clickedImage);
    }
    // Cleanup surfaces
    if (game->startBackground) {
        SDL_FreeSurface(game->startBackground);
        game->startBackground = NULL;
    }
    if (game->startButton.image) {
        SDL_FreeSurface(game->startButton.image);
        game->startButton.image = NULL;
    }
    if (game->startButton.clickedImage) {
        SDL_FreeSurface(game->startButton.clickedImage);
        game->startButton.clickedImage = NULL;
    }
    if (game->screen) {
        SDL_FreeSurface(game->screen);
        game->screen = NULL;
    }
    if (game->font) {
        TTF_CloseFont(game->font);
        game->font = NULL;
    }

    // Close SDL subsystems
    Mix_CloseAudio();
    TTF_CloseFont(game->font);
    TTF_Quit();
    Mix_Quit();
    SDL_Quit();
}
