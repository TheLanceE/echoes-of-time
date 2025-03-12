#include "header.h"

void show_image(Image img, SDL_Surface *screen) {
    SDL_BlitSurface(img.image, &img.ipos, screen, &img.pos);
}

void free_image(Image *img) {
    if (img->image) {
        SDL_FreeSurface(img->image);
        img->image = NULL;
    }
}

void init_image(Image *img, const char *path, int x, int y, int w, int h) {
    img->image = IMG_Load(path);
    if (img->image == NULL) {
        printf("Error Loading Image : %s\n", SDL_GetError());
        return;
    }
    img->pos.x = x;
    img->pos.y = y;
    img->ipos.x = 0;
    img->ipos.y = 0;
    img->ipos.w = w;
    img->ipos.h = h;
}

int image_collision(Image img, int x, int y) {
    return (x >= img.pos.x && x <= img.pos.x + img.ipos.w &&
            y >= img.pos.y && y <= img.pos.y + img.ipos.h);
}
