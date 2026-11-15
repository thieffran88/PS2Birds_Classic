/* Egg Defender vehicle controllers: the 72 assets/data/vehicles/EggDefender_N/EggDefender_N_Wk_V1.scripts.lua of the APK,
   translated by hand to C++ (egg_vehicles.cpp). The game never runs the original Lua: each
   controller calls the small vehicle API below, which mirrors EggDefenderVehicle.lua. */
#ifndef PS2BIRDS_EGG_VEHICLES_H
#define PS2BIRDS_EGG_VEHICLES_H

struct EggVehicle; /* one wave's vehicle (game.cpp) */
typedef void (*EggTimed)(EggVehicle *v);
typedef struct {
    const char *name;                                   /* e.g. "EggDefender_1_W2_V1" */
    void (*init)(EggVehicle *v);                        /* init(vehicle) */
    void (*update)(EggVehicle *v,float dt);             /* update(vehicle, dt) */
    void (*egg_collected)(EggVehicle *v);               /* eggCollected(vehicle, egg, pig) */
    void (*block_removed)(EggVehicle *v,const char *original_name); /* blockRemoved(vehicle, block) */
    int never_abandon;                                  /* setAbandoningLogic(function() return false end) */
} EggController;
const EggController *egg_controller(const char *name);

/* EggDefenderVehicle API used by the scripts (names are the vehicle file's original names). */
float ev_x(EggVehicle *v);                  /* getX: first pig x minus the slingshot x */
float ev_y(EggVehicle *v);                  /* getY */
float ev_vx(EggVehicle *v);                 /* getVX */
float ev_vy(EggVehicle *v);                 /* getVY */
int ev_has_block(EggVehicle *v,const char *name);   /* hasBlock / blockExists */
float ev_block_x(EggVehicle *v,const char *name);   /* getBlock(name).x */
float ev_block_y(EggVehicle *v,const char *name);   /* getBlock(name).y */
float ev_thrust(EggVehicle *v,const char *name);    /* getBlock(name).forceRelative */
void ev_set_thrust(EggVehicle *v,const char *name,float force);
float ev_force_x(EggVehicle *v,const char *name);   /* getBlock(name).forceX */
float ev_force_y(EggVehicle *v,const char *name);   /* getBlock(name).forceY */
void ev_set_force(EggVehicle *v,const char *name,float fx,float fy);
float ev_target(EggVehicle *v,const char *name);   /* getBlock(name).targetVelocity */
void ev_set_target(EggVehicle *v,const char *name,float speed);
void ev_power(EggVehicle *v,const char *name,int on);          /* togglePower */
void ev_remove_joint(EggVehicle *v,const char *joint);         /* removeJoint */
void ev_remove_block(EggVehicle *v,const char *name);          /* removeBlock */
void ev_motor_speed(EggVehicle *v,const char *joint,float speed); /* updateJointDefinition(joint, "motorSpeed", s) */
void ev_abandon(EggVehicle *v);                                /* abandonVehicle */
void ev_surrender(EggVehicle *v);                              /* pig:surrender */
int ev_egg_collected(EggVehicle *v);                           /* a pig of the vehicle holds the egg */
void ev_timer(EggVehicle *v,EggTimed fn,float seconds);        /* doWithTimer */
float ev_closest_egg_x(EggVehicle *v,const char *name);        /* getClosestEggX(name or first pig) */
void ev_auto_balance(EggVehicle *v,const char *a,const char *b,float force); /* toggleAutoBalance */
void ev_gadget(EggVehicle *v,const char *name,const char *what); /* triggerGadgetFunctionality: startSuction, stopSuction, startRocket, inflate */
void ev_gadget_property(EggVehicle *v,const char *name,const char *prop,float value); /* updateGadgetProperties */
float *ev_vars(EggVehicle *v);                                 /* 16 floats for the script's globals */

#endif
