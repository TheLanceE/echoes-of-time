#include "game.h"

void init_minimap(minimap *m, int level) {
    m->level = level;
    m->is_colliding = 0;

    // Load minimap background based on level
    char *path = (level == LEVEL1) ? "minimap_level1.png" : "minimap_level2.png";
    m->thumbnail = IMG_Load(path);
    if (!m->thumbnail) {
        printf("Failed to load minimap background\n");
        exit(1);
    }

    // Set fixed minimap position
    m->position_minimap.x = MINIMAP_X; // 1280
    m->position_minimap.y = MINIMAP_Y; // 720
    m->position_minimap.w = MINIMAP_WIDTH; // 320
    m->position_minimap.h = MINIMAP_HEIGHT; // 180

    // Load player marker
    m->man_image = IMG_Load("man.png");
    if (!m->man_image) {
        printf("Failed to load man image, using fallback\n");
        m->man_image = SDL_CreateRGBSurface(SDL_SWSURFACE, 5, 5, 32, 0, 0, 0, 0);
        SDL_FillRect(m->man_image, NULL, SDL_MapRGB(m->man_image->format, 255, 0, 0));
    }

    // Initial player position on minimap
    m->position_man.x = MINIMAP_X;
    m->position_man.y = MINIMAP_Y;
    m->position_man.w = 5; // Small size for marker
    m->position_man.h = 5;
}

void MAJMinimap(SDL_Rect posJoueur, minimap *m, SDL_Rect camera) {
    int posJoueurABS_x = posJoueur.x + camera.x;
    int posJoueurABS_y = posJoueur.y + camera.y;

    // Update player position on minimap using fixed scale factor
    m->position_man.x = MINIMAP_X + (posJoueurABS_x / SCALE_FACTOR);
    m->position_man.y = MINIMAP_Y + (posJoueurABS_y / SCALE_FACTOR);
    m->position_man.w = posJoueur.w / SCALE_FACTOR;
    m->position_man.h = posJoueur.h / SCALE_FACTOR;
}

void display_minimap(minimap m, SDL_Surface *screen) {
    SDL_BlitSurface(m.thumbnail, NULL, screen, &m.position_minimap);
    SDL_BlitSurface(m.man_image, NULL, screen, &m.position_man);
}

void Liberer_minimap(minimap *m) {
    if (m->thumbnail) SDL_FreeSurface(m->thumbnail);
    if (m->man_image) SDL_FreeSurface(m->man_image);
    m->thumbnail = NULL;
    m->man_image = NULL;
}

void background_animation_after_collision(minimap *m, SDL_Surface *screen) {
    m->is_colliding = 1;
    
    // Shake animation - move background left and right quickly
    SDL_Rect shake_pos = m->position_minimap;
    
    // Number of shakes and shake intensity
    int num_shakes = 5;
    int shake_intensity = 5;
    
    for (int i = 0; i < num_shakes; i++) {
        // Move right
        shake_pos.x = m->position_minimap.x + shake_intensity;
        SDL_BlitSurface(m->thumbnail, NULL, screen, &shake_pos);
        SDL_Flip(screen);
        SDL_Delay(30);
        
        // Move left
        shake_pos.x = m->position_minimap.x - shake_intensity;
        SDL_BlitSurface(m->thumbnail, NULL, screen, &shake_pos);
        SDL_Flip(screen);
        SDL_Delay(30);
    }
    
    // Return to normal position
    SDL_BlitSurface(m->thumbnail, NULL, screen, &m->position_minimap);
    SDL_Flip(screen);
    
    m->is_colliding = 0;
}