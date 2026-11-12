#include "game.h"
#if defined(PS2BIRDS_TRIANGLE_TEST)
#include "triangle_fixture.h"
#elif defined(PS2BIRDS_TNT_TEST)
#include "tnt_fixture.h"
#elif defined(PS2BIRDS_WHITE_TEST)
#include "white_fixture.h"
#elif defined(PS2BIRDS_BLAST_TEST)
#include "blast_fixture.h"
#else
#include "campaign_data.h"
#endif
#include <box2d/box2d.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
static_assert(CAMPAIGN_COUNT<=MAX_CAMPAIGN_LEVELS,"Campaign exceeds progress capacity");

static float clamp(float v,float lo,float hi) { return v<lo?lo:v>hi?hi:v; }
struct Physics: b2ContactListener {
    b2World world;
    const CampaignLevel *level;
    b2Body *bodies[MAX_LEVEL_OBJECTS], *bird;
    float health[MAX_LEVEL_OBJECTS], impact[MAX_LEVEL_OBJECTS], age;
    bool has_launched,special_used,bird_collided;
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
    Physics(const CampaignLevel *l):world(b2Vec2(0,l->gravity)),level(l),bird(NULL),age(0),has_launched(false),special_used(false),bird_collided(false),bird_type(BIRD_RED_TYPE) {
        egg=NULL;egg_collided=false;bomb_requested=detonated=false;bomb_fuse=-1;blast_life=0;blast_radius=0;blast_position.SetZero();
        damage_contact_count=0;spawn_ignore_count=0;split[0]=split[1]=NULL;split_collided[0]=split_collided[1]=false;
        memset(chain_delay,0,sizeof(chain_delay));memset(disturbed,0,sizeof(disturbed));memset(bodies,0,sizeof(bodies)); memset(impact,0,sizeof(impact));
        world.SetContactListener(this);
        b2BodyDef floor_def; floor_def.position.Set(20,level->ground_y+1);
        b2Body *floor=world.CreateBody(&floor_def);
        b2PolygonShape shape;shape.SetAsBox(120,1); floor->CreateFixture(&shape,0);
        for (int i=0;i<level->count;i++) {
            const CampaignObject &o=level->objects[i];
            b2BodyDef def;def.type=o.kind==3?b2_staticBody:b2_dynamicBody;
            def.position.Set(o.x,o.y);def.angle=o.angle;
            b2Body *body=world.CreateBody(&def);
            body->GetUserData().pointer=i+1;
            b2FixtureDef fixture; fixture.density=o.density;fixture.friction=0.6f;fixture.restitution=0.03f;
            b2PolygonShape box;b2CircleShape circle;
            if (o.shape==SHAPE_CIRCLE) {circle.m_radius=o.width/2;fixture.shape=&circle;}
            else if(o.shape==SHAPE_TRIANGLE_L || o.shape==SHAPE_TRIANGLE_M){
                const float apex=o.shape==SHAPE_TRIANGLE_L?0.0f:0.5f;
                b2Vec2 v[3]={b2Vec2((apex-0.5f)*o.width,-0.5f*o.height),b2Vec2(0.5f*o.width,0.5f*o.height),b2Vec2(-0.5f*o.width,0.5f*o.height)};
                box.Set(v,3);fixture.shape=&box;
            }
            else {box.SetAsBox(o.width/2,o.height/2);fixture.shape=&box;}
            body->CreateFixture(&fixture);bodies[i]=body;health[i]=o.strength;
        }
    }
    b2Body *make_bird(const b2Vec2 &position,const b2Vec2 &velocity,int type){
        b2BodyDef def;def.type=b2_dynamicBody;def.bullet=true;def.position=position;
        b2Body *body=world.CreateBody(&def);
        /* Radii, densities and restitutions consulted in the recovered blocks.lua. */
        b2CircleShape circle;circle.m_radius=type==BIRD_BLUE_TYPE?0.6f:type==BIRD_YELLOW_TYPE?0.8f:type==BIRD_BLACK_TYPE?1.0f:type==BIRD_WHITE_TYPE?1.3f:type==BIRD_EGG_TYPE?WHITE_EGG_RADIUS:0.75f;
        b2FixtureDef fixture;fixture.shape=&circle;fixture.density=(type==BIRD_BLUE_TYPE || type==BIRD_WHITE_TYPE)?4.5f:type==BIRD_EGG_TYPE?3:6;
        fixture.friction=0.3f;fixture.restitution=(type==BIRD_YELLOW_TYPE || type==BIRD_WHITE_TYPE || type==BIRD_EGG_TYPE)?0.23f:type==BIRD_BLACK_TYPE?0.03f:0.25f;
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
                impact[i]=fmaxf(impact[i],normal/body->GetMass()*factor);
            }
        }
    }
};
void game_init(Game *g) { memset(g,0,sizeof(*g));g->unlocked=1;game_load_level(g,0); }
void game_destroy(Game *g) { delete static_cast<Physics*>(g->physics);g->physics=NULL; }
int game_level_count(void){return (int)CAMPAIGN_COUNT;}
const char *game_level_title(int index){return index>=0 && index<(int)CAMPAIGN_COUNT?campaign_levels[index].title:"";}
int game_level_bank(int index){return index>=0 && index<(int)CAMPAIGN_COUNT?campaign_levels[index].bank:0;}
const char *game_level_script(int index){return index>=0 && index<(int)CAMPAIGN_COUNT?campaign_levels[index].script:"";}
int game_load_level(Game *g,int index) {
    if(index<0 || index>=(int)CAMPAIGN_COUNT || index>=g->unlocked)return 0;
    int unlocked=g->unlocked,best[MAX_CAMPAIGN_LEVELS];memcpy(best,g->highscores,sizeof(best));
    unsigned char losses[MAX_CAMPAIGN_LEVELS];memcpy(losses,g->losses,sizeof(losses));
    game_destroy(g);memset(g,0,sizeof(*g));g->level=index;g->unlocked=unlocked;memcpy(g->highscores,best,sizeof(best));memcpy(g->losses,losses,sizeof(losses));
    const CampaignLevel *l=&campaign_levels[index];g->physics=new Physics(l);
    g->view_x=l->view_x;g->view_y=l->view_y;g->view_scale=l->view_scale;g->pixels=l->pixels;
    g->ground_y=l->view_y+l->ground_y*l->view_scale;g->gravity=l->gravity;
    g->sling_x=l->view_x+l->sling_x*l->view_scale;g->sling_y=l->view_y+l->sling_y*l->view_scale;
    g->x=g->sling_x;g->y=g->sling_y;g->angle=30;g->power=260;g->birds=l->birds;g->state=AIMING;
    for(int i=0;i<l->count;i++){if(l->objects[i].kind==4)g->pig_alive++;if(l->objects[i].kind!=3 && l->objects[i].kind!=4)g->block_alive++;}
    return 1;
}
void game_reset(Game *g){game_load_level(g,g->level);}
int game_next_level(Game *g){return g->state==WON?game_load_level(g,g->level+1):0;}
void game_aim(Game *g,float a,float p) {
    if(g->state!=AIMING)return;
    g->angle=clamp(g->angle+a,10,75);g->power=clamp(g->power+p,160,420);
}
void game_launch(Game *g) {
    if(g->state!=AIMING || g->birds<=0)return;
    Physics *p=static_cast<Physics*>(g->physics);
    if(p->bird)p->world.DestroyBody(p->bird);
    if(p->egg)p->world.DestroyBody(p->egg);
    p->egg=NULL;p->egg_collided=false;
    p->spawn_ignore_count=0;
    for(int i=0;i<2;i++){if(p->split[i])p->world.DestroyBody(p->split[i]);p->split[i]=NULL;p->split_collided[i]=false;}
    p->bird_type=game_queue_type(g,0);p->special_used=false;p->bird_collided=false;
    float a=g->angle*0.01745329252f;
    p->bird=p->make_bird(b2Vec2(p->level->sling_x,p->level->sling_y),b2Vec2(cosf(a)*g->power/g->view_scale,-sinf(a)*g->power/g->view_scale),p->bird_type);
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
    return (p->bird_type==BIRD_BLUE_TYPE || p->bird_type==BIRD_YELLOW_TYPE || p->bird_type==BIRD_WHITE_TYPE) && !p->bird_collided;
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
    out->vx=vel.x*g->view_scale;out->vy=vel.y*g->view_scale;out->angle=body->GetAngle();out->type=body==p->egg?BIRD_EGG_TYPE:p->bird_type;out->special_used=p->special_used;out->fuse=p->bird_type==BIRD_BLACK_TYPE?p->bomb_fuse:-1;out->collided=index==0?p->bird_collided:body==p->egg?p->egg_collided:p->split_collided[index-1];return 1;
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
    *x=g->sling_x+cosf(a)*g->power*t;
    *y=g->sling_y-sinf(a)*g->power*t+0.5f*g->gravity*g->view_scale*t*(t+1.0f/120.0f);
}
static void tick(Game *g) {
    Physics *p=static_cast<Physics*>(g->physics); const float dt=1.0f/120;
    p->age+=dt;p->damage_contact_count=0;p->world.Step(dt,8,3);
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
        if(o.kind!=3 && (p->health[i]<=0 || body->GetPosition().y>p->level->ground_y+25)) {
            b2Vec2 where=body->GetPosition();bool exploded=o.kind==OBJECT_KIND_TNT && p->health[i]<=0;
            p->world.DestroyBody(body);p->bodies[i]=NULL;
            /* A destroyed TNT crate blasts its surroundings; crates caught in the blast
               go off in the following ticks, giving chain reactions. */
            if(exploded)radial_blast(g,where,TNT_BLAST_RADIUS,TNT_BLAST_FORCE,TNT_DAMAGE_RADIUS,TNT_BLAST_DAMAGE);
            if(o.kind==4){g->pig_alive--;g->score+=5000;g->events|=GAME_EVENT_PIG;}else {g->block_alive--;g->score+=500;g->events|=o.kind==1?GAME_EVENT_WOOD:o.kind==2?GAME_EVENT_GLASS:o.kind==5?GAME_EVENT_STONE:GAME_EVENT_EXTRA;}
        }
    }
    if(g->pig_alive==0) {
        g->score+=g->birds*10000;g->state=WON;g->losses[g->level]=0;
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
        else{
            g->state=LOST;
            if(g->losses[g->level]<255)g->losses[g->level]++;
            if(g->losses[g->level]>=GAME_SKIP_AFTER_LOSSES && g->level+2>g->unlocked)g->unlocked=g->level+2>game_level_count()?game_level_count():g->level+2;
        }
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
