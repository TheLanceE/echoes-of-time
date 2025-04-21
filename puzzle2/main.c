#include "puzzle.h"

int main(int argc, char* argv[]) {
    // Initialize game data structure
 PuzzleGame game;
    
    // Initialize SDL and load assets
    initSDL(&game);
    loadAssets(&game);
    shuffleButtons(game.buttons);
    // Run the game loop
    gameLoop(&game);
    
    // Clean up resources
    cleanup(&game);
    
    return 0;
}
