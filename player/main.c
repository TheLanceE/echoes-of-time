/*
 * ======= الملف الرئيسي للعبة =======
 * يحتوي على نقطة البداية للبرنامج وحلقة اللعبة الرئيسية
 * المكتبات المستخدمة:
 * SDL.h        - مكتبة SDL الأساسية للرسوميات والمدخلات
 * SDL_image.h  - مكتبة تحميل الصور
 * SDL_ttf.h    - مكتبة معالجة الخطوط
 * SDL_mixer.h  - مكتبة الصوت
 * player.h     - ملف رأسي يحتوي على تعريفات اللعبة
 * stdio.h      - مكتبة الإدخال والإخراج
 * stdbool.h    - مكتبة القيم المنطقية
 */

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "player.h"
#include <stdio.h>
#include <stdbool.h>

// المتغيرات العامة
SDL_Surface *screen = NULL;      // سطح الشاشة الرئيسي
SDL_Surface *background = NULL;   // صورة الخلفية
SDL_Surface *heartSprite = NULL;  // صورة القلب
TTF_Font *font = NULL;           // الخط المستخدم

// حالة اللعبة
bool gameRunning = true;         // مؤشر استمرار اللعبة
Player player1, player2;         // كائنات اللاعبين
Menu menu;                       // القائمة الرئيسية
Obstacle obstacles[MAX_OBSTACLES]; // مصفوفة العقبات

/*
 * رسم قلوب (أرواح) اللاعب
 * @param player مؤشر إلى اللاعب
 * @param screen سطح الشاشة للرسم عليه
 */
void drawHearts(Player *player, SDL_Surface *screen) {
    if (!heartSprite) return;  // التحقق من وجود صورة القلب
    
    SDL_Rect heartPos;         // مستطيل موقع القلب
    heartPos.y = 10;            // المسافة من أعلى الشاشة - تم تقليلها
    heartPos.w = 20;           // عرض القلب - تم تصغيره
    heartPos.h = 20;           // ارتفاع القلب - تم تصغيره
    
    // رسم قلوب اللاعب الأول (على اليسار)
    if (!player->isPlayer2) {
        for (int i = 0; i < player->lives; i++) {
            heartPos.x = 20 + (i * 40);  // تقليل المسافة بين القلوب إلى 20 بكسل
            SDL_BlitSurface(heartSprite, NULL, screen, &heartPos);
        }
    }
    // رسم قلوب اللاعب الثاني (على اليمين)
    else {
        for (int i = 0; i < player->lives; i++) {
            heartPos.x = SCREEN_WIDTH - 130+ (i * 40);  // تعديل موقع البداية وتقليل المسافة بين القلوب
            SDL_BlitSurface(heartSprite, NULL, screen, &heartPos);
        }
    }
}

/*
 * الدالة الرئيسية للبرنامج
 */
int main(void) {
    // تهيئة SDL والأنظمة الفرعية
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // تهيئة نظام الصور
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image could not initialize! Error: %s\n", IMG_GetError());
        return -1;
    }

    // تهيئة نظام الخطوط
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! Error: %s\n", TTF_GetError());
        return -1;
    }

    // تهيئة نظام الصوت
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! Error: %s\n", Mix_GetError());
        return -1;
    }

    // إنشاء نافذة اللعبة
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);
    if (!screen) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    SDL_WM_SetCaption("Player Game", NULL);  // تعيين عنوان النافذة

    // تهيئة عناصر اللعبة
    initPlayer(&player1, false);  // تهيئة اللاعب الأول
    initPlayer(&player2, true);   // تهيئة اللاعب الثاني
    initMenu(&menu);              // تهيئة القائمة
    initObstacles(obstacles, MAX_OBSTACLES);  // تهيئة العقبات

    // تحميل صورة الخلفية
    background = IMG_Load("assets/backgrounds/main_bg.png");
    if (!background) {
        printf("Failed to load background: %s\n", IMG_GetError());
        printf("Creating fallback background\n");
        // إنشاء خلفية بديلة في حالة فشل التحميل
        background = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
        SDL_FillRect(background, NULL, SDL_MapRGB(screen->format, 0, 0, 128));
    }

    // تحميل صورة القلب
    heartSprite = IMG_Load("assets/ui/heart.png");
    if (!heartSprite) {
        printf("Failed to load heart sprite: %s\n", IMG_GetError());
    }

    // تحميل الخط للقائمة
    menu.font = TTF_OpenFont("/home/user/Desktop/player_/assets/ui/alagard.ttf", 24);
    if (menu.font == NULL) {
        printf("Unable to load font! SDL_ttf Error: %s\n", TTF_GetError());
        // محاولة تحميل الخط من المجلد الحالي كخيار بديل
        menu.font = TTF_OpenFont("./assets/ui/alagard.ttf", 24);
        if (menu.font == NULL) {
            printf("Fallback font load failed! SDL_ttf Error: %s\n", TTF_GetError());
        }
    }

    // ====== حلقة اللعبة الرئيسية ======
    while (gameRunning) {
        SDL_Event event;
        // معالجة الأحداث
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gameRunning = false;  // إنهاء اللعبة عند الضغط على زر الإغلاق
            }
            
            // معالجة مدخلات اللاعبين والقائمة
            handlePlayerInput(&player1, &event);
            handlePlayerInput(&player2, &event);
            updateMenu(&menu, &event, &player1, &player2);
        }

        // تحديث حالة عناصر اللعبة
        updatePlayer(&player1, NULL);
        updatePlayer(&player2, NULL);
        updateObstacles(obstacles, &player1, &player2);

        // ====== الرسم على الشاشة ======
        // رسم الخلفية
        if (background) {
            SDL_BlitSurface(background, NULL, screen, NULL);
        } else {
            SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 128));
        }
        
        // رسم عناصر اللعبة
        drawPlayer(&player1, screen);  // رسم اللاعب الأول
        drawPlayer(&player2, screen);  // رسم اللاعب الثاني
        drawObstacles(obstacles, screen);  // رسم العقبات
        drawMenu(&menu, screen);  // رسم القائمة
        
        // رسم قلوب اللاعبين
        drawHearts(&player1, screen);
        drawHearts(&player2, screen);
        
        SDL_Flip(screen);  // تحديث الشاشة
        SDL_Delay(16);     // تأخير لتحقيق 60 إطار في الثانية
    }

    // ====== تنظيف الموارد ======
    freePlayer(&player1);  // تحرير موارد اللاعب الأول
    freePlayer(&player2);  // تحرير موارد اللاعب الثاني
    freeMenu(&menu);       // تحرير موارد القائمة
    
    if (heartSprite) SDL_FreeSurface(heartSprite);  // تحرير صورة القلب
    
    // إغلاق الأنظمة الفرعية
    Mix_CloseAudio();  // إغلاق نظام الصوت
    TTF_Quit();        // إغلاق نظام الخطوط
    IMG_Quit();        // إغلاق نظام الصور
    SDL_Quit();        // إغلاق SDL

    return 0;
}
