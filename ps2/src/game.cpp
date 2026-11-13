#include "game.h"
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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef PS2BIRDS_RUNTIME_CAMPAIGN
/* The stages are not part of the program: they come from CAMPAIGN.BIN, generated from the
   player's own APK by tools/gerar_iso.py (layout below and in tools/build_campaign.py). */
static const CampaignPolygon no_polygons[1]={{0,0,{0}}};
static const CampaignForce no_forces[1]={{0,0,0}};
static CampaignLevel *campaign_levels;
static const CampaignPolygon *campaign_polygons=no_polygons;
static const CampaignForce *campaign_forces=no_forces;
static unsigned campaign_count;
#define CAMPAIGN_COUNT campaign_count
static uint32_t rd32(const unsigned char *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static float rdf(const unsigned char *p){uint32_t v=rd32(p);float f;memcpy(&f,&v,4);return f;}
int game_campaign_load(const unsigned char *data,unsigned size)
{
    if(size<32 || memcmp(data,"PBCP",4) || rd32(data+4)!=2)return -1;
    unsigned nl=rd32(data+8),no=rd32(data+12),nj=rd32(data+16),np=rd32(data+20),nf=rd32(data+24),ns=rd32(data+28);
    const unsigned LV=4*(3+16+8+2+1+2+2),OB=4*14,JO=4*12,PO=4*18,FO=4*3;
    if(nl==0 || nl>MAX_CAMPAIGN_LEVELS || 32ull+(unsigned long long)nl*LV+(unsigned long long)no*OB+(unsigned long long)nj*JO+np*PO+nf*FO+ns!=size)return -1;
    const unsigned char *pl=data+32,*po=pl+nl*LV,*pj=po+no*OB,*pp=pj+nj*JO,*pf=pp+np*PO,*pstr=pf+nf*FO;
    char *strings=(char*)malloc(ns+1);CampaignLevel *levels=(CampaignLevel*)calloc(nl,sizeof(CampaignLevel));
    CampaignObject *objects=(CampaignObject*)calloc(no?no:1,sizeof(CampaignObject));CampaignJoint *joints=(CampaignJoint*)calloc(nj?nj:1,sizeof(CampaignJoint));
    CampaignPolygon *polys=(CampaignPolygon*)calloc(np+1,sizeof(CampaignPolygon));CampaignForce *forces=(CampaignForce*)calloc(nf+1,sizeof(CampaignForce));
    if(!strings||!levels||!objects||!joints||!polys||!forces){free(strings);free(levels);free(objects);free(joints);free(polys);free(forces);return -1;}
    memcpy(strings,pstr,ns);strings[ns]=0;
#define STR(o) (rd32(o)<ns?strings+rd32(o):strings)
    for(unsigned i=0;i<no;i++){
        const unsigned char *q=po+i*OB;CampaignObject &o=objects[i];
        o.x=rdf(q);o.y=rdf(q+4);o.angle=rdf(q+8);o.width=rdf(q+12);o.height=rdf(q+16);o.density=rdf(q+20);o.strength=rdf(q+24);o.defence=rdf(q+28);
        o.kind=(int)rd32(q+32);o.shape=(int)rd32(q+36);o.definition=STR(q+40);o.friction=rdf(q+44);o.restitution=rdf(q+48);o.spawn_of=(int)rd32(q+52);if(o.spawn_of<0||(unsigned)o.spawn_of>no)o.spawn_of=0;
    }
    for(unsigned i=0;i<nj;i++){
        const unsigned char *q=pj+i*JO;CampaignJoint &j=joints[i];
        j.type=(int)rd32(q);j.a=(int)rd32(q+4);j.b=(int)rd32(q+8);j.ax=rdf(q+12);j.ay=rdf(q+16);j.bx=rdf(q+20);j.by=rdf(q+24);
        j.frequency=rdf(q+28);j.damping=rdf(q+32);j.break_force=rdf(q+36);j.collide=(int)rd32(q+40);j.destroy_timer=rdf(q+44);
    }
    for(unsigned i=0;i<np;i++){const unsigned char *q=pp+i*PO;polys[i].definition=STR(q);polys[i].count=(int)rd32(q+4);for(int k=0;k<16;k++)polys[i].v[k]=rdf(q+8+4*k);}
    for(unsigned i=0;i<nf;i++){const unsigned char *q=pf+i*FO;forces[i].definition=STR(q);forces[i].fx=rdf(q+4);forces[i].fy=rdf(q+8);}
    for(unsigned i=0;i<nl;i++){
        const unsigned char *q=pl+i*LV;CampaignLevel &l=levels[i];
        unsigned o0=rd32(q),oc=rd32(q+4),j0,jc;
        if(o0+oc>no||oc>MAX_LEVEL_OBJECTS)goto bad;
        l.objects=objects+o0;l.count=(int)oc;l.birds=(int)rd32(q+8);if(l.birds<0||l.birds>16)goto bad;
        for(int k=0;k<16;k++)l.bird_types[k]=(int)rd32(q+12+4*k);
        q+=76;l.gravity=rdf(q);l.sling_x=rdf(q+4);l.sling_y=rdf(q+8);l.ground_y=rdf(q+12);l.pixels=rdf(q+16);l.view_x=rdf(q+20);l.view_y=rdf(q+24);l.view_scale=rdf(q+28);
        l.title=STR(q+32);l.script=STR(q+36);l.bank=(int)rd32(q+40);j0=rd32(q+44);jc=rd32(q+48);
        if(j0+jc>nj||jc>MAX_LEVEL_JOINTS)goto bad;
        l.joints=jc?joints+j0:0;l.joint_count=(int)jc;l.star_silver=(int)rd32(q+52);l.star_gold=(int)rd32(q+56);
    }
#undef STR
    campaign_levels=levels;campaign_polygons=polys;campaign_forces=forces;campaign_count=nl;return (int)nl;
bad:free(strings);free(levels);free(objects);free(joints);free(polys);free(forces);return -1;
}
int game_campaign_load_file(const char *path)
{
    /* open/read in large blocks (stdio reads from the disc in tiny requests). */
    int fd=open(path,O_RDONLY);if(fd<0)return -1;
    long n=(long)lseek(fd,0,SEEK_END);lseek(fd,0,SEEK_SET);
    unsigned char *buffer=n>0?(unsigned char*)malloc((size_t)n):0;int result=-1;long done=0;
    while(buffer && done<n){long k=n-done>262144?262144:n-done;int r=(int)read(fd,buffer+done,(size_t)k);if(r<=0)break;done+=r;}
    if(buffer && done==n)result=game_campaign_load(buffer,(unsigned)n);
    close(fd);free(buffer);return result;
}
#if !defined(_EE)
/* Host builds (tests and solvers) read the generated file before main(). */
static struct HostCampaign{HostCampaign(){const char *p=getenv("PS2BIRDS_CAMPAIGN");if(game_campaign_load_file(p?p:"assets/CAMPAIGN.BIN")<0)fprintf(stderr,"CAMPAIGN.BIN missing: generate it from the APK (tools/gerar_iso.py)\n");}} host_campaign;
#endif
#else
static_assert(CAMPAIGN_COUNT<=MAX_CAMPAIGN_LEVELS,"Campaign exceeds progress capacity");
int game_campaign_load(const unsigned char *,unsigned){return (int)CAMPAIGN_COUNT;}
int game_campaign_load_file(const char *){return (int)CAMPAIGN_COUNT;}
#endif

static float clamp(float v,float lo,float hi) { return v<lo?lo:v>hi?hi:v; }
struct Physics;
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
    int puffer_state;float puffer_timer,puffer_auto,puffer_direction;unsigned puffer_seed;
    float pink_timer,bird_bubble;bool pink_hover;float bubble_timer[MAX_LEVEL_OBJECTS],bubble_float[MAX_LEVEL_OBJECTS];
    int bird_type;
    b2Body *split[2],*egg;bool egg_collided;
    bool split_collided[2],disturbed[MAX_LEVEL_OBJECTS];
    bool bomb_requested,detonated;
    float bomb_fuse,blast_life,blast_radius;
    float chain_delay[MAX_LEVEL_OBJECTS];
    b2Vec2 blast_position;
    b2Contact *damage_contacts[MAX_LEVEL_OBJECTS*2];int damage_contact_count;
    /* Static scenery the launched bird overlapped at spawn; ignored until they separate. */
    b2Body *spawn_ignore[8];int spawn_ignore_count;
    b2Body *floor_body;
    b2Joint *joints[MAX_LEVEL_JOINTS];float joint_break[MAX_LEVEL_JOINTS];int joint_count;
    float detach_time[MAX_LEVEL_JOINTS];int detach_source[MAX_LEVEL_JOINTS],detach_target[MAX_LEVEL_JOINTS];int detach_count;
    const CampaignForce *force_of[MAX_LEVEL_OBJECTS];
    Physics(const CampaignLevel *l):world(b2Vec2(0,l->gravity)),level(l),bird(NULL),age(0),has_launched(false),special_used(false),bird_collided(false),boomerang_active(false),boomerang_x_force(0),boomerang_y_force(0),boomerang_spin(0),puffer_state(0),puffer_timer(0),puffer_auto(-1),puffer_direction(0),puffer_seed(12345u),pink_timer(-1),bird_bubble(0),pink_hover(false),bird_type(BIRD_RED_TYPE) {
        egg=NULL;egg_collided=false;bomb_requested=detonated=false;bomb_fuse=-1;blast_life=0;blast_radius=0;blast_position.SetZero();
        damage_contact_count=0;spawn_ignore_count=0;split[0]=split[1]=NULL;split_collided[0]=split_collided[1]=false;
        memset(chain_delay,0,sizeof(chain_delay));memset(disturbed,0,sizeof(disturbed));memset(bodies,0,sizeof(bodies)); memset(impact,0,sizeof(impact));memset(force_of,0,sizeof(force_of));memset(bubble_timer,0,sizeof(bubble_timer));memset(bubble_float,0,sizeof(bubble_float));
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
            if(o.spawn_of)continue; /* released later by release_spawns() */
            create_body(i,o.x,o.y,o.angle);
        }
        for(int i=0;i<level->joint_count && i<MAX_LEVEL_JOINTS;i++)create_joint(level->joints[i]);
    }
    /* deathSpawn (gamelogic.lua): the pieces stored for a destroyed parent appear where it was. */
    void release_spawns(int parent,b2Vec2 where){
        for(int j=0;j<level->count;j++)if(level->objects[j].spawn_of==parent+1 && !bodies[j])create_body(j,where.x,where.y,level->objects[j].angle);
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
                box.Set(v,3);fixture.shape=&box;
            }
            else if(o.shape==SHAPE_POLYGON){
                /* ObjectCreation.lua: vertex = (x*width - width/2, y*height - height/2). */
                const CampaignPolygon *poly=NULL;
                for(const CampaignPolygon *p=campaign_polygons;p->definition;p++)if(!strcmp(p->definition,o.definition)){poly=p;break;}
                if(poly){b2Vec2 v[8];for(int k=0;k<poly->count && k<8;k++)v[k].Set(poly->v[2*k]*o.width-o.width/2,poly->v[2*k+1]*o.height-o.height/2);box.Set(v,poly->count);}
                else box.SetAsBox(o.width/2,o.height/2);
                fixture.shape=&box;
            }
            else {box.SetAsBox(o.width/2,o.height/2);fixture.shape=&box;}
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
            made=world.CreateJoint(&def);
        }
        if(made){
            /* Port approximation: a breakable joint fails when its reaction force exceeds
               breakForce x (mass of both bodies) x gravity. */
            joint_break[joint_count]=j.break_force>0?j.break_force*(a->GetMass()+b->GetMass())*level->gravity:0;
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
        b2CircleShape circle;circle.m_radius=type==BIRD_PINK_TYPE?PINK_RADIUS:type==BIRD_PUFFER_TYPE?GLOBE_RADIUS:type==BIRD_BIG_TYPE?BIG_RADIUS:type==BIRD_GREEN_TYPE?GREEN_RADIUS:type==BIRD_BLUE_TYPE?0.6f:type==BIRD_YELLOW_TYPE?0.8f:type==BIRD_BLACK_TYPE?1.0f:type==BIRD_WHITE_TYPE?1.3f:type==BIRD_EGG_TYPE?WHITE_EGG_RADIUS:0.75f;
        b2FixtureDef fixture;fixture.shape=&circle;fixture.density=type==BIRD_PINK_TYPE?PINK_DENSITY:type==BIRD_PUFFER_TYPE?GLOBE_DENSITY:type==BIRD_BIG_TYPE?BIG_DENSITY:type==BIRD_GREEN_TYPE?GREEN_DENSITY:(type==BIRD_BLUE_TYPE || type==BIRD_WHITE_TYPE)?4.5f:type==BIRD_EGG_TYPE?3:6;
        fixture.friction=0.3f;fixture.restitution=type==BIRD_PINK_TYPE?PINK_RESTITUTION:type==BIRD_PUFFER_TYPE?GLOBE_RESTITUTION:type==BIRD_BIG_TYPE?BIG_RESTITUTION:type==BIRD_GREEN_TYPE?GREEN_RESTITUTION:(type==BIRD_YELLOW_TYPE || type==BIRD_WHITE_TYPE || type==BIRD_EGG_TYPE)?0.23f:type==BIRD_BLACK_TYPE?0.03f:0.25f;
        if(type==BIRD_BLUE_TYPE)fixture.filter.groupIndex=-1;
        if(type==BIRD_WHITE_TYPE || type==BIRD_EGG_TYPE)fixture.filter.groupIndex=-2;
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
            for(b2Body *body:pair){
                uintptr_t id=body->GetUserData().pointer;
                if(id>0 && id<=(uintptr_t)level->count && body->GetType()==b2_dynamicBody)disturbed[id-1]=true;
            }
            if(damage_contact_count<MAX_LEVEL_OBJECTS*2)damage_contacts[damage_contact_count++]=contact;
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
        bool damaging=false;for(int i=0;i<damage_contact_count;i++)if(damage_contacts[i]==contact){damaging=true;break;}
        if(!damaging)return;
        float normal=0;for(int i=0;i<impulse->count;i++)normal+=impulse->normalImpulses[i];
        b2Body *pair[]={contact->GetFixtureA()->GetBody(),contact->GetFixtureB()->GetBody()};
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
                if(projectile && bird_type==BIRD_BLACK_TYPE && level->objects[i].kind==5)factor=4.0f;
                /* BoomerangBirdDamageFactors: wood 3, rock 1.5, light 3. */
                /* RedBigBirdDamageFactors: wood, rock and light 0.3. */
                if(projectile && bird_type==BIRD_BIG_TYPE && level->objects[i].kind!=4)factor=BIG_DAMAGE_FACTOR;
                /* PinkBirdDamageFactors: wood, rock and light 0.5. */
                if(projectile && bird_type==BIRD_PINK_TYPE && level->objects[i].kind!=4)factor=PINK_DAMAGE_FACTOR;
                /* GlobeBirdDamageFactors: wood 3, rock 1.5, light 3; GlobeBirdBigDamageFactors (puffed): 0. */
                if(projectile && bird_type==BIRD_PUFFER_TYPE && level->objects[i].kind!=4)factor=puffer_state>0?0.0f:level->objects[i].kind==1||level->objects[i].kind==2?3.0f:level->objects[i].kind==5?1.5f:1.0f;
                if(projectile && bird_type==BIRD_GREEN_TYPE)factor=level->objects[i].kind==1||level->objects[i].kind==2?3.0f:level->objects[i].kind==5?1.5f:1.0f;
                impact[i]=fmaxf(impact[i],normal/body->GetMass()*factor);
            }
        }
    }
};
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
int game_level_bank(int index){return index>=0 && index<(int)CAMPAIGN_COUNT?campaign_levels[index].bank:0;}
const char *game_level_script(int index){return index>=0 && index<(int)CAMPAIGN_COUNT?campaign_levels[index].script:"";}
int game_load_level(Game *g,int index) {
    if(index<0 || index>=(int)CAMPAIGN_COUNT || index>=g->unlocked)return 0;
    int unlocked=g->unlocked,best[MAX_CAMPAIGN_LEVELS];memcpy(best,g->highscores,sizeof(best));
    game_destroy(g);memset(g,0,sizeof(*g));g->level=index;g->unlocked=unlocked;memcpy(g->highscores,best,sizeof(best));
    const CampaignLevel *l=&campaign_levels[index];g->physics=new Physics(l);
    g->view_x=l->view_x;g->view_y=l->view_y;g->view_scale=l->view_scale;g->pixels=l->pixels;
    g->ground_y=l->view_y+l->ground_y*l->view_scale;g->gravity=l->gravity;
    g->sling_x=l->view_x+l->sling_x*l->view_scale;g->sling_y=l->view_y+l->sling_y*l->view_scale;
    g->x=g->sling_x;g->y=g->sling_y;g->angle=30;g->power=75;g->birds=l->birds;g->state=AIMING;
    for(int i=0;i<l->count;i++){if(l->objects[i].kind==4)g->pig_alive++;if(l->objects[i].kind!=3 && l->objects[i].kind!=4)g->block_alive++;}
    return 1;
}
void game_reset(Game *g){game_load_level(g,g->level);}
int game_next_level(Game *g){return g->state==WON?game_load_level(g,g->level+1):0;}
void game_aim(Game *g,float a,float p) {
    if(g->state!=AIMING)return;
    /* Callers pass deltas sized for the earlier 260-wide power scale; keep the same feel. */
    g->angle=clamp(g->angle+a,ANGLE_MIN,ANGLE_MAX);g->power=clamp(g->power+p*(POWER_MAX-POWER_MIN)/260.0f,POWER_MIN,POWER_MAX);
}
void game_launch(Game *g) {
    if(g->state!=AIMING || g->birds<=0)return;
    Physics *p=static_cast<Physics*>(g->physics);
    if(p->bird)p->world.DestroyBody(p->bird);
    if(p->egg)p->world.DestroyBody(p->egg);
    p->egg=NULL;p->egg_collided=false;
    p->spawn_ignore_count=0;
    for(int i=0;i<2;i++){if(p->split[i])p->world.DestroyBody(p->split[i]);p->split[i]=NULL;p->split_collided[i]=false;}
    p->bird_type=game_queue_type(g,0);p->special_used=false;p->bird_collided=false;p->boomerang_active=false;p->boomerang_spin=0;p->puffer_state=0;p->puffer_timer=0;p->puffer_auto=-1;p->puffer_direction=0;p->puffer_seed=12345u;p->pink_timer=-1;p->bird_bubble=0;p->pink_hover=false;
    g->angle=clamp(g->angle,ANGLE_MIN,ANGLE_MAX);g->power=clamp(g->power,POWER_MIN,POWER_MAX);
    float a=g->angle*0.01745329252f;
    p->bird=p->make_bird(b2Vec2(p->level->sling_x,p->level->sling_y),b2Vec2(cosf(a)*LAUNCH_SPEED_MAX*g->power/100.0f,-sinf(a)*LAUNCH_SPEED_MAX*g->power/100.0f),p->bird_type);
    p->record_spawn_overlaps();
    p->bomb_requested=p->detonated=false;p->bomb_fuse=-1;
    p->has_launched=true;g->birds--;g->flight_time=g->cooldown=0;g->state=FLYING;
}
int game_queue_type(const Game *g,int offset){
    const Physics *p=static_cast<const Physics*>(g->physics);
    if(offset<0 || offset>=g->birds)return -1;
    return p->level->bird_types[p->level->birds-g->birds+offset];
}
int game_special_available(const Game *g){
    const Physics *p=static_cast<const Physics*>(g->physics);
    if(g->state!=FLYING || p->special_used || !p->bird)return 0;
    if(p->bird_type==BIRD_BLACK_TYPE)return 1;
    if(p->bird_type==BIRD_PUFFER_TYPE)return 1; /* specialtyAvailableAfterCollision */
    if(p->bird_type==BIRD_PINK_TYPE)return !p->special_used && p->pink_timer<0;
    return (p->bird_type==BIRD_BLUE_TYPE || p->bird_type==BIRD_YELLOW_TYPE || p->bird_type==BIRD_WHITE_TYPE || p->bird_type==BIRD_GREEN_TYPE) && !p->bird_collided;
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
    if(p->bird_type==BIRD_BLACK_TYPE){p->bomb_requested=true;p->special_used=true;return 1;}
    b2Vec2 velocity=p->bird->GetLinearVelocity();
    if(p->bird_type==BIRD_WHITE_TYPE){
        p->egg=p->make_bird(p->bird->GetPosition()+b2Vec2(0,1.3f+WHITE_EGG_RADIUS+0.1f),b2Vec2(0,WHITE_DROP_SPEED),BIRD_EGG_TYPE);
        p->bird->SetLinearVelocity(b2Vec2(velocity.x*WHITE_RECOIL_X_FACTOR,-WHITE_RECOIL_SPEED));
        p->special_used=true;p->egg_collided=false;g->cooldown=0;return 1;
    }
    if(p->bird_type==BIRD_PUFFER_TYPE){puff(g);return 1;}
    if(p->bird_type==BIRD_PINK_TYPE){p->pink_timer=PINK_BOOM_TIME;p->special_used=true;g->cooldown=0;return 1;}
    if(p->bird_type==BIRD_GREEN_TYPE){
        float mass=p->bird->GetMass(),coeff=velocity.x!=0?2.0f-fminf(fabsf(velocity.y/velocity.x),2.0f):0.0f;
        p->boomerang_x_force=velocity.x*GREEN_PHYSICS_SCALE*mass*GREEN_HORIZONTAL_FORCE;
        p->boomerang_y_force=coeff*GREEN_PHYSICS_SCALE*mass*GREEN_VERTICAL_FORCE;
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
    out->vx=vel.x*g->view_scale;out->vy=vel.y*g->view_scale;out->angle=body->GetAngle();out->type=body==p->egg?BIRD_EGG_TYPE:p->bird_type;out->special_used=p->special_used;out->fuse=p->bird_type==BIRD_BLACK_TYPE?p->bomb_fuse:p->bird_type==BIRD_PUFFER_TYPE&&index==0&&p->puffer_state?body->GetFixtureList()->GetShape()->m_radius/GLOBE_PUFFED_RADIUS:-1;out->collided=index==0?p->bird_collided:body==p->egg?p->egg_collided:p->split_collided[index-1];return 1;
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
        float impulse=(force/120.0f)*(1-distance/radius);
        body->ApplyLinearImpulseToCenter(impulse*direction,true);
        if(distance<damage_radius){
            bool alive=p->health[i]>0;p->health[i]-=damage*(1-distance/damage_radius);
            if(alive && p->health[i]<=0 && p->level->objects[i].kind==OBJECT_KIND_TNT)p->chain_delay[i]=TNT_CHAIN_SECONDS;
        }
    }
    g->events|=GAME_EVENT_EXPLOSION;g->cooldown=0;
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
    float speed=LAUNCH_SPEED_MAX*g->power/100.0f*g->view_scale;
    *x=g->sling_x+cosf(a)*speed*t;
    *y=g->sling_y-sinf(a)*speed*t+0.5f*g->gravity*g->view_scale*t*(t+1.0f/120.0f);
}
static void tick(Game *g) {
    Physics *p=static_cast<Physics*>(g->physics); const float dt=1.0f/120;
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
    for(int i=0;i<p->level->count;i++)if(p->force_of[i] && p->bodies[i]){b2Body *b=p->bodies[i];float m=b->GetMass();b->ApplyForceToCenter(b2Vec2(p->force_of[i]->fx*m,p->force_of[i]->fy*m),true);}
    p->age+=dt;p->damage_contact_count=0;p->world.Step(dt,8,3);
    for(int k=0;k<p->detach_count;k++){
        int t=p->detach_target[k];
        if(t<0 || p->bodies[p->detach_source[k]])continue;
        if(p->detach_time[k]>0){p->detach_time[k]-=dt;continue;}
        if(p->bodies[t]){
            {b2Vec2 where=p->bodies[t]->GetPosition();p->world.DestroyBody(p->bodies[t]);p->bodies[t]=NULL;p->detach_target[k]=-1;p->release_spawns(t,where);}
            if(p->level->objects[t].kind==4){g->pig_alive--;g->score+=5000;g->events|=GAME_EVENT_PIG;}else if(p->level->objects[t].kind!=3){g->block_alive--;g->events|=GAME_EVENT_EXTRA;}
        }
    }p->break_joints(1.0f/dt);
    if(p->bird_type==BIRD_BLACK_TYPE && p->bird && !p->detonated){
        if(p->bird_collided){if(p->bomb_fuse<0)p->bomb_fuse=0;p->bomb_fuse+=dt;}
        if(p->bomb_requested || p->bomb_fuse>=BLACK_FUSE_SECONDS)detonate(g);
    }
    if(p->egg && p->egg_collided){
        radial_blast(g,p->egg->GetPosition(),EGG_BLAST_RADIUS,EGG_BLAST_FORCE,EGG_DAMAGE_RADIUS,EGG_BLAST_DAMAGE);
        p->world.DestroyBody(p->egg);p->egg=NULL;p->egg_collided=false;g->flight_time=0;
    }
    for(int i=0;i<p->level->count;i++) {
        b2Body *body=p->bodies[i];if(!body)continue;
        const CampaignObject &o=p->level->objects[i];
        if(o.kind!=3 && p->impact[i]>3) p->health[i]-=(p->impact[i]-3)*12/fmaxf(o.defence,1);
        p->impact[i]=0;
        if(p->chain_delay[i]>0){p->chain_delay[i]-=dt;if(p->chain_delay[i]>0)continue;}
        if(IS_FRAGILE_KIND(o.kind) && p->fragile_hit[i])p->health[i]=0;
        if(o.kind!=3 && (p->health[i]<=0 || body->GetPosition().y>p->level->ground_y+25)) {
            b2Vec2 where=body->GetPosition();bool exploded=o.kind==OBJECT_KIND_TNT && p->health[i]<=0;
            p->world.DestroyBody(body);p->bodies[i]=NULL;p->release_spawns(i,where);
            /* A destroyed TNT crate blasts its surroundings; crates caught in the blast
               go off in the following ticks, giving chain reactions. */
            if(exploded)radial_blast(g,where,TNT_BLAST_RADIUS,TNT_BLAST_FORCE,TNT_DAMAGE_RADIUS,TNT_BLAST_DAMAGE);
            if(o.kind==4){g->pig_alive--;g->score+=5000;g->events|=GAME_EVENT_PIG;}else {g->block_alive--;g->score+=o.kind==OBJECT_KIND_FRAGILE?FRAGILE_DESTROYED_SCORE:o.kind==OBJECT_KIND_FRAGILE_GROUND?0:500;g->events|=o.kind==1?GAME_EVENT_WOOD:o.kind==2?GAME_EVENT_GLASS:o.kind==5?GAME_EVENT_STONE:GAME_EVENT_EXTRA;}
        }
    }
    if(g->pig_alive==0) {
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
    if(g->cooldown>2.5f || (g->flight_time>10 && !(p->bird_type==BIRD_BLACK_TYPE && p->bird_collided && !p->detonated))) {
        if(p->egg){p->world.DestroyBody(p->egg);p->egg=NULL;p->egg_collided=false;}
        if(g->birds>0){g->state=AIMING;g->x=g->sling_x;g->y=g->sling_y;}
        else g->state=LOST;
    }
}
void game_update(Game *g,float seconds) {
    g->events=0;
    Physics *p=static_cast<Physics*>(g->physics);p->blast_life=fmaxf(0,p->blast_life-clamp(seconds,0,0.1f));
    if(g->state==WON || g->state==LOST)return;
    g->accumulator+=clamp(seconds,0,0.1f);
    while(g->accumulator+0.000001f>=1.0f/120 && g->state!=WON && g->state!=LOST) {
        tick(g);g->accumulator-=1.0f/120;
    }
}
int game_object_count(const Game *g) {return static_cast<const Physics*>(g->physics)->level->count;}
int game_get_object(const Game *g,int index,GameObject *out) {
    const Physics *p=static_cast<const Physics*>(g->physics);
    if(index<0 || index>=p->level->count || !out)return 0;
    const CampaignObject &o=p->level->objects[index];
    b2Body *body=p->bodies[index];memset(out,0,sizeof(*out));out->alive=body!=NULL;out->kind=o.kind;out->definition=o.definition;
    out->width=o.width*g->view_scale;out->height=o.height*g->view_scale;out->health=p->health[index]/o.strength;
    if(body){out->x=g->view_x+body->GetPosition().x*g->view_scale;out->y=g->view_y+body->GetPosition().y*g->view_scale;out->angle=body->GetAngle();}
    return 1;
}
