#include "header.h"

int main() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        printf("Initialization Error : %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (screen == NULL) {
        printf("Error Initializing The Screen : %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) {
        printf("Error Initializing Audio : %s\n", Mix_GetError());
        SDL_Quit();
        return 1;
    }

    if (TTF_Init() == -1) {
        printf("Error Initializing TTF : %s\n", TTF_GetError());
        Mix_CloseAudio();
        SDL_Quit();
        return 1;
    }

    // Charger la musique
    Mix_Music *music = Mix_LoadMUS("music.mp3");
    if (music == NULL) {
        printf("Error loading music: %s\n", Mix_GetError());
    } else {
        Mix_PlayMusic(music, -1); // Jouer la musique en boucle
    }

    int running = 1, mx, my;
    Image backg, play_btn, play_hover, history_btn, history_hover, highscores_btn, highscores_hover, options_btn, options_hover;
    SDL_Event event;
    
    // Initialiser les images
    init_image(&backg, "backg1.jpeg", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); // Agrandir le fond d'écran
    init_image(&play_btn, "play.png", 600, 200, 400, 100); // Bouton Play (normal)
    init_image(&play_hover, "play_hover.png", 600, 200, 400, 100); // Bouton Play (hover)
    init_image(&history_btn, "History.png", 600, 350, 400, 100); // Bouton History (normal)
    init_image(&history_hover, "History_hover.png", 600, 350, 400, 100); // Bouton History (hover)
    init_image(&highscores_btn, "highscorespressed.png", 600, 500, 400, 100); // Bouton Highscores (normal)
    init_image(&highscores_hover, "highscorespressed_hover.png", 600, 500, 400, 100); // Bouton Highscores (hover)
    init_image(&options_btn, "optionspressed.png", 600, 650, 400, 100); // Bouton Options (normal)
    init_image(&options_hover, "optionspressed_hover.png", 600, 650, 400, 100); // Bouton Options (hover)

    while (running) {
        SDL_PollEvent(&event);
        SDL_GetMouseState(&mx, &my);
        
        // Afficher l'arrière-plan
        show_image(backg, screen);

        // Afficher les boutons avec effet hover
        if (image_collision(play_btn, mx, my)) {
            show_image(play_hover, screen); // Afficher l'image hover
        } else {
            show_image(play_btn, screen); // Afficher l'image normale
        }

        if (image_collision(history_btn, mx, my)) {
            show_image(history_hover, screen); // Afficher l'image hover
        } else {
            show_image(history_btn, screen); // Afficher l'image normale
        }

        if (image_collision(highscores_btn, mx, my)) {
            show_image(highscores_hover, screen); // Afficher l'image hover
        } else {
            show_image(highscores_btn, screen); // Afficher l'image normale
        }

        if (image_collision(options_btn, mx, my)) {
            show_image(options_hover, screen); // Afficher l'image hover
        } else {
            show_image(options_btn, screen); // Afficher l'image normale
        }

        // Gestion des événements
        switch (event.type) {
            case SDL_QUIT:
                running = 0;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (image_collision(play_btn, mx, my)) {
                        printf("Play button clicked!\n");
                    } else if (image_collision(history_btn, mx, my)) {
                        printf("History button clicked!\n");
                    } else if (image_collision(highscores_btn, mx, my)) {
                        printf("Highscores button clicked!\n");
                    } else if (image_collision(options_btn, mx, my)) {
                        printf("Options button clicked!\n");
                    }
                }
                break;
        }
        
        SDL_Flip(screen);
    }
    
    // Libérer les ressources
    free_image(&backg);
    free_image(&play_btn);
    free_image(&play_hover);
    free_image(&history_btn);
    free_image(&history_hover);
    free_image(&highscores_btn);
    free_image(&highscores_hover);
    free_image(&options_btn);
    free_image(&options_hover);

    if (music) {
        Mix_FreeMusic(music);
    }
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
