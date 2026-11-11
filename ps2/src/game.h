#ifndef PS2BIRDS_GAME_H
#define PS2BIRDS_GAME_H
#include "level_config.h"
#include "campaign_types.h"
#define VIEW_SCALE LEVEL_VIEW_SCALE
#define VIEW_X LEVEL_VIEW_X
#define VIEW_Y LEVEL_VIEW_Y
#define BIRD_X (VIEW_X+LEVEL_SLING_X*VIEW_SCALE)
#define BIRD_Y (VIEW_Y+LEVEL_SLING_Y*VIEW_SCALE)
#define GROUND_Y (VIEW_Y+LEVEL_GROUND_Y*VIEW_SCALE)
#define GRAVITY (LEVEL_GRAVITY*VIEW_SCALE)
#ifdef __cplusplus
extern "C" {
#endif
typedef enum { BIRD_RED_TYPE, BIRD_BLUE_TYPE, BIRD_YELLOW_TYPE } BirdType;
/* Yellow boost: prototype approximation of the native BOOST specialty. */
#define YELLOW_BOOST_FACTOR 1.8f
typedef struct {float x,y,vx,vy,angle;int type,collided;} GameBird;
typedef enum { AIMING, FLYING, WON, LOST } GameState;
typedef struct {
    float x,y,vx,vy,angle,power,flight_time,cooldown,accumulator;
    int birds,score,pig_alive,block_alive;
    int level,unlocked,highscores[MAX_CAMPAIGN_LEVELS];
    float view_x,view_y,view_scale,pixels,ground_y,gravity,sling_x,sling_y;
    GameState state;
    void *physics;
} Game;
typedef struct { float x,y,width,height,angle,health; int kind,alive; const char *definition; } GameObject;
/* Initialize once before reset; destroy releases the Box2D world. */
void game_init(Game *game);
void game_destroy(Game *game);
void game_reset(Game *game);
void game_aim(Game *game,float angle_delta,float power_delta);
void game_launch(Game *game);
void game_update(Game *game,float seconds);
int game_special(Game *game);
int game_special_available(const Game *game);
int game_queue_type(const Game *game,int offset);
int game_projectile_count(const Game *game);
int game_get_projectile(const Game *game,int index,GameBird *bird);
void game_preview(const Game *game,float time,float *x,float *y);
int game_object_count(const Game *game);
int game_level_count(void);
int game_load_level(Game *game,int index);
int game_next_level(Game *game);
const char *game_level_title(int index);
const char *game_level_script(int index);
int game_get_object(const Game *game,int index,GameObject *object);
#ifdef __cplusplus
}
#endif
#endif
