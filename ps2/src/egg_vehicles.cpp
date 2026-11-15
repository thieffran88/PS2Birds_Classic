/* Egg Defender vehicle controllers, translated by hand from the APK's
   assets/data/vehicles/EggDefender_N/EggDefender_N_Wk_V1.scripts.lua (one per wave).
   Each block names its source script; script globals live in ev_vars(v). A script that only
   sets fields or calls the vehicle API translates one to one; nothing of the Lua is executed. */
#include "egg_vehicles.h"
#include <string.h>

/* Common pieces of many scripts. */
static void abandon(EggVehicle *v){ev_abandon(v);}                     /* abandon = function(v) v:abandonVehicle() end */
static void flip_thrust(EggVehicle *v,const char *fan){ev_set_thrust(v,fan,-ev_thrust(v,fan));}
/* forceRelative = -forceRelative + 400; targetVelocity = targetVelocity + 100 (flee with the egg). */
static void flee_thrust(EggVehicle *v,const char *fan){ev_set_thrust(v,fan,-ev_thrust(v,fan)+400);ev_set_target(v,fan,ev_target(v,fan)+100);}
static int named(const char *name,const char *const *list){for(;*list;list++)if(!strcmp(name,*list))return 1;return 0;}
static void abandon_on(EggVehicle *v,const char *name,const char *const *list){if(named(name,list))ev_timer(v,abandon,1);}

/* ---- EggDefender_1 (tutorial) ---- */
/* W1: Fan1.forceRelative = -4; the pig never abandons (customAbandoningFunction returns false). */
static void ed1_w1_init(EggVehicle *v){ev_set_thrust(v,"BP_FAN_1",-4);}
/* W2: engine = BP_FAN_1 (forceRelative -700); it breaks down before reaching the nest. */
static void ed1_w2_init(EggVehicle *v){ev_set_thrust(v,"BP_FAN_1",-700);}
static void ed1_w2_update(EggVehicle *v,float){
    if(ev_x(v)<50){ev_remove_joint(v,"BP_WOODEN_WHEEL_2BP_WOODEN_TRIANGLE_2");ev_set_thrust(v,"BP_FAN_1",0);}
    if(ev_x(v)<48)ev_remove_block(v,"BP_WOODEN_FRAME_2");
    if(ev_x(v)<45){ev_motor_speed(v,"BP_WOODEN_WHEEL_1BP_WOODEN_TRIANGLE_1",0);ev_remove_block(v,"BP_WOODEN_TRIANGLE_3");}
}
/* W3: a balloon cart (spawnAtGroundLevel = false) pushed by forceX -100; the balloon eases at
   x < 50 and 45 and the cart falls apart at x < 15. */
static void ed1_w3_init(EggVehicle *v){ev_set_force(v,"BP_SINGLE_BALLOON_1",0,-90);ev_set_thrust(v,"BP_FAN_1",0);ev_set_force(v,"BP_FAN_1",-100,ev_force_y(v,"BP_FAN_1"));}
static void ed1_w3_update(EggVehicle *v,float){
    if(ev_x(v)<50)ev_set_force(v,"BP_SINGLE_BALLOON_1",ev_force_x(v,"BP_SINGLE_BALLOON_1"),-180);
    if(ev_x(v)<45)ev_set_force(v,"BP_SINGLE_BALLOON_1",ev_force_x(v,"BP_SINGLE_BALLOON_1"),-50);
    if(ev_x(v)<15){
        ev_remove_joint(v,"BP_ROPE_THICK_2BP_WOODEN_FRAME_1");ev_motor_speed(v,"BP_WOODEN_WHEEL_1BP_WOODEN_TRIANGLE_1",0);
        ev_power(v,"BP_FAN_1",0);ev_remove_joint(v,"BP_FAN_1BP_WOODEN_FRAME_1");
    }
}

/* ---- EggDefender_2 ---- */
/* W1: BP_FAN_1 pushes with forceX -500 and turns around with the egg. */
static void ed2_w1_init(EggVehicle *v){ev_set_thrust(v,"BP_FAN_1",0);ev_set_force(v,"BP_FAN_1",-500,ev_force_y(v,"BP_FAN_1"));}
static void ed2_w1_egg(EggVehicle *v){ev_set_force(v,"BP_FAN_1",-ev_force_x(v,"BP_FAN_1"),ev_force_y(v,"BP_FAN_1"));}
static void ed2_w1_removed(EggVehicle *v,const char *name){if(strcmp(name,"BP_FAN_MESH_1"))ev_timer(v,abandon,1);}
/* W2: with the egg the fan reverses; while the balloon holds (vars[0]) the cart drops its frames
   and floats away on it. */
static void ed2_w2_init(EggVehicle *v){ev_vars(v)[0]=1;}
static void ed2_w2_egg(EggVehicle *v){
    flee_thrust(v,"BP_FAN_1");
    if(ev_vars(v)[0]!=0){ev_remove_joint(v,"BP_WOODEN_FRAME_4BP_WOODEN_FRAME_1");ev_remove_joint(v,"BP_WOODEN_FRAME_2BP_WOODEN_FRAME_1");
        ev_set_force(v,"BP_SINGLE_BALLOON_1",ev_force_x(v,"BP_SINGLE_BALLOON_1"),-100);}
}
static void ed2_w2_removed(EggVehicle *v,const char *name){
    static const char *const stop[]={"BP_FAN_1","BP_WOODEN_FRAME_6",0},*const pop[]={"BP_SINGLE_BALLOON_1","BP_ROPE_THICK_1",0};
    abandon_on(v,name,stop);if(named(name,pop))ev_vars(v)[0]=0;
}
/* W3: balloon forceY -30; BP_FAN_5 reverses with the egg. */
static void ed2_w3_init(EggVehicle *v){ev_set_force(v,"BP_SINGLE_BALLOON_3",ev_force_x(v,"BP_SINGLE_BALLOON_3"),-30);}
static void ed2_w3_egg(EggVehicle *v){flee_thrust(v,"BP_FAN_5");}
static void ed2_w3_removed(EggVehicle *v,const char *name){static const char *const stop[]={"BP_FAN_5","BP_WOODEN_FRAME_6",0};abandon_on(v,name,stop);}
/* W4: balloon forceY -115, two pigs that never abandon; BP_FAN_5 reverses and the cart lets go. */
static void ed2_w4_init(EggVehicle *v){ev_set_force(v,"BP_SINGLE_BALLOON_4",ev_force_x(v,"BP_SINGLE_BALLOON_4"),-115);}
static void ed2_w4_egg(EggVehicle *v){flip_thrust(v,"BP_FAN_5");ev_remove_joint(v,"BP_WOODEN_TRIANGLE_3BP_WOODEN_FRAME_6");}
static void ed2_w4_removed(EggVehicle *v,const char *name){static const char *const stop[]={"BP_SINGLE_BALLOON_4","BP_ROPE_THICK_1","BP_FAN_5","ExtraRopeThick01_8",0};abandon_on(v,name,stop);}
/* W5: balloon forceY -30; with the egg the fan reverses, the TNT is dropped and the balloon pulls
   up with -200. (pigTargeted, the Red Bird's aiming, is not in the port.) */
static void ed2_w5_init(EggVehicle *v){ev_set_force(v,"BP_SINGLE_BALLOON_3",ev_force_x(v,"BP_SINGLE_BALLOON_3"),-30);}
static void ed2_w5_egg(EggVehicle *v){
    flee_thrust(v,"BP_FAN_5");ev_remove_joint(v,"BP_WOODEN_FRAME_3BP_WOODEN_FRAME_1");ev_remove_joint(v,"BP_TNT_SMALL_1BP_WOODEN_TRIANGLE_3");
    ev_set_force(v,"BP_SINGLE_BALLOON_3",ev_force_x(v,"BP_SINGLE_BALLOON_3"),-200);
}
static void ed2_w5_removed(EggVehicle *v,const char *name){static const char *const stop[]={"BP_FAN_5","BP_WOODEN_FRAME_6",0};abandon_on(v,name,stop);}

/* ---- EggDefender_3 ---- */
/* W1: BP_FAN_3/BP_FAN_4 auto-balanced (1); BP_FAN_1 pushes with forceX -200, doubled back with the egg. */
static void ed3_w1_init(EggVehicle *v){ev_auto_balance(v,"BP_FAN_3","BP_FAN_4",1);ev_set_thrust(v,"BP_FAN_1",0);ev_set_force(v,"BP_FAN_1",-200,ev_force_y(v,"BP_FAN_1"));}
static void ed3_w1_egg(EggVehicle *v){ev_set_force(v,"BP_FAN_1",-ev_force_x(v,"BP_FAN_1")*2,ev_force_y(v,"BP_FAN_1"));}
static void ed3_w1_removed(EggVehicle *v,const char *name){static const char *const stop[]={"BP_FAN_1","BP_FAN_4","BP_FAN_3","BP_WOODEN_WHEEL_2","BP_WOODEN_TRIANGLE_2",0};abandon_on(v,name,stop);}
/* W2 and W3: a single fan (BP_FAN_7 / BP_FAN_6) reversed with the egg. */
static void ed3_w2_egg(EggVehicle *v){flip_thrust(v,"BP_FAN_7");}
static void ed3_w2_removed(EggVehicle *v,const char *name){if(!strcmp(name,"BP_FAN_7"))ev_timer(v,abandon,1);}
static void ed3_w3_egg(EggVehicle *v){flip_thrust(v,"BP_FAN_6");}
static void ed3_w3_removed(EggVehicle *v,const char *name){if(!strcmp(name,"BP_FAN_6"))ev_timer(v,abandon,1);}
/* W4: two fans and a propeller, all reversed with the egg. */
static void ed3_w4_egg(EggVehicle *v){flip_thrust(v,"BP_FAN_5");flip_thrust(v,"BP_FAN_6");flip_thrust(v,"BP_PROPELLER_1");}
static void ed3_w4_removed(EggVehicle *v,const char *name){static const char *const stop[]={"BP_FAN_5","BP_FAN_6","BP_BIG_LANDING_WHEEL_1","BP_WOODEN_WHEEL_2","BP_WOODEN_TRIANGLE_2",0};abandon_on(v,name,stop);}
/* W5: a transforming flyer. BP_FAN_11/12 push with forceX -500; with the egg it drops its third
   fan, folds its arms (motors 4/-4, then transform/transformtwo after 2 and 3 s) and flies back. */
static void ed3_w5_transform(EggVehicle *v){if(ev_vars(v)[0]==0)ev_motor_speed(v,"BP_LIGHT_METAL_FRAME_6BP_LIGHT_METAL_FRAME_4",4);}
static void ed3_w5_transformtwo(EggVehicle *v){if(ev_vars(v)[1]==0){ev_motor_speed(v,"BP_LIGHT_METAL_FRAME_6BP_LIGHT_METAL_FRAME_5",-4);ev_vars(v)[1]=1;}}
static void ed3_w5_init(EggVehicle *v){
    ev_set_thrust(v,"BP_FAN_11",0);ev_set_force(v,"BP_FAN_11",-500,ev_force_y(v,"BP_FAN_11"));
    ev_set_thrust(v,"BP_FAN_12",0);ev_set_force(v,"BP_FAN_12",-500,ev_force_y(v,"BP_FAN_12"));
    ev_vars(v)[0]=0;ev_vars(v)[1]=0;  /* transformed, transformtwod */
}
static void ed3_w5_egg(EggVehicle *v){
    ev_auto_balance(v,"BP_FAN_12","BP_FAN_11",5);
    ev_remove_joint(v,"BP_LIGHT_METAL_FRAME_5BP_LIGHT_METAL_FRAME_7");ev_remove_joint(v,"BP_LIGHT_METAL_FRAME_7BP_LIGHT_METAL_FRAME_4");
    ev_remove_block(v,"BP_FAN_10");ev_remove_block(v,"BP_FAN_MESH_10");
    ev_motor_speed(v,"BP_LIGHT_METAL_FRAME_6BP_LIGHT_METAL_FRAME_5",4);ev_motor_speed(v,"BP_LIGHT_METAL_FRAME_6BP_LIGHT_METAL_FRAME_4",-4);
    ev_timer(v,ed3_w5_transform,2);ev_timer(v,ed3_w5_transformtwo,3);
    ev_set_thrust(v,"BP_FAN_11",0);ev_set_force(v,"BP_FAN_11",500,ev_force_y(v,"BP_FAN_11"));ev_set_target(v,"BP_FAN_11",ev_target(v,"BP_FAN_11")*2);
    ev_set_thrust(v,"BP_FAN_12",0);ev_set_force(v,"BP_FAN_12",100,ev_force_y(v,"BP_FAN_12"));ev_set_target(v,"BP_FAN_12",ev_target(v,"BP_FAN_12")*2);
}

static const EggController controllers[]={
    {"EggDefender_1_W1_V1",ed1_w1_init,0,0,0,1},
    {"EggDefender_1_W2_V1",ed1_w2_init,ed1_w2_update,0,0,1},
    {"EggDefender_1_W3_V1",ed1_w3_init,ed1_w3_update,0,0,1},
    {"EggDefender_2_W1_V1",ed2_w1_init,0,ed2_w1_egg,ed2_w1_removed,0},
    {"EggDefender_2_W2_V1",ed2_w2_init,0,ed2_w2_egg,ed2_w2_removed,0},
    {"EggDefender_2_W3_V1",ed2_w3_init,0,ed2_w3_egg,ed2_w3_removed,0},
    {"EggDefender_2_W4_V1",ed2_w4_init,0,ed2_w4_egg,ed2_w4_removed,1},
    {"EggDefender_2_W5_V1",ed2_w5_init,0,ed2_w5_egg,ed2_w5_removed,0},
    {"EggDefender_3_W1_V1",ed3_w1_init,0,ed3_w1_egg,ed3_w1_removed,0},
    {"EggDefender_3_W2_V1",0,0,ed3_w2_egg,ed3_w2_removed,0},
    {"EggDefender_3_W3_V1",0,0,ed3_w3_egg,ed3_w3_removed,0},
    {"EggDefender_3_W4_V1",0,0,ed3_w4_egg,ed3_w4_removed,0},
    {"EggDefender_3_W5_V1",ed3_w5_init,0,ed3_w5_egg,0,0},
};
const EggController *egg_controller(const char *name){
    for(unsigned i=0;i<sizeof(controllers)/sizeof(controllers[0]);i++)if(!strcmp(controllers[i].name,name))return &controllers[i];
    return 0;
}
