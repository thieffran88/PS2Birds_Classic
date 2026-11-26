#include "game.h"
#include "egg_vehicles.h"
#if defined(PS2BIRDS_JOINT_TEST)
#include "joint_fixture.h"
#elif defined(PS2BIRDS_TRIANGLE_TEST)
#include "triangle_fixture.h"
#elif defined(PS2BIRDS_TNT_TEST)
#include "tnt_fixture.h"
#elif defined(PS2BIRDS_WHITE_TEST)
#include "white_fixture.h"
#elif defined(PS2BIRDS_BLAST_TEST)
#include "blast_fixture.h"
#else
#define PS2BIRDS_RUNTIME_CAMPAIGN 1
#endif
#include <box2d/box2d.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
/* Why the last CAMPAIGN.BIN load failed (shown on the missing-data screen): 0 ok, 1 cannot open, 2 empty or short read,
   3 out of memory, 4 not a CAMPAIGN.BIN of this version, 5 sizes do not match the header, 6 a stage over the limits. */
static int campaign_err;
int game_campaign_error(void){return campaign_err;}
#ifdef PS2BIRDS_RUNTIME_CAMPAIGN
/* The stages are not part of the program: they come from CAMPAIGN.BIN, generated from the
   player's own APK by tools/gerar_iso.py (layout below and in tools/build_campaign.py). */
static const CampaignPolygon no_polygons[1]={{0,0,{0}}};
static const CampaignForce no_forces[1]={{0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
static CampaignLevel *campaign_levels;
static const CampaignPolygon *campaign_polygons=no_polygons;
static const CampaignForce *campaign_forces=no_forces;
static unsigned campaign_count;
/* Original names of the Egg Defender vehicle pieces and joints (getBlock, removeJoint). */
static const CampaignObject *object_base;static const char **object_names;
static const CampaignJoint *joint_base;static const char **joint_names;
static const char *piece_name(const CampaignObject *o){return object_names && o>=object_base?object_names[o-object_base]:"";}
static const char *joint_name(const CampaignJoint *j){return joint_names && j>=joint_base?joint_names[j-joint_base]:"";}
#define CAMPAIGN_COUNT campaign_count
static uint32_t rd32(const unsigned char *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static float rdf(const unsigned char *p){uint32_t v=rd32(p);float f;memcpy(&f,&v,4);return f;}
/* CAMPAIGN.BIN is read in two small pieces (the stage table, then the joints, polygons, forces and strings); the pieces of the stages
   (the biggest part: 14 MB with the Mighty League) stay on the disc and the one the player opens is read when it is loaded. */
static char campaign_file[256];static const unsigned char *campaign_mem;
static unsigned campaign_no,campaign_obj_off,*campaign_first;
static char *campaign_strings;static unsigned campaign_ns;
static CampaignObject *pool;static const char **pool_names;static unsigned pool_cap;static int pool_level=-1;
#define CAMPAIGN_OB (4*17)
static int campaign_read(unsigned off,void *dst,unsigned n)
{
    if(campaign_mem){memcpy(dst,campaign_mem+off,n);return 1;}
    int fd=open(campaign_file,O_RDONLY);if(fd<0)return 0;
    int ok=lseek(fd,(off_t)off,SEEK_SET)==(off_t)off;unsigned done=0;
    while(ok && done<n){unsigned k=n-done>262144?262144:n-done;int r=(int)read(fd,(char*)dst+done,k);if(r<=0)ok=0;else done+=(unsigned)r;}
    close(fd);return ok && done==n;
}
static int campaign_parse(unsigned size)
{
    campaign_err=0;unsigned char h[32];
    if(size<32 || !campaign_read(0,h,32) || memcmp(h,"PBCP",4) || rd32(h+4)!=10){campaign_err=4;return -1;}
    unsigned nl=rd32(h+8),no=rd32(h+12),nj=rd32(h+16),np=rd32(h+20),nf=rd32(h+24),ns=rd32(h+28);
    const unsigned LV=4*(3+16+8+2+1+2+2+1+1+1),OB=CAMPAIGN_OB,JO=4*20,PO=4*18,FO=4*14;
    if(nl==0 || nl>MAX_CAMPAIGN_LEVELS || 32ull+(unsigned long long)nl*LV+(unsigned long long)no*OB+(unsigned long long)nj*JO+np*PO+nf*FO+ns!=size){campaign_err=5;return -1;}
    unsigned obj_off=32+nl*LV,tail_off=obj_off+no*OB,tail=size-tail_off;
    unsigned char *pl=(unsigned char*)malloc(nl*LV),*pj=(unsigned char*)malloc(tail?tail:1);
    char *strings=(char*)malloc(ns+1);CampaignLevel *levels=(CampaignLevel*)calloc(nl,sizeof(CampaignLevel));
    unsigned *first=(unsigned*)calloc(nl,sizeof(unsigned));
    CampaignJoint *joints=(CampaignJoint*)calloc(nj?nj:1,sizeof(CampaignJoint));
    CampaignPolygon *polys=(CampaignPolygon*)calloc(np+1,sizeof(CampaignPolygon));CampaignForce *forces=(CampaignForce*)calloc(nf+1,sizeof(CampaignForce));
    const char **jnames=(const char**)calloc(nj?nj:1,sizeof(char*));
    if(!pl||!pj||!strings||!levels||!first||!joints||!polys||!forces||!jnames||!campaign_read(32,pl,nl*LV)||!campaign_read(tail_off,pj,tail)){
        campaign_err=pl&&pj&&strings&&levels&&first&&joints&&polys&&forces&&jnames?2:3;
        free(pl);free(pj);free(strings);free(levels);free(first);free(joints);free(polys);free(forces);free(jnames);return -1;}
    const unsigned char *pp=pj+nj*JO,*pf=pp+np*PO,*pstr=pf+nf*FO;
    memcpy(strings,pstr,ns);strings[ns]=0;
#define STR(o) (rd32(o)<ns?strings+rd32(o):strings)
    for(unsigned i=0;i<nj;i++){
        const unsigned char *q=pj+i*JO;CampaignJoint &j=joints[i];
        j.type=(int)rd32(q);j.a=(int)rd32(q+4);j.b=(int)rd32(q+8);j.ax=rdf(q+12);j.ay=rdf(q+16);j.bx=rdf(q+20);j.by=rdf(q+24);
        j.frequency=rdf(q+28);j.damping=rdf(q+32);j.break_force=rdf(q+36);j.collide=(int)rd32(q+40);j.destroy_timer=rdf(q+44);
        j.axis_x=rdf(q+48);j.axis_y=rdf(q+52);j.lower=rdf(q+56);j.upper=rdf(q+60);j.motor_speed=rdf(q+64);j.max_force=rdf(q+68);j.flags=(int)rd32(q+72);jnames[i]=STR(q+76);
    }
    for(unsigned i=0;i<np;i++){const unsigned char *q=pp+i*PO;polys[i].definition=STR(q);polys[i].count=(int)rd32(q+4);for(int k=0;k<16;k++)polys[i].v[k]=rdf(q+8+4*k);}
    for(unsigned i=0;i<nf;i++){const unsigned char *q=pf+i*FO;forces[i].definition=STR(q);forces[i].fx=rdf(q+4);forces[i].fy=rdf(q+8);forces[i].target=rdf(q+12);forces[i].blast_radius=rdf(q+16);forces[i].blast_force=rdf(q+20);forces[i].damage_radius=rdf(q+24);forces[i].damage=rdf(q+28);forces[i].vial=(int)rd32(q+32);forces[i].vial_radius=rdf(q+36);forces[i].thrust=rdf(q+40);forces[i].thrust_target=rdf(q+44);forces[i].thrust_interval=rdf(q+48);forces[i].thrust_flags=(int)rd32(q+52);}
    for(unsigned i=0;i<nl;i++){
        const unsigned char *q=pl+i*LV;CampaignLevel &l=levels[i];
        unsigned o0=rd32(q),oc=rd32(q+4),j0,jc;
        if(o0+oc>no||oc>MAX_LEVEL_OBJECTS)goto bad;
        l.objects=0;first[i]=o0;l.count=(int)oc;l.birds=(int)rd32(q+8);if(l.birds<0||l.birds>16)goto bad;
        for(int k=0;k<16;k++)l.bird_types[k]=(int)rd32(q+12+4*k);
        q+=76;l.gravity=rdf(q);l.sling_x=rdf(q+4);l.sling_y=rdf(q+8);l.ground_y=rdf(q+12);l.pixels=rdf(q+16);l.view_x=rdf(q+20);l.view_y=rdf(q+24);l.view_scale=rdf(q+28);
        l.title=STR(q+32);l.script=STR(q+36);l.bank=(int)rd32(q+40);j0=rd32(q+44);jc=rd32(q+48);
        if(j0+jc>nj||jc>MAX_LEVEL_JOINTS)goto bad;
        l.joints=jc?joints+j0:0;l.joint_count=(int)jc;l.star_silver=(int)rd32(q+52);l.star_gold=(int)rd32(q+56);l.powerup=(int)rd32(q+60);l.music=(int)rd32(q+64);l.theme=(int)rd32(q+68);
    }
#undef STR
    free(pl);free(pj);
    campaign_levels=levels;campaign_polygons=polys;campaign_forces=forces;campaign_count=nl;campaign_first=first;campaign_no=no;campaign_obj_off=obj_off;
    campaign_strings=strings;campaign_ns=ns;pool_level=-1;
    joint_base=joints;joint_names=jnames;return (int)nl;
bad:campaign_err=6;free(pl);free(pj);free(strings);free(levels);free(first);free(joints);free(polys);free(forces);free(jnames);return -1;
}
/* The pieces of one stage (a few hundred records) into the pool; the previous stage's are dropped. */
static int campaign_objects(int index)
{
    if(index<0 || index>=(int)campaign_count)return 0;
    CampaignLevel &l=campaign_levels[index];
    if(pool_level==index && l.objects)return 1;
    if(pool_level>=0 && pool_level<(int)campaign_count)campaign_levels[pool_level].objects=0;
    pool_level=-1;unsigned oc=(unsigned)l.count;
    if(oc>pool_cap){free(pool);free(pool_names);pool=(CampaignObject*)calloc(oc,sizeof(CampaignObject));pool_names=(const char**)calloc(oc,sizeof(char*));pool_cap=pool?oc:0;if(!pool||!pool_names)return 0;}
    unsigned char *raw=(unsigned char*)malloc(oc?oc*CAMPAIGN_OB:1);
    if(!raw || !campaign_read(campaign_obj_off+campaign_first[index]*CAMPAIGN_OB,raw,oc*CAMPAIGN_OB)){free(raw);return 0;}
    const unsigned ns=campaign_ns;const char *strings=campaign_strings;
#define STR(o) (rd32(o)<ns?strings+rd32(o):strings)
    for(unsigned i=0;i<oc;i++){
        const unsigned char *q=raw+i*CAMPAIGN_OB;CampaignObject &o=pool[i];
        o.x=rdf(q);o.y=rdf(q+4);o.angle=rdf(q+8);o.width=rdf(q+12);o.height=rdf(q+16);o.density=rdf(q+20);o.strength=rdf(q+24);o.defence=rdf(q+28);
        o.kind=(int)rd32(q+32);o.shape=(int)rd32(q+36);o.definition=STR(q+40);o.friction=rdf(q+44);o.restitution=rdf(q+48);o.spawn_of=(int)rd32(q+52);if(o.spawn_of<0||(unsigned)o.spawn_of>oc)o.spawn_of=0;o.flags=(int)rd32(q+56);pool_names[i]=STR(q+60);o.score=rdf(q+64);
    }
#undef STR
    free(raw);l.objects=pool;object_base=pool;object_names=pool_names;pool_level=index;return 1;
}
int game_campaign_load(const unsigned char *data,unsigned size){campaign_mem=data;return campaign_parse(size);}
int game_campaign_load_file(const char *path)
{
    campaign_err=0;campaign_mem=0;strncpy(campaign_file,path,sizeof(campaign_file)-1);campaign_file[sizeof(campaign_file)-1]=0;
    int fd=open(path,O_RDONLY);if(fd<0){campaign_err=1;return -1;}
    long n=(long)lseek(fd,0,SEEK_END);close(fd);if(n<=0){campaign_err=2;return -1;}
    return campaign_parse((unsigned)n);
}
#if !defined(_EE)
/* Host builds (tests and solvers) read the generated file before main(). */
static struct HostCampaign{HostCampaign(){const char *p=getenv("PS2BIRDS_CAMPAIGN");if(game_campaign_load_file(p?p:"assets/CAMPAIGN.BIN")<0)fprintf(stderr,"CAMPAIGN.BIN missing: generate it from the APK (tools/gerar_iso.py)\n");}} host_campaign;
#endif
#else
static_assert(CAMPAIGN_COUNT<=MAX_CAMPAIGN_LEVELS,"Campaign exceeds progress capacity");
int game_campaign_load(const unsigned char *,unsigned){return (int)CAMPAIGN_COUNT;}
int game_campaign_load_file(const char *){return (int)CAMPAIGN_COUNT;}
static const char *piece_name(const CampaignObject *){return "";}
static const char *joint_name(const CampaignJoint *){return "";}
static int campaign_objects(int index){return index>=0 && index<(int)CAMPAIGN_COUNT;}   /* the stages are compiled in: nothing to load */
#endif

static float clamp(float v,float lo,float hi) { return v<lo?lo:v>hi?hi:v; }
static void popup_at(Game *g,b2Vec2 where,int score,int kind,int material);
/* The red bird's body. The Classic values are 0.75 / 0.25; the Rio's red bird (birds.lua, RedBird) is a
   little bigger and bouncier: radius 0.85, restitution 0.43 (make GAME=rio). */
#ifdef PS2BIRDS_RIO
#define RED_RADIUS 0.85f
#define RED_RESTITUTION 0.43f
#else
#define RED_RADIUS 0.75f
#define RED_RESTITUTION 0.25f
#endif
#define HIT_SOUND_IMPULSE 6.0f   /* normal impulse (kg m/s) above which a block's impact makes its material sound */
/* Sound family of a piece, by definition name: 1 Dino bone, 2 vial or glass pipe, 3 stalaktite (they have their own sounds in the original); 0 the material's. */
static bool name_has(const char *s,const char *part){
    for(;*s;s++){const char *a=s,*b=part;while(*a && *b && tolower((unsigned char)*a)==tolower((unsigned char)*b)){a++;b++;}if(!*b)return true;}
    return false;
}
static int sound_family(const char *def){
    if(!def)return 0;
    if(name_has(def,"bone") && !name_has(def,"deco"))return 1;
    if(name_has(def,"vial") || name_has(def,"glass_pipe"))return 2;
    if(name_has(def,"stalaktite"))return 3;
#ifdef PS2BIRDS_RIO
    if(name_has(def,"monkey"))return 4;   /* the Rio's goal monkeys */
    if(name_has(def,"metal"))return 5;     /* metal boxes */
#else
    if(name_has(def,"cake"))return 4;                                  /* birthday cakes and cupcakes */
    if(name_has(def,"palm_leaf"))return 5;                             /* palm leaves */
    if(name_has(def,"pipe") && !name_has(def,"glass"))return 6;        /* metal pipes (the stove pipe of the Farm) */
#endif
    return 0;
}
/* The event a family's piece makes when it is destroyed / hit hard (0: the material's own). */
static unsigned family_destroyed_event(int fam){
    switch(fam){
        case 1:return GAME_EVENT_BONE;
        case 2:return GAME_EVENT_VIAL;
        case 3:return GAME_EVENT_STALAKTITE;
#ifdef PS2BIRDS_RIO
        case 5:return GAME_EVENT_METAL;
#else
        case 4:return GAME_EVENT_CAKE;
        case 5:return GAME_EVENT_PALM;
        case 6:return GAME_EVENT_PIPE;
#endif
        default:return 0;
    }
}
static unsigned family_hit_event(int fam){
    if(fam==1)return GAME_EVENT_HIT_BONE;
#ifdef PS2BIRDS_RIO
    if(fam==5)return GAME_EVENT_HIT_METAL;
#endif
    return 0;
}
struct Physics;
/* One Egg Defender wave's vehicle and its translated controller (egg_vehicles.cpp). */
struct EggVehicle {
    Physics *p;int wave;const EggController *c;float vars[16];
    EggTimed timer_fn[8];float timer_left[8];bool active,ever_inside;float last_x,last_y;
    const char *balance_a[4],*balance_b[4];float balance_force[4];
    /* startAim: piece, its revolute joint, target (0 egg, 1 bird), angle offset and speed. */
    int aim_piece[4],aim_joint[4],aim_target[4];float aim_offset[4],aim_speed[4],aim_motor[4];bool no_grab;unsigned seed;
};
struct JointListener: b2DestructionListener {
    Physics *owner;
    void SayGoodbye(b2Joint *joint) override;
    void SayGoodbye(b2Fixture *) override {}
};
struct Physics: b2ContactListener {
    JointListener destruction;
    b2World world;
    const CampaignLevel *level;
    b2Body *bodies[MAX_LEVEL_OBJECTS], *bird;
    float health[MAX_LEVEL_OBJECTS], impact[MAX_LEVEL_OBJECTS], age;
    bool fragile_hit[MAX_LEVEL_OBJECTS];
    bool has_launched,special_used,bird_collided;
    bool boomerang_active;float boomerang_x_force,boomerang_y_force,boomerang_spin;
    int forced_type;float blu_timer,blu_clock,blu_dir,blu_fx;bool blu_dir_set;   /* Rio: Blu's star dash */
    int puffer_state;float puffer_timer,puffer_auto,puffer_direction;unsigned puffer_seed;
    float pink_timer,bird_bubble;bool pink_hover;
    int extra_types[16],extra_count,pending_birds,pending_pigs;int trapped_by,current_wave,last_wave;bool egg_mode;float quake_time;bool homing_on,homing_used;bool quake_used,potion_armed,potion_used,shock_bonus,shock_used;float potion_scale;
    float shock_time;b2Vec2 shock_center;bool shock_hit[MAX_LEVEL_OBJECTS];float electrocute[MAX_LEVEL_OBJECTS];float bubble_timer[MAX_LEVEL_OBJECTS],bubble_float[MAX_LEVEL_OBJECTS];float gun_timer[MAX_LEVEL_OBJECTS],gun_bubble_grow[MAX_LEVEL_OBJECTS],gun_bubble_life[MAX_LEVEL_OBJECTS];bool is_gun[MAX_LEVEL_OBJECTS];unsigned gun_seed;float vial_wait[MAX_LEVEL_OBJECTS],vial_pop[MAX_LEVEL_OBJECTS];int vial_kind[MAX_LEVEL_OBJECTS];bool vial_shield[MAX_LEVEL_OBJECTS];
    int bird_type;
    bool bird_hit[MAX_LEVEL_OBJECTS];b2Vec2 bird_pre_velocity;   /* velocityMultiplier: pieces the bird touched this step, its speed before the solver */
    b2Body *split[2],*egg;bool egg_collided;
    bool split_collided[2],disturbed[MAX_LEVEL_OBJECTS];
    bool bomb_requested,detonated;
    float bomb_fuse,blast_life,blast_radius;
    float chain_delay[MAX_LEVEL_OBJECTS];
    b2Vec2 blast_position;
    b2Contact *damage_contacts[MAX_LEVEL_OBJECTS*2];b2Vec2 damage_va[MAX_LEVEL_OBJECTS*2],damage_vb[MAX_LEVEL_OBJECTS*2];int damage_contact_count;unsigned hit_events;
#ifdef PS2BIRDS_RIO
    /* Water (LevelEffectWater.lua, constants read from its bytecode). The surface is the y of the marker block BLOCK_INDESTRUCTIBLE_WATER_LEVEL.
       A body is underwater below it (it leaves when it is 0.05 above); entering sets its linear damping (bird 0.3, puffed Globe bird 8, egg 1.3, any other piece 1.6) and leaving
       resets it to 0; monkeys drown (strength 0). Underwater every step: buoyancy -20 x mass x min(depth, 4) / 4 (the gravity of its mass at depth 4 and more) and, for the
       pieces, a quadratic drag; a body at rest for 2 s gets no more force. Splash sounds after the first 2 s of the stage. */
    bool has_water,bird_under;float water_y;unsigned char under[MAX_LEVEL_OBJECTS];float still[MAX_LEVEL_OBJECTS];
    void water_setup(){
        has_water=false;bird_under=false;water_y=0;memset(under,0,sizeof(under));memset(still,0,sizeof(still));
        for(int i=0;i<level->count;i++)if(!strcmp(level->objects[i].definition,"BLOCK_INDESTRUCTIBLE_WATER_LEVEL")){water_y=level->objects[i].y;has_water=true;break;}
    }
    /* Before the world step: buoyancy and drag on what is underwater. */
    void water_forces(float dt){
        if(!has_water)return;
        for(int i=0;i<level->count;i++){
            b2Body *b=bodies[i];
            if(!b || !under[i] || b->GetType()!=b2_dynamicBody || b->GetMass()<=0)continue;
            const b2Vec2 v=b->GetLinearVelocity();
            if(v.Length()<0.005f){still[i]+=dt;if(still[i]>=2.0f)continue;}else still[i]=0;
            const float depth=fminf(b->GetPosition().y-water_y,4.0f);
            b2Vec2 f(0,-20.0f*b->GetMass()*depth/4.0f);
            const CampaignObject &o=level->objects[i];
            /* a piece (not a bird) is also slowed by the water: 0.5 v^2 x its cross section (port scale 0.1), the push up is dropped when it rises faster than 18 */
            f.x=-(v.x>0?1.0f:v.x<0?-1.0f:0.0f)*0.5f*v.x*v.x*o.height*0.1f;
            if(v.y<-18.0f)f.y=0;else f.y-=(v.y>0?1.0f:v.y<0?-1.0f:0.0f)*0.5f*v.y*v.y*o.width*0.1f;
            b->ApplyForceToCenter(f,true);
        }
        if(bird && bird_under){   /* the bird: buoyancy only (1.1 when it moves up) */
            const float depth=fminf(bird->GetPosition().y-water_y,4.0f);
            const float k=bird->GetLinearVelocity().y<=0?1.1f:1.0f;
            bird->ApplyForceToCenter(b2Vec2(0,-20.0f*bird->GetMass()*depth/4.0f*k),true);
        }
    }
    /* After the world step: crossings (sounds, damping, drowning). */
    void water_check(Game *g){
        if(!has_water)return;
        const bool settling=age<2.0f;   /* the opening drop is silent: only crossings after 2 s make a sound */
        for(int i=0;i<level->count;i++){
            b2Body *b=bodies[i];
            if(!b || b->GetType()!=b2_dynamicBody){under[i]=0;continue;}
            const float y=b->GetPosition().y;
            const bool was=under[i]!=0,u=was?(y>water_y-0.05f):(y>water_y);
            if(u==was)continue;
            under[i]=u;still[i]=0;
            if(!settling)g->events|=u?GAME_EVENT_WATER_OBJ_IN:GAME_EVENT_WATER_OBJ_OUT;
            b->SetLinearDamping(u?1.6f:0.0f);
            if(u && sound_family(level->objects[i].definition)==4 && health[i]>0)health[i]=0;   /* a monkey in the water drowns */
        }
        if(bird){
            const float y=bird->GetPosition().y;
            const bool u=bird_under?(y>water_y-0.05f):(y>water_y);
            if(u!=bird_under){
                if(!settling)g->events|=u?GAME_EVENT_WATER_BIRD_IN:GAME_EVENT_WATER_BIRD_OUT;
                bird->SetLinearDamping(u?(bird_type==BIRD_PUFFER_TYPE?8.0f:0.3f):0.0f);bird_under=u;
            }
        }
    }
#endif
    /* Static scenery the launched bird overlapped at spawn; ignored until they separate. */
    b2Body *spawn_ignore[8];int spawn_ignore_count;
    b2Body *floor_body;
    b2Joint *joints[MAX_LEVEL_JOINTS];float joint_break[MAX_LEVEL_JOINTS];bool joint_swing[MAX_LEVEL_JOINTS];int joint_count;
    float detach_time[MAX_LEVEL_JOINTS];int detach_source[MAX_LEVEL_JOINTS],detach_target[MAX_LEVEL_JOINTS];int detach_count;
    const CampaignForce *force_of[MAX_LEVEL_OBJECTS];
    float thrust[MAX_LEVEL_OBJECTS],thrust_clock[MAX_LEVEL_OBJECTS];bool thrust_off[MAX_LEVEL_OBJECTS];
    /* Egg Defender: runtime forceX/forceY of the vehicle pieces, the pigs' abandoning state
       (EggDefenderPig.lua) and the egg (EggDefenderEggMovement.lua). */
    float wforce_x[MAX_LEVEL_OBJECTS],wforce_y[MAX_LEVEL_OBJECTS],target_v[MAX_LEVEL_OBJECTS],interval_v[MAX_LEVEL_OBJECTS];
    bool abandoned[MAX_LEVEL_OBJECTS],jump_ready[MAX_LEVEL_OBJECTS],pig_no_grab[MAX_LEVEL_OBJECTS];
    float progress_clock[MAX_LEVEL_OBJECTS],progress_hist[MAX_LEVEL_OBJECTS][EGG_PROGRESS_SAMPLES+1];int progress_n[MAX_LEVEL_OBJECTS];
    int egg_i,nest_i,egg_grabber;float egg_tween,egg_home_angle;b2Vec2 egg_from,egg_home;bool egg_lost;
    float game_right;int escaped_pigs;
    /* RedBird_Special (redbird_special.lua) in the Mighty Feathers modes (g_red_bird_special_available):
       while the red bird flies the closest pig is its target; on the special the world slows for
       0.5 s, then the bird flies straight at the target at 75 m/s without gravity until it hits. */
    float cannon_timer[MAX_LEVEL_OBJECTS];int cannon_shots[MAX_LEVEL_OBJECTS];bool cannon_aimed[MAX_LEVEL_OBJECTS];float cannon_clock;
    bool red_mode;int red_target;float red_delay;b2Vec2 red_aim;bool red_fly;int red_targeted_count,red_triggered_count;
    /* Egg Defender gadgets (EggDefenderGadget_*.lua): sucker state (0 idle, 1 starting, 2 sucking)
       and timers of rockets and balloon inflaters. */
    int gadget_state[MAX_LEVEL_OBJECTS];float gadget_run[MAX_LEVEL_OBJECTS],gadget_time[MAX_LEVEL_OBJECTS],gadget_value[MAX_LEVEL_OBJECTS],gadget_value2[MAX_LEVEL_OBJECTS];
    EggVehicle vehicles[16];
    Physics(const CampaignLevel *l):world(b2Vec2(0,l->gravity)),level(l),bird(NULL),age(0),has_launched(false),special_used(false),bird_collided(false),boomerang_active(false),boomerang_x_force(0),boomerang_y_force(0),boomerang_spin(0),puffer_state(0),puffer_timer(0),puffer_auto(-1),puffer_direction(0),puffer_seed(12345u),pink_timer(-1),bird_bubble(0),pink_hover(false),bird_type(BIRD_RED_TYPE) {
        forced_type=-1;blu_timer=-1;blu_clock=0;blu_dir=0;blu_fx=0;blu_dir_set=false;egg=NULL;egg_collided=false;bomb_requested=detonated=false;bomb_fuse=-1;blast_life=0;blast_radius=0;blast_position.SetZero();
        damage_contact_count=0;hit_events=0;
#ifdef PS2BIRDS_RIO
        has_water=false;bird_under=false;water_y=0;memset(under,0,sizeof(under));
#endif
        spawn_ignore_count=0;split[0]=split[1]=NULL;split_collided[0]=split_collided[1]=false;
        memset(chain_delay,0,sizeof(chain_delay));memset(bird_hit,0,sizeof(bird_hit));bird_pre_velocity.SetZero();memset(disturbed,0,sizeof(disturbed));memset(bodies,0,sizeof(bodies)); memset(impact,0,sizeof(impact));memset(force_of,0,sizeof(force_of));memset(thrust,0,sizeof(thrust));memset(thrust_clock,0,sizeof(thrust_clock));memset(thrust_off,0,sizeof(thrust_off));memset(bubble_timer,0,sizeof(bubble_timer));memset(vial_wait,0,sizeof(vial_wait));memset(gun_timer,0,sizeof(gun_timer));memset(gun_bubble_grow,0,sizeof(gun_bubble_grow));memset(gun_bubble_life,0,sizeof(gun_bubble_life));memset(is_gun,0,sizeof(is_gun));gun_seed=12345u;memset(vial_pop,0,sizeof(vial_pop));memset(vial_kind,0,sizeof(vial_kind));memset(vial_shield,0,sizeof(vial_shield));memset(bubble_float,0,sizeof(bubble_float));shock_time=-1;shock_center.SetZero();extra_count=0;pending_birds=0;pending_pigs=0;quake_time=0;homing_on=false;homing_used=false;quake_used=false;trapped_by=-1;current_wave=1;last_wave=1;egg_mode=false;potion_armed=false;potion_used=false;shock_bonus=false;shock_used=false;potion_scale=1;memset(extra_types,0,sizeof(extra_types));memset(shock_hit,0,sizeof(shock_hit));memset(electrocute,0,sizeof(electrocute));
        world.SetContactListener(this);destruction.owner=this;world.SetDestructionListener(&destruction);
        /* The Ground object of the stage data spans the whole level; stages place pieces up to
           x=825, so the floor is FLOOR_HALF_WIDTH wide on each side of the origin. */
        b2BodyDef floor_def; floor_def.position.Set(0,level->ground_y+1);
        b2Body *floor=world.CreateBody(&floor_def);floor_body=floor;joint_count=0;detach_count=0;
        b2PolygonShape shape;shape.SetAsBox(FLOOR_HALF_WIDTH,1);
        /* staticGround material: friction 0.8, restitution 0. */
        b2FixtureDef ground;ground.shape=&shape;ground.friction=0.8f;ground.restitution=0;floor->CreateFixture(&ground);
        for (int i=0;i<level->count;i++){
            const CampaignObject &o=level->objects[i];
            health[i]=o.strength;fragile_hit[i]=false;force_of[i]=NULL;
            for(const CampaignForce *cf=campaign_forces;cf->definition;cf++)if(!strcmp(cf->definition,o.definition)){force_of[i]=cf;break;}
            if(force_of[i]){thrust[i]=force_of[i]->thrust;thrust_off[i]=(force_of[i]->thrust_flags&1)!=0;}
            if(o.spawn_of && (o.flags&OBJECT_GUN_BUBBLE))is_gun[o.spawn_of-1]=true;
            if(o.spawn_of)continue; /* released later by release_spawns() */
            if(OBJECT_WAVE(o.flags))egg_mode=true;
            if(OBJECT_WAVE(o.flags)>1){if((int)OBJECT_WAVE(o.flags)>last_wave)last_wave=OBJECT_WAVE(o.flags);continue;} /* Egg Defender: arrives with its wave */
            create_body(i,o.x,o.y,o.angle);
        }
        for(int i=0;i<level->joint_count && i<MAX_LEVEL_JOINTS;i++)create_joint(level->joints[i]);
#ifdef PS2BIRDS_RIO
        water_setup();
#endif
        egg_setup();
    }
    /* deathSpawn (gamelogic.lua): the pieces stored for a destroyed parent appear where it was. */
    /* Bubble guns (EggDefenderGadget_BubbleGun.lua): shoot a hidden bubble in turn every interval. */
    b2Vec2 muzzle(int gun,int bubble)const{
        const b2Body *b=bodies[gun];float a=b->GetAngle(),fw=level->objects[gun].width*0.47f+level->objects[bubble].width*0.5f,side=level->objects[gun].height*-0.05f;
        return b->GetPosition()+b2Vec2(fw*cosf(a)-side*sinf(a),fw*sinf(a)+side*cosf(a));
    }
    /* EggDefender.lua spawnNextVehicle: the next wave's vehicle comes in when the current wave's
       pigs are all gone (a vehicle without pigs is abandoned). */
    bool is_goal(int i)const{if(!egg_mode)return true;return OBJECT_WAVE(level->objects[i].flags)>0;}
    void update_waves(){
        if(current_wave>=last_wave)return;
        for(int i=0;i<level->count;i++){int w=(int)OBJECT_WAVE(level->objects[i].flags);if(w<1)continue;
            if(w==current_wave && level->objects[i].kind==4 && bodies[i])return;}
        current_wave++;
        for(int i=0;i<level->count;i++)if((int)OBJECT_WAVE(level->objects[i].flags)==current_wave && !bodies[i] && !level->objects[i].spawn_of)
            {create_body(i,level->objects[i].x,level->objects[i].y,level->objects[i].angle);if(bodies[i])vehicle_filter(i);}
        for(int k=0;k<level->joint_count && joint_count<MAX_LEVEL_JOINTS;k++){
            const CampaignJoint &j=level->joints[k];
            bool fresh=(j.a>=0 && (int)OBJECT_WAVE(level->objects[j.a].flags)==current_wave)||(j.b>=0 && (int)OBJECT_WAVE(level->objects[j.b].flags)==current_wave);
            if(fresh)create_joint(j);
        }
        vehicle_start(current_wave);
    }
    /* ---- Egg Defender (phase 2): vehicles, pigs and the egg ---- */
    int piece(int wave,const char *name)const{
        for(int i=0;i<level->count;i++)if((int)OBJECT_WAVE(level->objects[i].flags)==wave && !strcmp(piece_name(&level->objects[i]),name))return i;
        return -1;
    }
    void drop_joints(b2Body *b,b2Body *keep){
        for(int k=0;k<joint_count;k++)if(joints[k]){b2Body *a=joints[k]->GetBodyA(),*c=joints[k]->GetBodyB();
            if((a==b && c!=keep)||(c==b && a!=keep)){world.DestroyJoint(joints[k]);joints[k]=NULL;}}
    }
    b2Joint *add_weld(b2Body *a,b2Body *b){
        if(joint_count>=MAX_LEVEL_JOINTS)return NULL;
        b2WeldJointDef def;def.Initialize(a,b,a->GetPosition());def.collideConnected=false;b2Joint *j=world.CreateJoint(&def);
        joint_break[joint_count]=0;joint_swing[joint_count]=false;joints[joint_count++]=j;return j;
    }
    void set_mask(b2Body *b,uint16 mask){for(b2Fixture *f=b->GetFixtureList();f;f=f->GetNext()){b2Filter fl=f->GetFilterData();fl.maskBits=mask;f->SetFilterData(fl);}}
    void vehicle_start(int wave){
        if(wave<1 || wave>15)return;
        EggVehicle &v=vehicles[wave];memset(&v,0,sizeof(v));v.p=this;v.wave=wave;v.active=true;
        char name[96];snprintf(name,sizeof(name),"%s_W%d_V1",level->script?level->script:"",wave);v.c=egg_controller(name);
        if(v.c && v.c->init)v.c->init(&v);
    }
    void egg_setup(){
        escaped_pigs=0;for(int i=0;i<MAX_LEVEL_OBJECTS;i++){cannon_timer[i]=-1;cannon_shots[i]=CANNON_SHOTS;cannon_aimed[i]=false;}cannon_clock=0;red_mode=level->script && !strncmp(level->script,"EggDefender_",12);red_target=-1;red_targeted_count=red_triggered_count=0;red_delay=-1;red_fly=false;red_aim.SetZero();memset(gadget_state,0,sizeof(gadget_state));memset(gadget_time,0,sizeof(gadget_time));
        for(int i=0;i<MAX_LEVEL_OBJECTS;i++){gadget_run[i]=EGG_ROCKET_RUN;gadget_value[i]=EGG_INFLATED_FORCE_X;gadget_value2[i]=EGG_INFLATED_FORCE_Y;}
        egg_i=nest_i=egg_grabber=-1;egg_tween=-1;egg_lost=false;egg_home_angle=0;egg_home.SetZero();egg_from.SetZero();
        game_right=level->sling_x-EGG_SCREEN_MARGIN+EGG_SCREEN;
        memset(abandoned,0,sizeof(abandoned));memset(pig_no_grab,0,sizeof(pig_no_grab));memset(jump_ready,0,sizeof(jump_ready));memset(progress_clock,0,sizeof(progress_clock));memset(progress_n,0,sizeof(progress_n));
        memset(vehicles,0,sizeof(vehicles));
        for(int i=0;i<level->count;i++){const CampaignForce *f=force_of[i];wforce_x[i]=f?f->fx:0;wforce_y[i]=f?f->fy:0;target_v[i]=f?(f->thrust_target>0?f->thrust_target:f->target):0;interval_v[i]=f?f->thrust_interval:0;
            if(!strncmp(level->objects[i].definition,"BP_BALLOON_INFLATER",19))thrust_off[i]=true;}  /* EggDefenderGadget_BalloonInflater.init: disableForce */
        if(!egg_mode)return;
        for(int i=0;i<level->count;i++)if(!OBJECT_WAVE(level->objects[i].flags) && bodies[i]){
            if(!strcmp(level->objects[i].definition,"BP_EGG") && egg_i<0){egg_i=i;egg_home=bodies[i]->GetPosition();egg_home_angle=bodies[i]->GetAngle();}
            else if(!strcmp(level->objects[i].definition,"BP_NEST") && nest_i<0)nest_i=i;
        }
        if(egg_i>=0)for(b2Fixture *f=bodies[egg_i]->GetFixtureList();f;f=f->GetNext()){b2Filter fl=f->GetFilterData();fl.categoryBits=EGG_CATEGORY;f->SetFilterData(fl);}
        for(int i=0;i<level->count;i++)if(bodies[i] && OBJECT_WAVE(level->objects[i].flags))vehicle_filter(i);
        vehicle_start(1);
    }
    /* collider_types.egg: the egg does not stop the vehicles (they drive up to it to take it). */
    void vehicle_filter(int i){for(b2Fixture *f=bodies[i]->GetFixtureList();f;f=f->GetNext()){b2Filter fl=f->GetFilterData();fl.maskBits&=(uint16)~EGG_CATEGORY;f->SetFilterData(fl);}}
    /* EggDefenderVehicle.getX: the first living pig of the wave, relative to the slingshot. */
    int first_pig(int wave)const{for(int i=0;i<level->count;i++)if((int)OBJECT_WAVE(level->objects[i].flags)==wave && level->objects[i].kind==4 && bodies[i] && !level->objects[i].spawn_of)return i;return -1;}
    /* abandonVehicle (EggDefenderPig): the pig lets go of the vehicle (keeping a grabbed egg),
       straightens up and jumps off once it lands. */
    void abandon(int i){
        if(abandoned[i] || !bodies[i])return;
#ifdef EGG_DEBUG
        fprintf(stderr,"abandon %d at %.1f,%.1f age %.1f\n",i,bodies[i]->GetPosition().x,bodies[i]->GetPosition().y,age);
#endif
        abandoned[i]=true;jump_ready[i]=true;
        drop_joints(bodies[i],egg_grabber==i && egg_i>=0?bodies[egg_i]:NULL);
        bodies[i]->SetTransform(bodies[i]->GetPosition(),0);bodies[i]->SetAngularVelocity(0);
    }
    /* EggDefenderGadget_Sucker: once switched on (startSuction, 1 s to start) it pulls the egg and the
       bird in flight within its range and cone (factor 1 for the egg, 25 for birds), and takes the egg
       when it is closer than 4 m; that counts as the vehicle collecting the egg. */
    void egg_sucker(int s){
        if(gadget_state[s]!=2)return;
        b2Vec2 c=bodies[s]->GetPosition();float back=bodies[s]->GetAngle()+b2_pi,half=SUCKER_CONE*b2_pi/360.0f;
        b2Body *targets[2]={egg_i>=0 && egg_grabber<0 && egg_tween<0?bodies[egg_i]:NULL,trapped_by<0?bird:NULL};
        for(int k=0;k<2;k++){b2Body *b=targets[k];if(!b)continue;
            b2Vec2 d=b->GetPosition()-c;float dist=d.Length();if(dist>SUCKER_RANGE)continue;
            float off=atan2f(d.y,d.x)-back;while(off<-b2_pi)off+=2*b2_pi;while(off>b2_pi)off-=2*b2_pi;
            if(fabsf(off)>=half)continue;
            float force=SUCKER_FORCE*(SUCKER_RANGE-dist)/SUCKER_RANGE*b->GetMass()*(k?EGG_SUCKER_BIRD_FACTOR:1.0f);
            if(force>=SUCKER_MIN_FORCE && dist>0)b->ApplyForceToCenter(-force/dist*d,true);
            if(k==0 && dist<EGG_SUCKER_GRAB){egg_grabber=s;egg_tween=0;egg_from=b->GetPosition();drop_joints(b,NULL);set_mask(b,0);b->SetLinearVelocity(b2Vec2(0,0));}
        }
    }
    /* EggDefenderGadget_Cannon.prepareToShoot: when a bird is launched, a cannon whose path the bird
       will cross within 15 m lights its fuse so that the ball (shootingForce / cannon mass speed)
       meets it there, if that leaves at least 0.4 s. */
    /* First point of the bird's predicted path within 15 m of the cannon: time to it minus the ball's
       flight (|shootingForce| / cannon mass), or a large negative value when there is none. */
    float cannon_lead(int c)const{
        b2Vec2 p0=bird->GetPosition(),v=bird->GetLinearVelocity(),gv(0,level->gravity),at=bodies[c]->GetPosition();
        for(float t=0;t<4;t+=1.0f/30){
            b2Vec2 q=p0+t*v+(0.5f*t*t)*gv;float d=(q-at).Length();
            if(d<=CANNON_PREDICTION_RADIUS)return t-d/(fabsf(CANNON_FORCE)/bodies[c]->GetMass());
        }
        return -1e9f;
    }
    bool is_cannon(int c)const{return bodies[c] && !strcmp(level->objects[c].definition,"BP_CANNON") && cannon_shots[c]>0;}
    void cannons_aim(){  /* prepareToShoot (onBirdShot): the cannon follows this bird; fuse lit if 0.4 s ahead */
        if(!egg_mode || !bird)return;
        cannon_clock=0;
        for(int c=0;c<level->count;c++)if(is_cannon(c) && cannon_timer[c]<0){cannon_aimed[c]=true;float lead=cannon_lead(c);if(lead>CANNON_MIN_LEAD)cannon_timer[c]=lead;}
    }
    /* shoot: a ball leaves the muzzle with an impulse of shootingForce along the cannon and the cannon
       recoils with 0.3 of it; balls leaving the game area are removed (EggDefenderCannonBallManager). */
    void cannons_update(float dt){
        if(!egg_mode)return;
        /* updateAimingPredictionTrajectory: every aim update (5 frames) the path is predicted again from
           the bird as it is and the countdown reset; an idle cannon lights its fuse. */
        if(bird && !bird_collided && (cannon_clock+=dt)>=EGG_AIM_UPDATES/30.0f){
            cannon_clock=0;
            for(int c=0;c<level->count;c++)if(cannon_aimed[c] && is_cannon(c)){float lead=cannon_lead(c);if(lead>-1e8f)cannon_timer[c]=fmaxf(lead,0);}
        }
        for(int c=0;c<level->count;c++){
            if(cannon_timer[c]<0)continue;
            if(!bodies[c]){cannon_timer[c]=-1;continue;}
            if((cannon_timer[c]-=dt)>CANNON_FIRE_AT)continue;
            cannon_timer[c]=-1;cannon_aimed[c]=false;int ball=-1;
            for(int j=0;j<level->count && ball<0;j++)if(level->objects[j].spawn_of==c+1 && (level->objects[j].flags&OBJECT_CANNON_BALL) && !bodies[j])ball=j;
            for(int j=0;j<level->count && ball<0;j++)if(level->objects[j].spawn_of==c+1 && (level->objects[j].flags&OBJECT_CANNON_BALL)){world.DestroyBody(bodies[j]);bodies[j]=NULL;ball=j;}
            if(ball<0)continue;
            float a=bodies[c]->GetAngle();b2Vec2 dir(cosf(a),sinf(a)),at=bodies[c]->GetPosition()-(0.5f*level->objects[c].width)*dir;
            create_body(ball,at.x,at.y,0);if(!bodies[ball])continue;
            for(b2Fixture *f=bodies[ball]->GetFixtureList();f;f=f->GetNext()){b2Filter fl=f->GetFilterData();fl.groupIndex=(int16)(-1-c);f->SetFilterData(fl);}
            for(b2Fixture *f=bodies[c]->GetFixtureList();f;f=f->GetNext()){b2Filter fl=f->GetFilterData();fl.groupIndex=(int16)(-1-c);f->SetFilterData(fl);}
            bodies[ball]->ApplyLinearImpulseToCenter(CANNON_FORCE*dir,true);
            bodies[c]->ApplyLinearImpulseToCenter((-CANNON_FORCE*CANNON_RECOIL)*dir,true);
            cannon_shots[c]--;hit_events|=GAME_EVENT_CANNON;
        }
        for(int j=0;j<level->count;j++)if(bodies[j] && (level->objects[j].flags&OBJECT_CANNON_BALL)){
            b2Vec2 at=bodies[j]->GetPosition();
            if(at.x>game_right+EGG_AREA_MARGIN || at.x<level->sling_x-EGG_SCREEN_MARGIN-EGG_AREA_MARGIN || at.y>level->ground_y+30){world.DestroyBody(bodies[j]);bodies[j]=NULL;}
        }
    }
    void update_suckers(){
        /* EggDefenderGadget_Sucker.onBirdCollision: a bird touching a sucker is trapped inside it
           (Terence cannot be, nor a GlobeBird after its puff); it stays there until the sucker goes. */
        if(trapped_by>=0){
            if(!bird || !bodies[trapped_by])trapped_by=-1;
            else {bird->SetTransform(bodies[trapped_by]->GetPosition(),bird->GetAngle());bird->SetLinearVelocity(b2Vec2(0,0));bird->SetAngularVelocity(0);}
        } else if(bird && bird_type!=BIRD_BIG_TYPE && !(bird_type==BIRD_PUFFER_TYPE && special_used)){
            for(int s=0;s<level->count && trapped_by<0;s++)if(bodies[s] && (level->objects[s].flags&OBJECT_SUCKER) && !(egg_mode && OBJECT_WAVE(level->objects[s].flags) && gadget_state[s]!=2))
                for(b2ContactEdge *e=bodies[s]->GetContactList();e;e=e->next)if(e->other==bird && e->contact->IsTouching()){trapped_by=s;break;}
        }
        for(int s=0;s<level->count;s++){
            if(!bodies[s] || !(level->objects[s].flags&OBJECT_SUCKER))continue;
            if(egg_mode && OBJECT_WAVE(level->objects[s].flags)){egg_sucker(s);continue;}
            b2Vec2 c=bodies[s]->GetPosition();float back=bodies[s]->GetAngle()+b2_pi,half=SUCKER_CONE*b2_pi/360.0f;
            for(int j=0;j<level->count;j++){
                b2Body *b=bodies[j];if(j==s || !b || b->GetType()!=b2_dynamicBody)continue;
                b2Vec2 d=b->GetPosition()-c;float dist=d.Length();if(dist>SUCKER_RANGE)continue;
                float off=atan2f(d.y,d.x)-back;while(off<-b2_pi)off+=2*b2_pi;while(off>b2_pi)off-=2*b2_pi;
                if(fabsf(off)>=half)continue;
                float force=SUCKER_FORCE*(SUCKER_RANGE-dist)/SUCKER_RANGE*b->GetMass()*SUCKER_CLASSIC_FACTOR;
                if(force>=SUCKER_MIN_FORCE && dist>0)b->ApplyForceToCenter(-force/dist*d,true);
                /* RMFClassic_Sucker.onCollision: a pulled piece touching the sucker is destroyed. */
                for(b2ContactEdge *e=bodies[s]->GetContactList();e;e=e->next)if(e->other==b && e->contact->IsTouching()){health[j]=0;break;}
            }
        }
    }
    void update_guns(float dt){
        for(int j=0;j<level->count;j++){
            if(!(level->objects[j].flags&OBJECT_GUN_BUBBLE) || !bodies[j])continue;
            int gun=level->objects[j].spawn_of-1;
            if(gun_bubble_grow[j]>0){
                if(!bodies[gun]){gun_bubble_grow[j]=0;gun_bubble_life[j]=BUBBLE_GUN_LIFE;continue;}
                bodies[j]->SetTransform(muzzle(gun,j),bodies[gun]->GetAngle());bodies[j]->SetLinearVelocity(b2Vec2(0,0));
                if((gun_bubble_grow[j]-=dt)<=0){
                    gun_seed=gun_seed*1103515245u+12345u;float force=BUBBLE_GUN_FORCE_MIN+(BUBBLE_GUN_FORCE_MAX-BUBBLE_GUN_FORCE_MIN)*((gun_seed>>16)&32767)/32767.0f;
                    float a=bodies[gun]->GetAngle();bodies[j]->ApplyLinearImpulseToCenter(b2Vec2(cosf(a)*force,sinf(a)*force),true);gun_bubble_life[j]=BUBBLE_GUN_LIFE;
                }
            } else if(gun_bubble_life[j]>0 && (gun_bubble_life[j]-=dt)<=0){world.DestroyBody(bodies[j]);bodies[j]=NULL;}
        }
        for(int i=0;i<level->count;i++){
            if(!is_gun[i] || !bodies[i] || (gun_timer[i]+=dt)<BUBBLE_GUN_INTERVAL)continue;
            gun_timer[i]=0;
            for(int j=0;j<level->count;j++)if(level->objects[j].spawn_of==i+1 && (level->objects[j].flags&OBJECT_GUN_BUBBLE) && !bodies[j]){
                b2Vec2 at=muzzle(i,j);create_body(j,at.x,at.y,bodies[i]->GetAngle());health[j]=level->objects[j].strength;gun_bubble_grow[j]=BUBBLE_GUN_GROW;break;}
        }
    }
    /* TNT and every piece whose blocks.lua definition declares an explosion (specialty BOMB). */
    bool explosive(int i)const{return level->objects[i].kind==OBJECT_KIND_TNT || (force_of[i] && force_of[i]->blast_radius>0);}
    void release_spawns(int parent,b2Vec2 where){
        /* A broken vial marks the pigs around it (VialEffects.lua blackBird*VialRemoved). */
        const CampaignForce *vf=force_of[parent];
        if(vf && (vf->vial==VIAL_ORANGE || vf->vial==VIAL_PINK))
            for(int j=0;j<level->count;j++)if(bodies[j] && level->objects[j].kind==4 && !vial_kind[j] && (bodies[j]->GetPosition()-where).LengthSquared()<vf->vial_radius*vf->vial_radius){vial_kind[j]=vf->vial;vial_wait[j]=VIAL_DELAY;}
        int caged=OBJECT_RELEASED_BIRD(level->objects[parent].flags);
        if(caged>=0 && extra_count<16){extra_types[extra_count++]=caged;pending_birds++;}
        for(int j=0;j<level->count;j++)if(level->objects[j].spawn_of==parent+1 && !bodies[j] && !(level->objects[j].flags&(OBJECT_GUN_BUBBLE|OBJECT_CANNON_BALL))){
            int source=(int)OBJECT_CLONE_OF(level->objects[j].flags);
            if(source<0){create_body(j,where.x,where.y,level->objects[j].angle);continue;}
            /* Blue vial clone: only for a pig still alive within the vial radius, next to it. */
            if(source>=level->count || !bodies[source] || !vf || vf->vial!=VIAL_BLUE || (bodies[source]->GetPosition()-where).LengthSquared()>=vf->vial_radius*vf->vial_radius)continue;
            b2Vec2 at=bodies[source]->GetPosition();float side=at.x>=where.x?1.0f:-1.0f;
            create_body(j,at.x+side*level->objects[j].width,at.y,0);pending_pigs++;
        }
    }
    void create_body(int i,float x,float y,float angle){
        {
            const CampaignObject &o=level->objects[i];
            b2BodyDef def;def.type=(o.kind==3||IS_FRAGILE_KIND(o.kind)||o.density==0)?b2_staticBody:b2_dynamicBody; /* density 0 is a fixed piece, as in ObjectCreation.lua */def.awake=!SCENE_STARTS_ASLEEP;
            def.position.Set(x,y);def.angle=angle;
            b2Body *body=world.CreateBody(&def);
            body->GetUserData().pointer=i+1;
            b2FixtureDef fixture; fixture.density=o.density;fixture.friction=o.friction;fixture.restitution=o.restitution;
            b2PolygonShape box;b2CircleShape circle;
            if (o.shape==SHAPE_CIRCLE) {circle.m_radius=o.width/2;fixture.shape=&circle;}
            else if(o.shape==SHAPE_TRIANGLE_L || o.shape==SHAPE_TRIANGLE_M){
                const float apex=o.shape==SHAPE_TRIANGLE_L?0.0f:0.5f;
                b2Vec2 v[3]={b2Vec2((apex-0.5f)*o.width,-0.5f*o.height),b2Vec2(0.5f*o.width,0.5f*o.height),b2Vec2(-0.5f*o.width,0.5f*o.height)};
                for(int k=0;k<3;k++){if(o.flags&OBJECT_MIRROR_X)v[k].x=-v[k].x;if(o.flags&OBJECT_MIRROR_Y)v[k].y=-v[k].y;}
                box.Set(v,3);fixture.shape=&box;
            }
            else if(o.shape==SHAPE_POLYGON){
                /* ObjectCreation.lua: vertex = (x*width - width/2, y*height - height/2). */
                const CampaignPolygon *poly=NULL;
                for(const CampaignPolygon *p=campaign_polygons;p->definition;p++)if(!strcmp(p->definition,o.definition)){poly=p;break;}
                if(poly){b2Vec2 v[8];for(int k=0;k<poly->count && k<8;k++){v[k].Set(poly->v[2*k]*o.width-o.width/2,poly->v[2*k+1]*o.height-o.height/2);if(o.flags&OBJECT_MIRROR_X)v[k].x=-v[k].x;if(o.flags&OBJECT_MIRROR_Y)v[k].y=-v[k].y;}box.Set(v,poly->count);}
                else box.SetAsBox(o.width/2,o.height/2);
                fixture.shape=&box;
            }
            else {box.SetAsBox(o.width/2,o.height/2);fixture.shape=&box;}
            fixture.isSensor=(o.flags&OBJECT_NO_COLLISION)!=0; /* blocks.lua collision = false */
            body->CreateFixture(&fixture);bodies[i]=body;
        }
    }
    b2Body *joint_body(int index) const {return index>=0 && index<level->count?bodies[index]:floor_body;}
    void create_joint(const CampaignJoint &j){
        b2Body *a=joint_body(j.a),*b=joint_body(j.b);if(!a || !b || a==b)return;
        b2Vec2 pa(j.ax,j.ay),pb(j.bx,j.by);b2Joint *made=NULL;
        if(j.type==JOINT_DISTANCE){
            b2DistanceJointDef def;def.Initialize(a,b,pa,pb);def.collideConnected=j.collide!=0;
            if(j.frequency>0)b2LinearStiffness(def.stiffness,def.damping,j.frequency,j.damping,a,b);
            made=world.CreateJoint(&def);
        }else if(j.type==JOINT_WELD || j.type==JOINT_DESTROY_ATTACHED){
            /* Like b2WeldJointDef::Initialize: one anchor (end1's) keeps the bodies' current relative pose. */
            b2WeldJointDef def;def.Initialize(a,b,pa);def.collideConnected=j.collide!=0;
            if(j.frequency>0)b2AngularStiffness(def.stiffness,def.damping,j.frequency,j.damping,a,b);
            made=world.CreateJoint(&def);
        }else if(j.type==JOINT_REVOLUTE){
            b2RevoluteJointDef def;def.Initialize(a,b,pa);def.collideConnected=j.collide!=0;
            def.enableLimit=(j.flags&JOINT_LIMIT)!=0;def.lowerAngle=j.lower;def.upperAngle=j.upper;
            def.enableMotor=(j.flags&JOINT_MOTOR)!=0;def.motorSpeed=j.motor_speed;def.maxMotorTorque=j.max_force;
            made=world.CreateJoint(&def);
        }else if(j.type==JOINT_PRISMATIC){
            b2Vec2 axis(j.axis_x,j.axis_y);if(axis.Normalize()<1e-6f)axis.Set(1,0);
            b2PrismaticJointDef def;def.Initialize(a,b,pa,axis);def.collideConnected=j.collide!=0;
            def.enableLimit=(j.flags&JOINT_LIMIT)!=0;def.lowerTranslation=j.lower;def.upperTranslation=j.upper;
            def.enableMotor=(j.flags&JOINT_MOTOR)!=0;def.motorSpeed=j.motor_speed;def.maxMotorForce=j.max_force;
            made=world.CreateJoint(&def);
        }
        if(made){
            /* Port approximation: a breakable joint fails when its reaction force exceeds
               breakForce x (mass of both bodies) x gravity. */
            joint_break[joint_count]=j.break_force>0?j.break_force*(a->GetMass()+b->GetMass())*fabsf(level->gravity):0;
            joint_swing[joint_count]=(j.flags&JOINT_BACK_AND_FORTH) && (j.flags&JOINT_MOTOR) && (j.flags&JOINT_LIMIT);
            joints[joint_count++]=made;
            /* Port approximation of the native "Destroy-attached" joint: it holds like a weld;
               when end1 is destroyed, end2 is removed destroy_timer seconds later. */
            if(j.type==JOINT_DESTROY_ATTACHED && j.a>=0 && j.b>=0 && detach_count<MAX_LEVEL_JOINTS){detach_time[detach_count]=j.destroy_timer;detach_source[detach_count]=j.a;detach_target[detach_count++]=j.b;}
        }
    }
    void forget_joint(b2Joint *joint){for(int i=0;i<joint_count;i++)if(joints[i]==joint)joints[i]=NULL;}
    void break_joints(float inv_dt){
        for(int i=0;i<joint_count;i++)if(joints[i] && joint_break[i]>0 && joints[i]->GetReactionForce(inv_dt).Length()>joint_break[i]){
            world.DestroyJoint(joints[i]);joints[i]=NULL;
        }
    }
    b2Body *make_bird(const b2Vec2 &position,const b2Vec2 &velocity,int type){
        b2BodyDef def;def.type=b2_dynamicBody;def.bullet=true;def.position=position;
        b2Body *body=world.CreateBody(&def);
        /* Radii, densities and restitutions consulted in the recovered blocks.lua. */
        b2CircleShape circle;circle.m_radius=type==BIRD_PINK_TYPE?PINK_RADIUS:type==BIRD_PUFFER_TYPE?GLOBE_RADIUS:type==BIRD_BIG_TYPE?BIG_RADIUS:type==BIRD_GREEN_TYPE?GREEN_RADIUS:type==BIRD_BLUE_TYPE?0.6f:type==BIRD_YELLOW_TYPE?0.8f:(type==BIRD_BLACK_TYPE||type==BIRD_SHOCK_TYPE)?1.0f:type==BIRD_WHITE_TYPE?1.3f:type==BIRD_EGG_TYPE?WHITE_EGG_RADIUS:RED_RADIUS;
        b2FixtureDef fixture;fixture.shape=&circle;fixture.density=type==BIRD_PINK_TYPE?PINK_DENSITY:type==BIRD_PUFFER_TYPE?GLOBE_DENSITY:type==BIRD_BIG_TYPE?BIG_DENSITY:type==BIRD_GREEN_TYPE?GREEN_DENSITY:(type==BIRD_BLUE_TYPE || type==BIRD_WHITE_TYPE)?4.5f:type==BIRD_EGG_TYPE?3:6;
        fixture.friction=0.3f;fixture.restitution=type==BIRD_PINK_TYPE?PINK_RESTITUTION:type==BIRD_PUFFER_TYPE?GLOBE_RESTITUTION:type==BIRD_BIG_TYPE?BIG_RESTITUTION:type==BIRD_GREEN_TYPE?GREEN_RESTITUTION:(type==BIRD_YELLOW_TYPE || type==BIRD_WHITE_TYPE || type==BIRD_EGG_TYPE)?0.23f:(type==BIRD_BLACK_TYPE||type==BIRD_SHOCK_TYPE)?0.03f:type==BIRD_RED_TYPE?RED_RESTITUTION:0.25f;
        if(type==BIRD_BLUE_TYPE)fixture.filter.groupIndex=-1;
        if(type==BIRD_WHITE_TYPE || type==BIRD_EGG_TYPE)fixture.filter.groupIndex=-2;
        if(type==BIRD_BLU_TYPE){circle.m_radius=BLU_RADIUS;fixture.density=BLU_DENSITY;fixture.restitution=BLU_RESTITUTION;fixture.friction=BLU_FRICTION;}
        else if(type==BIRD_JEWEL_TYPE){circle.m_radius=GREEN_RADIUS;fixture.density=GREEN_DENSITY;fixture.restitution=GREEN_RESTITUTION;}
        body->CreateFixture(&fixture);body->SetLinearVelocity(velocity);return body;
    }
    void PreSolve(b2Contact *contact,const b2Manifold *) override {
        if(spawn_ignored(contact)>=0){contact->SetEnabled(false);return;}
        if(!has_launched || age<0.75f)return;
        b2WorldManifold manifold;contact->GetWorldManifold(&manifold);
        b2Body *a=contact->GetFixtureA()->GetBody(),*b=contact->GetFixtureB()->GetBody();
        b2Body *pair[]={a,b};bool activated=false;
        for(b2Body *body:pair){
            uintptr_t id=body->GetUserData().pointer;
            if(body==bird || body==egg || body==split[0] || body==split[1] || (id>0 && id<=(uintptr_t)level->count && disturbed[id-1]))activated=true;
        }
        if(!activated)return;
        float approach=0;
        for(int i=0;i<contact->GetManifold()->pointCount;i++){
            b2Vec2 relative=b->GetLinearVelocityFromWorldPoint(manifold.points[i])-a->GetLinearVelocityFromWorldPoint(manifold.points[i]);
            approach=fmaxf(approach,-b2Dot(relative,manifold.normal));
        }
        /* Resting support force is not an impact. Keep only contacts that
           approached faster than 0.5 physics units/s before the solver. */
        if(approach>0.5f){
            if(bird && (a==bird || b==bird)){
                uintptr_t id=(a==bird?b:a)->GetUserData().pointer;
                if(id>0 && id<=(uintptr_t)level->count){bird_hit[id-1]=true;bird_pre_velocity=bird->GetLinearVelocity();}
            }
            for(b2Body *body:pair){
                uintptr_t id=body->GetUserData().pointer;
                if(id>0 && id<=(uintptr_t)level->count && body->GetType()==b2_dynamicBody)disturbed[id-1]=true;
            }
            if(damage_contact_count<MAX_LEVEL_OBJECTS*2){damage_va[damage_contact_count]=a->GetLinearVelocity();damage_vb[damage_contact_count]=b->GetLinearVelocity();damage_contacts[damage_contact_count++]=contact;}
        }
    }
    /* Scenery hulls are box approximations, so a slingshot placed on a slope can
       overlap the spawned bird. Those static bodies are recorded at launch and
       their contacts stay disabled until the bird separates from them. */
    void record_spawn_overlaps(){
        spawn_ignore_count=0;b2Fixture *mine=bird->GetFixtureList();
        for(int i=0;i<level->count && spawn_ignore_count<8;i++){
            b2Body *body=bodies[i];if(!body || body->GetType()!=b2_staticBody)continue;
            for(b2Fixture *f=body->GetFixtureList();f;f=f->GetNext())
                if(b2TestOverlap(f->GetShape(),0,mine->GetShape(),0,body->GetTransform(),bird->GetTransform())){spawn_ignore[spawn_ignore_count++]=body;break;}
        }
    }
    int spawn_ignored(b2Contact *contact) const {
        b2Body *a=contact->GetFixtureA()->GetBody(),*b=contact->GetFixtureB()->GetBody();
        if(!bird || (a!=bird && b!=bird))return -1;
        b2Body *other=a==bird?b:a;
        for(int i=0;i<spawn_ignore_count;i++)if(spawn_ignore[i]==other)return i;
        return -1;
    }
    void EndContact(b2Contact *contact) override {
        int i=spawn_ignored(contact);
        if(i>=0)spawn_ignore[i]=spawn_ignore[--spawn_ignore_count];
    }
    bool counts_as_hit(b2Contact *contact,b2Body *projectile) const {
        b2Body *a=contact->GetFixtureA()->GetBody(),*b=contact->GetFixtureB()->GetBody();
        return a==projectile || b==projectile;
    }
    void PostSolve(b2Contact *contact,const b2ContactImpulse *impulse) override {
        if(counts_as_hit(contact,bird))bird_collided=true;
        if(egg && counts_as_hit(contact,egg))egg_collided=true;
        for(int i=0;i<2;i++)if(split[i] && counts_as_hit(contact,split[i]))split_collided[i]=true;
        if (!has_launched || age<0.75f) return;
        /* staticFragile group (clouds material: strength 0.1, defence 1): a static piece
           that pops on the first impact from a moving body. */
        {
            b2Body *pair[]={contact->GetFixtureA()->GetBody(),contact->GetFixtureB()->GetBody()};
            for(int k=0;k<2;k++){
                uintptr_t id=pair[k]->GetUserData().pointer;b2Body *other=pair[1-k];
                if(id>0 && id<=(uintptr_t)level->count && IS_FRAGILE_KIND(level->objects[id-1].kind) && other->GetType()==b2_dynamicBody && impulse->normalImpulses[0]>0)
                    fragile_hit[id-1]=true;
            }
        }
        int dk=-1;for(int i=0;i<damage_contact_count;i++)if(damage_contacts[i]==contact){dk=i;break;}
        if(dk<0)return;
        float normal=0;for(int i=0;i<impulse->count;i++)normal+=impulse->normalImpulses[i];
        b2Body *pair[]={contact->GetFixtureA()->GetBody(),contact->GetFixtureB()->GetBody()};
        if(normal>HIT_SOUND_IMPULSE)for(b2Body *body:pair){   /* the materials' collision sound */
            uintptr_t hid=body->GetUserData().pointer;
            if(hid>0 && hid<=(uintptr_t)level->count){const int k=level->objects[hid-1].kind;{const unsigned fh=family_hit_event(sound_family(level->objects[hid-1].definition));hit_events|=fh?fh:(unsigned)(k==1?GAME_EVENT_HIT_WOOD:k==2?GAME_EVENT_HIT_GLASS:k==5?GAME_EVENT_HIT_STONE:k==4?GAME_EVENT_HIT_PIG:0);}}
        }
        for (b2Body *body:pair) {
            uintptr_t id=body->GetUserData().pointer;
            if (id>0 && id<=(uintptr_t)level->count && body->GetMass()>0) {
                unsigned i=(unsigned)id-1;
                b2Body *other=pair[0]==body?pair[1]:pair[0];
                bool projectile=other==bird || other==egg || other==split[0] || other==split[1];
                /* Direct-hit multipliers from the recovered damage tables: blue vs glass 2.9, yellow vs wood 2.4. */
                float factor=1.0f;
                if(projectile && bird_type==BIRD_BLUE_TYPE && level->objects[i].kind==2)factor=2.9f;
                if(projectile && bird_type==BIRD_YELLOW_TYPE && level->objects[i].kind==1)factor=2.4f;
                if(projectile && (bird_type==BIRD_BLACK_TYPE||bird_type==BIRD_SHOCK_TYPE) && level->objects[i].kind==5)factor=4.0f;
                /* BoomerangBirdDamageFactors: wood 3, rock 1.5, light 3. */
                /* RedBigBirdDamageFactors: wood, rock and light 0.3. */
                if(projectile && bird_type==BIRD_BIG_TYPE && level->objects[i].kind!=4)factor=BIG_DAMAGE_FACTOR;
                /* PinkBirdDamageFactors: wood, rock and light 0.5. */
                if(projectile && bird_type==BIRD_PINK_TYPE && level->objects[i].kind!=4)factor=PINK_DAMAGE_FACTOR;
                /* GlobeBirdDamageFactors: wood 3, rock 1.5, light 3; GlobeBirdBigDamageFactors (puffed): 0. */
                if(projectile && bird_type==BIRD_PUFFER_TYPE && level->objects[i].kind!=4)factor=puffer_state>0?0.0f:level->objects[i].kind==1||level->objects[i].kind==2?3.0f:level->objects[i].kind==5?1.5f:1.0f;
                if(projectile && bird_type==BIRD_GREEN_TYPE)factor=level->objects[i].kind==1||level->objects[i].kind==2?3.0f:level->objects[i].kind==5?1.5f:1.0f;
#ifndef PS2BIRDS_EXACT_COLLISION
                impact[i]=fmaxf(impact[i],normal/body->GetMass()*factor);
#else
                /* EXPERIMENTAL (build with -DPS2BIRDS_EXACT_COLLISION; off by default, see docs/LACUNAS_PORT.md): AdventureModeCollisions.lua, with the velocities before the solver (the native passes them to onCollision, which can disable
                   the contact) in world units (physics x physicsToWorld 20), divided by 10:
                   bird x piece:  |vBird - vPiece| / 10 * damageMultiplier(material) - defence
                   piece x piece: |vA - vB| * massA * damageMultiplier(A, default 1) / 10 - defence of the piece hit
                   piece x static (ground, fixed blocks): |v| * damageMultiplier(ground, default 20) / 10 - defence. */
                {(void)normal;
                 b2Vec2 va=damage_va[dk],vb=damage_vb[dk];const bool body_is_a=pair[0]==body;
                 float rel=(va-vb).Length(),speed=(body_is_a?va:vb).Length();
                 float dmg;
                 if(projectile)dmg=rel*EXACT_WORLD_SCALE/EXACT_DAMAGE_DIVISOR*factor;
                 else if(other->GetType()==b2_dynamicBody)dmg=rel*other->GetMass()*EXACT_WORLD_SCALE/EXACT_DAMAGE_DIVISOR;
                 else dmg=speed*EXACT_WORLD_SCALE*EXACT_GROUND_MULTIPLIER/EXACT_DAMAGE_DIVISOR;
                 dmg-=level->objects[i].defence;
                 if(dmg>0)impact[i]+=dmg;}
#endif
            }
        }
    }
};
/* velocityMultiplier of blocks.lua for the port's materials (1 wood, 2 glass = light, 5 rock):
   BlueBird light 1.9; RedBigBird 1.8; BoomerangBird wood/light 1.3; GlobeBird 4 (puffed 2). */
static float velocity_multiplier(int type,int kind,bool puffed){
    if(kind!=1 && kind!=2 && kind!=5)return 1.0f;
    switch(type){
        case BIRD_BLUE_TYPE:return kind==2?1.9f:1.0f;
        case BIRD_BIG_TYPE:return 1.8f;
        case BIRD_GREEN_TYPE:return kind==5?1.0f:1.3f;
        case BIRD_PUFFER_TYPE:return puffed?2.0f:4.0f;
        default:return 1.0f;
    }
}
void JointListener::SayGoodbye(b2Joint *joint){owner->forget_joint(joint);}
void game_init(Game *g) { memset(g,0,sizeof(*g));g->unlocked=1;game_load_level(g,0); }
void game_destroy(Game *g) { delete static_cast<Physics*>(g->physics);g->physics=NULL; }
int game_level_count(void){return (int)CAMPAIGN_COUNT;}
const char *game_level_title(int index){return index>=0 && index<(int)CAMPAIGN_COUNT?campaign_levels[index].title:"";}
float game_level_gravity(int index){return index<0||index>=(int)CAMPAIGN_COUNT?0.0f:campaign_levels[index].gravity;}
int game_level_stars(int index,int score){
    if(index<0 || index>=(int)CAMPAIGN_COUNT || score<=0)return 0;
    const CampaignLevel &l=campaign_levels[index];
    return score>=l.star_gold?3:score>=l.star_silver?2:1;
}
int game_level_theme(int index){return index>=0 && index<(int)CAMPAIGN_COUNT?campaign_levels[index].theme:-1;}
int game_music_override(const Game *g){
    if(!g->physics)return -1;
    const Physics *p=static_cast<const Physics*>(g->physics);
    if(!p->egg_mode)return -1;
    const int w=p->current_wave<1?1:p->current_wave>5?5:p->current_wave;
    return 16+w-1;   /* MUSIC_EGG */
}
int game_level_music(int index){return index>=0 && index<(int)CAMPAIGN_COUNT?campaign_levels[index].music:-1;}
int game_level_silver(int index){return index>=0 && index<(int)CAMPAIGN_COUNT?campaign_levels[index].star_silver:0;}
int game_level_bank(int index){return index>=0 && index<(int)CAMPAIGN_COUNT?campaign_levels[index].bank:0;}
const char *game_level_script(int index){return index>=0 && index<(int)CAMPAIGN_COUNT?campaign_levels[index].script:"";}
int game_load_level(Game *g,int index) {
    if(index<0 || index>=(int)CAMPAIGN_COUNT || index>=g->unlocked)return 0;
    int unlocked=g->unlocked,shop_mask=g->shop_mask,shop_bird=g->shop_bird,best[MAX_CAMPAIGN_LEVELS];memcpy(best,g->highscores,sizeof(best));
    game_destroy(g);memset(g,0,sizeof(*g));g->level=index;g->unlocked=unlocked;g->shop_mask=shop_mask;g->shop_bird=shop_bird;memcpy(g->highscores,best,sizeof(best));
    if(!campaign_objects(index))return 0;
    const CampaignLevel *l=&campaign_levels[index];g->physics=new Physics(l);
    g->view_x=l->view_x;g->view_y=l->view_y;g->view_scale=l->view_scale;g->pixels=l->pixels;
    g->ground_y=l->view_y+l->ground_y*l->view_scale;g->gravity=l->gravity;
    g->sling_x=l->view_x+l->sling_x*l->view_scale;g->sling_y=l->view_y+l->sling_y*l->view_scale;
    g->x=g->sling_x;g->y=g->sling_y;g->angle=30;g->power=75;g->birds=l->birds;g->state=AIMING;
    {Physics *p=static_cast<Physics*>(g->physics);
     /* Powerup_ExtraBird.lua / ExtraBirdManager.lua: the extra bird waits at the end of the queue. */
     if((g->shop_mask&SHOP_EXTRA_BIRD) && l->birds>0 && p->extra_count<16){p->extra_types[p->extra_count++]=g->shop_bird;g->birds++;}}
    /* Camera: the stage's right edge (the Classic stages are fitted to the screen, so they never scroll). */
    g->base_view_x=l->view_x;g->base_sling_x=g->sling_x;g->cam_x=0;g->cam_max=0;
#ifdef PS2BIRDS_RIO   /* only the Rio build: the Classic stages (Egg Defender included) keep their fixed camera */
    {float right=-1e9f;for(int i=0;i<l->count;i++){const CampaignObject *o=&l->objects[i];float r=(o->x+fmaxf(o->width,o->height)*0.5f)*l->view_scale+l->view_x;if(r>right)right=r;}
     if(right-608>8)g->cam_max=right-608;}
#endif
    bool waved=false;for(int i=0;i<l->count;i++)if(OBJECT_WAVE(l->objects[i].flags))waved=true;
    /* Egg Defender: only the vehicles' pigs are goals (scene pigs, e.g. the opening scene's king, are not). */
    for(int i=0;i<l->count;i++){if(l->objects[i].kind==4 && !l->objects[i].spawn_of && (!waved || OBJECT_WAVE(l->objects[i].flags))){g->pig_alive++;if(sound_family(l->objects[i].definition)==4)g->monkey_goals++;}if(l->objects[i].kind!=3 && l->objects[i].kind!=4 && !(l->objects[i].flags&(OBJECT_GUN_BUBBLE|OBJECT_CANNON_BALL)))g->block_alive++;}
    return 1;
}
void game_reset(Game *g){game_load_level(g,g->level);}
int game_next_level(Game *g){return g->state==WON?game_load_level(g,g->level+1):0;}
void game_aim(Game *g,float a,float p) {
    if(g->state!=AIMING)return;
    /* Callers pass deltas sized for the earlier 260-wide power scale; keep the same feel. */
    g->angle=clamp(g->angle+a,ANGLE_MIN,ANGLE_MAX);g->power=clamp(g->power+p*(POWER_MAX-POWER_MIN)/260.0f,POWER_MIN,POWER_MAX);
}
/* Powerup_KingSling.lua: the slingshot's power goes from 1 to 1.3. */
static float sling_power(const Physics *p){return p->level->powerup==POWERUP_KING_SLING?KING_SLING_POWER:1.0f;}
void game_launch(Game *g) {
    if(g->state!=AIMING || g->birds<=0)return;
    Physics *p=static_cast<Physics*>(g->physics);
    if(p->bird)p->world.DestroyBody(p->bird);
    if(p->egg)p->world.DestroyBody(p->egg);
    p->egg=NULL;p->egg_collided=false;
    p->spawn_ignore_count=0;
    for(int i=0;i<2;i++){if(p->split[i])p->world.DestroyBody(p->split[i]);p->split[i]=NULL;p->split_collided[i]=false;}
    p->bird_type=game_queue_type(g,0);p->homing_on=false;p->red_delay=-1;p->red_fly=false;p->red_target=-1;p->shock_bonus=false;p->trapped_by=-1;p->special_used=false;p->bird_collided=false;p->boomerang_active=false;p->boomerang_spin=0;p->blu_timer=-1;p->blu_clock=0;p->blu_dir_set=false;p->puffer_state=0;p->puffer_timer=0;p->puffer_auto=-1;p->puffer_direction=0;p->puffer_seed=12345u;p->pink_timer=-1;p->bird_bubble=0;p->pink_hover=false;
    g->angle=clamp(g->angle,ANGLE_MIN,ANGLE_MAX);g->power=clamp(g->power,POWER_MIN,POWER_MAX);
    float a=g->angle*0.01745329252f;
    p->bird=p->make_bird(b2Vec2(p->level->sling_x,p->level->sling_y),b2Vec2(cosf(a)*LAUNCH_SPEED_MAX*sling_power(p)*g->power/100.0f,-sinf(a)*LAUNCH_SPEED_MAX*sling_power(p)*g->power/100.0f),p->bird_type);
    p->potion_scale=1;
    if(p->potion_armed && p->bird_type>=0 && p->bird_type<=BIRD_SHOCK_TYPE){
        /* Power Potion: rebuild the bird's circle bigger and denser. */
        static const float radius_factor[]=POWER_POTION_RADIUS,density_factor[]=POWER_POTION_DENSITY;
        b2Fixture *f=p->bird->GetFixtureList();b2CircleShape circle=*static_cast<b2CircleShape*>(f->GetShape());
        b2FixtureDef def;def.shape=&circle;def.density=f->GetDensity()*density_factor[p->bird_type];def.friction=f->GetFriction();def.restitution=f->GetRestitution();def.filter=f->GetFilterData();
        circle.m_radius*=radius_factor[p->bird_type];b2Vec2 v=p->bird->GetLinearVelocity();
        p->bird->DestroyFixture(f);p->bird->CreateFixture(&def);p->bird->SetLinearVelocity(v);
        p->potion_scale=radius_factor[p->bird_type];p->potion_armed=false;
    }
    p->record_spawn_overlaps();
    p->bomb_requested=p->detonated=false;p->bomb_fuse=-1;
    p->has_launched=true;g->birds--;g->flight_time=g->cooldown=0;g->state=FLYING;
    p->cannons_aim();
}
int game_queue_type(const Game *g,int offset){
    const Physics *p=static_cast<const Physics*>(g->physics);
    if(offset<0 || offset>=g->birds)return -1;
    /* Released cage birds join the end of the queue. */
    /* Powerup_Shockwave.lua: the bird on the slingshot becomes a shockwave bird. */
    if(offset==0 && p->shock_bonus)return BIRD_SHOCK_TYPE;
    if(offset==0 && p->forced_type>=0)return p->forced_type;   /* host tests */
    int index=p->level->birds+p->extra_count-p->pending_birds-g->birds+offset;
    return index<p->level->birds?p->level->bird_types[index]:p->extra_types[index-p->level->birds];
}
void game_force_bird_type(Game *g,int type){static_cast<Physics*>(g->physics)->forced_type=type;}
int game_special_available(const Game *g){
    const Physics *p=static_cast<const Physics*>(g->physics);
    if(g->state!=FLYING || p->special_used || !p->bird)return 0;
    if(p->bird_type==BIRD_BLACK_TYPE || p->bird_type==BIRD_SHOCK_TYPE)return 1;
    if(p->bird_type==BIRD_PUFFER_TYPE)return 1; /* specialtyAvailableAfterCollision */
    if(p->bird_type==BIRD_PINK_TYPE)return !p->special_used && p->pink_timer<0;
    if(p->bird_type==BIRD_RED_TYPE && p->red_mode)return !p->bird_collided;
    return (p->bird_type==BIRD_BLUE_TYPE || p->bird_type==BIRD_YELLOW_TYPE || p->bird_type==BIRD_WHITE_TYPE || p->bird_type==BIRD_GREEN_TYPE || p->bird_type==BIRD_BLU_TYPE || p->bird_type==BIRD_JEWEL_TYPE) && !p->bird_collided;
}
static void radial_blast(Game *g,const b2Vec2 &position,float radius,float force,float damage_radius,float damage);
static void set_puffer_radius(Physics *p,float radius,bool puffed){
    b2Fixture *f=p->bird->GetFixtureList();if(!f)return;
    b2CircleShape *c=static_cast<b2CircleShape*>(f->GetShape());c->m_radius=radius>0.05f?radius:0.05f;
    if(puffed){f->SetFriction(GLOBE_PUFFED_FRICTION);f->SetRestitution(GLOBE_PUFFED_RESTITUTION);f->SetDensity(GLOBE_PUFFED_DENSITY);}
    p->bird->ResetMassData();
}
static void puff(Game *g){
    Physics *p=static_cast<Physics*>(g->physics);
    if(!p->bird || p->puffer_state)return;
    set_puffer_radius(p,GLOBE_PUFFED_RADIUS,true);
    radial_blast(g,p->bird->GetPosition(),GLOBE_BLAST_RADIUS,GLOBE_BLAST_FORCE,GLOBE_DAMAGE_RADIUS,0);
    p->puffer_state=1;p->puffer_timer=GLOBE_PUFF_TIME;p->special_used=true;g->cooldown=0;
}
int game_special(Game *g){
    if(!game_special_available(g))return 0;
    Physics *p=static_cast<Physics*>(g->physics);
    /* RMFClassic_Sucker: inside a sucker Stella's bubbles are disabled; the GlobeBird's puff and the
       shockwave bird's blast destroy the sucker (specialtyDestroysSucker) and free the bird. */
    if(p->trapped_by>=0){
        if(p->bird_type==BIRD_PINK_TYPE)return 0;
        if(p->bird_type==BIRD_PUFFER_TYPE || p->bird_type==BIRD_SHOCK_TYPE){p->health[p->trapped_by]=0;p->trapped_by=-1;}
    }
    if(p->bird_type==BIRD_RED_TYPE && p->red_mode){
        p->special_used=true;p->red_delay=RED_SPECIAL_DELAY;
        if(p->red_target>=0 && p->bodies[p->red_target])p->red_aim=p->bodies[p->red_target]->GetPosition();
        else{b2Vec2 v=p->bird->GetLinearVelocity();if(v.Normalize()<0.01f)v.Set(1,0);p->red_aim=p->bird->GetPosition()+10*v;}
        return 1;
    }
    if(p->bird_type==BIRD_BLACK_TYPE || p->bird_type==BIRD_SHOCK_TYPE){p->bomb_requested=true;p->special_used=true;return 1;}
    b2Vec2 velocity=p->bird->GetLinearVelocity();
    if(p->bird_type==BIRD_WHITE_TYPE){
        p->egg=p->make_bird(p->bird->GetPosition()+b2Vec2(0,1.3f+WHITE_EGG_RADIUS+0.1f),b2Vec2(0,WHITE_DROP_SPEED),BIRD_EGG_TYPE);
        p->bird->SetLinearVelocity(b2Vec2(velocity.x*WHITE_RECOIL_X_FACTOR,-WHITE_RECOIL_SPEED));
        p->special_used=true;p->egg_collided=false;g->cooldown=0;return 1;
    }
    if(p->bird_type==BIRD_PUFFER_TYPE){puff(g);return 1;}
    if(p->bird_type==BIRD_PINK_TYPE){p->pink_timer=PINK_BOOM_TIME;p->special_used=true;g->cooldown=0;return 1;}
    if(p->bird_type==BIRD_BLU_TYPE){p->blu_timer=BLU_DURATION;p->blu_clock=BLU_INTERVAL;p->blu_dir_set=false;p->special_used=true;g->cooldown=0;return 1;}
    if(p->bird_type==BIRD_GREEN_TYPE || p->bird_type==BIRD_JEWEL_TYPE){
        bool jewel=p->bird_type==BIRD_JEWEL_TYPE;
        float mass=p->bird->GetMass(),coeff=velocity.x!=0?2.0f-fminf(fabsf(velocity.y/velocity.x),2.0f):0.0f;
        p->boomerang_x_force=velocity.x*GREEN_PHYSICS_SCALE*mass*(jewel?JEWEL_HORIZONTAL_FORCE:GREEN_HORIZONTAL_FORCE);
        p->boomerang_y_force=coeff*GREEN_PHYSICS_SCALE*mass*(jewel?JEWEL_VERTICAL_FORCE:GREEN_VERTICAL_FORCE);
        p->boomerang_active=true;p->special_used=true;g->cooldown=0;return 1;
    }
    float speed=velocity.Length();if(speed<1)return 0;
    if(p->bird_type==BIRD_YELLOW_TYPE){
        p->bird->SetLinearVelocity(YELLOW_BOOST_FACTOR*velocity);p->special_used=true;g->cooldown=0;return 1;
    }
    b2Vec2 normal(-velocity.y/speed,velocity.x/speed);
    for(int i=0;i<2;i++){
        float sign=i==0?-1.0f:1.0f,angle=sign*0.1745329252f;
        b2Vec2 v(cosf(angle)*velocity.x-sinf(angle)*velocity.y,sinf(angle)*velocity.x+cosf(angle)*velocity.y);
        p->split[i]=p->make_bird(p->bird->GetPosition()+sign*1.32f*normal,v,BIRD_BLUE_TYPE);
    }
    p->special_used=true;g->cooldown=0;return 1;
}
int game_projectile_count(const Game *g){
    const Physics *p=static_cast<const Physics*>(g->physics);return p->bird?(p->split[0]?3:p->egg?2:1):0;
}
int game_get_projectile(const Game *g,int index,GameBird *out){
    if(!out || index<0 || index>=game_projectile_count(g))return 0;
    const Physics *p=static_cast<const Physics*>(g->physics);b2Body *body=index==0?p->bird:p->egg?p->egg:p->split[index-1];
    b2Vec2 pos=body->GetPosition(),vel=body->GetLinearVelocity();
    out->x=g->view_x+pos.x*g->view_scale;out->y=g->view_y+pos.y*g->view_scale;
    out->vx=vel.x*g->view_scale;out->vy=vel.y*g->view_scale;out->angle=body->GetAngle();out->scale=body==p->bird?p->potion_scale:1.0f;out->type=body==p->egg?BIRD_EGG_TYPE:p->bird_type;out->special_used=p->special_used;out->fuse=p->bird_type==BIRD_BLACK_TYPE?p->bomb_fuse:p->bird_type==BIRD_PUFFER_TYPE&&index==0&&p->puffer_state?body->GetFixtureList()->GetShape()->m_radius/GLOBE_PUFFED_RADIUS:-1;out->collided=index==0?p->bird_collided:body==p->egg?p->egg_collided:p->split_collided[index-1];return 1;
}
int game_get_blast(const Game *g,GameBlast *out){
    const Physics *p=static_cast<const Physics*>(g->physics);
    if(!out || p->blast_life<=0)return 0;
    out->x=g->view_x+p->blast_position.x*g->view_scale;out->y=g->view_y+p->blast_position.y*g->view_scale;
    out->radius=p->blast_radius*g->view_scale;out->life=p->blast_life;return 1;
}
static void radial_blast(Game *g,const b2Vec2 &position,float radius,float force,float damage_radius,float damage){
    Physics *p=static_cast<Physics*>(g->physics);
    p->blast_position=position;p->blast_life=BLACK_VISUAL_SECONDS;p->blast_radius=radius;
    for(int i=0;i<p->level->count;i++){
        b2Body *body=p->bodies[i];if(!body || body->GetType()!=b2_dynamicBody)continue;
        b2Vec2 delta=body->GetPosition()-position;float distance=delta.Length();
        if(distance>=radius)continue;
        p->disturbed[i]=true;
        b2Vec2 direction=distance>0.001f?(1.0f/distance)*delta:b2Vec2(0,-1);
        float impulse=(force*BLAST_IMPULSE_SCALE)*(1-distance/radius);
        body->ApplyLinearImpulseToCenter(impulse*direction,true);
        if(distance<damage_radius){
            bool alive=p->health[i]>0;
            /* Explosions.lua: explosionDamage falls linearly to 0 over explosionDamageRadius, minus the piece's defence; only a positive result hurts. */
            float hurt=damage*(1-distance/damage_radius)-p->level->objects[i].defence;if(hurt>0)p->health[i]-=hurt;
            if(alive && p->health[i]<=0 && p->explosive(i))p->chain_delay[i]=TNT_CHAIN_SECONDS;
        }
    }
    g->events|=GAME_EVENT_EXPLOSION;g->cooldown=0;
    g->explosion_x=g->view_x+position.x*g->view_scale;g->explosion_y=g->view_y+position.y*g->view_scale;
}
static void detonate(Game *g){
    Physics *p=static_cast<Physics*>(g->physics);
    if(p->detonated || !p->bird)return;
    radial_blast(g,p->bird->GetPosition(),BLACK_BLAST_RADIUS,BLACK_BLAST_FORCE,BLACK_DAMAGE_RADIUS,BLACK_BLAST_DAMAGE);
    p->detonated=p->special_used=true;p->bomb_requested=false;
    p->world.DestroyBody(p->bird);p->bird=NULL;p->spawn_ignore_count=0;
    g->x=g->view_x+p->blast_position.x*g->view_scale;g->y=g->view_y+p->blast_position.y*g->view_scale;
    g->vx=g->vy=g->flight_time=0;
}
void game_preview(const Game *g,float t,float *x,float *y) {
    float a=g->angle*0.01745329252f;
    /* Match Box2D's semi-implicit integration at fixed 120 Hz in free flight. */
    float speed=LAUNCH_SPEED_MAX*sling_power(static_cast<const Physics*>(g->physics))*g->power/100.0f*g->view_scale;
    *x=g->sling_x+cosf(a)*speed*t;
    *y=g->sling_y-sinf(a)*speed*t+0.5f*g->gravity*g->view_scale*t*(t+1.0f/120.0f);
}
/* Egg Defender, each step: the vehicles' controllers and timers, the pigs grabbing the egg or
   giving up (EggDefenderPig.lua), the egg going home (EggDefenderEggMovement.lua), pieces leaving
   the game area (EggDefenderVehicle.breakBlocks) and the stolen egg (EggDefender.checkLevelFailed). */
static void remove_silently(Game *g,Physics *p,int i){
    if(!p->bodies[i])return;
#ifdef EGG_DEBUG
    fprintf(stderr,"remove %d %s at %.1f,%.1f\n",i,p->level->objects[i].definition,p->bodies[i]->GetPosition().x,p->bodies[i]->GetPosition().y);
#endif
    if(i==p->egg_grabber){p->egg_grabber=-1;p->egg_tween=-1;if(p->egg_i>=0 && p->bodies[p->egg_i])p->set_mask(p->bodies[p->egg_i],0xFFFF);}
    p->world.DestroyBody(p->bodies[i]);p->bodies[i]=NULL;
    const CampaignObject &o=p->level->objects[i];
    if(o.kind==4 && p->is_goal(i) && !o.spawn_of)p->escaped_pigs++;  /* a pig that got away no longer has to be beaten */
    (void)g;
}
static b2Vec2 egg_hold_point(const Physics *p,int pig){
    /* EggDefenderPig: the egg sits a third of the pig's size ahead, at the pig's angle + 0.2. */
    const b2Body *b=p->bodies[pig];float a=b->GetAngle()+0.2f,r=p->level->objects[pig].width/3;
    return b->GetPosition()+b2Vec2(r*cosf(a),r*sinf(a));
}
static void auto_balance(EggVehicle *v);static void update_aim(EggVehicle *v);
/* onRedBirdTarget / onRedBirdTriggered: the vehicle owning the pig reacts (pigTargeted, redBirdTriggered). */
static void red_notify(Physics *p,int pig,bool triggered){
    if(triggered)p->red_triggered_count++;else p->red_targeted_count++;
    int w=(int)OBJECT_WAVE(p->level->objects[pig].flags);if(!p->egg_mode || w<1 || w>15 || !p->vehicles[w].active)return;
    EggVehicle &v=p->vehicles[w];const EggReaction *r=egg_reaction(v.c?v.c->name:"");if(!r)return;
    if(triggered){if(r->red_triggered)r->red_triggered(&v);}
    else if(r->pig_targeted)r->pig_targeted(&v,piece_name(&p->level->objects[pig]));
}
static void egg_tick(Game *g,Physics *p,float dt){
    if(!p->egg_mode)return;
    for(int i=0;i<p->level->count;i++)if(p->bodies[i] && p->gadget_state[i]){
        float &tm=p->gadget_time[i];tm+=dt;
        if(p->gadget_state[i]==1 && tm>=EGG_SUCKER_START){p->gadget_state[i]=2;}
        else if(p->gadget_state[i]==3){  /* rocket: thrust ramps up over 3 s, burns 2 s more, then stops */
            if(tm<EGG_ROCKET_START)p->thrust[i]=p->gadget_value[i]*tm/EGG_ROCKET_START;
            else if(tm<EGG_ROCKET_START+p->gadget_run[i])p->thrust[i]=p->gadget_value[i];
            else{p->thrust_off[i]=true;p->gadget_state[i]=0;}
        }else if(p->gadget_state[i]==4 && tm>=EGG_INFLATE_TIME){  /* inflater: the balloon is full and pulls */
            p->thrust[i]=0;p->wforce_x[i]=p->gadget_value[i];p->wforce_y[i]=p->gadget_value2[i];p->thrust_off[i]=false;p->gadget_state[i]=5;
        }
    }
    for(int w=1;w<16;w++){EggVehicle &v=p->vehicles[w];if(!v.active)continue;
        int pig=p->first_pig(w);
        if(pig<0){bool any=false;for(int i=0;i<p->level->count && !any;i++)if(p->bodies[i] && (int)OBJECT_WAVE(p->level->objects[i].flags)==w)any=true;if(!any){v.active=false;continue;}}
        else{v.last_x=p->bodies[pig]->GetPosition().x-p->level->sling_x;v.last_y=p->bodies[pig]->GetPosition().y;
            if(p->bodies[pig]->GetPosition().x<p->game_right)v.ever_inside=true;}
        for(int k=0;k<8;k++)if(v.timer_fn[k] && (v.timer_left[k]-=dt)<=0){EggTimed fn=v.timer_fn[k];v.timer_fn[k]=NULL;fn(&v);}
        auto_balance(&v);update_aim(&v);
        if(v.c && v.c->update && pig>=0)v.c->update(&v,dt);
    }
    b2Body *egg=p->egg_i>=0?p->bodies[p->egg_i]:NULL;
    if(egg)p->health[p->egg_i]=p->level->objects[p->egg_i].strength;  /* BP_EGG: defence 1000, it does not break */
    for(int i=0;i<p->level->count;i++){
        const CampaignObject &o=p->level->objects[i];int w=(int)OBJECT_WAVE(o.flags);
        if(!w || o.kind!=4 || o.spawn_of || !p->bodies[i])continue;
        b2Body *b=p->bodies[i];b2Vec2 at=b->GetPosition();
        bool on_static=false;
        for(b2ContactEdge *e=b->GetContactList();e;e=e->next)if(e->contact->IsTouching() && e->other->GetType()==b2_staticBody && !e->contact->GetFixtureA()->IsSensor() && !e->contact->GetFixtureB()->IsSensor())on_static=true;
        if(!p->abandoned[i]){
            /* grabEggs: the closest free egg within 5 m across and 10 m up or down. */
            if(egg && !p->vehicles[w&15].no_grab && !p->pig_no_grab[i] && p->egg_grabber<0 && p->egg_tween<0 && fabsf(at.x-egg->GetPosition().x)<EGG_GRAB_DX && fabsf(at.y-egg->GetPosition().y)<EGG_GRAB_DY){
#ifdef EGG_DEBUG
                fprintf(stderr,"grab by %d age %.1f\n",i,p->age);
#endif
                p->egg_grabber=i;p->egg_tween=0;p->egg_from=egg->GetPosition();p->drop_joints(egg,NULL);p->set_mask(egg,0);
                egg->SetLinearVelocity(b2Vec2(0,0));egg->SetAngularVelocity(0);
            }
            /* onPigCollision: a pig on a vehicle that touches the ground gives up. */
            EggVehicle &v=p->vehicles[w<16?w:0];
            if(on_static){p->abandon(i);continue;}
            if(!(v.c && v.c->never_abandon)){
                /* checkProgress: a sample of x + y every 0.5 s; no progress over the window: give up. */
                if((p->progress_clock[i]+=dt)>EGG_PROGRESS_STEP){
                    p->progress_clock[i]-=EGG_PROGRESS_STEP;float s=at.x+at.y;
                    if(p->progress_n[i]<=EGG_PROGRESS_SAMPLES)p->progress_hist[i][p->progress_n[i]++]=s;
                    if(p->progress_n[i]>EGG_PROGRESS_SAMPLES){
                        float oldest=p->progress_hist[i][0],second=p->progress_hist[i][1];
                        memmove(p->progress_hist[i],p->progress_hist[i]+1,sizeof(float)*EGG_PROGRESS_SAMPLES);p->progress_n[i]--;
                        if(fabsf(s-oldest)<EGG_PROGRESS_MIN && fabsf(s-second)<EGG_PROGRESS_MIN)p->abandon(i);
                    }
                }
            }
        }else if(p->jump_ready[i] && on_static){
            /* doJumping: once down, the pig hops away and surrenders. */
            p->jump_ready[i]=false;b->SetLinearVelocity(b2Vec2(EGG_PIG_JUMP_X,EGG_PIG_JUMP_Y));b->SetTransform(at,0);b->SetAngularVelocity(0);
        }
    }
    /* The egg: jumping into a pig's hands, carried, rolling home or jumping back into the nest. */
    if(egg){
        if(p->egg_grabber>=0 && !p->bodies[p->egg_grabber]){p->egg_grabber=-1;p->egg_tween=-1;p->set_mask(egg,0xFFFF);}
        if(p->egg_grabber>=0 && p->egg_tween>=0){
            b2Vec2 to=egg_hold_point(p,p->egg_grabber);float k=fminf(1,(p->egg_tween+=dt)/EGG_JUMP_TIME);
            b2Vec2 at=p->egg_from+k*(to-p->egg_from);at.y-=4*k*(1-k)*2;egg->SetTransform(at,egg->GetAngle());egg->SetLinearVelocity(b2Vec2(0,0));
            if(k>=1){p->egg_tween=-1;p->add_weld(p->bodies[p->egg_grabber],egg);
                EggVehicle &v=p->vehicles[(int)OBJECT_WAVE(p->level->objects[p->egg_grabber].flags)&15];if(v.c && v.c->egg_collected)v.c->egg_collected(&v);}
        }else if(p->egg_grabber<0){
            bool jointed=false;for(int k=0;k<p->joint_count;k++)if(p->joints[k] && (p->joints[k]->GetBodyA()==egg || p->joints[k]->GetBodyB()==egg))jointed=true;
            if(p->egg_tween>=0){
                float k=fminf(1,(p->egg_tween+=dt)/EGG_JUMP_TIME);b2Vec2 at=p->egg_from+k*(p->egg_home-p->egg_from);at.y-=4*k*(1-k)*2;
                egg->SetTransform(at,p->egg_home_angle);egg->SetLinearVelocity(b2Vec2(0,0));egg->SetAngularVelocity(0);
                if(k>=1){p->egg_tween=-1;if(p->nest_i>=0 && p->bodies[p->nest_i])p->add_weld(p->bodies[p->nest_i],egg);}
            }else if(!jointed){
                float dx=p->egg_home.x-egg->GetPosition().x,dy=p->egg_home.y-egg->GetPosition().y,r=p->level->objects[p->egg_i].width/2;
                if(fabsf(dx)>2*r)egg->ApplyForce(b2Vec2(dx*EGG_RETURN_FORCE/fmaxf(1,0.06f*dy),0),egg->GetPosition()+b2Vec2(0,-1),true);
                else if(dy<0){p->egg_tween=0;p->egg_from=egg->GetPosition();}
            }
        }
        b2Vec2 e=egg->GetPosition();
        if(e.x>p->game_right+EGG_AREA_MARGIN || e.y>p->level->ground_y+30){p->world.DestroyBody(egg);p->bodies[p->egg_i]=NULL;p->egg_grabber=-1;p->egg_lost=true;}
    }else if(p->egg_i>=0)p->egg_lost=true;
    /* breakBlocks / checkExiting: vehicle pieces that have left the game area are removed. */
    for(int i=0;i<p->level->count;i++)if(p->bodies[i] && OBJECT_WAVE(p->level->objects[i].flags) && p->vehicles[OBJECT_WAVE(p->level->objects[i].flags)].ever_inside){
        b2Vec2 at=p->bodies[i]->GetPosition();
        const CampaignObject &o=p->level->objects[i];float m=5*fmaxf(o.width,o.height);  /* isBlockInsideGameArea: 5 sprite sizes */
        if(at.x>p->game_right+m || at.x<p->level->sling_x-EGG_SCREEN_MARGIN-m || at.y>p->level->ground_y+30 || at.y<p->level->sling_y-EGG_SCREEN)remove_silently(g,p,i);
    }
    if(p->egg_lost)g->state=LOST;
}
/* EggDefenderVehicle API for the translated controllers (egg_vehicles.h). */
static b2Body *ev_body(EggVehicle *v,const char *name,int *index=NULL){
    int i=v->p->piece(v->wave,name);if(index)*index=i;return i>=0?v->p->bodies[i]:NULL;
}
float ev_x(EggVehicle *v){int i=v->p->first_pig(v->wave);return i>=0?v->p->bodies[i]->GetPosition().x-v->p->level->sling_x:v->last_x;}
float ev_y(EggVehicle *v){int i=v->p->first_pig(v->wave);return i>=0?v->p->bodies[i]->GetPosition().y:v->last_y;}
float ev_vx(EggVehicle *v){int i=v->p->first_pig(v->wave);return i>=0?v->p->bodies[i]->GetLinearVelocity().x:0;}
float ev_vy(EggVehicle *v){int i=v->p->first_pig(v->wave);return i>=0?v->p->bodies[i]->GetLinearVelocity().y:0;}
int ev_has_block(EggVehicle *v,const char *name){return ev_body(v,name)!=NULL;}
float ev_block_x(EggVehicle *v,const char *name){b2Body *b=ev_body(v,name);return b?b->GetPosition().x:0;}
float ev_block_y(EggVehicle *v,const char *name){b2Body *b=ev_body(v,name);return b?b->GetPosition().y:0;}
float ev_thrust(EggVehicle *v,const char *name){int i=v->p->piece(v->wave,name);return i>=0?v->p->thrust[i]:0;}
void ev_set_thrust(EggVehicle *v,const char *name,float force){int i=v->p->piece(v->wave,name);if(i>=0)v->p->thrust[i]=force;}
float ev_force_x(EggVehicle *v,const char *name){int i=v->p->piece(v->wave,name);return i>=0?v->p->wforce_x[i]:0;}
float ev_force_y(EggVehicle *v,const char *name){int i=v->p->piece(v->wave,name);return i>=0?v->p->wforce_y[i]:0;}
void ev_set_force(EggVehicle *v,const char *name,float fx,float fy){int i=v->p->piece(v->wave,name);if(i>=0){v->p->wforce_x[i]=fx;v->p->wforce_y[i]=fy;}}
void ev_power(EggVehicle *v,const char *name,int on){int i=v->p->piece(v->wave,name);if(i>=0)v->p->thrust_off[i]=!on;}
void ev_remove_joint(EggVehicle *v,const char *name){
    Physics *p=v->p;
    for(int k=0;k<p->level->joint_count && k<p->joint_count;k++){
        const CampaignJoint &j=p->level->joints[k];if(!p->joints[k] || strcmp(joint_name(&j),name))continue;
        int w=j.a>=0?(int)OBJECT_WAVE(p->level->objects[j.a].flags):j.b>=0?(int)OBJECT_WAVE(p->level->objects[j.b].flags):0;
        if(w==v->wave){p->world.DestroyJoint(p->joints[k]);p->joints[k]=NULL;}
    }
}
void ev_motor_speed(EggVehicle *v,const char *name,float speed){
    Physics *p=v->p;
    for(int k=0;k<p->level->joint_count && k<p->joint_count;k++){
        const CampaignJoint &j=p->level->joints[k];if(!p->joints[k] || strcmp(joint_name(&j),name))continue;
        int w=j.a>=0?(int)OBJECT_WAVE(p->level->objects[j.a].flags):j.b>=0?(int)OBJECT_WAVE(p->level->objects[j.b].flags):0;
        if(w!=v->wave)continue;
        b2Joint *jt=p->joints[k];
        if(jt->GetType()==e_revoluteJoint)((b2RevoluteJoint*)jt)->SetMotorSpeed(speed);
        else if(jt->GetType()==e_prismaticJoint)((b2PrismaticJoint*)jt)->SetMotorSpeed(speed);
        jt->GetBodyA()->SetAwake(true);jt->GetBodyB()->SetAwake(true);
    }
}
static Game *ev_game; /* the game being stepped (set by tick) */
void ev_remove_block(EggVehicle *v,const char *name){int i=v->p->piece(v->wave,name);if(i>=0 && v->p->bodies[i] && ev_game)remove_silently(ev_game,v->p,i);}
void ev_abandon(EggVehicle *v){Physics *p=v->p;for(int i=0;i<p->level->count;i++)if((int)OBJECT_WAVE(p->level->objects[i].flags)==v->wave && p->level->objects[i].kind==4 && p->bodies[i])p->abandon(i);}
void ev_surrender(EggVehicle *v){ev_abandon(v);}
void ev_abandon_pig(EggVehicle *v,const char *pig){int i=v->p->piece(v->wave,pig);if(i>=0 && v->p->bodies[i])v->p->abandon(i);}
int ev_egg_collected(EggVehicle *v){Physics *p=v->p;return p->egg_grabber>=0 && (int)OBJECT_WAVE(p->level->objects[p->egg_grabber].flags)==v->wave && p->egg_tween<0;}
void ev_timer(EggVehicle *v,EggTimed fn,float seconds){
    for(int k=0;k<8;k++)if(v->timer_fn[k]==fn){v->timer_left[k]=seconds;return;}  /* doWithTimer keys on the function */
    for(int k=0;k<8;k++)if(!v->timer_fn[k]){v->timer_fn[k]=fn;v->timer_left[k]=seconds;return;}
}
float ev_closest_egg_x(EggVehicle *v,const char *name){
    Physics *p=v->p;b2Body *from=name?ev_body(v,name):NULL;
    if(!from){int i=p->first_pig(v->wave);from=i>=0?p->bodies[i]:NULL;}
    if(!from || p->egg_i<0 || !p->bodies[p->egg_i])return 9999999.0f;
    return from->GetPosition().x-p->bodies[p->egg_i]->GetPosition().x;
}
float *ev_vars(EggVehicle *v){return v->vars;}
void ev_no_grab(EggVehicle *v){v->no_grab=true;}
void ev_pig_no_grab(EggVehicle *v,const char *pig){int i=v->p->piece(v->wave,pig);if(i>=0)v->p->pig_no_grab[i]=true;}
float ev_slingshot_x(EggVehicle *v,const char *name){b2Body *b=ev_body(v,name);if(!b){int i=v->p->first_pig(v->wave);b=i>=0?v->p->bodies[i]:NULL;}return b?b->GetPosition().x-v->p->level->sling_x:0;}
void ev_create_joint(EggVehicle *v,const char *name){
    /* createJoint: the joint comes back between the two pieces as they are now (port: anchored on end1). */
    Physics *p=v->p;
    for(int k=0;k<p->level->joint_count && k<p->joint_count;k++){
        const CampaignJoint &j=p->level->joints[k];if(p->joints[k] || strcmp(joint_name(&j),name))continue;
        b2Body *a=p->joint_body(j.a),*b=p->joint_body(j.b);if(!a || !b || a==b)return;
        CampaignJoint c=j;c.ax=a->GetPosition().x;c.ay=a->GetPosition().y;c.bx=b->GetPosition().x;c.by=b->GetPosition().y;
        int n=p->joint_count;p->joint_count=k;p->create_joint(c);if(p->joint_count==k)p->joint_count=n;else p->joint_count=n;  /* refill slot k */
        return;
    }
}
float ev_random(EggVehicle *v){v->seed=v->seed*1103515245u+12345u+(unsigned)v->wave;return ((v->seed>>16)&32767)/32768.0f;}
void ev_set_interval(EggVehicle *v,const char *name,float seconds){int i=v->p->piece(v->wave,name);if(i>=0)v->p->interval_v[i]=seconds;}
float ev_closest_egg_distance(EggVehicle *v){
    Physics *p=v->p;int i=p->first_pig(v->wave);
    if(i<0 || p->egg_i<0 || !p->bodies[p->egg_i])return 9999999.0f;
    return (p->bodies[i]->GetPosition()-p->bodies[p->egg_i]->GetPosition()).Length();
}
void ev_stop_aim(EggVehicle *v,const char *name){
    Physics *p=v->p;int i=p->piece(v->wave,name);
    for(int k=0;k<4;k++)if(v->aim_speed[k] && v->aim_piece[k]==i){b2Joint *jt=p->joints[v->aim_joint[k]];if(jt && jt->GetType()==e_revoluteJoint)((b2RevoluteJoint*)jt)->SetMotorSpeed(0);v->aim_speed[k]=0;}
}
void ev_set_velocity(EggVehicle *v,float vx,float vy){Physics *p=v->p;for(int i=0;i<p->level->count;i++)if(p->bodies[i] && (int)OBJECT_WAVE(p->level->objects[i].flags)==v->wave)p->bodies[i]->SetLinearVelocity(b2Vec2(vx,vy));}
void ev_start_aim(EggVehicle *v,const char *name,const char *target,float offset_degrees,float speed){
    Physics *p=v->p;int i=p->piece(v->wave,name);if(i<0)return;
    int joint=-1;for(int k=0;k<p->level->joint_count && k<p->joint_count && joint<0;k++){const CampaignJoint &j=p->level->joints[k];if(j.type==JOINT_REVOLUTE && (j.a==i || j.b==i))joint=k;}
    if(joint<0)return;
    for(int k=0;k<4;k++)if(!v->aim_speed[k] || v->aim_piece[k]==i){v->aim_piece[k]=i;v->aim_joint[k]=joint;v->aim_target[k]=strcmp(target,"egg")?1:0;v->aim_offset[k]=offset_degrees*b2_pi/180;v->aim_speed[k]=speed>0?speed:10;v->aim_motor[k]=0;return;}
}
/* updateAiming: the aiming piece's revolute motor turns it towards its target (the egg, or the bird:
   the flying bird, else the slingshot), at aimingSpeed, slowing down within one update of travel. */
static void update_aim(EggVehicle *v){
    Physics *p=v->p;
    for(int k=0;k<4;k++)if(v->aim_speed[k]){
        int i=v->aim_piece[k];b2Joint *jt=p->joints[v->aim_joint[k]];
        if(!p->bodies[i] || !jt){v->aim_speed[k]=0;continue;}
        b2Vec2 to(p->level->sling_x,p->level->sling_y);
        if(v->aim_target[k]==0){if(p->egg_i>=0 && p->bodies[p->egg_i] && p->egg_grabber<0)to=p->bodies[p->egg_i]->GetPosition();}
        else if(p->bird)to=p->bird->GetPosition();
        b2Vec2 at=p->bodies[i]->GetPosition();float target=atan2f(to.y-at.y,to.x-at.x);while(target<0)target+=2*b2_pi;
        float cur=p->bodies[i]->GetAngle()+b2_pi-v->aim_offset[k];while(cur<0)cur+=2*b2_pi;
        float diff=target-cur,span=v->aim_speed[k]*EGG_AIM_UPDATES/30.0f,speed=0;
        if(diff<-0.001f)speed=v->aim_speed[k]*(fabsf(diff)<span?fabsf(diff)/span:1);
        else if(diff>0.001f)speed=-v->aim_speed[k]*(fabsf(diff)<span?fabsf(diff)/span:1);
        if(fabsf(v->aim_motor[k]-speed)>0.001f){v->aim_motor[k]=speed;if(jt->GetType()==e_revoluteJoint){((b2RevoluteJoint*)jt)->SetMotorSpeed(speed);((b2RevoluteJoint*)jt)->EnableMotor(true);}}
    }
}
void ev_gadget(EggVehicle *v,const char *name,const char *what){
    Physics *p=v->p;int i=p->piece(v->wave,name);if(i<0 || !p->bodies[i])return;
    if(!strcmp(what,"startSuction")){if(p->gadget_state[i]==0){p->gadget_state[i]=1;p->gadget_time[i]=0;}}
    else if(!strcmp(what,"stopSuction"))p->gadget_state[i]=0;
    else if(!strcmp(what,"startRocket")){if(p->gadget_state[i]!=3){p->gadget_value[i]=p->thrust[i];p->thrust[i]=0;p->thrust_off[i]=false;p->gadget_state[i]=3;p->gadget_time[i]=0;}}
    else if(!strcmp(what,"inflate")){if(p->gadget_state[i]<4){p->gadget_state[i]=4;p->gadget_time[i]=0;}}
}
void ev_gadget_property(EggVehicle *v,const char *name,const char *prop,float value){
    Physics *p=v->p;int i=p->piece(v->wave,name);if(i<0)return;
    if(!strcmp(prop,"runDuration"))p->gadget_run[i]=value;
    else if(!strcmp(prop,"inflatedForceX")){p->gadget_value[i]=value;if(p->gadget_state[i]==5)p->wforce_x[i]=value;}
    else if(!strcmp(prop,"inflatedForceY")){p->gadget_value2[i]=value;if(p->gadget_state[i]==5)p->wforce_y[i]=value;}
}
void ev_auto_balance(EggVehicle *v,const char *a,const char *b,float force){
    for(int k=0;k<4;k++)if(v->balance_a[k] && !strcmp(v->balance_a[k],a) && !strcmp(v->balance_b[k],b)){if(force>0)v->balance_force[k]=force;else v->balance_a[k]=NULL;return;}
    if(force>0)for(int k=0;k<4;k++)if(!v->balance_a[k]){v->balance_a[k]=a;v->balance_b[k]=b;v->balance_force[k]=force;return;}
}
/* doAutoBalance: of two thrusters, the upper one gets the larger forceRelative; when both are
   equal they are split into max x (1 + force) and min x -force. */
static void auto_balance(EggVehicle *v){
    for(int k=0;k<4;k++)if(v->balance_a[k]){
        int a=v->p->piece(v->wave,v->balance_a[k]),b=v->p->piece(v->wave,v->balance_b[k]);
        if(a<0 || b<0 || !v->p->bodies[a] || !v->p->bodies[b]){v->balance_a[k]=NULL;continue;}
        int up=v->p->bodies[a]->GetPosition().y<v->p->bodies[b]->GetPosition().y?a:b,down=up==a?b:a;
        float lo=fminf(v->p->thrust[a],v->p->thrust[b]),hi=fmaxf(v->p->thrust[a],v->p->thrust[b]);
        if(lo==hi){hi*=1+v->balance_force[k];lo*=-v->balance_force[k];}
        v->p->thrust[up]=hi;v->p->thrust[down]=lo;
    }
}
float ev_target(EggVehicle *v,const char *name){int i=v->p->piece(v->wave,name);return i>=0?v->p->target_v[i]:0;}
void ev_set_target(EggVehicle *v,const char *name,float speed){int i=v->p->piece(v->wave,name);if(i>=0)v->p->target_v[i]=speed;}
static void tick(Game *g) {
    Physics *p=static_cast<Physics*>(g->physics); const float dt=1.0f/120;ev_game=g;
    g->birds+=p->pending_birds;p->pending_birds=0;
    p->update_guns(dt);
    p->update_suckers();
    p->update_waves();
    egg_tick(g,p,dt);if(g->state==LOST)return;
    p->cannons_update(dt);
    if(p->homing_on && p->bird && !p->bird_collided && g->state==FLYING){
        /* Powerup_HomingHat.lua: the nearest pig is the target; each update (30 a second) its direction is added, 40 strong, to the bird's
           velocity, which is then turned back to the old speed. It locks on within 370 of the original's 800 pixels of width. */
        b2Vec2 at=p->bird->GetPosition(),v=p->bird->GetLinearVelocity();int best=-1;float near=1e9f;
        for(int i=0;i<p->level->count;i++){if(p->level->objects[i].kind!=4 || !p->is_goal(i) || p->health[i]<=0 || !p->bodies[i])continue;
            b2Vec2 d=p->bodies[i]->GetPosition()-at;float l=d.Length();if(l<near){near=l;best=i;}}
        float lock=370.0f/800.0f*608.0f/g->view_scale;
        if(best>=0 && near<lock){
            b2Vec2 d=p->bodies[best]->GetPosition()-at;d.Normalize();float speed=v.Length();
            b2Vec2 w=v+(HOMING_PULL*dt*30.0f)*d;if(w.Normalize()>0)p->bird->SetLinearVelocity(speed*w);
        }
    }
    if(p->quake_time>0){
        float step=fminf(dt,p->quake_time);p->quake_time-=step;
        for(int i=0;i<p->level->count;i++){b2Body *b=p->bodies[i];
            if(!b || b->GetType()!=b2_dynamicBody)continue;
            float m=b->GetMass();b->ApplyLinearImpulseToCenter(b2Vec2(step*cosf(p->quake_time*b2_pi)*m*BIRD_QUAKE_X,-step*m*BIRD_QUAKE_Y),true);}
        g->cooldown=0;
    }
    if(p->bird_type==BIRD_JEWEL_TYPE && p->bird){
        if(p->bird_collided)p->boomerang_active=false;
        b2Vec2 v=p->bird->GetLinearVelocity();float mass=p->bird->GetMass();
        if(p->boomerang_active){
            if(v.x>=-JEWEL_MAX_HORIZONTAL_SPEED && v.x<=JEWEL_MAX_HORIZONTAL_SPEED)p->bird->ApplyForceToCenter(b2Vec2(p->boomerang_x_force,0),true);
            if(v.y>5)p->bird->ApplyForceToCenter(b2Vec2(0,p->boomerang_y_force),true);
        }else if(!p->special_used && !p->bird_collided && p->has_launched)p->bird->ApplyForceToCenter(b2Vec2(0,JEWEL_FLY_VERTICAL_FORCE),true);
        (void)mass;
    }
    if(p->bird_type==BIRD_BLU_TYPE && p->bird && p->blu_timer>-1){
        if(p->blu_timer>0){
            p->blu_timer-=dt;p->blu_clock+=dt;
            while(p->blu_clock>=BLU_INTERVAL){
                radial_blast(g,p->bird->GetPosition(),BLU_BLAST_RADIUS,BLU_BLAST_FORCE,-1,0);
                b2Vec2 v=p->bird->GetLinearVelocity();
                float a=p->blu_dir_set?p->blu_dir:atan2f(v.y,v.x)-0.8f*b2_pi;
                a=fmodf(a+0.8f*b2_pi,2*b2_pi);p->blu_dir=a;p->blu_dir_set=true;
                float m=p->bird->GetMass()*BLU_IMPULSE;p->blu_fx=m*cosf(a);
                p->bird->SetLinearVelocity(b2Vec2(p->blu_fx,m*sinf(a)));
                p->blu_clock-=BLU_INTERVAL;
            }
            p->bird->ApplyForceToCenter(b2Vec2(0,-p->level->gravity*p->bird->GetMass()),true);
        }else{p->bird->SetLinearVelocity(b2Vec2(p->blu_fx/5,-10));p->blu_timer=-1;}
    }
    if(p->bird_type==BIRD_GREEN_TYPE && p->bird){
        if(p->bird_collided)p->boomerang_active=false;
        b2Vec2 v=p->bird->GetLinearVelocity();float mass=p->bird->GetMass();
        if(p->boomerang_active){
            if(v.x>=-GREEN_MAX_HORIZONTAL_SPEED && v.x<=GREEN_MAX_HORIZONTAL_SPEED)p->bird->ApplyForceToCenter(b2Vec2(p->boomerang_x_force,0),true);
            if(v.y>5)p->bird->ApplyForceToCenter(b2Vec2(0,p->boomerang_y_force),true);
            p->boomerang_spin=fminf(p->boomerang_spin+dt*20,20);p->bird->SetAngularVelocity(p->boomerang_spin);
        }else if(!p->special_used && !p->bird_collided && p->has_launched){
            p->bird->ApplyForceToCenter(b2Vec2(0,GREEN_FLY_VERTICAL_ACCELERATION*mass),true);
            p->boomerang_spin=fminf(p->boomerang_spin+dt*15,6.28f);p->bird->SetAngularVelocity(p->boomerang_spin);
        }
    }
    if(p->bird_type==BIRD_PUFFER_TYPE && p->bird){
        if(!p->puffer_state && p->bird_collided && p->puffer_auto<0)p->puffer_auto=GLOBE_AUTO_PUFF_DELAY;
        if(!p->puffer_state && p->puffer_auto>=0 && (p->puffer_auto-=dt)<=0)puff(g);
        else if(p->puffer_state==1 && (p->puffer_timer-=dt)<=0){p->puffer_state=2;p->puffer_timer=GLOBE_DEATH_TIME;p->puffer_direction=GLOBE_DIRECTION_TIME;}
        else if(p->puffer_state==2){
            p->puffer_timer-=dt;p->puffer_direction-=dt;
            if(p->puffer_direction<=0){
                /* Deterministic stand-in for math.random(-magnitude, magnitude). */
                b2Vec2 v=p->bird->GetLinearVelocity();
                for(int k=0;k<2;k++){p->puffer_seed=p->puffer_seed*1664525u+1013904223u;float r=((p->puffer_seed>>8)&0xffff)/65535.0f*2-1;if(k==0)v.x+=r*GLOBE_DIRECTION_MAGNITUDE;else v.y+=r*GLOBE_DIRECTION_MAGNITUDE;}
                p->bird->SetLinearVelocity(v);p->bird->SetTransform(p->bird->GetPosition(),atan2f(-v.x,v.y)-1.5707963f);
                p->puffer_direction=GLOBE_DIRECTION_TIME;
            }
            if(p->puffer_timer<=0){p->world.DestroyBody(p->bird);p->bird=NULL;p->puffer_state=3;g->flight_time=0;}
            else set_puffer_radius(p,GLOBE_PUFFED_RADIUS*p->puffer_timer/GLOBE_DEATH_TIME,false);
        }
    }
    if(p->bird_type==BIRD_PINK_TYPE && p->bird){
        if(p->pink_timer<0 && !p->special_used && p->bird_collided){p->pink_timer=PINK_CONTACT_DELAY;p->special_used=true;}
        if(p->pink_timer>=0){
            p->pink_timer-=dt;
            if(p->pink_timer<PINK_BOOM_TIME){
                if(!p->pink_hover){p->pink_hover=true;p->bird->SetLinearVelocity(b2Vec2(0,0));}
                p->bird->ApplyForceToCenter(b2Vec2(0,-p->level->gravity*p->bird->GetMass()+1),true);
            }
            if(p->pink_timer<0){
                /* triggerPinkBirdSpecialty: bubble every movable piece in range and the bird. */
                b2Vec2 c=p->bird->GetPosition();p->pink_timer=-2;
                for(int i=0;i<p->level->count;i++){b2Body *b=p->bodies[i];if(!b || b->GetType()!=b2_dynamicBody)continue;
                    if((b->GetPosition()-c).Length()<PINK_BUBBLE_RADIUS){p->bubble_timer[i]=BUBBLE_TIME;p->bubble_float[i]=(b->GetPosition().x-c.x)*BUBBLE_FLOAT_FACTOR;p->disturbed[i]=true;
                        for(int k=0;k<p->joint_count;k++)if(p->joints[k] && p->joint_break[k]>0 && (p->joints[k]->GetBodyA()==b || p->joints[k]->GetBodyB()==b)){p->world.DestroyJoint(p->joints[k]);p->joints[k]=NULL;}}}
                p->bird->SetLinearVelocity(b2Vec2(0,0));p->bird_bubble=BUBBLE_TIME;g->cooldown=0;
            }
        }
        if(p->bird_bubble>0){
            b2Body *b=p->bird;if(b->GetLinearVelocity().y>-BUBBLE_MAX_VELOCITY)b->ApplyForceToCenter(b2Vec2(0,-p->level->gravity*b->GetMass()*BUBBLE_ANTI_GRAVITY),true);
            if((p->bird_bubble-=dt)<=0){p->world.DestroyBody(p->bird);p->bird=NULL;g->flight_time=0;}
        }
    }
    for(int i=0;i<p->level->count;i++)if(p->bubble_timer[i]>0 && p->bodies[i]){
        b2Body *b=p->bodies[i];float m=b->GetMass();
        if(b->GetLinearVelocity().y>-BUBBLE_MAX_VELOCITY)b->ApplyForceToCenter(b2Vec2(p->bubble_float[i]*m,-p->level->gravity*m*BUBBLE_ANTI_GRAVITY),true);
        p->bubble_timer[i]-=dt;
    }
    /* Port approximation of the native force adder: force = (forceX, forceY) x mass. */
    for(int i=0;i<p->level->count;i++)if(p->force_of[i] && p->bodies[i] && !OBJECT_WAVE(p->level->objects[i].flags) && (p->force_of[i]->fx!=0 || p->force_of[i]->fy!=0)){
        b2Body *b=p->bodies[i];const CampaignForce *f=p->force_of[i];float m=b->GetMass();b2Vec2 dir(f->fx,f->fy);float len=dir.Length();
        if(f->target>0 && len>0 && b2Dot(b->GetLinearVelocity(),dir)/len>=f->target)continue;
        b->ApplyForceToCenter(b2Vec2(f->fx*m,f->fy*m),true);
    }
    /* Egg Defender force adder (native, libAngryBirdsClassic): a piece with a non-zero
       forceRelative is pushed along its own x axis, (cos angle, sin angle) x forceRelative; otherwise by
       forceX/forceY. The values are accelerations: the force is value x the piece's mass, as for the
       balloons of the classic stages (a 4-2 balloon at -147 x its 21 kg holds the 3125 N vehicle, as
       the script means). With targetVelocity, each component is dropped while the speed on that axis
       is at least the target and in the same direction. With applyForceInterval (bellows) it is a
       puff: one 1/30 s impulse per interval. disableForce (togglePower) switches the piece off. */
    if(p->egg_mode)for(int i=0;i<p->level->count;i++)if(p->bodies[i] && OBJECT_WAVE(p->level->objects[i].flags) && !p->thrust_off[i] && (p->thrust[i]!=0 || p->wforce_x[i]!=0 || p->wforce_y[i]!=0)){
        b2Body *b=p->bodies[i];float a=b->GetAngle();
        float m=p->gadget_state[i]==5?EGG_BALLOON_MASS:b->GetMass();  /* an inflated balloon pulls with its own mass */
        b2Vec2 force=m*(p->thrust[i]!=0?b2Vec2(p->thrust[i]*cosf(a),p->thrust[i]*sinf(a)):b2Vec2(p->wforce_x[i],p->wforce_y[i])),v=b->GetLinearVelocity();
        float target=p->target_v[i];
        if(target>0){
            if(fabsf(v.x)>=target && v.x*force.x>0)force.x=0;
            if(fabsf(v.y)>=target && v.y*force.y>0)force.y=0;
        }
        if(p->interval_v[i]>0){
            if((p->thrust_clock[i]+=dt)>=p->interval_v[i]){p->thrust_clock[i]=0;b->ApplyLinearImpulseToCenter((1.0f/EGG_THRUST_RATE)*force,true);}
        }else b->ApplyForceToCenter(force,true);
    }
    /* backAndForth (native checkJointLimits): the motor turns around at each limit. */
    for(int k=0;k<p->joint_count;k++)if(p->joints[k] && p->joint_swing[k]){
        b2Joint *jt=p->joints[k];
        if(jt->GetType()==e_prismaticJoint){b2PrismaticJoint *pj=(b2PrismaticJoint*)jt;float t=pj->GetJointTranslation(),v=pj->GetMotorSpeed();
            if((t<=pj->GetLowerLimit() && v<0)||(t>=pj->GetUpperLimit() && v>0))pj->SetMotorSpeed(-v);}
        else if(jt->GetType()==e_revoluteJoint){b2RevoluteJoint *rj=(b2RevoluteJoint*)jt;float t=rj->GetJointAngle(),v=rj->GetMotorSpeed();
            if((t<=rj->GetLowerLimit() && v<0)||(t>=rj->GetUpperLimit() && v>0))rj->SetMotorSpeed(-v);}
    }
    float step=dt;
    if(p->red_mode && p->bird && p->bird_type==BIRD_RED_TYPE){
        if(!p->special_used && !p->bird_collided){
            /* redBirdNormalUpdate: the closest living pig; a new target tells its vehicle (pigTargeted). */
            int best=-1;float bd=1e9f;b2Vec2 at=p->bird->GetPosition();
            for(int i=0;i<p->level->count;i++)if(p->bodies[i] && p->level->objects[i].kind==4 && !p->level->objects[i].spawn_of){float d=(p->bodies[i]->GetPosition()-at).Length();if(d<bd){bd=d;best=i;}}
            if(best!=p->red_target){p->red_target=best;if(best>=0)red_notify(p,best,false);}
        }
        if(p->red_delay>0){
            /* redBirdSpecialDelayUpdate: physicsSpeedFactor eases from 1 to 0 over the 0.5 s. */
            float k=1-p->red_delay/RED_SPECIAL_DELAY;step=dt*fmaxf(0.02f,1-k*(2-k));
            if((p->red_delay-=dt)<=0){
                b2Vec2 d=p->red_aim-p->bird->GetPosition();if(d.Normalize()<0.01f)d.Set(1,0);
                p->bird->SetLinearVelocity(RED_SPECIAL_SPEED*d);p->red_fly=true;
                if(p->red_target>=0 && p->bodies[p->red_target])red_notify(p,p->red_target,true);
            }
        }
        if(p->red_fly){if(p->bird_collided)p->red_fly=false;else p->bird->ApplyForceToCenter(b2Vec2(0,-p->level->gravity*p->bird->GetMass()),true);}
    }
#ifdef PS2BIRDS_RIO
    p->water_forces(dt);
#endif
    p->age+=dt;p->damage_contact_count=0;p->world.Step(step,8,3);g->events|=p->hit_events;p->hit_events=0;
#ifdef PS2BIRDS_RIO
    p->water_check(g);
#endif
    for(int k=0;k<p->detach_count;k++){
        int t=p->detach_target[k];
        if(t<0 || p->bodies[p->detach_source[k]])continue;
        if(p->detach_time[k]>0){p->detach_time[k]-=dt;continue;}
        if(p->bodies[t]){
            b2Vec2 where=p->bodies[t]->GetPosition();p->world.DestroyBody(p->bodies[t]);p->bodies[t]=NULL;p->detach_target[k]=-1;p->release_spawns(t,where);
            if(p->level->objects[t].kind==4 && p->is_goal(t)){g->pig_alive--;{int pts=p->level->objects[t].score>=0?(int)p->level->objects[t].score:5000;g->score+=pts;popup_at(g,where,pts,POPUP_PIG,4);}g->events|=GAME_EVENT_PIG;}else if(p->level->objects[t].kind!=3 && !(p->level->objects[t].flags&(OBJECT_GUN_BUBBLE|OBJECT_CANNON_BALL))){g->block_alive--;g->events|=GAME_EVENT_EXTRA;}
        }
    }p->break_joints(1.0f/dt);
    if(p->bird_type==BIRD_BLACK_TYPE && p->bird && !p->detonated){
        if(p->bird_collided){if(p->bomb_fuse<0)p->bomb_fuse=0;p->bomb_fuse+=dt;}
        if(p->bomb_requested || p->bomb_fuse>=BLACK_FUSE_SECONDS)detonate(g);
    }
    if(p->bird_type==BIRD_SHOCK_TYPE && p->bird && !p->detonated){
        if(p->bird_collided){if(p->bomb_fuse<0)p->bomb_fuse=0;p->bomb_fuse+=dt;}
        if(p->bomb_requested || p->bomb_fuse>=SHOCK_FUSE_SECONDS){
            p->shock_center=p->bird->GetPosition();p->shock_time=0;memset(p->shock_hit,0,sizeof(p->shock_hit));
            p->detonated=p->special_used=true;p->bomb_requested=false;
            p->world.DestroyBody(p->bird);p->bird=NULL;p->spawn_ignore_count=0;
            g->x=g->view_x+p->shock_center.x*g->view_scale;g->y=g->view_y+p->shock_center.y*g->view_scale;
            g->vx=g->vy=g->flight_time=0;g->events|=GAME_EVENT_EXPLOSION;g->cooldown=0;g->explosion_x=g->x;g->explosion_y=g->y;
        }
    }
    if(p->shock_time>=0){
        p->shock_time+=dt;g->cooldown=0;
        float elapsed=p->shock_time-SHOCK_DELAY;
        if(elapsed>=SHOCK_DURATION)p->shock_time=-1;
        else if(elapsed>=0){
            float left=1-elapsed/SHOCK_DURATION;                 /* time/duration in the Lua */
            float t=cosf(3.14159265f*0.5f*left),radius=SHOCK_RADIUS*t;
            float force=(1-left)<SHOCK_DROP_OFF?SHOCK_FORCE:SHOCK_FORCE+(1-left/(1-SHOCK_DROP_OFF))*(SHOCK_MIN_FORCE-SHOCK_FORCE);
            p->blast_position=p->shock_center;p->blast_radius=radius;p->blast_life=BLACK_VISUAL_SECONDS*0.5f;
            for(int i=0;i<p->level->count;i++){
                b2Body *b=p->bodies[i];if(!b || b->GetType()!=b2_dynamicBody)continue;
                b2Vec2 d=b->GetPosition()-p->shock_center;float d2=d.LengthSquared();
                const CampaignObject &o=p->level->objects[i];
                if(o.kind==4 && p->electrocute[i]<=0 && d2<fminf(SHOCK_RADIUS*SHOCK_RADIUS,radius*radius*1.2f)){p->electrocute[i]=SHOCK_ELECTROCUTE_SECONDS;g->events|=GAME_EVENT_ELECTRO;}
                if(d2>=radius*radius)continue;
                p->disturbed[i]=true;
                /* destroyBreakableJoints(object, 1000) */
                for(int k=0;k<p->joint_count;k++)if(p->joints[k] && p->joint_break[k]>0 && (p->joints[k]->GetBodyA()==b || p->joints[k]->GetBodyB()==b)){p->world.DestroyJoint(p->joints[k]);p->joints[k]=NULL;}
                if(!p->shock_hit[i]){
                    p->shock_hit[i]=true;
                    float damage=(SHOCK_MIN_DAMAGE+(1-t)*(SHOCK_DAMAGE-SHOCK_MIN_DAMAGE))*o.strength-o.defence;
                    if(damage>0 && o.kind!=4)p->health[i]-=damage;
                }
                if(force>=1)b->ApplyForceToCenter(force*d,true);
            }
        }
    }
    for(int i=0;i<p->level->count;i++)if(p->electrocute[i]>0){
        g->cooldown=0;
        if((p->electrocute[i]-=dt)<=0){p->electrocute[i]=0;if(p->bodies[i])p->health[i]=0;}
    }
    if(p->egg && p->egg_collided){
        radial_blast(g,p->egg->GetPosition(),EGG_BLAST_RADIUS,EGG_BLAST_FORCE,EGG_DAMAGE_RADIUS,EGG_BLAST_DAMAGE);
        p->world.DestroyBody(p->egg);p->egg=NULL;p->egg_collided=false;g->flight_time=0;
    }
    for(int i=0;i<p->level->count;i++) {
        b2Body *body=p->bodies[i];if(!body)continue;
        const CampaignObject &o=p->level->objects[i];
        if(p->vial_wait[i]>0 && (p->vial_wait[i]-=dt)<=0){
            if(p->vial_kind[i]==VIAL_ORANGE)p->vial_pop[i]=VIAL_ORANGE_POP;
            else {p->bubble_timer[i]=BUBBLE_TIME;p->bubble_float[i]=0;p->vial_shield[i]=true;p->disturbed[i]=true;}
        }
        if(p->vial_pop[i]>0 && (p->vial_pop[i]-=dt)<=0)p->health[i]=0;
        if(p->vial_shield[i] && p->bubble_timer[i]<=0)p->vial_shield[i]=false;
        if(p->vial_shield[i])p->impact[i]=0; /* ignoreAllDamage while in the pink bubble */
        float before=p->health[i],dealt=0;
#ifndef PS2BIRDS_EXACT_COLLISION
        if(o.kind!=3 && p->impact[i]>3){dealt=(p->impact[i]-3)*12/fmaxf(o.defence,1);p->health[i]-=dealt;
            /* CollisionCallbacks.lua: each hit that deals damage scores floor(damage applied) x 10 (a killing hit applies what strength was left). */
            g->score+=(int)floorf(fminf(dealt,fmaxf(before,0)))*10;}
#else
        if(o.kind!=3 && p->impact[i]>0){dealt=p->impact[i];p->health[i]-=dealt;
            /* CollisionCallbacks.lua: each hit that deals damage scores floor(damage applied) x 10 (a killing hit applies what strength was left). */
            g->score+=(int)floorf(fminf(dealt,fmaxf(before,0)))*10;}
#endif
        if(dealt>0 && p->health[i]>0)g->events|=sound_family(o.definition)==1?GAME_EVENT_DMG_BONE:o.kind==1?GAME_EVENT_DMG_WOOD:o.kind==2?GAME_EVENT_DMG_GLASS:o.kind==5?GAME_EVENT_DMG_STONE:o.kind==4?GAME_EVENT_DMG_PIG:0;   /* hurt but standing: the material's damage sound */
        p->impact[i]=0;
        /* blocks.lua <Bird>DamageFactors.velocityMultiplier: when the bird's hit breaks the piece,
           it keeps its speed before the hit x min(1, leftover damage share x multiplier)
           (AdventureModeCollisions.lua). Applied where the multiplier is above 1. */
#ifndef PS2BIRDS_NO_VELOCITY_MULTIPLIER   /* on since 0.90.0; the stage scripts were re-solved with it */
        if(p->bird_hit[i] && p->bird && before>0 && p->health[i]<=0 && dealt>0){
            float m=velocity_multiplier(p->bird_type,o.kind,p->puffer_state>0);
            if(m>1){
                b2Vec2 v=fminf(1.0f,(dealt-before)/dealt*m)*p->bird_pre_velocity;
                if(v.LengthSquared()>p->bird->GetLinearVelocity().LengthSquared())p->bird->SetLinearVelocity(v);
            }
        }
#else
        (void)before;
#endif
        p->bird_hit[i]=false;
        if(p->chain_delay[i]>0){p->chain_delay[i]-=dt;if(p->chain_delay[i]>0)continue;}
        if(IS_FRAGILE_KIND(o.kind) && p->fragile_hit[i])p->health[i]=0;
        if(o.kind!=3 && ((p->health[i]<=0 && i!=p->egg_i) || body->GetPosition().y>p->level->ground_y+25)) {
            b2Vec2 where=body->GetPosition();bool exploded=p->explosive(i) && p->health[i]<=0;
            p->world.DestroyBody(body);p->bodies[i]=NULL;p->release_spawns(i,where);
            if(p->egg_mode && OBJECT_WAVE(o.flags)){EggVehicle &v=p->vehicles[OBJECT_WAVE(o.flags)];if(v.active && v.c && v.c->block_removed)v.c->block_removed(&v,piece_name(&o));}
            if(i==p->egg_grabber){p->egg_grabber=-1;p->egg_tween=-1;if(p->egg_i>=0 && p->bodies[p->egg_i])p->set_mask(p->bodies[p->egg_i],0xFFFF);}  /* releaseEgg */
            /* A destroyed TNT crate blasts its surroundings; crates caught in the blast
               go off in the following ticks, giving chain reactions. */
            if(exploded){const CampaignForce *f=p->force_of[i];
                if(f && f->blast_radius>0)radial_blast(g,where,f->blast_radius,f->blast_force,f->damage_radius,f->damage);
                else radial_blast(g,where,TNT_BLAST_RADIUS,TNT_BLAST_FORCE,TNT_DAMAGE_RADIUS,TNT_BLAST_DAMAGE);}
            if(o.flags&(OBJECT_GUN_BUBBLE|OBJECT_CANNON_BALL)){}
            else if(o.kind==4 && p->is_goal(i)){g->pig_alive--;{int pts=o.score>=0?(int)o.score:5000;g->score+=pts;popup_at(g,where,pts,POPUP_PIG,4);}g->events|=GAME_EVENT_PIG|(sound_family(o.definition)==4?GAME_EVENT_MONKEY:0);}else {g->block_alive--;int earned=o.score>=0?(int)o.score:o.kind==OBJECT_KIND_FRAGILE?FRAGILE_DESTROYED_SCORE:o.kind==OBJECT_KIND_FRAGILE_GROUND?0:500;   /* getScoreToAddForBreaking: the piece's destroyedScoreInc, else 500 */if(g->shop_mask&SHOP_DOUBLE)earned*=2;g->score+=earned;popup_at(g,where,earned,POPUP_PIECE,o.kind);{const unsigned fe=family_destroyed_event(sound_family(o.definition));g->events|=fe?fe:(unsigned)(o.kind==1?GAME_EVENT_WOOD:o.kind==2?GAME_EVENT_GLASS:o.kind==5?GAME_EVENT_STONE:GAME_EVENT_EXTRA);}}
        }
    }
    g->pig_alive+=p->pending_pigs;p->pending_pigs=0;
    /* checkLevelComplete: in Egg Defender the egg must also be back in its nest. */
    bool egg_home=true;
    if(p->egg_mode && p->egg_i>=0 && !p->bodies[p->egg_i])egg_home=false;
    if(p->egg_mode && p->egg_i>=0 && p->bodies[p->egg_i]){egg_home=false;b2Body *e=p->bodies[p->egg_i];
        b2Body *n=p->nest_i>=0?p->bodies[p->nest_i]:NULL;
        for(int k=0;k<p->joint_count;k++)if(p->joints[k] && ((p->joints[k]->GetBodyA()==e && p->joints[k]->GetBodyB()==n)||(p->joints[k]->GetBodyB()==e && p->joints[k]->GetBodyA()==n)))egg_home=true;
        if(!n && p->egg_grabber<0 && p->egg_tween<0)egg_home=true;}
    if(g->pig_alive-p->escaped_pigs<=0 && egg_home) {
        g->score+=g->birds*10000;g->state=WON;
        g->highscores[g->level]=g->score>g->highscores[g->level]?g->score:g->highscores[g->level];
        int unlocked=g->level+2;if(unlocked>game_level_count())unlocked=game_level_count();
        if(unlocked>g->unlocked)g->unlocked=unlocked;
        return;
    }
    if(g->state!=FLYING)return;
    g->flight_time+=dt;
    b2Vec2 pos=p->bird?p->bird->GetPosition():p->blast_position,vel=p->bird?p->bird->GetLinearVelocity():b2Vec2(0,0);
    g->x=g->view_x+pos.x*g->view_scale;g->y=g->view_y+pos.y*g->view_scale;
    g->vx=vel.x*g->view_scale;g->vy=vel.y*g->view_scale;
    bool moving=false;
    for(int i=0;i<game_projectile_count(g);i++){
        b2Body *body=i==0?p->bird:p->egg?p->egg:p->split[i-1];b2Vec2 position=body->GetPosition();
        if(body->GetLinearVelocity().LengthSquared()>=0.3f && position.x<=fmaxf(65,(660-g->view_x)/g->view_scale) && position.x>=fminf(-40,(-20-g->view_x)/g->view_scale) && position.y<=p->level->ground_y+25)moving=true;
    }
    if(moving)g->cooldown=0;else g->cooldown+=dt;
    if(g->cooldown>2.5f || (g->flight_time>10 && !((p->bird_type==BIRD_BLACK_TYPE||p->bird_type==BIRD_SHOCK_TYPE) && p->bird_collided && !p->detonated) && p->shock_time<0)) {
        if(p->egg){p->world.DestroyBody(p->egg);p->egg=NULL;p->egg_collided=false;}
        if(g->birds>0){g->state=AIMING;g->x=g->sling_x;g->y=g->sling_y;}
        else g->state=LOST;
    }
}
void game_camera(Game *g,float seconds){
    if(g->cam_max<=0)return;
    float want=g->cam_x;
    if(g->state==AIMING)want=0;
    else{GameBird b;float far_x=-1e9f;
        for(int i=0;game_get_projectile(g,i,&b);i++){float x=b.x+g->cam_x;if(x>far_x)far_x=x;}
        if(far_x>-1e8f){want=far_x-360;if(want<g->cam_x && g->state!=WON && g->state!=LOST)want=g->cam_x;}}
    want=fminf(fmaxf(want,0.0f),g->cam_max);
    float k=fminf(seconds*4,1.0f);float next=g->cam_x+(want-g->cam_x)*k,delta=next-g->cam_x;
    g->cam_x=next;g->view_x=g->base_view_x-next;g->sling_x=g->base_sling_x-next;
    if(g->state==AIMING){g->x-=delta;}
    for(int i=0;i<GAME_POPUPS;i++)g->popups[i].x-=delta;
    g->explosion_x-=delta;
}
void game_update(Game *g,float seconds) {
    g->events=0;
    Physics *p=static_cast<Physics*>(g->physics);p->blast_life=fmaxf(0,p->blast_life-clamp(seconds,0,0.1f));
    /* Floating scores keep running after the end (the birds-left bonus appears then). */
    for(int k=0;k<GAME_POPUPS;k++)if(g->popups[k].time<g->popups[k].life)g->popups[k].time+=clamp(seconds,0,0.1f);
    if(g->state==WON || g->state==LOST)return;
    g->accumulator+=clamp(seconds,0,0.1f);
    while(g->accumulator+0.000001f>=1.0f/120 && g->state!=WON && g->state!=LOST) {
        tick(g);g->accumulator-=1.0f/120;
    }
}
int game_bird_trapped(const Game *g){return static_cast<const Physics*>(g->physics)->trapped_by>=0;}
int game_homing_available(const Game *g){const Physics *p=static_cast<const Physics*>(g->physics);return (g->shop_mask&SHOP_HOMING) && !p->homing_used && g->state==FLYING && p->bird && !p->bird_collided;}
int game_use_homing(Game *g){if(!game_homing_available(g))return 0;Physics *p=static_cast<Physics*>(g->physics);p->homing_used=true;p->homing_on=true;return 1;}
int game_powerup(const Game *g){return static_cast<const Physics*>(g->physics)->level->powerup;}
int game_powerup_available(const Game *g){const Physics *p=static_cast<const Physics*>(g->physics);
    if(p->level->powerup==POWERUP_BIRD_QUAKE)return !p->quake_used && (g->state==AIMING||g->state==FLYING);
    if(p->level->powerup==POWERUP_POWER_POTION)return !p->potion_used && g->state==AIMING;
    if(p->level->powerup==POWERUP_SHOCKWAVE)return !p->shock_used && g->state==AIMING && g->birds>0;
    return 0;}
int game_use_powerup(Game *g){
    if(!game_powerup_available(g))return 0;
    Physics *p=static_cast<Physics*>(g->physics);
    if(p->level->powerup==POWERUP_POWER_POTION){p->potion_used=true;p->potion_armed=true;return 1;}
    if(p->level->powerup==POWERUP_SHOCKWAVE){p->shock_used=true;p->shock_bonus=true;return 1;}
    p->quake_used=true;p->quake_time=BIRD_QUAKE_TIME;return 1;
}
void game_damage_object(Game *g,int index){Physics *p=static_cast<Physics*>(g->physics);if(index>=0 && index<p->level->count)p->health[index]=0;}
int game_object_count(const Game *g) {return static_cast<const Physics*>(g->physics)->level->count;}

void game_add_popup(Game *g,float x,float y,int score,int kind,int material){
    if(score<=0)return;
    GamePopup *p=&g->popups[g->popup_next];g->popup_next=(g->popup_next+1)%GAME_POPUPS;
    p->x=x;p->y=y;p->score=score;p->kind=kind;p->material=material;p->time=0;
    p->life=kind==POPUP_PIECE?0.6f:0.9f;p->scale=kind==POPUP_PIECE?0.25f+score/3000.0f:1.0f;
}
float game_popup_scale(const GamePopup *p){
    if(p->life<=0 || p->time<0 || p->time>=p->life)return 0;   /* negative time: waiting its turn */
    if(p->time<0.25f)return p->scale*p->time/0.25f;
    if(p->time<p->life-0.25f)return p->scale;
    return p->scale*(p->life-p->time)/0.25f;
}
static void popup_at(Game *g,b2Vec2 where,int score,int kind,int material){game_add_popup(g,g->view_x+where.x*g->view_scale,g->view_y+where.y*g->view_scale,score,kind,material);}
int game_get_object(const Game *g,int index,GameObject *out) {
    const Physics *p=static_cast<const Physics*>(g->physics);
    if(index<0 || index>=p->level->count || !out)return 0;
    const CampaignObject &o=p->level->objects[index];
    b2Body *body=p->bodies[index];memset(out,0,sizeof(*out));out->alive=body!=NULL;out->kind=o.kind;out->definition=o.definition;out->mirror=o.flags&(OBJECT_MIRROR_X|OBJECT_MIRROR_Y);
    out->width=o.width*g->view_scale;out->height=o.height*g->view_scale;out->health=p->health[index]/o.strength;
    if(body){out->x=g->view_x+body->GetPosition().x*g->view_scale;out->y=g->view_y+body->GetPosition().y*g->view_scale;out->angle=body->GetAngle();}
    return 1;
}

/* The vehicle controllers are compiled with the game (one translation unit for every build). */
#include "egg_vehicles.cpp"

/* Egg Defender red bird: how many times a pig became the target (pigTargeted) and how many
   times the special was fired at one (redBirdTriggered) since the stage was loaded. */
int game_red_notifications(const Game *g,int triggered){
    const Physics *p=static_cast<const Physics*>(g->physics);if(!p)return 0;
    return triggered?p->red_triggered_count:p->red_targeted_count;
}
