#include "game.h"
#include "campaign_types.h"
#include <cassert>
#include <cmath>
#include <cstdio>
/* Green boomerang bird: values and algorithm recovered from blocks.lua and gamelogic.lua. */
static void update(Game *g,int frames){for(int i=0;i<frames;i++)game_update(g,1.f/120);}
static int first_level_with(int type){for(int i=0;i<game_level_count();i++){Game g;game_init(&g);g.unlocked=game_level_count();game_load_level(&g,i);int found=game_queue_type(&g,0)==type;game_destroy(&g);if(found)return i;}return -1;}
int main(){
 int level=first_level_with(BIRD_GREEN_TYPE);assert(level>=0);
 Game g;game_init(&g);g.unlocked=game_level_count();assert(game_load_level(&g,level));
 int birds=g.birds;assert(game_queue_type(&g,0)==BIRD_GREEN_TYPE && !game_special_available(&g) && !game_special(&g));
 update(&g,240);g.angle=30;g.power=100;game_launch(&g);assert(g.birds==birds-1 && game_projectile_count(&g)==1 && game_special_available(&g));
 // Before the tap the upward flyVerticalAcceleration (-2.8) lightens gravity.
 GameBird a,b;assert(game_get_projectile(&g,0,&a));update(&g,12);assert(game_get_projectile(&g,0,&b));
 float scale=g.view_scale,dv=(b.vy-a.vy)/scale/0.1f;assert(std::fabs(dv-(game_level_gravity(level)+GREEN_FLY_VERTICAL_ACCELERATION))<0.05f*game_level_gravity(level));
 update(&g,48);GameBird before;assert(game_get_projectile(&g,0,&before) && before.vx>0 && !before.collided);
 assert(game_special(&g) && !game_special_available(&g) && !game_special(&g) && game_projectile_count(&g)==1);
 // After the tap the horizontal force reverses the flight and stops at the speed limit.
 float min_vx=1e9f;bool reversed=false;
 for(int i=0;i<480 && g.state==FLYING;i++){update(&g,1);GameBird c;if(!game_get_projectile(&g,0,&c)||c.collided)break;
  float vx=c.vx/scale;min_vx=fminf(min_vx,vx);if(vx<0)reversed=true;assert(std::isfinite(c.x) && std::isfinite(c.y));}
 assert(reversed && min_vx>=-GREEN_MAX_HORIZONTAL_SPEED-1.0f && min_vx<=-GREEN_MAX_HORIZONTAL_SPEED+1.0f);
 // A green bird that touches something can no longer turn back.
 game_reset(&g);update(&g,240);g.angle=5;g.power=100;game_launch(&g);bool touched=false;
 for(int i=0;i<1320 && g.state==FLYING;i++){update(&g,1);GameBird c;assert(game_get_projectile(&g,0,&c));if(c.collided){assert(!game_special_available(&g) && !game_special(&g));touched=true;break;}}
 assert(touched);
 for(int i=0;i<30;i++){assert(game_load_level(&g,level));update(&g,240);game_launch(&g);update(&g,40);assert(game_special(&g));assert(game_load_level(&g,0) && game_projectile_count(&g)==0);}
 // StaticBalloon pieces (staticFragile/clouds): static and intact until hit, popped by a shot.
 int balloon=-1;for(int i=0;i<game_level_count() && balloon<0;i++){assert(game_load_level(&g,i));for(int k=0;k<game_object_count(&g);k++){GameObject o;game_get_object(&g,k,&o);if(o.kind==OBJECT_KIND_FRAGILE){balloon=i;break;}}}
 assert(balloon>=0 && game_load_level(&g,balloon));int fragile=0;float fx[MAX_LEVEL_OBJECTS],fy[MAX_LEVEL_OBJECTS];
 for(int k=0;k<game_object_count(&g);k++){GameObject o;game_get_object(&g,k,&o);fx[k]=o.x;fy[k]=o.y;fragile+=o.kind==OBJECT_KIND_FRAGILE;}
 update(&g,240);
 for(int k=0;k<game_object_count(&g);k++){GameObject o;game_get_object(&g,k,&o);if(o.kind==OBJECT_KIND_FRAGILE)assert(o.alive && o.x==fx[k] && o.y==fy[k]);}
 bool popped=false;
 for(int angle=-10;angle<=80 && !popped;angle+=2)for(int power=40;power<=100 && !popped;power+=6){
  assert(game_load_level(&g,balloon));update(&g,240);g.angle=(float)angle;g.power=(float)power;game_launch(&g);
  for(int i=0;i<1320 && g.state==FLYING;i++)update(&g,1);
  for(int k=0;k<game_object_count(&g);k++){GameObject o;game_get_object(&g,k,&o);if(o.kind==OBJECT_KIND_FRAGILE && !o.alive){popped=true;assert(g.score>=FRAGILE_DESTROYED_SCORE);}}
 }
 assert(fragile>0 && popped);
 game_destroy(&g);assert(!g.physics);
 printf("Green OK: level %d queue, lighter flight before the tap, boomerang turn capped at %.0f u/s, single activation, unavailable after contact, 30 transitions; %d static balloons on stage %d stay put and pop when hit.\n",level,GREEN_MAX_HORIZONTAL_SPEED,fragile,balloon+1);
}
