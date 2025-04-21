#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdbool.h>

// أبعاد الشاشة
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

// أبعاد اللاعب وسرعته
#define PLAYER_WIDTH 300
#define PLAYER_HEIGHT 300
#define PLAYER_SPEED 5
#define PLAYER_SCALE 5.0  // زيادة حجم اللاعب من 2.0 إلى 3.0
#define MAX_OBSTACLES 10

// تعريف حالات اللاعب المختلفة
typedef enum {
    IDLE,    // وضع الوقوف
    WALK,    // وضع المشي
    ATTACK,  // وضع الهجوم
    DAMAGE,  // وضع تلقي الضرر
    DEAD     // وضع الموت
} PlayerState;

// تعريف خيارات القائمة الرئيسية
typedef enum {
    MENU_RESUME,           // استئناف اللعب
    MENU_CHANGE_CHARACTER, // تغيير الشخصية
    MENU_SETTINGS,         // الإعدادات
    MENU_QUIT             // الخروج من اللعبة
} MenuOption;

// هيكل للمؤثرات الصوتية الخاصة باللاعب
typedef struct {
    Mix_Chunk *walkSound;    // صوت المشي
    Mix_Chunk *attackSound;  // صوت الهجوم
    Mix_Chunk *damageSound;  // صوت تلقي الضرر
    Mix_Chunk *deadSound;    // صوت الموت
} PlayerSounds;

// هيكل بيانات اللاعب الرئيسي
typedef struct {
    SDL_Surface *sprite[5];  // مصفوفة تحتوي على صور الحركات المختلفة
    SDL_Rect position;       // موقع اللاعب على الشاشة
    SDL_Rect spriteRect;     // موقع الصورة الحالية في ملف الحركة
    int lives;              // عدد الأرواح المتبقية
    int score;              // النقاط المحرزة
    PlayerState state;      // الحالة الحالية للاعب
    bool isFacingRight;     // هل اللاعب يواجه اليمين
    bool isAttacking;       // هل اللاعب في وضع الهجوم
    bool isTakingDamage;    // هل يتلقى اللاعب ضرراً
    int frame;              // رقم الإطار الحالي في الحركة
    int frameTimer;         // مؤقت لتحريك الإطارات
    int health;             // نسبة الصحة الحالية
    bool isPlayer2;         // هل هو اللاعب الثاني
    PlayerSounds sounds;    // الأصوات الخاصة باللاعب
} Player;

// هيكل بيانات القائمة
typedef struct {
    bool isVisible;           // هل القائمة ظاهرة
    MenuOption selectedOption; // الخيار المحدد حالياً
    TTF_Font *font;           // الخط المستخدم
    SDL_Color textColor;      // لون النص العادي
    SDL_Color selectedColor;  // لون النص المحدد
} Menu;

// هيكل بيانات العقبات
typedef struct {
    SDL_Rect position;  // موقع العقبة
    bool isActive;      // هل العقبة نشطة
} Obstacle;

// المتغيرات العامة المشتركة
extern SDL_Surface *screen;     // سطح الشاشة الرئيسي
extern SDL_Surface *heartSprite; // صورة القلب للأرواح
extern TTF_Font *font;          // الخط المستخدم
extern SDL_Surface *background; // صورة الخلفية

// ======= دوال اللاعب =======

// تهيئة بيانات اللاعب الأساسية
// تقوم بتحميل الصور والأصوات وتعيين القيم الابتدائية
void initPlayer(Player *player, bool isPlayer2);

// تحديث حالة اللاعب في كل إطار
// تتحكم في حركة اللاعب وتحديث حالته وتشغيل الأصوات
void updatePlayer(Player *player, SDL_Event *event);

// رسم اللاعب على الشاشة
// تقوم برسم الإطار الحالي للاعب مع مراعاة الاتجاه
void drawPlayer(Player *player, SDL_Surface *screen);

// معالجة مدخلات المستخدم
// تستجيب لضغطات المفاتيح وتغير حالة اللاعب
void handlePlayerInput(Player *player, SDL_Event *event);

// تحريك صور اللاعب
// تتحكم في تتابع إطارات الحركة وسرعتها
void animatePlayer(Player *player);

// معالجة تلقي الضرر
// تخفض الصحة وتغير الحالة وتشغل الصوت المناسب
void takeDamage(Player *player);

// إعادة تعيين حالة اللاعب
// تستخدم عند بدء جولة جديدة
void resetPlayer(Player *player);

// تحرير الموارد المستخدمة
// تنظيف الذاكرة عند إغلاق اللعبة
void freePlayer(Player *player);

// تغيير شكل اللاعب
// تحميل مجموعة صور جديدة للاعب
void changePlayerSprite(Player *player, const char *newSpritePath);

// ======= دوال القائمة =======

// تهيئة القائمة الرئيسية
// تحميل الخط وتعيين الألوان والقيم الابتدائية
void initMenu(Menu *menu);

// تحديث حالة القائمة
// معالجة اختيارات المستخدم في القائمة
void updateMenu(Menu *menu, SDL_Event *event, Player *player1, Player *player2);

// رسم القائمة على الشاشة
// عرض الخيارات مع تمييز الخيار المحدد
void drawMenu(Menu *menu, SDL_Surface *screen);

// تحرير موارد القائمة
// تنظيف الذاكرة المستخدمة للقائمة
void freeMenu(Menu *menu);

// ======= دوال العقبات =======

// تهيئة العقبات في المستوى
// تحديد مواقع العقبات وحالتها
void initObstacles(Obstacle obstacles[], int count);

// تحديث حالة العقبات
// التحقق من الاصطدام مع اللاعبين
void updateObstacles(Obstacle obstacles[], Player *player1, Player *player2);

// رسم العقبات على الشاشة
// عرض العقبات النشطة فقط
void drawObstacles(Obstacle obstacles[], SDL_Surface *screen);

// التحقق من التصادم
// تحديد ما إذا كان هناك تداخل بين مستطيلين
bool checkCollision(SDL_Rect a, SDL_Rect b);

#endif // PLAYER_H
