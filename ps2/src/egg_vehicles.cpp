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

/* ---- EggDefender_4 ---- */
/* W1: a fan plane (no ground spawn). Until it has the egg, BP_FAN_2 keeps it at about y -20
   (50 above, -800 below) and BP_FAN_1 steers over the egg (-600 when the egg is ahead, 300 and lift
   off when it is under). With the egg it flies back on BP_FAN_1 (1500) and BP_FAN_2's forceX 200. */
static void ed4_w1_update(EggVehicle *v,float){
    if(ev_vars(v)[0]!=0)return;  /* eggcollected */
    if(ev_y(v)<-20)ev_set_thrust(v,"BP_FAN_2",50);
    if(-20<ev_y(v))ev_set_thrust(v,"BP_FAN_2",-800);
    if(1<ev_closest_egg_x(v,0))ev_set_thrust(v,"BP_FAN_1",-600);
    if(ev_closest_egg_x(v,0)<3){ev_set_thrust(v,"BP_FAN_1",300);ev_power(v,"BP_FAN_2",0);}
}
static void ed4_w1_egg(EggVehicle *v){
    ev_vars(v)[0]=1;ev_set_thrust(v,"BP_FAN_1",1500);ev_power(v,"BP_FAN_2",1);ev_set_force(v,"BP_FAN_2",200,ev_force_y(v,"BP_FAN_2"));
}
static void ed4_w1_removed(EggVehicle *v,const char *name){if(strcmp(name,"BP_FAN_MESH_1") && strcmp(name,"BP_FAN_MESH_2"))ev_timer(v,abandon,1);}
/* W2: a balloon (forceY -100) with a fan (-500): it hovers at about y -25 (-147/-142), chases the
   egg with the fan (±1000 beyond 5 m) and sinks over it (-130); with the egg it rises (-165) and
   flies back (1000). vars: 0 rotorDescend, 1 eggIsCollected. */
static void ed4_w2_init(EggVehicle *v){ev_set_thrust(v,"BP_FAN_2",-500);ev_set_force(v,"BP_SINGLE_BALLOON_1",0,-100);ev_vars(v)[0]=0;ev_vars(v)[1]=0;}
static void ed4_w2_update(EggVehicle *v,float){
    float *s=ev_vars(v);const char *b="BP_SINGLE_BALLOON_1";
    if(s[0]==0 && s[1]==0){if(-25<ev_y(v))ev_set_force(v,b,ev_force_x(v,b),-147);if(ev_y(v)<-25)ev_set_force(v,b,ev_force_x(v,b),-142);}
    float ex=ev_closest_egg_x(v,0);
    if(5<ex){ev_set_thrust(v,"BP_FAN_2",-1000);s[0]=0;}
    if(ex<-5){ev_set_thrust(v,"BP_FAN_2",1000);s[0]=0;}
    if(-5<ex && ex<5 && s[1]==0){ev_set_force(v,b,ev_force_x(v,b),-130);s[0]=1;}
}
static void ed4_w2_egg(EggVehicle *v){ev_set_force(v,"BP_SINGLE_BALLOON_1",ev_force_x(v,"BP_SINGLE_BALLOON_1"),-165);ev_set_thrust(v,"BP_FAN_2",1000);ev_vars(v)[1]=1;}
static void ed4_w2_removed(EggVehicle *v,const char *name){static const char *const stop[]={"BP_SINGLE_BALLOON_1","BP_FAN_2",0};if(named(name,stop))ev_timer(v,abandon,2);}
/* W3: a glider: wing lift (forceY -700, then -650/-550 around y -35) and propeller forceX -1000
   (target 15); it drops its tail at x < 55 and its TNT at x < 40 (wing -300, propeller -2000).
   vars: 0 dropone, 1 droptail. */
static void ed4_w3_init(EggVehicle *v){
    ev_set_force(v,"BP_WOODEN_WING_1",ev_force_x(v,"BP_WOODEN_WING_1"),-700);
    ev_set_thrust(v,"BP_PROPELLER_2",0);ev_set_force(v,"BP_PROPELLER_2",-1000,ev_force_y(v,"BP_PROPELLER_2"));ev_set_target(v,"BP_PROPELLER_2",15);
    ev_vars(v)[0]=0;ev_vars(v)[1]=0;
}
static void ed4_w3_update(EggVehicle *v,float){
    float *s=ev_vars(v);const char *w="BP_WOODEN_WING_1";
    if(-35<ev_y(v) && s[0]==0)ev_set_force(v,w,ev_force_x(v,w),-650);
    if(ev_y(v)<-35 && s[0]==0)ev_set_force(v,w,ev_force_x(v,w),-550);
    if(ev_x(v)<55 && s[1]==0){ev_remove_joint(v,"BP_WOODEN_FRAME_6BP_WOODEN_TAIL_1");s[1]=1;}
    if(ev_x(v)<40 && s[0]==0){ev_remove_joint(v,"BP_TNT_SMALL_5BP_WOODEN_FRAME_6");ev_set_force(v,w,ev_force_x(v,w),-300);
        ev_set_force(v,"BP_PROPELLER_2",-2000,ev_force_y(v,"BP_PROPELLER_2"));s[0]=1;}
}
static void ed4_w3_egg(EggVehicle *v){ev_timer(v,abandon,1);}
static void ed4_w3_removed(EggVehicle *v,const char *name){if(!strcmp(name,"BP_PROPELLER_1"))ev_timer(v,abandon,1);}
/* W4: a rubber balloon (forceY -150) with a front and a back propeller (-500, -1000): it hovers at
   about y -35 (-196/-191), switches propellers to steer over the egg and sinks (-180); with the egg
   it rises (-210) on the back propeller. vars: 0 rotorDescend, 1 eggIsCollected. */
static void ed4_w4_init(EggVehicle *v){
    ev_set_thrust(v,"BP_PROPELLER_3",-500);ev_set_thrust(v,"BP_PROPELLER_2",-1000);ev_set_force(v,"BP_RUBBER_BALLOON_4",0,-150);ev_vars(v)[0]=0;ev_vars(v)[1]=0;
}
static void ed4_w4_update(EggVehicle *v,float){
    float *s=ev_vars(v);const char *b="BP_RUBBER_BALLOON_4";
    if(s[0]==0 && s[1]==0){if(-35<ev_y(v))ev_set_force(v,b,ev_force_x(v,b),-196);if(ev_y(v)<-35)ev_set_force(v,b,ev_force_x(v,b),-191);}
    float ex=ev_closest_egg_x(v,0);
    if(5<ex){ev_power(v,"BP_PROPELLER_3",1);ev_power(v,"BP_PROPELLER_2",0);s[0]=0;}
    if(ex<-1){ev_power(v,"BP_PROPELLER_3",0);ev_power(v,"BP_PROPELLER_2",1);s[0]=0;}
    if(-5<ex && ex<5 && s[1]==0){ev_set_force(v,b,ev_force_x(v,b),-180);s[0]=1;}
}
static void ed4_w4_egg(EggVehicle *v){ev_set_force(v,"BP_RUBBER_BALLOON_4",ev_force_x(v,"BP_RUBBER_BALLOON_4"),-210);ev_power(v,"BP_PROPELLER_3",0);ev_power(v,"BP_PROPELLER_2",1);ev_vars(v)[1]=1;}
/* W5: a twin-rotor flyer: rotors auto-balanced (1) with forceY lift (-1000 below y -35, -500 above),
   two fans steer with forceX ±200 (target 10) and it sinks (-450) within 15 m of the egg. With the egg
   it folds its frames (motors -4/4/4), the left fan pushes 500 and both rotors switch off.
   vars: 0 eggIsCollected, 1 rotorDescend, 2 egged. */
static void ed4_w5_init(EggVehicle *v){
    ev_set_thrust(v,"BP_PROPELLER_3",0);ev_set_thrust(v,"BP_PROPELLER_2",0);ev_auto_balance(v,"BP_PROPELLER_2","BP_PROPELLER_3",1);
    ev_set_thrust(v,"BP_FAN_6",0);ev_set_thrust(v,"BP_FAN_5",0);ev_vars(v)[0]=0;ev_vars(v)[1]=0;ev_vars(v)[2]=1;
}
static void ed4_w5_lift(EggVehicle *v,float fy){ev_set_force(v,"BP_PROPELLER_3",ev_force_x(v,"BP_PROPELLER_3"),fy);ev_set_force(v,"BP_PROPELLER_2",ev_force_x(v,"BP_PROPELLER_2"),fy);}
static void ed4_w5_update(EggVehicle *v,float){
    float *s=ev_vars(v);
    if(s[1]==0 && s[0]==0){if(-35<ev_y(v))ed4_w5_lift(v,-1000);if(ev_y(v)<-35)ed4_w5_lift(v,-500);}
    float ex=ev_closest_egg_x(v,0);
    if(5<ex){ev_set_force(v,"BP_FAN_6",-200,ev_force_y(v,"BP_FAN_6"));ev_set_target(v,"BP_FAN_6",10);ev_set_force(v,"BP_FAN_5",-200,ev_force_y(v,"BP_FAN_5"));ev_set_target(v,"BP_FAN_5",10);s[1]=0;}
    if(ex<-5){ev_set_force(v,"BP_FAN_6",200,ev_force_y(v,"BP_FAN_6"));ev_set_force(v,"BP_FAN_5",200,ev_force_y(v,"BP_FAN_5"));s[1]=0;}
    if(-15<ex && ex<15 && s[0]==0){ed4_w5_lift(v,-450);s[1]=1;}
}
static void ed4_w5_egg(EggVehicle *v){
    float *s=ev_vars(v);s[0]=1;
    ev_remove_joint(v,"BP_LIGHT_METAL_FRAME_4BP_LIGHT_METAL_FRAME_3");ev_remove_joint(v,"BP_LIGHT_METAL_FRAME_4BP_LIGHT_METAL_FRAME_2");
    ev_motor_speed(v,"BP_LIGHT_METAL_FRAME_3BP_WOODEN_FRAME_3",-4);ev_motor_speed(v,"BP_WOODEN_FRAME_3BP_LIGHT_METAL_FRAME_2",4);ev_motor_speed(v,"BP_COG_OFF_1BP_LIGHT_METAL_FRAME_4",4);
    ev_set_force(v,"BP_FAN_6",500,ev_force_y(v,"BP_FAN_6"));  /* the script sets fanl twice (fanr is left as it was) */
    if(s[2]!=0){ev_power(v,"BP_PROPELLER_2",0);ev_power(v,"BP_PROPELLER_3",0);s[2]=0;}  /* togglePower(name) with no flag: off */
}
/* blockRemoved asks for a name equal to both BP_FAN_6 and BP_FAN_5: it never fires. */

/* ---- EggDefender_5 ---- */
/* W1: a sucker cart: the sucker starts at x < 90; at x < 45, still without the egg, BP_FAN_8 pushes
   back with forceX 200 (300 with the egg). vars: 0 suckerOn, 1 eggCollected. */
static void ed5_w1_init(EggVehicle *v){ev_vars(v)[0]=0;ev_vars(v)[1]=0;}
static void ed5_w1_update(EggVehicle *v,float){
    float *s=ev_vars(v);
    if(ev_x(v)<90 && s[0]==0){ev_gadget(v,"BP_SUCKER_1","startSuction");s[0]=1;}
    if(ev_x(v)<45 && s[1]==0){ev_set_thrust(v,"BP_FAN_8",0);ev_set_force(v,"BP_FAN_8",200,ev_force_y(v,"BP_FAN_8"));}
}
static void ed5_w1_egg(EggVehicle *v){ev_set_force(v,"BP_FAN_8",300,ev_force_y(v,"BP_FAN_8"));ev_vars(v)[1]=1;}
static void ed5_w1_removed(EggVehicle *v,const char *name){static const char *const stop[]={"BP_FAN_8","BP_WOODEN_WHEEL_16","BP_WOODEN_WHEEL_1","BP_WOODEN_TRIANGLE_2","BP_WOODEN_TRIANGLE_17","BP_WOODEN_FRAME_22",0};abandon_on(v,name,stop);}
/* W2: sucker on from the start; BP_FAN_2 reverses with the egg. */
static void ed5_w2_init(EggVehicle *v){ev_gadget(v,"BP_SUCKER_2","startSuction");}
static void ed5_w2_egg(EggVehicle *v){flip_thrust(v,"BP_FAN_2");}
static void ed5_w2_removed(EggVehicle *v,const char *name){static const char *const stop[]={"BP_FAN_2","BP_WOODEN_WHEEL_4","BP_WOODEN_TRIANGLE_5","BP_WOODEN_WHEEL_5","BP_WOODEN_TRIANGLE_4",0};abandon_on(v,name,stop);}
/* W3: sucker on, rear bellows off; with the egg the front bellows stop and the rear ones blow. */
static void ed5_w3_init(EggVehicle *v){ev_gadget(v,"BP_SUCKER_1","startSuction");ev_power(v,"BP_BELLOWS_2",0);}
static void ed5_w3_egg(EggVehicle *v){ev_power(v,"BP_BELLOWS_1",0);ev_power(v,"BP_BELLOWS_2",1);}
static void ed5_w3_removed(EggVehicle *v,const char *name){static const char *const stop[]={"BP_BELLOWS_2","BP_WOODEN_TRIANGLE_1","BP_WOODEN_WHEEL_1","BP_WOODEN_TRIANGLE_2","BP_WOODEN_WHEEL_2",0};abandon_on(v,name,stop);}
/* W4: sucker on; both fans reverse with the egg; losing the sucker makes the pig give up. */
static void ed5_w4_init(EggVehicle *v){ev_gadget(v,"BP_SUCKER_1","startSuction");}
static void ed5_w4_egg(EggVehicle *v){flip_thrust(v,"BP_FAN_4");flip_thrust(v,"BP_FAN_5");}
static void ed5_w4_run(EggVehicle *v){ev_surrender(v);}
static void ed5_w4_removed(EggVehicle *v,const char *name){
    static const char *const stop[]={"BP_WOODEN_FRAME_15","BP_WOODEN_TRIANGLE_8","BP_WOODEN_WHEEL_8",0};abandon_on(v,name,stop);
    if(!strcmp(name,"BP_SUCKER_1"))ev_timer(v,ed5_w4_run,1);
}
/* W5: two suckers switched on at x < 75; BP_FAN_8 reverses at double force with the egg. */
static void ed5_w5_update(EggVehicle *v,float){
    if(ev_x(v)<75 && ev_vars(v)[0]==0){ev_gadget(v,"BP_SUCKER_2","startSuction");ev_gadget(v,"BP_SUCKER_3","startSuction");ev_vars(v)[0]=1;}
}
static void ed5_w5_egg(EggVehicle *v){ev_set_thrust(v,"BP_FAN_8",-ev_thrust(v,"BP_FAN_8")*2);}
static void ed5_w5_removed(EggVehicle *v,const char *name){static const char *const stop[]={"BP_BIG_LANDING_WHEEL_3","BP_BIG_LANDING_WHEEL_2","BP_FAN_8",0};abandon_on(v,name,stop);}

/* ---- EggDefender_6 ---- */
static void set_fy(EggVehicle *v,const char *n,float fy){ev_set_force(v,n,ev_force_x(v,n),fy);}
static void set_fx(EggVehicle *v,const char *n,float fx){ev_set_force(v,n,fx,ev_force_y(v,n));}
/* W1: a sucker cart with a folded rotor (off), fan -500, arm motor -0.1. With the egg the arms fold
   (motors 3), propeller and fan push back (forceX 300 and 400) and after 2 s the rotor lifts it
   (forceY -150) as the frame lets go. Its update tests eggIsCollected == false, a field the script
   never sets (nil), so it never acts. */
static void ed6_w1_release(EggVehicle *v){
    set_fy(v,"BP_ROTOR_1",-150);ev_set_thrust(v,"BP_ROTOR_1",0);ev_power(v,"BP_ROTOR_1",1);ev_set_thrust(v,"BP_PROPELLER_1",0);
    ev_remove_joint(v,"BP_LIGHT_METAL_FRAME_3BP_WOODEN_TRIANGLE_3");ev_remove_joint(v,"BP_LIGHT_METAL_FRAME_3BP_WOODEN_FRAME_6");
}
static void ed6_w1_init(EggVehicle *v){
    ev_power(v,"BP_ROTOR_1",0);ev_set_thrust(v,"BP_FAN_1",-500);ev_motor_speed(v,"BP_LIGHT_METAL_FRAME_2BP_LIGHT_METAL_FRAME_3",-0.1f);ev_gadget(v,"BP_SUCKER_1","startSuction");
}
static void ed6_w1_egg(EggVehicle *v){
    ev_motor_speed(v,"BP_LIGHT_METAL_FRAME_2BP_LIGHT_METAL_FRAME_3",3);ev_motor_speed(v,"BP_COG_OFF_1BP_LIGHT_METAL_FRAME_2",3);ev_timer(v,ed6_w1_release,2);
    ev_set_thrust(v,"BP_PROPELLER_1",0);set_fx(v,"BP_PROPELLER_1",300);ev_set_thrust(v,"BP_FAN_1",0);set_fx(v,"BP_FAN_1",400);
}
static void ed6_w1_removed(EggVehicle *v,const char *name){if(!strcmp(name,"BP_ROTOR_1"))ev_timer(v,abandon,1);}
/* W2: two suckers; both fans reverse with the egg; the pig never abandons. */
static void ed6_w2_init(EggVehicle *v){ev_gadget(v,"BP_SUCKER_1","startSuction");ev_gadget(v,"BP_SUCKER_2","startSuction");}
static void ed6_w2_egg(EggVehicle *v){flip_thrust(v,"BP_FAN_1");flip_thrust(v,"BP_FAN_2");}
static void ed6_w2_removed(EggVehicle *v,const char *name){if(!strcmp(name,"BP_FAN_1"))ev_timer(v,abandon,1);}
/* W3: a rotor flyer with a sucker (scriptTargetY -45 is not used by the port): fan forceX ±500
   (target doubled from 2) steers over the egg, rotor forceY -100 above y -35 and -250 below,
   propellers balanced (10). With the egg it escapes on the rotor (forceRelative -7000).
   vars: 0 escaping. */
static void ed6_w3_init(EggVehicle *v){
    ev_set_target(v,"BP_FAN_5",2);ev_gadget(v,"BP_SUCKER_1","startSuction");ev_set_thrust(v,"BP_ROTOR_7",0);ev_set_thrust(v,"BP_FAN_5",0);
    ev_auto_balance(v,"BP_PROPELLER_6","BP_PROPELLER_8",10);ev_set_target(v,"BP_FAN_5",ev_target(v,"BP_FAN_5")*2);ev_vars(v)[0]=0;
}
static void ed6_w3_update(EggVehicle *v,float){
    if(ev_vars(v)[0]!=0){ev_set_thrust(v,"BP_ROTOR_7",-7000);return;}
    float ex=ev_closest_egg_x(v,0);
    if(5<ex)set_fx(v,"BP_FAN_5",-500);
    if(ex<-5)set_fx(v,"BP_FAN_5",500);
    if(ev_y(v)<-35)set_fy(v,"BP_ROTOR_7",-100);
    if(-35<ev_y(v))set_fy(v,"BP_ROTOR_7",-250);
}
static void ed6_w3_egg(EggVehicle *v){ev_vars(v)[0]=1;}
static void ed6_w3_flee(EggVehicle *v){ev_surrender(v);ev_vars(v)[0]=1;}
static void ed6_w3_removed(EggVehicle *v,const char *name){
    static const char *const stop[]={"BP_ROTOR_7","BP_PROPELLER_6","BP_PROPELLER_8",0};abandon_on(v,name,stop);
    if(!strcmp(name,"BP_SUCKER_1"))ev_timer(v,ed6_w3_flee,1);
}
/* W4: a rotor flyer with two suckers, the first aiming at the egg: rotor forceY -200 below y -35 and
   -100 above, fan forceX ∓300 over the egg; with the egg (or escaping) fan 200/300 and rotor -200.
   vars: 0 eggIsCollected, 1 escaping. */
static void ed6_w4_init(EggVehicle *v){
    ev_set_thrust(v,"BP_ROTOR_7",0);set_fy(v,"BP_ROTOR_7",-100);ev_set_thrust(v,"BP_PROPELLER_8",10);ev_set_thrust(v,"BP_PROPELLER_6",10);
    ev_auto_balance(v,"BP_PROPELLER_8","BP_PROPELLER_6",50);ev_gadget(v,"BP_SUCKER_1","startSuction");ev_gadget(v,"BP_SUCKER_3","startSuction");
    ev_start_aim(v,"BP_SUCKER_1","egg",0,0);ev_set_thrust(v,"BP_FAN_5",0);set_fx(v,"BP_FAN_5",-100);ev_vars(v)[0]=0;ev_vars(v)[1]=0;
}
static void ed6_w4_update(EggVehicle *v,float){
    float *s=ev_vars(v);bool idle=s[0]==0 && s[1]==0;
    if(-35<ev_y(v) && idle)set_fy(v,"BP_ROTOR_7",-200);
    if(ev_y(v)<-35 && idle)set_fy(v,"BP_ROTOR_7",-100);
    float ex=ev_closest_egg_x(v,0);
    if(ex<5 && idle)set_fx(v,"BP_FAN_5",300);
    if(5<ex && idle)set_fx(v,"BP_FAN_5",-300);
    if(s[1]!=0){set_fx(v,"BP_FAN_5",300);set_fy(v,"BP_ROTOR_7",-200);}
}
static void ed6_w4_egg(EggVehicle *v){ev_vars(v)[0]=1;set_fx(v,"BP_FAN_5",200);set_fy(v,"BP_ROTOR_7",-200);}
static void ed6_w4_flee(EggVehicle *v){ev_surrender(v);ev_vars(v)[1]=1;}
static void ed6_w4_removed(EggVehicle *v,const char *name){if(!strcmp(name,"BP_ROTOR_7"))ev_timer(v,abandon,1);if(!strcmp(name,"BP_SUCKER_1"))ev_timer(v,ed6_w4_flee,1);}
/* W5: two suckers, the whole vehicle launched at -5 m/s; with the egg the propeller pushes back
   (forceX 400). Losing the sucker: the pig surrenders and the propeller pushes back too. */
static void ed6_w5_init(EggVehicle *v){
    ev_gadget(v,"BP_SUCKER_1","startSuction");ev_gadget(v,"BP_SUCKER_2","startSuction");ev_set_thrust(v,"BP_ROTOR_2",0);set_fx(v,"BP_ROTOR_2",-10);
    ev_set_thrust(v,"BP_PROPELLER_2",0);ev_set_velocity(v,-5,0);
}
static void ed6_w5_egg(EggVehicle *v){set_fx(v,"BP_ROTOR_2",0);ev_set_thrust(v,"BP_PROPELLER_2",0);set_fx(v,"BP_PROPELLER_2",400);}
static void ed6_w5_flee(EggVehicle *v){ev_surrender(v);ev_set_thrust(v,"BP_PROPELLER_2",0);set_fx(v,"BP_PROPELLER_2",400);}
static void ed6_w5_removed(EggVehicle *v,const char *name){if(!strcmp(name,"BP_LIGHT_METAL_FRAME_13"))ev_timer(v,abandon,1);if(!strcmp(name,"BP_SUCKER_1"))ev_timer(v,ed6_w5_flee,1);}

/* ---- EggDefender_7 ---- */
/* W1: BP_FAN_1 reverses at double force with the egg. */
static void ed7_w1_egg(EggVehicle *v){ev_set_thrust(v,"BP_FAN_1",-ev_thrust(v,"BP_FAN_1")*2);}
static void ed7_w1_removed(EggVehicle *v,const char *name){static const char *const stop[]={"BP_FAN_1","BP_TNT_CASING_1",0};abandon_on(v,name,stop);}
/* W2: with the egg the fan reverses at double force, the frame lets go and the balloon lifts (-100). */
static void ed7_w2_egg(EggVehicle *v){
    ev_set_thrust(v,"BP_FAN_1",-ev_thrust(v,"BP_FAN_1")*2);ev_remove_joint(v,"BP_WOODEN_FRAME_1BP_WOODEN_FRAME_2");
    ev_set_thrust(v,"BP_RUBBER_BALLOON_1",0);set_fy(v,"BP_RUBBER_BALLOON_1",-100);
}
static void ed7_w2_removed(EggVehicle *v,const char *name){static const char *const stop[]={"BP_FAN_1","BP_WOODEN_FRAME_2","BP_WOODEN_TRIANGLE_2","BP_WOODEN_WHEEL_2","BP_WOODEN_TRIANGLE_1","BP_WOODEN_WHEEL_1","BP_WOODEN_FRAME_3",0};abandon_on(v,name,stop);}
/* W3: a sucker cart whose frame aims at the bird (offset 270°, speed 1); the pig does not take the
   egg itself (setGrabLogic()); the fan (-500) steers over the egg and pushes back (500) with it. */
static void ed7_w3_init(EggVehicle *v){
    ev_set_thrust(v,"BP_FAN_2",-500);ev_start_aim(v,"BP_WOODEN_FRAME_8","bird",270,1);ev_vars(v)[0]=0;ev_no_grab(v);ev_gadget(v,"BP_SUCKER_1","startSuction");
}
static void ed7_w3_update(EggVehicle *v,float){
    if(ev_vars(v)[0]!=0)return;
    float ex=ev_closest_egg_x(v,0);if(3<ex)ev_set_thrust(v,"BP_FAN_2",-500);if(ex<-3)ev_set_thrust(v,"BP_FAN_2",500);
}
static void ed7_w3_egg(EggVehicle *v){ev_set_thrust(v,"BP_FAN_2",500);ev_vars(v)[0]=1;}
static void ed7_w3_removed(EggVehicle *v,const char *name){static const char *const stop[]={"BP_FAN_2","BP_ROCKET_HOLDER_1","BP_WOODEN_WHEEL_1","BP_WOODEN_TRIANGLE_1","BP_WOODEN_WHEEL_2","BP_WOODEN_TRIANGLE_2","BP_WOODEN_FRAME_8","BP_SUCKER_1",0};abandon_on(v,name,stop);}
/* W4: two balloons (-100) and a fan (-600): hovers at about y -40 (-130/-100), steers over the egg
   and sinks (-70, fan off); with the egg balloons -150 and fan 600. The pig never abandons.
   vars: 0 rotorDescend, 1 eggIsCollected. */
static void ed7_w4_balloons(EggVehicle *v,float fy){set_fy(v,"BP_SINGLE_BALLOON_1",fy);set_fy(v,"BP_SINGLE_BALLOON_2",fy);}
static void ed7_w4_init(EggVehicle *v){ev_set_thrust(v,"BP_FAN_1",-600);ed7_w4_balloons(v,-100);ev_vars(v)[0]=0;ev_vars(v)[1]=0;}
static void ed7_w4_update(EggVehicle *v,float){
    float *s=ev_vars(v);
    if(s[0]==0 && s[1]==0){if(-40<ev_y(v))ed7_w4_balloons(v,-130);if(ev_y(v)<-40)ed7_w4_balloons(v,-100);}
    float ex=ev_closest_egg_x(v,0);
    if(5<ex && s[1]==0){ev_set_thrust(v,"BP_FAN_1",-600);s[0]=0;}
    if(ex<-5 && s[1]==0){ev_set_thrust(v,"BP_FAN_1",600);s[0]=0;}
    if(-5<ex && ex<5 && s[1]==0){ev_set_thrust(v,"BP_FAN_1",0);ed7_w4_balloons(v,-70);s[0]=1;}
}
static void ed7_w4_egg(EggVehicle *v){ev_set_thrust(v,"BP_FAN_1",600);ed7_w4_balloons(v,-150);ev_vars(v)[1]=1;}
static void ed7_w4_removed(EggVehicle *v,const char *name){static const char *const stop[]={"BP_SINGLE_BALLOON_1","BP_SINGLE_BALLOON_2","BP_FAN_1","BP_WOODEN_FRAME_5",0};abandon_on(v,name,stop);}
/* W5: two suckers under two rubber balloons (-80; -100 below y -35, -50 above; the balance call with
   -50 switches balancing off); the fan steers (±400) and over the egg the TNT drops; with the egg
   the fan pushes -2000. vars: 0 eggIsCollected. */
static void ed7_w5_balloons(EggVehicle *v,float fy){set_fy(v,"BP_RUBBER_BALLOON_1",fy);set_fy(v,"BP_RUBBER_BALLOON_3",fy);}
static void ed7_w5_init(EggVehicle *v){ev_gadget(v,"BP_SUCKER_1","startSuction");ev_gadget(v,"BP_SUCKER_2","startSuction");ed7_w5_balloons(v,-80);ev_vars(v)[0]=0;}
static void ed7_w5_update(EggVehicle *v,float){
    if(-35<ev_y(v))ed7_w5_balloons(v,-100);
    if(ev_y(v)<-35)ed7_w5_balloons(v,-50);
    float ex=ev_closest_egg_x(v,0);
    if(5<ex)ev_set_thrust(v,"BP_FAN_1",400);
    if(ex<5)ev_set_thrust(v,"BP_FAN_1",-400);
    if(-6<ex && ex<6 && ev_vars(v)[0]==0){ev_remove_joint(v,"BP_LIGHT_METAL_FRAME_2BP_TNT_SMALL_2");ev_remove_joint(v,"BP_LIGHT_METAL_FRAME_2BP_TNT_CASING_1");}
}
static void ed7_w5_egg(EggVehicle *v){ev_set_thrust(v,"BP_FAN_1",-2000);ev_vars(v)[0]=1;}
static void ed7_w5_removed(EggVehicle *v,const char *name){if(!strcmp(name,"BP_FAN_1"))ev_timer(v,abandon,1);}

/* ---- EggDefender_8 (rockets) ---- */
static void rocket(EggVehicle *v,const char *n){ev_gadget(v,n,"startRocket");}
static void burn(EggVehicle *v,const char *n,float seconds){ev_gadget_property(v,n,"runDuration",seconds);}
/* W1: red rockets (-450, 4 s) fire after 3 s; with the egg the blue ones (-700, 8 s) take it away. */
static void ed8_w1_rockets(EggVehicle *v){rocket(v,"BP_ROCKET_RED_4");rocket(v,"BP_ROCKET_RED_6");}
static void ed8_w1_init(EggVehicle *v){
    burn(v,"BP_ROCKET_BLUE_1",8);burn(v,"BP_ROCKET_BLUE_2",8);burn(v,"BP_ROCKET_RED_4",4);burn(v,"BP_ROCKET_RED_6",4);ev_timer(v,ed8_w1_rockets,3);
    ev_set_thrust(v,"BP_ROCKET_RED_4",-450);ev_set_thrust(v,"BP_ROCKET_RED_6",-450);
}
static void ed8_w1_egg(EggVehicle *v){ev_set_thrust(v,"BP_ROCKET_BLUE_1",-700);ev_set_thrust(v,"BP_ROCKET_BLUE_2",-700);rocket(v,"BP_ROCKET_BLUE_1");rocket(v,"BP_ROCKET_BLUE_2");}
/* W2: four blue rockets fire at once (4 s; the -2000 set afterwards is overridden by the ramp, as in
   the game); with the egg the cart drops its frame and the red rockets (-2000) fire. */
static void ed8_w2_init(EggVehicle *v){
    burn(v,"BP_ROCKET_BLUE_7",4);burn(v,"BP_ROCKET_BLUE_5",4);burn(v,"BP_ROCKET_BLUE_8",4);
    rocket(v,"BP_ROCKET_BLUE_7");rocket(v,"BP_ROCKET_BLUE_6");rocket(v,"BP_ROCKET_BLUE_5");rocket(v,"BP_ROCKET_BLUE_8");
    ev_set_thrust(v,"BP_ROCKET_RED_3",-2000);ev_set_thrust(v,"BP_ROCKET_RED_4",-2000);
}
static void ed8_w2_egg(EggVehicle *v){ev_remove_joint(v,"BP_WOODEN_TRIANGLE_3BP_WOODEN_FRAME_4");ev_remove_joint(v,"BP_WOODEN_FRAME_5BP_WOODEN_FRAME_4");rocket(v,"BP_ROCKET_RED_3");rocket(v,"BP_ROCKET_RED_4");}
/* W3: a blue rocket (-800, 4 s) brings it in; two red ones (-800) take the egg away. */
static void ed8_w3_init(EggVehicle *v){
    ev_set_thrust(v,"BP_ROCKET_BLUE_2",-800);burn(v,"BP_ROCKET_BLUE_2",4);ev_set_thrust(v,"BP_ROCKET_RED_3",-800);ev_set_thrust(v,"BP_ROCKET_RED_4",-800);rocket(v,"BP_ROCKET_BLUE_2");
}
static void ed8_w3_egg(EggVehicle *v){rocket(v,"BP_ROCKET_RED_3");rocket(v,"BP_ROCKET_RED_4");}
/* W4: a propeller cart (forceX -500 while the egg is ahead, 1000 back at x < 20) with a folded
   rocket (-3000, 4 s): with the egg the arm unfolds (motors 3), the rocket fires after 1 s and the
   frame lets go after 2 s. */
static void ed8_w4_launch(EggVehicle *v){rocket(v,"BP_ROCKET_BLUE_1");}
static void ed8_w4_release(EggVehicle *v){ev_remove_joint(v,"BP_LIGHT_METAL_FRAME_8BP_LIGHT_METAL_FRAME_2");ev_remove_joint(v,"BP_LIGHT_METAL_FRAME_8BP_TNT_2");}
static void ed8_w4_init(EggVehicle *v){burn(v,"BP_ROCKET_BLUE_1",4);ev_set_thrust(v,"BP_ROCKET_BLUE_1",-3000);ev_set_thrust(v,"BP_PROPELLER_1",0);ev_motor_speed(v,"BP_LIGHT_METAL_FRAME_6BP_LIGHT_METAL_FRAME_2",0);}
static void ed8_w4_update(EggVehicle *v,float){if(3<ev_closest_egg_x(v,0))set_fx(v,"BP_PROPELLER_1",-500);if(ev_x(v)<20)set_fx(v,"BP_PROPELLER_1",1000);}
static void ed8_w4_egg(EggVehicle *v){
    if(!ev_has_block(v,"BP_ROCKET_BLUE_1"))return;
    ev_motor_speed(v,"BP_LIGHT_METAL_FRAME_6BP_LIGHT_METAL_FRAME_2",3);ev_motor_speed(v,"BP_COG_OFF_1BP_LIGHT_METAL_FRAME_6",3);ev_timer(v,ed8_w4_launch,1);ev_timer(v,ed8_w4_release,2);
}
/* W5: placed by the nest (disableTeleporting); with the egg two red rockets (-1100/-1111) fire after 2 s. */
static void ed8_w5_out(EggVehicle *v){rocket(v,"BP_ROCKET_RED_1");rocket(v,"BP_ROCKET_RED_2");}
static void ed8_w5_init(EggVehicle *v){ev_set_thrust(v,"BP_ROCKET_RED_2",-1100);ev_set_thrust(v,"BP_ROCKET_RED_1",-1111);}
static void ed8_w5_egg(EggVehicle *v){ev_timer(v,ed8_w5_out,2);}

/* ---- EggDefender_9 (cannons aiming at the bird) ---- */
static void ed9_aim(EggVehicle *v){ev_start_aim(v,"BP_CANNON_1","bird",0,0);}
static void ed9_w1_egg(EggVehicle *v){ev_set_thrust(v,"BP_FAN_1",-ev_thrust(v,"BP_FAN_1")*2);}
static void ed9_w1_removed(EggVehicle *v,const char *name){static const char *const stop[]={"BP_FAN_1","BP_WOODEN_WHEEL_1","BP_WOODEN_TRIANGLE_1","BP_WOODEN_WHEEL_2","BP_WOODEN_TRIANGLE_2",0};abandon_on(v,name,stop);}
static void ed9_w2_init(EggVehicle *v){ev_vars(v)[0]=0;ed9_aim(v);}
static void ed9_w2_egg(EggVehicle *v){ev_vars(v)[0]=1;flip_thrust(v,"BP_FAN_1");}
static void ed9_w2_removed(EggVehicle *v,const char *name){
    static const char *const stop[]={"BP_WOODEN_WHEEL_1","BP_WOODEN_TRIANGLE_1","BP_WOODEN_WHEEL_2","BP_WOODEN_TRIANGLE_2",0};
    if((!strcmp(name,"BP_FAN_1") && ev_vars(v)[0]==0) || named(name,stop))ev_timer(v,abandon,1);
}
static void ed9_w3_removed(EggVehicle *v,const char *name){if(!strcmp(name,"BP_FAN_1"))ev_timer(v,abandon,1);}
static void ed9_w4_init(EggVehicle *v){ed9_aim(v);ev_set_thrust(v,"BP_FAN_1",0);set_fx(v,"BP_FAN_1",-100);}
static void ed9_w4_egg(EggVehicle *v){set_fx(v,"BP_FAN_1",400);}
/* W5: a rotor flyer with a sucker aiming at the egg and a cannon at the bird; fans balanced (100)
   steer with forceX ±100; rotor -120 below y -35, -80 above, -150 with the egg. Losing the sucker
   the pig surrenders (fans 300, rotor -200). vars: 0 eggIsCollected, 1 surrender. */
static void ed9_w5_init(EggVehicle *v){
    ev_set_thrust(v,"BP_ROTOR_1",0);set_fy(v,"BP_ROTOR_1",-100);ev_set_thrust(v,"BP_FAN_3",0);ev_set_thrust(v,"BP_FAN_2",0);ev_vars(v)[0]=0;ev_vars(v)[1]=0;
    ev_gadget(v,"BP_SUCKER_1","startSuction");ed9_aim(v);ev_start_aim(v,"BP_SUCKER_1","egg",0,0);ev_auto_balance(v,"BP_FAN_2","BP_FAN_3",100);
}
static void ed9_w5_update(EggVehicle *v,float){
    float *s=ev_vars(v);
    if(s[0]==0 && s[1]==0){float ex=ev_closest_egg_x(v,0);if(3<ex){set_fx(v,"BP_FAN_3",-100);set_fx(v,"BP_FAN_2",-100);}if(ex<-3){set_fx(v,"BP_FAN_3",100);set_fx(v,"BP_FAN_2",100);}}
    if(s[0]==0){if(-35<ev_y(v))set_fy(v,"BP_ROTOR_1",-120);if(ev_y(v)<-35)set_fy(v,"BP_ROTOR_1",-80);}
    if(s[0]!=0)set_fy(v,"BP_ROTOR_1",-150);
}
static void ed9_w5_egg(EggVehicle *v){ev_stop_aim(v,"BP_SUCKER_1");set_fx(v,"BP_FAN_3",300);set_fx(v,"BP_FAN_2",300);ev_vars(v)[0]=1;}
static void ed9_w5_surrender(EggVehicle *v){ev_surrender(v);set_fx(v,"BP_FAN_3",300);set_fx(v,"BP_FAN_2",300);set_fy(v,"BP_ROTOR_1",-200);ev_vars(v)[1]=1;}
static void ed9_w5_removed(EggVehicle *v,const char *name){
    static const char *const stop[]={"BP_ROTOR_1","BP_FAN_2","BP_FAN_3",0};abandon_on(v,name,stop);
    if(!strcmp(name,"BP_SUCKER_1"))ev_timer(v,ed9_w5_surrender,1);
}

/* ---- EggDefender_10 ---- */
/* W1: cannon aims at the bird; with the egg the fan reverses and the rope lets go. */
static void ed10_w1_init(EggVehicle *v){ev_start_aim(v,"BP_CANNON_2","bird",0,0);}
static void ed10_w1_egg(EggVehicle *v){flip_thrust(v,"BP_FAN_1");ev_remove_joint(v,"BP_ROPE_THICK_1BP_WOODEN_FRAME_1");ev_remove_joint(v,"BP_ROPE_THICK_1BP_LIGHT_METAL_FRAME_2");}
static void ed10_w1_removed(EggVehicle *v,const char *name){static const char *const stop[]={"BP_FAN_1","BP_WOODEN_FRAME_2","BP_WOODEN_WHEEL_2","BP_WOODEN_TRIANGLE_2",0};abandon_on(v,name,stop);}
/* W2: two rubber balloons (-150; -100 above y -35) and a sucker; the fan steers with forceX ±100,
   and within 10 m of the egg the ropes let the sucker down; with the egg the fan pushes 300.
   (The balance call with -1000 switches balancing off.) vars: 0 eggIsCollected. */
static void ed10_w2_balloons(EggVehicle *v,float fy){set_fy(v,"BP_RUBBER_BALLOON_2",fy);set_fy(v,"BP_RUBBER_BALLOON_1",fy);}
static void ed10_w2_init(EggVehicle *v){ed10_w2_balloons(v,-150);ev_set_thrust(v,"BP_FAN_1",0);ev_vars(v)[0]=0;ev_gadget(v,"BP_SUCKER_1","startSuction");}
static void ed10_w2_update(EggVehicle *v,float){
    float ex=ev_closest_egg_x(v,0);
    if(ev_vars(v)[0]==0){if(3<ex)set_fx(v,"BP_FAN_1",-100);if(ex<-3)set_fx(v,"BP_FAN_1",100);
        if(-35<ev_y(v))ed10_w2_balloons(v,-150);
        if(ev_y(v)<-35)ed10_w2_balloons(v,-100);}
    else ed10_w2_balloons(v,-150);
    if(ex<10){ev_remove_joint(v,"BP_ROPE_RUBBER_1BP_LIGHT_METAL_FRAME_3");ev_remove_joint(v,"BP_ROPE_RUBBER_2BP_LIGHT_METAL_FRAME_1");}
}
static void ed10_w2_egg(EggVehicle *v){set_fx(v,"BP_FAN_1",300);ev_vars(v)[0]=1;}
static void ed10_w2_removed(EggVehicle *v,const char *name){if(!strcmp(name,"BP_FAN_1"))ev_timer(v,abandon,1);}
/* W3: cannon; the propeller reverses at double force with the egg. */
static void ed10_w3_egg(EggVehicle *v){ev_set_thrust(v,"BP_PROPELLER_1",-ev_thrust(v,"BP_PROPELLER_1")*2);}
static void ed10_w3_removed(EggVehicle *v,const char *name){if(!strcmp(name,"BP_PROPELLER_1"))ev_timer(v,abandon,1);}
static void ed10_w3_init(EggVehicle *v){ev_start_aim(v,"BP_CANNON_1","bird",0,0);}
/* W4: a sucker cart with folded propellers (off): at x < 70 the arm turns (0.4) and after 6 s the
   propellers switch on, balanced (10); with the egg BP_FAN_2 pushes 2000 and the frame lets go 1 s
   later. vars: 0 balance. */
static void ed10_w4_balance(EggVehicle *v){ev_power(v,"BP_PROPELLER_1",1);ev_power(v,"BP_PROPELLER_2",1);ev_auto_balance(v,"BP_PROPELLER_1","BP_PROPELLER_2",10);}
static void ed10_w4_release(EggVehicle *v){ev_remove_joint(v,"BP_WOODEN_FRAME_2BP_LIGHT_METAL_FRAME_6");}
static void ed10_w4_init(EggVehicle *v){ev_power(v,"BP_PROPELLER_1",0);ev_power(v,"BP_PROPELLER_2",0);ev_gadget(v,"BP_SUCKER_1","startSuction");ev_vars(v)[0]=0;}
static void ed10_w4_update(EggVehicle *v,float){if(ev_x(v)<70 && ev_vars(v)[0]==0){ev_motor_speed(v,"BP_LIGHT_METAL_FRAME_1BP_LIGHT_METAL_FRAME_6",0.4f);ev_timer(v,ed10_w4_balance,6);ev_vars(v)[0]=1;}}
static void ed10_w4_egg(EggVehicle *v){ev_timer(v,ed10_w4_release,1);ev_set_thrust(v,"BP_FAN_2",2000);}
static void ed10_w4_removed(EggVehicle *v,const char *name){static const char *const stop[]={"BP_FAN_2","BP_WOODEN_WHEEL_5","BP_WOODEN_TRIANGLE_7","BP_WOODEN_TRIANGLE_6","BP_WOODEN_WHEEL_4",0};abandon_on(v,name,stop);}
/* W5: sucker cart with a cannon; the fan steers with forceX ±100, 500 with the egg; losing the
   sucker the pig surrenders and the fan pushes 1000. vars: 0 eggIsCollected, 1 surrender. */
static void ed10_w5_init(EggVehicle *v){ev_set_thrust(v,"BP_FAN_1",0);ev_vars(v)[0]=0;ev_vars(v)[1]=0;ev_gadget(v,"BP_SUCKER_1","startSuction");ev_start_aim(v,"BP_CANNON_1","bird",0,0);}
static void ed10_w5_update(EggVehicle *v,float){if(ev_vars(v)[0]==0 && ev_vars(v)[1]==0){float ex=ev_closest_egg_x(v,0);if(3<ex)set_fx(v,"BP_FAN_1",-100);if(ex<-3)set_fx(v,"BP_FAN_1",100);}}
static void ed10_w5_egg(EggVehicle *v){set_fx(v,"BP_FAN_1",500);ev_vars(v)[0]=1;}
static void ed10_w5_surrender(EggVehicle *v){ev_surrender(v);set_fx(v,"BP_FAN_1",1000);ev_vars(v)[1]=1;}
static void ed10_w5_removed(EggVehicle *v,const char *name){
    static const char *const stop[]={"BP_FAN_1","BP_WOODEN_WHEEL_2","BP_WOODEN_TRIANGLE_2",0};abandon_on(v,name,stop);
    if(!strcmp(name,"BP_SUCKER_1"))ev_timer(v,ed10_w5_surrender,1);
}

/* ---- EggDefender_11 ---- */
/* W1: a balloon flyer (single balloon -100, two rubber -80) driven by two bellows whose puff
   interval is max(min(10 / egg distance, 2), 0.3); the front bellows blow while the egg is more than
   10 m ahead, the rear ones once over it; within -6..45 m the sucker starts. With the egg it drops
   its frame and bellows and rises (-130). If the single balloon goes (ballooned) the rubber ones take
   over (-120; -150/-50 by height) and with the egg the rear bellows puff every 0.2 s.
   vars: 0 eggIsCollected, 1 suckerOn, 2 ballooned. */
static void ed11_w1_drop(EggVehicle *v){
    ev_remove_joint(v,"BP_LIGHT_METAL_FRAME_2BP_LIGHT_METAL_FRAME_3");ev_remove_joint(v,"BP_LIGHT_METAL_FRAME_2BP_LIGHT_METAL_FRAME_1");
    ev_remove_joint(v,"BP_ROPE_RUBBER_5BP_TELESCOPIC_ARM_2");ev_remove_joint(v,"BP_ROPE_RUBBER_1BP_TELESCOPIC_ARM_1");
    ev_remove_block(v,"BP_BELLOWS_1");ev_remove_block(v,"BP_BELLOWS_2");
}
static void ed11_w1_init(EggVehicle *v){set_fy(v,"BP_RUBBER_BALLOON_1",-80);set_fy(v,"BP_RUBBER_BALLOON_3",-80);set_fy(v,"BP_SINGLE_BALLOON_1",-100);for(int k=0;k<3;k++)ev_vars(v)[k]=0;}
static void ed11_w1_update(EggVehicle *v,float){
    float *s=ev_vars(v),d=ev_closest_egg_distance(v),k=10/d;k=k<2?k:2;ev_set_interval(v,"BP_BELLOWS_1",k>0.3f?k:0.3f);
    if(s[0]==0){
        if(s[2]==0){if(-30<ev_y(v))set_fy(v,"BP_SINGLE_BALLOON_1",-100);if(ev_y(v)<-30)set_fy(v,"BP_SINGLE_BALLOON_1",-50);}
        else{float fy=-30<ev_y(v)?-150:-50;if(ev_y(v)!=-30){set_fy(v,"BP_RUBBER_BALLOON_1",fy);set_fy(v,"BP_RUBBER_BALLOON_3",fy);}}
        float ex=ev_closest_egg_x(v,0);
        if(10<ex){ev_power(v,"BP_BELLOWS_1",1);ev_power(v,"BP_BELLOWS_2",0);}
        if(ex<10){ev_power(v,"BP_BELLOWS_1",0);ev_power(v,"BP_BELLOWS_2",1);}
        if(-6<ex && ex<45 && s[1]==0){ev_gadget(v,"BP_SUCKER_1","startSuction");s[1]=1;}
    }
}
static void ed11_w1_egg(EggVehicle *v){
    float *s=ev_vars(v);
    if(s[2]==0){ed11_w1_drop(v);set_fy(v,"BP_SINGLE_BALLOON_1",-130);}
    else{ev_power(v,"BP_BELLOWS_1",0);ev_power(v,"BP_BELLOWS_2",1);ev_set_interval(v,"BP_BELLOWS_2",0.2f);}
    s[0]=1;
}
static void ed11_w1_flee(EggVehicle *v){ev_surrender(v);ed11_w1_drop(v);set_fy(v,"BP_SINGLE_BALLOON_1",-150);ev_vars(v)[0]=1;}
static void ed11_w1_removed(EggVehicle *v,const char *name){
    static const char *const pop[]={"BP_SINGLE_BALLOON_1","BP_ROPE_THICK_1","BP_ROPE_THICK_2","BP_ROPE_THICK_3",0};
    if(named(name,pop)){set_fy(v,"BP_RUBBER_BALLOON_1",-120);set_fy(v,"BP_RUBBER_BALLOON_3",-120);ev_vars(v)[2]=1;}
    if(!strcmp(name,"BP_SUCKER_1"))ev_timer(v,ed11_w1_flee,1);
    if(!strcmp(name,"BP_BELLOWS_1") && ev_vars(v)[2]!=0)ev_timer(v,abandon,1);
}
/* W2: a sucker cart with telescopic arms (motors ∓0.1) and two fans off (-1500): the pig does not
   take the egg, BP_FAN_4 steers (forceX ∓500); with the egg the arms close (2/-2) and 1 s later the
   balloon inflates, the sucker is dropped from the frame and the fans switch on. vars: 0 eggIsCollected. */
static void ed11_w2_release(EggVehicle *v){
    ev_gadget(v,"BP_BALLOON_INFLATER_2","inflate");
    ev_remove_joint(v,"BP_WOODEN_FRAME_7BP_SUCKER_1");ev_remove_joint(v,"BP_WOODEN_FRAME_6BP_SUCKER_1");ev_remove_joint(v,"BP_WOODEN_FRAME_5BP_WOODEN_FRAME_6");ev_remove_joint(v,"BP_WOODEN_FRAME_5BP_WOODEN_FRAME_7");
    ev_power(v,"BP_FAN_3",1);ev_power(v,"BP_FAN_2",1);
}
static void ed11_w2_init(EggVehicle *v){
    ev_motor_speed(v,"BP_TELESCOPIC_ARM_4BP_WOODEN_FRAME_5",-0.1f);ev_motor_speed(v,"BP_TELESCOPIC_ARM_6BP_WOODEN_FRAME_5",0.1f);
    ev_power(v,"BP_FAN_3",0);ev_power(v,"BP_FAN_2",0);ev_set_thrust(v,"BP_FAN_3",-1500);ev_set_thrust(v,"BP_FAN_2",-1500);ev_set_thrust(v,"BP_FAN_4",0);
    ev_vars(v)[0]=0;ev_gadget(v,"BP_SUCKER_1","startSuction");ev_no_grab(v);set_fy(v,"BP_BALLOON_INFLATER_2",-120);
}
static void ed11_w2_update(EggVehicle *v,float){if(ev_vars(v)[0]==0){float ex=ev_closest_egg_x(v,0);if(5<ex)set_fx(v,"BP_FAN_4",-500);if(ex<-5)set_fx(v,"BP_FAN_4",500);}}
static void ed11_w2_egg(EggVehicle *v){
    ev_set_thrust(v,"BP_FAN_4",0);ev_timer(v,ed11_w2_release,1);ev_vars(v)[0]=1;
    ev_motor_speed(v,"BP_TELESCOPIC_ARM_4BP_WOODEN_FRAME_5",2);ev_motor_speed(v,"BP_TELESCOPIC_ARM_6BP_WOODEN_FRAME_5",-2);
}
/* W3: a twin-propeller flyer (forceY -600; -800 below y -45, -640 above; -1000/-600 with the egg)
   with a fan (-500/700 over the egg, forceX 500 with it) and a sucker; within 25 m it drops its rocket
   holder. (The balance call with -500 switches balancing off.) vars: 0 eggIsCollected. */
static void ed11_w3_lift(EggVehicle *v,float fy){set_fy(v,"BP_PROPELLER_1",fy);set_fy(v,"BP_PROPELLER_2",fy);}
static void ed11_w3_init(EggVehicle *v){ev_set_thrust(v,"BP_PROPELLER_1",0);ev_set_thrust(v,"BP_PROPELLER_2",0);ed11_w3_lift(v,-600);ev_set_thrust(v,"BP_FAN_1",-500);ev_vars(v)[0]=0;ev_gadget(v,"BP_SUCKER_2","startSuction");}
static void ed11_w3_update(EggVehicle *v,float){
    bool high=-45<ev_y(v),low=ev_y(v)<-45;
    if(ev_vars(v)[0]==0){
        float ex=ev_closest_egg_x(v,0);
        if(3<ex)ev_set_thrust(v,"BP_FAN_1",-500);
        if(ex<-3)ev_set_thrust(v,"BP_FAN_1",700);
        if(ex<25){ev_remove_joint(v,"BP_ROCKET_HOLDER_2BP_TELESCOPIC_ARM_2");ev_remove_joint(v,"BP_ROCKET_HOLDER_2BP_TELESCOPIC_ARM_3");}
        if(high)ed11_w3_lift(v,-800);
        if(low)ed11_w3_lift(v,-640);
    }else{if(high)ed11_w3_lift(v,-1000);if(low)ed11_w3_lift(v,-600);}
}
static void ed11_w3_egg(EggVehicle *v){ev_set_thrust(v,"BP_FAN_1",0);set_fx(v,"BP_FAN_1",500);ev_vars(v)[0]=1;}
static void ed11_w3_surrender(EggVehicle *v){ev_surrender(v);ev_set_thrust(v,"BP_FAN_1",1000);ev_vars(v)[0]=1;}
static void ed11_w3_removed(EggVehicle *v,const char *name){
    static const char *const stop[]={"BP_FAN_1","BP_PROPELLER_1","BP_PROPELLER_2",0},*const holder[]={"BP_TELESCOPIC_ARM_2","BP_TELESCOPIC_ARM_3","BP_ROCKET_HOLDER_4","BP_ROCKET_HOLDER_5",0};
    abandon_on(v,name,stop);if(!strcmp(name,"BP_SUCKER_2") || named(name,holder))ev_timer(v,ed11_w3_surrender,1);
}
/* W4: three rubber balloons (-150; -130/-110/-130 below y -40, -100/-80/-100 above), propeller and
   fan (∓500 beyond 13 m), a sucker on a hatch dropped over the egg; with the egg two red rockets
   (-2000) fire after 1.5 s and the hatch lets go after 1.6 s. vars: 0 rotorDescend, 1 eggIsCollected. */
static void ed11_w4_init(EggVehicle *v){
    ev_set_thrust(v,"BP_PROPELLER_1",-500);ev_set_thrust(v,"BP_FAN_1",-500);set_fy(v,"BP_RUBBER_BALLOON_2",-150);set_fy(v,"BP_RUBBER_BALLOON_1",-150);set_fy(v,"BP_RUBBER_BALLOON_3",-150);
    ev_set_thrust(v,"BP_ROCKET_RED_3",-2000);ev_set_thrust(v,"BP_ROCKET_RED_2",-2000);ev_vars(v)[0]=0;ev_vars(v)[1]=0;ev_gadget(v,"BP_SUCKER_2","startSuction");
}
static void ed11_w4_update(EggVehicle *v,float){
    float *s=ev_vars(v);
    if(s[0]==0 && s[1]==0){
        if(-40<ev_y(v)){set_fy(v,"BP_RUBBER_BALLOON_2",-130);set_fy(v,"BP_RUBBER_BALLOON_1",-110);set_fy(v,"BP_RUBBER_BALLOON_3",-130);}
        if(ev_y(v)<-40){set_fy(v,"BP_RUBBER_BALLOON_2",-100);set_fy(v,"BP_RUBBER_BALLOON_1",-80);set_fy(v,"BP_RUBBER_BALLOON_3",-100);}
    }
    float ex=ev_closest_egg_x(v,0);
    if(13<ex){ev_set_thrust(v,"BP_PROPELLER_1",-500);ev_set_thrust(v,"BP_FAN_1",-500);s[0]=0;}
    if(ex<-13){ev_set_thrust(v,"BP_PROPELLER_1",500);ev_set_thrust(v,"BP_FAN_1",500);s[0]=0;}
    if(-13<ex && ex<5 && s[1]==0){ev_remove_joint(v,"BP_WOODEN_FRAME_4BP_WOODEN_FRAME_2");s[0]=1;}
}
static void ed11_w4_hatch(EggVehicle *v){ev_remove_joint(v,"BP_SUCKER_2BP_WOODEN_FRAME_2");}
static void ed11_w4_launch(EggVehicle *v){rocket(v,"BP_ROCKET_RED_3");rocket(v,"BP_ROCKET_RED_2");}
static void ed11_w4_egg(EggVehicle *v){ev_timer(v,ed11_w4_hatch,1.6f);ev_timer(v,ed11_w4_launch,1.5f);ev_vars(v)[1]=1;}
/* W5: rotor (forceY -110) and sucker; the fan reverses at double force with the egg. */
static void ed11_w5_init(EggVehicle *v){ev_gadget(v,"BP_SUCKER_1","startSuction");ev_set_thrust(v,"BP_ROTOR_1",0);set_fy(v,"BP_ROTOR_1",-110);}
static void ed11_w5_egg(EggVehicle *v){ev_set_thrust(v,"BP_FAN_1",-ev_thrust(v,"BP_FAN_1")*2);}
static void ed11_w5_removed(EggVehicle *v,const char *name){if(!strcmp(name,"BP_FAN_1"))ev_timer(v,abandon,1);}

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
    {"EggDefender_4_W1_V1",0,ed4_w1_update,ed4_w1_egg,ed4_w1_removed,0},
    {"EggDefender_4_W2_V1",ed4_w2_init,ed4_w2_update,ed4_w2_egg,ed4_w2_removed,0},
    {"EggDefender_4_W3_V1",ed4_w3_init,ed4_w3_update,ed4_w3_egg,ed4_w3_removed,0},
    {"EggDefender_4_W4_V1",ed4_w4_init,ed4_w4_update,ed4_w4_egg,0,0},
    {"EggDefender_4_W5_V1",ed4_w5_init,ed4_w5_update,ed4_w5_egg,0,0},
    {"EggDefender_5_W1_V1",ed5_w1_init,ed5_w1_update,ed5_w1_egg,ed5_w1_removed,0},
    {"EggDefender_5_W2_V1",ed5_w2_init,0,ed5_w2_egg,ed5_w2_removed,0},
    {"EggDefender_5_W3_V1",ed5_w3_init,0,ed5_w3_egg,ed5_w3_removed,0},
    {"EggDefender_5_W4_V1",ed5_w4_init,0,ed5_w4_egg,ed5_w4_removed,0},
    {"EggDefender_5_W5_V1",0,ed5_w5_update,ed5_w5_egg,ed5_w5_removed,0},
    {"EggDefender_6_W1_V1",ed6_w1_init,0,ed6_w1_egg,ed6_w1_removed,0},
    {"EggDefender_6_W2_V1",ed6_w2_init,0,ed6_w2_egg,ed6_w2_removed,0},
    {"EggDefender_6_W3_V1",ed6_w3_init,ed6_w3_update,ed6_w3_egg,ed6_w3_removed,1},
    {"EggDefender_6_W4_V1",ed6_w4_init,ed6_w4_update,ed6_w4_egg,ed6_w4_removed,0},
    {"EggDefender_6_W5_V1",ed6_w5_init,0,ed6_w5_egg,ed6_w5_removed,0},
    {"EggDefender_7_W1_V1",0,0,ed7_w1_egg,ed7_w1_removed,0},
    {"EggDefender_7_W2_V1",0,0,ed7_w2_egg,ed7_w2_removed,0},
    {"EggDefender_7_W3_V1",ed7_w3_init,ed7_w3_update,ed7_w3_egg,ed7_w3_removed,0},
    {"EggDefender_7_W4_V1",ed7_w4_init,ed7_w4_update,ed7_w4_egg,ed7_w4_removed,1},
    {"EggDefender_7_W5_V1",ed7_w5_init,ed7_w5_update,ed7_w5_egg,ed7_w5_removed,0},
    {"EggDefender_8_W1_V1",ed8_w1_init,0,ed8_w1_egg,0,0},
    {"EggDefender_8_W2_V1",ed8_w2_init,0,ed8_w2_egg,0,0},
    {"EggDefender_8_W3_V1",ed8_w3_init,0,ed8_w3_egg,0,0},
    {"EggDefender_8_W4_V1",ed8_w4_init,ed8_w4_update,ed8_w4_egg,0,0},
    {"EggDefender_8_W5_V1",ed8_w5_init,0,ed8_w5_egg,0,0},
    {"EggDefender_9_W1_V1",ed9_aim,0,ed9_w1_egg,ed9_w1_removed,0},
    {"EggDefender_9_W2_V1",ed9_w2_init,0,ed9_w2_egg,ed9_w2_removed,0},
    {"EggDefender_9_W3_V1",ed9_aim,0,ed9_w1_egg,ed9_w3_removed,0},
    {"EggDefender_9_W4_V1",ed9_w4_init,0,ed9_w4_egg,ed9_w3_removed,0},
    {"EggDefender_9_W5_V1",ed9_w5_init,ed9_w5_update,ed9_w5_egg,ed9_w5_removed,1},
    {"EggDefender_10_W1_V1",ed10_w1_init,0,ed10_w1_egg,ed10_w1_removed,0},
    {"EggDefender_10_W2_V1",ed10_w2_init,ed10_w2_update,ed10_w2_egg,ed10_w2_removed,0},
    {"EggDefender_10_W3_V1",ed10_w3_init,0,ed10_w3_egg,ed10_w3_removed,0},
    {"EggDefender_10_W4_V1",ed10_w4_init,ed10_w4_update,ed10_w4_egg,ed10_w4_removed,0},
    {"EggDefender_10_W5_V1",ed10_w5_init,ed10_w5_update,ed10_w5_egg,ed10_w5_removed,0},
    {"EggDefender_11_W1_V1",ed11_w1_init,ed11_w1_update,ed11_w1_egg,ed11_w1_removed,0},
    {"EggDefender_11_W2_V1",ed11_w2_init,ed11_w2_update,ed11_w2_egg,0,0},
    {"EggDefender_11_W3_V1",ed11_w3_init,ed11_w3_update,ed11_w3_egg,ed11_w3_removed,0},
    {"EggDefender_11_W4_V1",ed11_w4_init,ed11_w4_update,ed11_w4_egg,0,0},
    {"EggDefender_11_W5_V1",ed11_w5_init,0,ed11_w5_egg,ed11_w5_removed,0},
};
const EggController *egg_controller(const char *name){
    for(unsigned i=0;i<sizeof(controllers)/sizeof(controllers[0]);i++)if(!strcmp(controllers[i].name,name))return &controllers[i];
    return 0;
}
