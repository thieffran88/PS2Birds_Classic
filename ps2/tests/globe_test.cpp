#include "game.h"
#include "campaign_types.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
/* GlobeBird (puffer_bird.lua) and balloon material forces (blocks.lua forceX/forceY). */
static void update(Game *g,int frames){for(int i=0;i<frames;i++)game_update(g,1.f/120);}
static int first_level_with(int type){for(int i=0;i<game_level_count();i++){Game g;game_init(&g);g.unlocked=game_level_count();game_load_level(&g,i);int found=game_queue_type(&g,0)==type;game_destroy(&g);if(found)return i;}return -1;}
int main(){
 int level=first_level_with(BIRD_PUFFER_TYPE);assert(level>=0);
 Game g;game_init(&g);g.unlocked=game_level_count();assert(game_load_level(&g,level));
 assert(!game_special_available(&g));update(&g,240);g.angle=30;g.power=90;game_launch(&g);
 GameBird b;update(&g,30);assert(game_get_projectile(&g,0,&b) && b.fuse<0 && game_special_available(&g));
 assert(game_special(&g) && !game_special_available(&g) && !game_special(&g));
 assert(game_get_projectile(&g,0,&b) && std::fabs(b.fuse-1.0f)<1e-4f);
 int puffed=0,shrunk=0,gone=-1;float last=1;
 for(int f=1;f<=600;f++){update(&g,1);
  if(game_projectile_count(&g)==0){gone=f;break;}
  assert(game_get_projectile(&g,0,&b));
  if(b.fuse>0.999f)puffed++;else{assert(b.fuse<=last+1e-4f);shrunk++;}
  last=b.fuse;}
 // Puffed for GLOBE_PUFF_TIME, then shrinks for GLOBE_DEATH_TIME and disappears.
 assert(std::abs(puffed-(int)(GLOBE_PUFF_TIME*120))<=2 && std::abs(shrunk-(int)(GLOBE_DEATH_TIME*120))<=2 && gone>0);
 // The puff is still available after a contact (specialtyAvailableAfterCollision) and fires on its own after the delay.
 game_reset(&g);update(&g,240);g.angle=0;g.power=100;game_launch(&g);bool touched=false;int auto_frame=-1;
 for(int f=0;f<1400 && g.state==FLYING;f++){update(&g,1);if(!game_projectile_count(&g))break;assert(game_get_projectile(&g,0,&b));
  if(b.collided && !touched){touched=true;assert(game_special_available(&g));auto_frame=f;}
  if(touched && b.fuse>0){assert(f-auto_frame>=(int)(GLOBE_AUTO_PUFF_DELAY*120)-2);break;}}
 assert(touched);
 // Balloons: forceY -30 x mass (beats gravity 20) keeps the balloons and their ropes in place.
 int balloon_level=-1;
 for(int i=0;i<game_level_count() && balloon_level<0;i++){assert(game_load_level(&g,i));for(int k=0;k<game_object_count(&g);k++){GameObject o;game_get_object(&g,k,&o);if(!strncmp(o.definition,"BirthdayBalloon",15)){balloon_level=i;break;}}}
 assert(balloon_level>=0 && game_load_level(&g,balloon_level));
 int n=game_object_count(&g),checked=0;float y0[MAX_LEVEL_OBJECTS];for(int k=0;k<n;k++){GameObject o;game_get_object(&g,k,&o);y0[k]=o.y;}
 update(&g,240);
 for(int k=0;k<n;k++){GameObject o;game_get_object(&g,k,&o);if(strncmp(o.definition,"BirthdayBalloon",15))continue;assert(o.alive && std::fabs(o.y-y0[k])<3);checked++;}
 assert(checked>0);
 game_destroy(&g);
 printf("Globe OK: level %d, puff on tap (radius %.1f), %.1f s puffed, shrinks over %.1f s and disappears, available after contact with automatic puff after %.1f s; %d balloons hold their ropes.\n",level,GLOBE_PUFFED_RADIUS,GLOBE_PUFF_TIME,GLOBE_DEATH_TIME,GLOBE_AUTO_PUFF_DELAY,checked);
}
