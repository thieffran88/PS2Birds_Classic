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
typedef enum { BIRD_RED_TYPE, BIRD_BLUE_TYPE, BIRD_YELLOW_TYPE, BIRD_BLACK_TYPE, BIRD_WHITE_TYPE, BIRD_EGG_TYPE, BIRD_GREEN_TYPE, BIRD_BIG_TYPE, BIRD_PUFFER_TYPE, BIRD_PINK_TYPE, BIRD_SHOCK_TYPE } BirdType;
/* Stella (PinkBird, specialty "BUBBLES"): body from blocks.lua, algorithm from mode_MuseumMode.lua,
   bubbles.lua and default_conf.lua. A tap starts a PINK_BOOM_TIME timer (1.5 s on a first contact
   without tap); during its last PINK_BOOM_TIME s the bird stops and hovers; then every movable
   piece within PINK_RADIUS (and the bird) floats in a bubble for BUBBLE_TIME s: force
   (dx*BUBBLE_FLOAT_FACTOR*mass, -gravity*mass*BUBBLE_ANTI_GRAVITY) while not rising faster than
   BUBBLE_MAX_VELOCITY; their breakable joints are destroyed; the bird leaves when its bubble pops. */
#define PINK_RADIUS 0.85f
#define PINK_DENSITY 6.0f
#define PINK_FRICTION 0.3f
#define PINK_RESTITUTION 0.43f
#define PINK_DAMAGE_FACTOR 0.5f
#define PINK_BOOM_TIME 0.5f
#define PINK_CONTACT_DELAY 1.5f
#define PINK_BUBBLE_RADIUS 7.0f
#define BUBBLE_TIME 2.0f
#define BUBBLE_ANTI_GRAVITY 1.8f
#define BUBBLE_MAX_VELOCITY 10.0f
#define BUBBLE_FLOAT_FACTOR 1.3f
/* GlobeBird ("PUFFER"), all values from blocks.lua; algorithm from puffer_bird.lua.
   Tap (also allowed after contact; automatic GLOBE_AUTO_PUFF_DELAY s after the first contact):
   friction/restitution/density switch to the puffed values, the circle grows to
   GLOBE_PUFFED_RADIUS, and makeExplosion pushes nearby pieces (radius 8, force 45000, no damage).
   After GLOBE_PUFF_TIME s it shrinks to nothing over GLOBE_DEATH_TIME s, jittering its velocity
   by +-GLOBE_DIRECTION_MAGNITUDE every GLOBE_DIRECTION_TIME s, then disappears. */
#define GLOBE_RADIUS 0.85f
#define GLOBE_DENSITY 5.0f
#define GLOBE_FRICTION 0.3f
#define GLOBE_RESTITUTION 0.25f
#define GLOBE_PUFFED_RADIUS 7.5f
#define GLOBE_PUFFED_FRICTION 0.5f
#define GLOBE_PUFFED_RESTITUTION 0.0f
#define GLOBE_PUFFED_DENSITY 0.1f
#define GLOBE_AUTO_PUFF_DELAY 1.5f
#define GLOBE_PUFF_TIME 1.5f
#define GLOBE_DEATH_TIME 1.5f
#define GLOBE_DIRECTION_TIME 0.05f
#define GLOBE_DIRECTION_MAGNITUDE 50.0f
#define GLOBE_BLAST_RADIUS 8.0f
#define GLOBE_BLAST_FORCE 45000.0f
#define GLOBE_DAMAGE_RADIUS 5.0f
/* Terence (RedBigBird), from blocks.lua: radius 1.8, density 10, friction 0.3, restitution 0.45,
   no specialty. RedBigBirdDamageFactors: damage x0.3 on wood/rock/light (velocity x1.8 not ported). */
#define BIG_RADIUS 1.8f
#define BIG_DENSITY 10.0f
#define BIG_RESTITUTION 0.45f
#define BIG_DAMAGE_FACTOR 0.3f
/* Green boomerang bird (BoomerangBird): every value below is recovered. blocks.lua gives the
   body (radius 1.5, density 3, friction 0.3, restitution 0.23) and the specialty fields;
   gamelogic.lua gives the algorithm. Before the tap the bird gets an upward force of
   flyVerticalAcceleration*mass. The tap sets xForce=xVel*physicsScale*mass*horizontalForce and
   yForce=(2-min(|yVel/xVel|,2))*physicsScale*mass*verticalForce (slingshot power is 1); until the
   first contact xForce is applied while |xVel|<=maxHorizontalSpeed and yForce while yVel>5. */
#define GREEN_RADIUS 1.5f
#define GREEN_DENSITY 3.0f
#define GREEN_RESTITUTION 0.23f
#define GREEN_FLY_VERTICAL_ACCELERATION -2.8f
#define GREEN_HORIZONTAL_FORCE -50.0f
#define GREEN_VERTICAL_FORCE -350.0f
#define GREEN_MAX_HORIZONTAL_SPEED 50.0f
#define GREEN_PHYSICS_SCALE (1.0f/20.0f)
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
/* Static fragile pieces (staticFragile group, clouds material), e.g. StaticBalloon03-05;
   destroyedScoreInc 100 from blocks.lua. */
#define FLOOR_HALF_WIDTH 1000.0f
/* Experimental: 1 creates scene pieces asleep (native "sleeping" state, see docs/LACUNAS_PORT.md). Off: evidence is indirect. */
#ifndef SCENE_STARTS_ASLEEP
#define SCENE_STARTS_ASLEEP 0
#endif
#define OBJECT_KIND_FRAGILE 8
#define FRAGILE_DESTROYED_SCORE 100
/* StaticFragileBlock06: ground piece of the staticFragile group (immovableFragile material,
   destroyedScoreInc 0). Shares the pop-on-impact rule of the balloons. */
#define OBJECT_KIND_FRAGILE_GROUND 9
#define IS_FRAGILE_KIND(k) ((k)==OBJECT_KIND_FRAGILE || (k)==OBJECT_KIND_FRAGILE_GROUND)
/* Delay before a crate destroyed by another blast goes off; port approximation. */
#define TNT_CHAIN_SECONDS 0.1f
#define BLACK_BLAST_RADIUS 15.0f
#define BLACK_DAMAGE_RADIUS 5.0f
#define BLACK_BLAST_FORCE 40000.0f
#define BLACK_BLAST_DAMAGE 300.0f
#define BLACK_FUSE_SECONDS 1.0f
#define BLACK_VISUAL_SECONDS 0.45f
/* Short Fuse black bird (BlackBird_Shockwave in blocks.lua; BlackBirdAbility.lua): an electric
   shockwave instead of an explosion. Triggered by a tap, or explosionTimer (1.5 s) after the
   first contact; after `delay` the wave grows to `radius` over `duration` (cosine ease). Every
   dynamic piece inside gets damage once and a push d*force each step (force drops from 470 to
   25 after 10% of the wave); pigs inside are electrocuted and die 2 s later. */
#define SHOCK_FUSE_SECONDS 1.5f
#define SHOCK_DELAY 0.4f
#define SHOCK_DURATION 1.9f
#define SHOCK_RADIUS 15.0f
#define SHOCK_FORCE 470.0f
#define SHOCK_MIN_FORCE 25.0f
#define SHOCK_DROP_OFF 0.1f
#define SHOCK_DAMAGE 0.35f
#define SHOCK_MIN_DAMAGE 0.25f
#define SHOCK_ELECTROCUTE_SECONDS 2.0f
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
typedef struct { float x,y,width,height,angle,health; int kind,alive; const char *definition; int mirror; } GameObject;
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
/* Host tests: removes the piece's health; it breaks on the next physics tick. */
void game_damage_object(Game *game,int index);
int game_get_blast(const Game *game,GameBlast *blast);
int game_projectile_count(const Game *game);
int game_get_projectile(const Game *game,int index,GameBird *bird);
void game_preview(const Game *game,float time,float *x,float *y);
int game_object_count(const Game *game);
/* Reads the stages (CAMPAIGN.BIN); returns the number of stages or -1. */
int game_campaign_load(const unsigned char *data,unsigned size);
int game_campaign_load_file(const char *path);
int game_level_count(void);
int game_load_level(Game *game,int index);
int game_next_level(Game *game);
const char *game_level_title(int index);
const char *game_level_script(int index);
/* Texture bank holding the sprites of a stage's episode. */
int game_level_bank(int index);
/* Stars (0-3) a score earns on a stage: 0 when unwon, then win/silver/gold thresholds. */
int game_level_stars(int index,int score);
float game_level_gravity(int index);
int game_get_object(const Game *game,int index,GameObject *object);
#ifdef __cplusplus
}
#endif
#endif
