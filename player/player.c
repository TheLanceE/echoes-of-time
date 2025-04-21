#include "player.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdbool.h>

/*
 * ======= شرح المكتبات المستخدمة =======
 * player.h     - ملف رأسي يحتوي على التعريفات الأساسية للعبة
 * stdio.h      - مكتبة الإدخال والإخراج القياسية (printf, scanf)
 * string.h     - مكتبة معالجة النصوص (strcpy, strlen)
 * stdlib.h     - مكتبة الدوال القياسية (malloc, free)
 * SDL.h        - مكتبة SDL الأساسية للرسوميات والمدخلات
 * SDL_image.h  - مكتبة تحميل الصور بتنسيقات مختلفة
 * SDL_ttf.h    - مكتبة معالجة الخطوط والنصوص
 * SDL_mixer.h  - مكتبة تشغيل الأصوات والموسيقى
 * stdbool.h    - مكتبة القيم المنطقية (true/false)
 */

// أبعاد الصور في ملف الحركة
#define PLAYER_WIDTH 32   // عرض كل إطار في ملف الحركة
#define PLAYER_HEIGHT 32  // ارتفاع كل إطار في ملف الحركة
#define PLAYER_SCALE 2.0  // معامل تكبير اللاعب على الشاشة

/*
* تهيئة بيانات اللاعب
* تقوم بتحميل الصور والأصوات وتعيين القيم الابتدائية
* @param player مؤشر إلى هيكل اللاعب
* @param isPlayer2 محدد ما إذا كان هذا هو اللاعب الثاني
*/
void initPlayer(Player *player, bool isPlayer2) {
    // تحديد المسار الأساسي للصور حسب نوع اللاعب
    const char* basePath = isPlayer2 ? "assets/players/player2/" : "assets/players/player1/";
    // قائمة بأسماء ملفات الحركات
    const char* spriteFiles[] = {"idle.png", "walk.png", "attack.png", "damage.png", "dead.png"};

    // تحميل صور الحركات
    for (int i = 0; i < 5; i++) {
        char fullPath[256];  // مصفوفة لتخزين المسار الكامل
        sprintf(fullPath, "%s%s", basePath, spriteFiles[i]);  // دمج المسار مع اسم الملف
        player->sprite[i] = IMG_Load(fullPath);  // تحميل الصورة
        if (!player->sprite[i]) {
            printf("Failed to load %s: %s\n", fullPath, IMG_GetError());
        }
    }
    
    // تحميل الأصوات
    const char* soundBasePath = isPlayer2 ? "assets/sounds/player2/" : "assets/sounds/player1/";
    char soundPath[256];
    
    // تحميل صوت المشي
    snprintf(soundPath, sizeof(soundPath), "%swalk.wav", soundBasePath);
    player->sounds.walkSound = Mix_LoadWAV(soundPath);
    
    // تحميل صوت الهجوم
    snprintf(soundPath, sizeof(soundPath), "%sattack.wav", soundBasePath);
    player->sounds.attackSound = Mix_LoadWAV(soundPath);
    
    // تحميل صوت تلقي الضرر
    snprintf(soundPath, sizeof(soundPath), "%sdamage.wav", soundBasePath);
    player->sounds.damageSound = Mix_LoadWAV(soundPath);
    
    // تحميل صوت الموت
    snprintf(soundPath, sizeof(soundPath), "%sdead.wav", soundBasePath);
    player->sounds.deadSound = Mix_LoadWAV(soundPath);
    
    // التحقق من نجاح تحميل الأصوات
    if (!player->sounds.walkSound || !player->sounds.attackSound || 
        !player->sounds.damageSound || !player->sounds.deadSound) {
        printf("Failed to load some sounds: %s\n", Mix_GetError());
    }
    
    // تعيين موقع وحجم اللاعب على الشاشة
    player->position.x = isPlayer2 ? SCREEN_WIDTH - 200 : 200;  // موقع البداية حسب نوع اللاعب
    player->position.y = isPlayer2 ? SCREEN_HEIGHT - (PLAYER_HEIGHT * PLAYER_SCALE) - 50 
                                 : SCREEN_HEIGHT - (PLAYER_HEIGHT * PLAYER_SCALE) - 50;
    player->position.w = PLAYER_WIDTH * PLAYER_SCALE;   // عرض اللاعب مع التكبير
    player->position.h = PLAYER_HEIGHT * PLAYER_SCALE;  // ارتفاع اللاعب مع التكبير
    
    // تعيين منطقة القص من ملف الحركة
    player->spriteRect.x = 0;
    player->spriteRect.y = 0;
    player->spriteRect.w = PLAYER_WIDTH;
    player->spriteRect.h = PLAYER_HEIGHT;
    
    // تعيين القيم الابتدائية للاعب
    player->lives = 3;           // عدد الأرواح
    player->score = 0;           // النقاط
    player->state = IDLE;        // حالة الوقوف
    player->isFacingRight = true; // اتجاه اليمين
    player->isAttacking = false;  // ليس في حالة هجوم
    player->isTakingDamage = false; // لا يتلقى ضرراً
    player->frame = 0;           // إطار الحركة الأول
    player->frameTimer = 0;      // مؤقت الحركة
    player->health = 100;        // الصحة كاملة
    player->isPlayer2 = isPlayer2; // تحديد نوع اللاعب
}

/*
* تحديث حالة اللاعب
* تعالج المدخلات وتحرك اللاعب وتحدث الحركات
* @param player مؤشر إلى هيكل اللاعب
* @param event مؤشر إلى حدث SDL
*/
void updatePlayer(Player *player, SDL_Event *event) {
    // حفظ الحالة السابقة للاعب
    PlayerState prevState = player->state;
    
    // معالجة المدخلات إذا وجدت
    if (event) {
        handlePlayerInput(player, event);
    }
    
    // تحريك الصور
    animatePlayer(player);
    
    // تحديث موقع اللاعب عند المشي
    if (player->state == WALK) {
        if (player->isFacingRight) {
            player->position.x += PLAYER_SPEED;  // التحرك يميناً
        } else {
            player->position.x -= PLAYER_SPEED;  // التحرك يساراً
        }
    }
    
    // تشغيل الصوت المناسب عند تغيير الحالة
    if (prevState != player->state) {
        switch (player->state) {
            case WALK:
                if (player->sounds.walkSound) Mix_PlayChannel(-1, player->sounds.walkSound, 0);
                break;
            case ATTACK:
                if (player->sounds.attackSound) Mix_PlayChannel(-1, player->sounds.attackSound, 0);
                break;
            case DAMAGE:
                if (player->sounds.damageSound) Mix_PlayChannel(-1, player->sounds.damageSound, 0);
                break;
            case DEAD:
                if (player->sounds.deadSound) Mix_PlayChannel(-1, player->sounds.deadSound, 0);
                break;
            default:
                break;
        }
    }
    
    // التحقق من حدود الشاشة
    if (player->position.x < 0) player->position.x = 0;  // منع الخروج من اليسار
    if (player->position.x > SCREEN_WIDTH - PLAYER_WIDTH) 
        player->position.x = SCREEN_WIDTH - PLAYER_WIDTH;  // منع الخروج من اليمين
}

/*
* رسم اللاعب على الشاشة
* تعرض الإطار الحالي للاعب مع مراعاة الاتجاه
* @param player مؤشر إلى هيكل اللاعب
* @param screen سطح الشاشة للرسم عليه
*/
void drawPlayer(Player *player, SDL_Surface *screen) {
    if (player->state >= 0 && player->state < 5 && player->sprite[player->state]) {
        SDL_Rect destRect = player->position;
        SDL_Rect srcRect = player->spriteRect;
        
        // Flip the sprite horizontally if facing left
        if (!player->isFacingRight) {
            SDL_Surface* flippedSprite = SDL_CreateRGBSurface(
                SDL_SWSURFACE,
                player->sprite[player->state]->w,
                player->sprite[player->state]->h,
                32,
                0x00FF0000,
                0x0000FF00,
                0x000000FF,
                0xFF000000
            );
            
            for (int y = 0; y < player->sprite[player->state]->h; y++) {
                for (int x = 0; x < player->sprite[player->state]->w; x++) {
                    Uint32* pixels = (Uint32*)player->sprite[player->state]->pixels;
                    Uint32* flippedPixels = (Uint32*)flippedSprite->pixels;
                    flippedPixels[y * flippedSprite->w + (flippedSprite->w - 1 - x)] = 
                        pixels[y * player->sprite[player->state]->w + x];
                }
            }
            
            SDL_BlitSurface(flippedSprite, &srcRect, screen, &destRect);
            SDL_FreeSurface(flippedSprite);
        } else {
            SDL_BlitSurface(player->sprite[player->state], &srcRect, screen, &destRect);
        }
    }
    drawPlayerHearts(player, screen);
}

/*
* معالجة مدخلات المستخدم
* تستجيب لضغطات المفاتيح وتغير حالة اللاعب
* @param player مؤشر إلى هيكل اللاعب
* @param event مؤشر إلى حدث SDL
*/
void handlePlayerInput(Player *player, SDL_Event *event) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            // ------ Player 1 Controls ------
            case SDLK_LEFT:
                if (!player->isPlayer2) {
                    player->state = WALK;
                    player->isFacingRight = false;
                }
                break;
                
            case SDLK_RIGHT:
                if (!player->isPlayer2) {
                    player->state = WALK;
                    player->isFacingRight = true;
                }
                break;
                
            case SDLK_SPACE:
                if (!player->isPlayer2) {
                    player->state = ATTACK;
                    player->isAttacking = true;
                }
                break;

            // ------ Player 2 Controls ------
            case SDLK_a:
                if (player->isPlayer2) {
                    player->state = WALK;
                    player->isFacingRight = false;
                }
                break;
                
            case SDLK_d:
                if (player->isPlayer2) {
                    player->state = WALK;
                    player->isFacingRight = true;
                }
                break;
                
            case SDLK_f:
                if (player->isPlayer2) {
                    player->state = ATTACK;
                    player->isAttacking = true;
                }
                break;

            default:
                break; // Ignore any other keys
        }
    }
    
    if (event->type == SDL_KEYUP) {
        switch (event->key.keysym.sym) {
            case SDLK_LEFT:
            case SDLK_RIGHT:
                if (!player->isPlayer2 && player->state == WALK) {
                    player->state = IDLE;
                }
                break;
                
            case SDLK_a:
            case SDLK_d:
                if (player->isPlayer2 && player->state == WALK) {
                    player->state = IDLE;
                }
                break;
                
            case SDLK_SPACE:
                if (!player->isPlayer2 && player->state == ATTACK) {
                    player->isAttacking = false;
                    player->state = IDLE;
                }
                break;
                
            case SDLK_f:
                if (player->isPlayer2 && player->state == ATTACK) {
                    player->isAttacking = false;
                    player->state = IDLE;
                }
                break;
                
            default:
                break; // Ignore any other keys
        }
    }
}

/*
* تحريك صور اللاعب
* تتحكم في تتابع إطارات الحركة وسرعتها
* @param player مؤشر إلى هيكل اللاعب
*/
void animatePlayer(Player *player) {
    if (!player) return;  // التحقق من صحة المؤشر
    
    // زيادة مؤقت الإطارات
    player->frameTimer++;
    
    // عدد الإطارات لكل حالة
    const int frames[] = {
        4,    // IDLE - عدد إطارات الوقوف
        6,    // WALK - عدد إطارات المشي
        10,   // ATTACK - عدد إطارات الهجوم
        2,    // DAMAGE - عدد إطارات تلقي الضرر
        19    // DEAD - عدد إطارات الموت
    };
    
    // سرعة الحركة لكل حالة (كلما زاد الرقم قلت السرعة)
    const int speeds[] = {
        12,   // IDLE - سرعة حركة الوقوف (بطيئة)
        6,    // WALK - سرعة حركة المشي (متوسطة)
        3,    // ATTACK - سرعة حركة الهجوم (سريعة)
        10,   // DAMAGE - سرعة حركة تلقي الضرر
        5     // DEAD - سرعة حركة الموت
    };
    
    // معالجة خاصة لحالة الموت
    if (player->state == DEAD) {
        // السماح بتشغيل حركة الموت مرة واحدة فقط
        if (player->frame < frames[DEAD] - 1) {
            if (player->frameTimer >= speeds[DEAD]) {
                player->frame++;  // الانتقال للإطار التالي
                player->spriteRect.x = player->frame * PLAYER_WIDTH;  // تحديث موقع القص
                player->frameTimer = 0;  // إعادة ضبط المؤقت
            }
        }
        return;
    }
    
    // تحريك باقي الحالات
    if (player->state >= 0 && player->state < 5) {
        if (player->frameTimer >= speeds[player->state]) {
            // تحديث رقم الإطار الحالي بشكل دائري
            player->frame = (player->frame + 1) % frames[player->state];
            // تحديث موقع القص من ملف الحركة
            player->spriteRect.x = player->frame * PLAYER_WIDTH;
            // إعادة ضبط المؤقت
            player->frameTimer = 0;
            
            // العودة لحالة الوقوف بعد انتهاء حركة الهجوم
            if (player->state == ATTACK && player->frame == frames[ATTACK] - 1) {
                player->state = IDLE;
                player->isAttacking = false;
                player->frame = 0;  // إعادة ضبط رقم الإطار
            }
            
            // العودة لحالة الوقوف بعد انتهاء حركة تلقي الضرر
            if (player->state == DAMAGE && player->frame == frames[DAMAGE] - 1) {
                player->state = IDLE;
                player->isTakingDamage = false;
                player->frame = 0;  // إعادة ضبط رقم الإطار
            }
        }
    }
}

/*
* معالجة تلقي الضرر
* تخفض الصحة وتغير الحالة وتشغل الصوت المناسب
* @param player مؤشر إلى هيكل اللاعب
*/
void takeDamage(Player *player) {
    // التحقق من أن اللاعب لا يتلقى ضرراً حالياً
    if (!player->isTakingDamage) {
        player->health -= 20;  // تخفيض الصحة
        player->isTakingDamage = true;  // تفعيل حالة تلقي الضرر
        player->state = DAMAGE;  // تغيير حالة اللاعب
        
        // التحقق من نفاد الصحة
        if (player->health <= 0) {
            player->lives--;  // تخفيض عدد الأرواح
            player->health = 100;  // إعادة ملء الصحة للحياة التالية
            
            // التحقق من نفاد الأرواح
            if (player->lives <= 0) {
                player->state = DEAD;  // تفعيل حالة الموت
            }
        }
    }
}

/*
* إعادة تعيين حالة اللاعب
* تستخدم عند بدء جولة جديدة
* @param player مؤشر إلى هيكل اللاعب
*/
void resetPlayer(Player *player) {
    player->lives = 3;  // إعادة تعيين عدد الأرواح
    player->score = 0;  // إعادة تعيين النقاط
    player->health = 100;  // إعادة تعيين الصحة
    player->state = IDLE;  // العودة لحالة الوقوف
    player->isTakingDamage = false;  // إلغاء حالة تلقي الضرر
    // إعادة تعيين الموقع حسب نوع اللاعب
    player->position.x = player->isPlayer2 ? SCREEN_WIDTH - 200 : 200;
    player->position.y = player->isPlayer2 ? SCREEN_HEIGHT - (PLAYER_HEIGHT * PLAYER_SCALE) - 150 
                                         : SCREEN_HEIGHT - (PLAYER_HEIGHT * PLAYER_SCALE) - 50;
}

/*
* تحرير الموارد المستخدمة
* تنظيف الذاكرة عند إغلاق اللعبة
* @param player مؤشر إلى هيكل اللاعب
*/
void freePlayer(Player *player) {
    // تحرير صور الحركات
    for (int i = 0; i < 5; i++) {
        if (player->sprite[i]) {
            SDL_FreeSurface(player->sprite[i]);
            player->sprite[i] = NULL;
        }
    }
    
    // تحرير الأصوات
    if (player->sounds.walkSound) Mix_FreeChunk(player->sounds.walkSound);
    if (player->sounds.attackSound) Mix_FreeChunk(player->sounds.attackSound);
    if (player->sounds.damageSound) Mix_FreeChunk(player->sounds.damageSound);
    if (player->sounds.deadSound) Mix_FreeChunk(player->sounds.deadSound);
}

/*
* تغيير شكل اللاعب
* تحميل مجموعة صور جديدة للاعب
* @param player مؤشر إلى هيكل اللاعب
* @param newSpritePath مسار ملفات الصور الجديدة
*/
void changePlayerSprite(Player *player, const char *newSpritePath) {
    // تحرير الصور القديمة
    for (int i = 0; i < 5; i++) {
        if (player->sprite[i]) {
            SDL_FreeSurface(player->sprite[i]);
            player->sprite[i] = NULL;
        }
    }
    
    // تحديد المسار الجديد للصور
    const char* basePath = strstr(newSpritePath, "player1") ? "players/player1/" : "players/player2/";
    const char* spriteFiles[] = {"idle.png", "walk.png", "attack.png", "damage.png", "dead.png"};
    
    // تحميل الصور الجديدة
    for (int i = 0; i < 5; i++) {
        char fullPath[256];
        sprintf(fullPath, "%s%s", basePath, spriteFiles[i]);
        player->sprite[i] = IMG_Load(fullPath);
        if (!player->sprite[i]) {
            printf("Failed to load %s: %s\n", fullPath, IMG_GetError());
        }
    }
}

/*
* تهيئة القائمة الرئيسية
* تحميل الخط وتعيين الألوان والقيم الابتدائية
* @param menu مؤشر إلى هيكل القائمة
*/
void initMenu(Menu *menu) {
    menu->isVisible = false;  // إخفاء القائمة عند البداية
    menu->selectedOption = MENU_RESUME;  // تحديد الخيار الافتراضي
    
    // تحميل الخط
    menu->font = TTF_OpenFont("./assets/ui/alagard.ttf", 24);
    if (menu->font == NULL) {
        printf("Unable to load font! SDL_ttf Error: %s\n", TTF_GetError());
        exit(1);
    }
    
    // تعيين ألوان النصوص
    menu->textColor.r = 255;  // لون النص العادي (أبيض)
    menu->textColor.g = 255;
    menu->textColor.b = 255;
    
    menu->selectedColor.r = 255;  // لون النص المحدد (أحمر)
    menu->selectedColor.g = 0;
    menu->selectedColor.b = 0;
}

/*
* تحديث حالة القائمة
* معالجة اختيارات المستخدم في القائمة
* @param menu مؤشر إلى هيكل القائمة
* @param event مؤشر إلى حدث SDL
* @param player1 مؤشر إلى اللاعب الأول
* @param player2 مؤشر إلى اللاعب الثاني
*/
void updateMenu(Menu *menu, SDL_Event *event, Player *player1, Player *player2) {
    // معالجة ضغطات المفاتيح
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_ESCAPE:  // مفتاح ESC لإظهار/إخفاء القائمة
                menu->isVisible = !menu->isVisible;
                break;
            case SDLK_UP:  // السهم لأعلى للتنقل بين الخيارات
                if (menu->isVisible) {
                    menu->selectedOption = (menu->selectedOption - 1 + 4) % 4;
                }
                break;
            case SDLK_DOWN:  // السهم لأسفل للتنقل بين الخيارات
                if (menu->isVisible) {
                    menu->selectedOption = (menu->selectedOption + 1) % 4;
                }
                break;
            case SDLK_RETURN:  // مفتاح Enter لتنفيذ الخيار المحدد
                if (menu->isVisible) {
                    switch (menu->selectedOption) {
                        case MENU_RESUME:  // استئناف اللعب
                            menu->isVisible = false;
                            break;
                        case MENU_CHANGE_CHARACTER:  // تغيير الشخصية
                            changePlayerSprite(player1, player1->isPlayer2 ? "player2.png" : "player1.png");
                            changePlayerSprite(player2, player2->isPlayer2 ? "player2.png" : "player1.png");
                            break;
                        case MENU_SETTINGS:  // الإعدادات (لم يتم تنفيذها بعد)
                            break;
                        case MENU_QUIT:  // الخروج من اللعبة
                            exit(0);
                            break;
                    }
                }
                break;
        }
    }
}

/*
* رسم القائمة على الشاشة
* عرض الخيارات مع تمييز الخيار المحدد
* @param menu مؤشر إلى هيكل القائمة
* @param screen سطح الشاشة للرسم عليه
*/
void drawMenu(Menu *menu, SDL_Surface *screen) {
    if (!menu->isVisible) return;  // لا نرسم القائمة إذا كانت مخفية

    // قائمة الخيارات المتاحة
    const char *options[] = {"Resume", "Change Character", "Settings", "Quit"};
    
    // تحديد موقع وحجم خلفية القائمة
    SDL_Rect menuPos = {SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 150, 300, 300};
    
    // إنشاء خلفية شبه شفافة للقائمة
    SDL_Surface *menuBg = SDL_CreateRGBSurface(SDL_SWSURFACE, 200, 200, 32, 
                                              0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    SDL_FillRect(menuBg, NULL, SDL_MapRGBA(screen->format, 0, 0, 0, 180));
    SDL_BlitSurface(menuBg, NULL, screen, &menuPos);
    SDL_FreeSurface(menuBg);

    // رسم خيارات القائمة
    for (int i = 0; i < 4; i++) {
        // تحديد لون النص (أحمر للمحدد، أبيض للباقي)
        SDL_Color color = (i == (int)menu->selectedOption) ? menu->selectedColor : menu->textColor;
        // إنشاء سطح النص
        SDL_Surface *text = TTF_RenderText_Solid(menu->font, options[i], color);
        if (text) {
            // تحديد موقع النص وعرضه
            SDL_Rect textPos = {menuPos.x + 50, menuPos.y + 20 + i * 40, text->w, text->h};
            SDL_BlitSurface(text, NULL, screen, &textPos);
            SDL_FreeSurface(text);
        }
    }
}

/*
* تحرير موارد القائمة
* تنظيف الذاكرة المستخدمة للقائمة
* @param menu مؤشر إلى هيكل القائمة
*/
void freeMenu(Menu *menu) {
    if (menu->font) {
        TTF_CloseFont(menu->font);  // تحرير الخط
        menu->font = NULL;
    }
}

/*
* تهيئة العقبات في المستوى
* تحديد مواقع العقبات وحالتها
* @param obstacles مصفوفة العقبات
* @param count عدد العقبات المطلوب
*/
void initObstacles(Obstacle obstacles[], int count) {
    // تعيين المسافة الأفقية بين العقبات
    const int OBSTACLE_WIDTH = 50;  // عرض العقبة
    const int OBSTACLE_HEIGHT = 50; // ارتفاع العقبة
    const int SPACING = 70;         // المسافة بين كل عقبة والأخرى
    
    // حساب ارتفاع المستوى الذي ستوضع عليه العقبات
    const int LEVEL_HEIGHT = SCREEN_HEIGHT - 150;  // ارتفاع المستوى من أسفل الشاشة
    
    // تهيئة كل العقبات
    for (int i = 0; i < count; i++) {
        obstacles[i].position.x = 100 + (i * (OBSTACLE_WIDTH + SPACING));  // وضع العقبات بجانب بعضها
        obstacles[i].position.y = LEVEL_HEIGHT;                            // نفس الارتفاع لكل العقبات
        obstacles[i].position.w = OBSTACLE_WIDTH;
        obstacles[i].position.h = OBSTACLE_HEIGHT;
        obstacles[i].isActive = true;  // تنشيط جميع العقبات
    }
}

/*
* تحديث حالة العقبات
* التحقق من الاصطدام مع اللاعبين
* @param obstacles مصفوفة العقبات
* @param player1 مؤشر إلى اللاعب الأول
* @param player2 مؤشر إلى اللاعب الثاني
*/
void updateObstacles(Obstacle obstacles[], Player *player1, Player *player2) {
    // فحص التصادم مع كل عقبة نشطة
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].isActive) {
            // فحص التصادم مع اللاعب الأول
            if (checkCollision(player1->position, obstacles[i].position)) {
                takeDamage(player1);  // إلحاق الضرر باللاعب
                obstacles[i].isActive = false;  // إزالة العقبة
            }
            // فحص التصادم مع اللاعب الثاني
            if (checkCollision(player2->position, obstacles[i].position)) {
                takeDamage(player2);  // إلحاق الضرر باللاعب
                obstacles[i].isActive = false;  // إزالة العقبة
            }
        }
    }
}

/*
* رسم العقبات على الشاشة
* عرض العقبات النشطة فقط
* @param obstacles مصفوفة العقبات
* @param screen سطح الشاشة للرسم عليه
*/
void drawObstacles(Obstacle obstacles[], SDL_Surface *screen) {
    // تحميل صورة العقبة
    SDL_Surface *obstacleSprite = IMG_Load("./assets/obstacles/obstacle.png");
    if (!obstacleSprite) {
        printf("Unable to load obstacle sprite: %s\n", IMG_GetError());
        return;
    }

    // رسم العقبات النشطة فقط
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].isActive) {
            SDL_BlitSurface(obstacleSprite, NULL, screen, &obstacles[i].position);
        }
    }

    // تحرير صورة العقبة
    SDL_FreeSurface(obstacleSprite);
}

/*
* التحقق من التصادم
* تحديد ما إذا كان هناك تداخل بين مستطيلين
* @param a المستطيل الأول
* @param b المستطيل الثاني
* @return true إذا كان هناك تصادم، false إذا لم يكن
*/
bool checkCollision(SDL_Rect a, SDL_Rect b) {
    // التحقق من التداخل بين المستطيلين في كل الاتجاهات
    return (a.x < b.x + b.w &&    // حافة a اليسرى أقل من حافة b اليمنى
            a.x + a.w > b.x &&    // حافة a اليمنى أكبر من حافة b اليسرى
            a.y < b.y + b.h &&    // حافة a العليا أقل من حافة b السفلى
            a.y + a.h > b.y);     // حافة a السفلى أكبر من حافة b العليا
}

/*
* رسم قلوب اللاعب
* عرض عدد الأرواح المتبقية للاعب بتصميم بكسل
* @param player مؤشر إلى هيكل اللاعب
* @param screen سطح الشاشة للرسم عليه
*/
void drawPlayerHearts(Player *player, SDL_Surface *screen) {
    // تحميل صورة القلب
    SDL_Surface *heartSprite = IMG_Load("./assets/ui/pixel_heart.png");
    if (!heartSprite) {
        printf("Unable to load heart sprite: %s\n", IMG_GetError());
        return;
    }

    // تحديد حجم القلب الجديد ليتناسب مع التصميم البكسل
    SDL_Rect heartRect = {0, 0, 5, 5};  // حجم أصغر للقلوب بتصميم البكسل
    
    // تحديد موقع القلوب حسب نوع اللاعب
    int startX = player->isPlayer2 ? SCREEN_WIDTH - 100: 10;  // يمين الشاشة للاعب 2، يسار الشاشة للاعب 1
    int y = 1;  // المسافة من أعلى الشاشة - تقريب القلوب من الحافة العليا
    
    // رسم القلوب في صف أفقي مع مسافات متقاربة
    for (int i = 0; i < player->lives; i++) {
        SDL_Rect heartPos = {
            startX + (i * 40),  // 20 بكسل فقط بين كل قلب للتقارب
            y,
            heartRect.w,
            heartRect.h
        };
        SDL_BlitSurface(heartSprite, &heartRect, screen, &heartPos);
    }
    
    // تحرير صورة القلب
    SDL_FreeSurface(heartSprite);
}
