#ifndef PS2BIRDS_CAMPAIGN_TYPES_H
#define PS2BIRDS_CAMPAIGN_TYPES_H
#define MAX_LEVEL_OBJECTS 640
#define MAX_LEVEL_JOINTS 512
#define MAX_CAMPAIGN_LEVELS 1024
/* spawn_of: 0 for pieces present from the start; 1 + parent index for pieces released when
   the parent is destroyed (blocks.lua deathSpawn, e.g. the cream of Spawning_cake). */
/* flags: OBJECT_NO_COLLISION for pieces declared with collision = false in blocks.lua
   (drawn, but nothing touches them). */
/* OBJECT_MIRROR_X/Y: negative scaleX/scaleY in the level (mirrored piece). */
/* OBJECT_SUCKER: pig vehicle sucker (EggDefenderGadget_Sucker.lua). OBJECT_GUN_BUBBLE: hidden bubble of the bubble gun it spawns from (spawn_of), shot in turn. */
enum {OBJECT_NO_COLLISION=1,OBJECT_MIRROR_X=2,OBJECT_MIRROR_Y=4,OBJECT_GUN_BUBBLE=8,OBJECT_SUCKER=16,OBJECT_CANNON_BALL=32};
/* OBJECT_CANNON_BALL: hidden ball of the vehicle cannon it spawns from (EggDefenderGadget_Cannon.lua). */
/* Bits 8-15 of flags: 1 + bird type released when this cage is destroyed (0: none). */
#define OBJECT_RELEASED_BIRD(flags) ((((flags)>>8)&255)-1)
/* Bits 16-31: 1 + index of the pig a blue vial clone copies (0: not a clone). */
#define OBJECT_CLONE_OF(flags) (((((unsigned)(flags))>>16)&1023)-1)
/* Bits 26-29: Egg Defender wave of the piece (0 or 1: present from the start; 2+: arrives later). */
#define OBJECT_WAVE(flags) ((((unsigned)(flags))>>26)&15)
typedef struct {float x,y,angle,width,height,density,strength,defence;int kind,shape;const char *definition;float friction,restitution;int spawn_of,flags;float score;} CampaignObject;   /* score: destroyedScoreInc, -1 when the piece has none */
/* friction/restitution come from the materials table of blocks.lua (wood 4.0/0.2,
   light 0.7/0.2, rock 4.0/0.1, immovable 0.8/0, piglette 0.7/0.05, extras 0.7/0.4). */
/* CampaignObject.shape: box, circle, right triangle (vertices 0,0 1,1 0,1) and
   isosceles triangle (0.5,0 1,1 0,1), normalized over the sprite extents. */
enum {SHAPE_BOX=0,SHAPE_CIRCLE=1,SHAPE_TRIANGLE_L=2,SHAPE_TRIANGLE_M=3,SHAPE_POLYGON=4};
/* Convex polygon from blocks.lua: vertices as fractions (x,y) of the sprite box. */
typedef struct {const char *definition;int count;float v[16];} CampaignPolygon;
/* Material forceX/forceY (e.g. balloon): constant push applied every step, scaled by the body mass. */
/* target: when non-zero (blocks.lua targetVelocity), the push stops while the piece already
   moves at that speed along the force (port reading of the native force adder). */
/* Per-definition values from blocks.lua: constant force (balloons) and explosion (specialty BOMB). */
enum {POWERUP_NONE=0,POWERUP_KING_SLING=1,POWERUP_SLING_SCOPE=2,POWERUP_BIRD_QUAKE=3,POWERUP_POWER_POTION=4,POWERUP_SHOCKWAVE=5};
enum {VIAL_ORANGE=1,VIAL_PINK=2,VIAL_BLUE=3};
typedef struct {const char *definition;float fx,fy,target,blast_radius,blast_force,damage_radius,damage;int vial;float vial_radius;float thrust,thrust_target,thrust_interval;int thrust_flags;} CampaignForce;
/* Joints from the stage's joints table (types as in the level editor: 1 distance,
   2 weld, 3 revolute). Anchors are world coordinates; a/b index the objects array,
   -1 meaning the ground. frequency/damping 0 makes the joint rigid. breakForce 0 means
   unbreakable. */
enum {JOINT_DISTANCE=1,JOINT_WELD=2,JOINT_REVOLUTE=3,JOINT_PRISMATIC=4,JOINT_DESTROY_ATTACHED=5};
enum {JOINT_LIMIT=1,JOINT_MOTOR=2,JOINT_BACK_AND_FORTH=4};
/* destroy_timer: type 5 (editor "Destroy-attached"), seconds after which end2 is removed. */
/* Revolute/prismatic: limits (radians or units), motor speed and maximum torque/force, axis
   of a prismatic joint; JOINT_BACK_AND_FORTH reverses the motor at each limit. */
typedef struct {int type,a,b;float ax,ay,bx,by,frequency,damping,break_force;int collide;float destroy_timer;float axis_x,axis_y,lower,upper,motor_speed,max_force;int flags;} CampaignJoint;
typedef struct {const CampaignObject *objects;int count,birds;int bird_types[16];float gravity,sling_x,sling_y,ground_y,pixels,view_x,view_y,view_scale;const char *title,*script;int bank;const CampaignJoint *joints;int joint_count;
    /* Star thresholds from starLimits.lua: a win is 1 star, silverScore 2, goldScore 3. */
    int star_silver,star_gold;
    /* Power-up of the stage (shop tutorials, episodes.lua shopTutorialPowerup): POWERUP_*. */
    int powerup;
    /* MUSIC.BIN track of the theme's ambience (themes.lua music), -1 when none. */
    int music;
    /* THEMES.BIN background (themes.lua theme of the stage), -1 when none. */
    int theme;} CampaignLevel;
#endif
