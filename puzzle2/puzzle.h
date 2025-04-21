#ifndef PUZZLE_H
#define PUZZLE_H

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include <SDL/SDL_rotozoom.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <string.h>

// Constants for audio generation
#define SAMPLE_RATE 44100  // Standard sample rate (44.1kHz)
#define AMPLITUDE 28000
#define DURATION_MS 300

// Button frequencies (musical notes)
#define red_btn 261.63  // Red button
#define green_btn 329.63  // Green button
#define blue_btn 392.00  // Blue button
#define NOTE_B4 493.88  // Added for success/failure sounds
#define yellow_btn 523.25  // Yellow button
#define NOTE_D5 587.33  // Added for success/failure sounds
#define intro_sound 659.25  // Added for intro sounds

// Screen dimensions
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

// Game constants
#define MAX_SEQUENCE_LENGTH 10
#define MAX_LEVELS 10
#define BASE_TIME 30  // Base time in seconds for level 1

// Colors for Simon buttons
#define RED 0
#define GREEN 1
#define BLUE 2
#define YELLOW 3

// Animation constants
#define INTRO_ANIMATION_FRAMES 30
#define FADE_STEP 15
#define COUNTDOWN_DURATION 1000
#define INTRO_SOUND_DURATION 2000
#define COUNTDOWN_BEEP_DURATION 200

// Button structure
typedef struct {
    SDL_Surface *image;          // Normal button image
    SDL_Surface *clickedImage;   // Lit up image
    SDL_Rect position;           // Button position
    int clicked;                 // Button state
    Mix_Chunk *clickSound;       // Button sound
    int highlighted;             // Highlight state
} Button;

// Game states
typedef enum {
    GAME_STATE_WELCOME,
    GAME_STATE_PLAYING_SEQUENCE,
    GAME_STATE_WAITING_FOR_INPUT,
    GAME_STATE_SUCCESS,
    GAME_STATE_FAILURE,
    GAME_STATE_GAME_OVER
} GameState;

// Game structure to hold all the game-related data
typedef struct {
    SDL_Surface* screen;               // Main screen surface
    SDL_Surface* backgroundImage;
    Button buttons[4];                  // 4 Simon buttons (red, green, blue, yellow)
    int sequence[MAX_LEVELS];         // The sequence for the player to follow (max 10 steps for 10 levels)
    int playingSequence;   // The player's input sequence (max 10 steps)
    int currentSequenceLength;          // Current length of the every sequence (level)
    int userIndex;                   // Current round number
    int currentInputIndex;
    Uint32 startTime;                   // Game start time for the timer                
    Uint32 lastButtonPress;          // Elapsed time for the round
    bool gameRunning;                   // Flag to control the game loop
    bool pieceSelected;                 // Whether the player has selected a button
    int level;                          // Current level (1 to 10)
    int levelTimers[10];                // Timer durations for each level (30 seconds for alllevels)
    GameState gameState;
    SDL_Rect menuRects[3];
    TTF_Font *font;
    // for starting game
    Mix_Chunk* buttonClickSound;   
    Mix_Chunk* buttonHoverSound;
    Mix_Chunk* startSound;
    Mix_Chunk* gameMusic;
    SDL_Surface* startBackground;
    Button startButton;
    bool wasHovered;
} PuzzleGame;


// Function declarations

// Audio functions
Mix_Chunk* generateTone(double frequency);
Mix_Chunk* generateLevelStartSound(int level);
Mix_Chunk* generateSuccessSound();
Mix_Chunk* generateFailureSound();
Mix_Chunk* generateButtonSound(int buttonIndex);

void loadButton(Button* btn, const char* path, const char* clickedPath, int x, int y, int width, int height);
// Initialize SDL, fonts, and sound
void initSDL(PuzzleGame* game);

// Load all images and sounds (buttons, background, etc.)
void loadAssets(PuzzleGame* game);
// Shuffle button colors/sounds .. when starting game and restart game
void shuffleButtons(Button buttons[4]);



// Render the main game screen (buttons, sequence, etc.)
void renderGame(PuzzleGame* game);
void renderText(SDL_Surface *screen, const char *text, int x, int y, SDL_Color color, TTF_Font *font);
void renderNumber(SDL_Surface* screen, int number, int x, int y, SDL_Color color, TTF_Font* font);
void renderFormattedTime(SDL_Surface* screen, int minutes, int seconds, int x, int y, SDL_Color color, TTF_Font* font);

// Play the current sequence of colors (buttons flashing in order)
void playSequence(PuzzleGame* game);

// Check if the player's sequence matches the correct sequence
bool checkPlayerSequence(PuzzleGame* game);

// Handle mouse click events to check if a button was clicked
void handleMouseClick(PuzzleGame* game, int x, int y);


// Rendering functions for rotozoom

void renderZoomText(SDL_Surface* screen, const char* text, int x, int y, 
                   SDL_Color color, TTF_Font* font, double zoom);
void showAnimatedMessage(PuzzleGame* game, const char* text, SDL_Color base_color);
void showSuccessMessage(PuzzleGame* game);
void showFailureMessage(PuzzleGame* game);


void renderTimer(PuzzleGame* game);
void formatTime(SDL_Surface* screen, int totalSeconds, int x, int y, SDL_Color color, TTF_Font* font);
// Update the game timer (used for visualizing time left in the round)
void updateTimer(PuzzleGame* game);


// Randomly generate the next color in the sequence
void generateSequence(PuzzleGame* game);
void playSequence(PuzzleGame* game);
void nextLevel(PuzzleGame* game);

void gameLoop(PuzzleGame* game);



void cleanup(PuzzleGame* game);
#endif // PUZZLE_H

