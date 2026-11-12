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
typedef enum { BIRD_RED_TYPE, BIRD_BLUE_TYPE, BIRD_YELLOW_TYPE, BIRD_BLACK_TYPE, BIRD_WHITE_TYPE, BIRD_EGG_TYPE } BirdType;
/* Yellow boost: prototype approximation of the native BOOST specialty. */
#define YELLOW_BOOST_FACTOR 1.8f
/* Launch speed recovered from gamelogic.lua: updateValues() sets defaultForce=-925 and
   physicsScale=1/physicsToWorld=1/20; SlingshotSystem.launchBird applies an impulse of
   -defaultForce*physicsScale*mass*stretch, i.e. 46.25 physics units/s at full stretch,
   independent of the camera. Game.power is the stretch in percent. */
#define LAUNCH_SPEED_MAX 46.25f
#define POWER_MIN 20.0f
#define POWER_MAX 100.0f
#define ANGLE_MIN -30.0f
#define ANGLE_MAX 85.0f
/* White drop/recoil are port approximations; egg blast values are recovered. */
#define WHITE_EGG_RADIUS 1.125f
#define WHITE_DROP_SPEED 35.0f
#define WHITE_RECOIL_SPEED 25.0f
#define WHITE_RECOIL_X_FACTOR 1.2f
#define EGG_BLAST_RADIUS 10.0f
#define EGG_DAMAGE_RADIUS 8.0f
#define EGG_BLAST_FORCE 10000.0f
#define EGG_BLAST_DAMAGE 400.0f
/* ExtraBlockTNT blast values recovered from blocks.lua; propagation shares the port's radial model. */
#define TNT_BLAST_RADIUS 10.0f
#define TNT_DAMAGE_RADIUS 5.0f
#define TNT_BLAST_FORCE 30000.0f
#define TNT_BLAST_DAMAGE 150.0f
#define OBJECT_KIND_TNT 7
/* Delay before a crate destroyed by another blast goes off; port approximation. */
#define TNT_CHAIN_SECONDS 0.1f
#define BLACK_BLAST_RADIUS 15.0f
#define BLACK_DAMAGE_RADIUS 5.0f
#define BLACK_BLAST_FORCE 40000.0f
#define BLACK_BLAST_DAMAGE 300.0f
#define BLACK_FUSE_SECONDS 1.0f
#define BLACK_VISUAL_SECONDS 0.45f
typedef struct {float x,y,vx,vy,angle,fuse;int type,collided,special_used;} GameBird;
typedef struct {float x,y,radius,life;} GameBlast;
enum {GAME_EVENT_PIG=1, GAME_EVENT_WOOD=2, GAME_EVENT_GLASS=4, GAME_EVENT_STONE=8, GAME_EVENT_EXTRA=16, GAME_EVENT_EXPLOSION=32};
typedef enum { AIMING, FLYING, WON, LOST } GameState;
typedef struct {
    float x,y,vx,vy,angle,power,flight_time,cooldown,accumulator;
    int birds,score,pig_alive,block_alive;
    unsigned int events;
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
int game_get_blast(const Game *game,GameBlast *blast);
int game_projectile_count(const Game *game);
int game_get_projectile(const Game *game,int index,GameBird *bird);
void game_preview(const Game *game,float time,float *x,float *y);
int game_object_count(const Game *game);
int game_level_count(void);
int game_load_level(Game *game,int index);
int game_next_level(Game *game);
const char *game_level_title(int index);
const char *game_level_script(int index);
/* Texture bank holding the sprites of a stage's episode. */
int game_level_bank(int index);
int game_get_object(const Game *game,int index,GameObject *object);
#ifdef __cplusplus
}
#endif
#endif
