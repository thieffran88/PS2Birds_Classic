#include "game.h"
#include <cassert>
#include <cmath>
#include <cstdio>
static void update(Game *g,int count){for(int i=0;i<count;i++)game_update(g,1.f/120);}
static GameObject object(Game *g,int i){GameObject o;assert(game_get_object(g,i,&o));return o;}
int main(){
 Game g;game_init(&g);assert(game_queue_type(&g,0)==BIRD_BLACK_TYPE && !game_special(&g));
 GameBlast blast;assert(!game_get_blast(&g,&blast));game_launch(&g);assert(g.birds==0 && game_projectile_count(&g)==1);
 assert(game_special(&g) && !game_special(&g) && !game_special_available(&g));
 update(&g,1);assert((g.events&GAME_EVENT_EXPLOSION) && game_projectile_count(&g)==0 && game_get_blast(&g,&blast));
 assert(std::isfinite(blast.x) && std::isfinite(blast.y) && blast.radius>0 && blast.life>0);
 assert(!object(&g,0).alive && !object(&g,5).alive); // close wood and pig receive direct blast damage
 assert(object(&g,1).alive && object(&g,1).health>0 && object(&g,1).health<1); // falloff
 assert(object(&g,2).alive && object(&g,2).health==1 && object(&g,3).health==1); // outside damage radius
 GameObject immovable=object(&g,4),far=object(&g,3),outer=object(&g,2);
 assert(immovable.health==1);update(&g,4);
 assert(object(&g,2).y-outer.y>object(&g,3).y-far.y+5); // impulse extends beyond direct damage
 assert(object(&g,3).x==far.x && object(&g,4).x==immovable.x && object(&g,4).y==immovable.y);
 int explosions=1;for(int i=0;i<360;i++){update(&g,1);if(g.events&GAME_EVENT_EXPLOSION)explosions++;}
 assert(explosions==1 && !game_get_blast(&g,&blast) && !game_special(&g));
 // The fuse arms on real contact and detonates once after about one second.
 g.unlocked=2;assert(game_load_level(&g,1));update(&g,240);g.angle=10;g.power=160;game_launch(&g);
 int contact=-1,detonation=-1;
 for(int t=0;t<1320 && g.state==FLYING;t++){
  update(&g,1);GameBird bird;if(game_get_projectile(&g,0,&bird) && bird.collided && contact<0)contact=t;
  if(g.events&GAME_EVENT_EXPLOSION){detonation=t;break;}
 }
 assert(contact>=0 && detonation-contact>=118 && detonation-contact<=122 && game_projectile_count(&g)==0 && g.birds==1);
 update(&g,360);assert(g.state==AIMING && g.birds==1);game_launch(&g);
 GameBird fresh;assert(game_get_projectile(&g,0,&fresh) && fresh.fuse<0 && game_special_available(&g));
 // The player can detonate after contact without waiting for the automatic fuse.
 game_reset(&g);update(&g,240);g.angle=10;g.power=160;game_launch(&g);bool touched=false;
 for(int i=0;i<1320 && g.state==FLYING;i++){update(&g,1);GameBird bird;if(game_get_projectile(&g,0,&bird) && bird.collided){touched=true;break;}}
 assert(touched && game_special_available(&g) && game_special(&g));update(&g,1);assert(g.events&GAME_EVENT_EXPLOSION);
 for(int i=0;i<20;i++){assert(game_load_level(&g,1));game_launch(&g);assert(game_special(&g));update(&g,1);assert(game_load_level(&g,0) && !game_get_blast(&g,&blast) && !game_projectile_count(&g));}
 game_destroy(&g);assert(!g.physics);
 puts("Black OK: queued manual detonation, single event, radial damage/falloff/impulse, static immunity, auto fuse, manual after contact, next bird, 20 world transitions.");
}
