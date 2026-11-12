#include "game.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
static void update(Game *g,int count){for(int i=0;i<count;i++)game_update(g,1.f/120);}
static GameObject object(Game *g,int i){GameObject o;assert(game_get_object(g,i,&o));return o;}
int main(){
 Game g;game_init(&g);assert(game_object_count(&g)==7 && g.pig_alive==2 && g.block_alive==4);
 assert(object(&g,0).kind==OBJECT_KIND_TNT && !strcmp(object(&g,0).definition,"ExtraBlockTNT"));
 // Resting crates never explode by themselves.
 GameBlast blast;update(&g,600);assert(object(&g,0).alive && object(&g,1).alive && g.score==0 && !game_get_blast(&g,&blast));
 // A direct hit destroys the first crate, which blasts and chains into the second one.
 g.angle=10;g.power=75.7f;game_launch(&g);
 int explosions=0,first=-1,second=-1,extra_events=0;float first_x=0;
 for(int t=0;t<1320 && g.state==FLYING;t++){
  int blocks=g.block_alive;update(&g,1);
  if(g.events&GAME_EVENT_EXPLOSION){
   explosions++;assert(game_get_blast(&g,&blast) && blast.radius==TNT_BLAST_RADIUS*g.view_scale && blast.life>0);
   if(first<0){first=t;first_x=blast.x;assert(!object(&g,0).alive);}
   else if(second<0){second=t;assert(!object(&g,1).alive && blast.x>first_x);}
  }
  if(g.events&GAME_EVENT_EXTRA)extra_events++;
  assert(bool(g.events&(GAME_EVENT_WOOD|GAME_EVENT_GLASS|GAME_EVENT_STONE|GAME_EVENT_EXTRA))==(blocks>g.block_alive));
 }
 assert(explosions==2 && first>=0 && second-first>=11 && second-first<=13 && extra_events>=1);
 assert(!object(&g,2).alive);                       // wood inside both damage radii
 assert(!object(&g,5).alive && g.pig_alive==1);     // pig beside the chained crate
 GameObject outside=object(&g,3),shelf=object(&g,4);
 assert(outside.alive && outside.health==1);        // beyond the blast radius: no damage
 assert(shelf.alive && shelf.health==1 && std::fabs(shelf.x-(g.view_x+23*g.view_scale))<1e-3f); // static immunity
 assert(g.score==500*3+5000 && g.block_alive==1);
 // Each crate explodes exactly once.
 for(int i=0;i<600;i++){update(&g,1);assert(!(g.events&GAME_EVENT_EXPLOSION));}
 assert(g.state==AIMING && g.birds==1);
 // Reset restores both crates; the blast marker does not survive a world change.
 game_reset(&g);assert(object(&g,0).alive && object(&g,1).alive && object(&g,0).health==1 && !game_get_blast(&g,&blast));
 // A crate that only falls out of the world is removed without a blast.
 g.unlocked=2;assert(game_load_level(&g,1));int fell=0;
 for(int i=0;i<1200;i++){update(&g,1);assert(!(g.events&GAME_EVENT_EXPLOSION));if(!object(&g,0).alive)fell=1;}
 assert(fell && g.block_alive==0 && !game_get_blast(&g,&blast));
 for(int i=0;i<20;i++){assert(game_load_level(&g,0));g.angle=10;g.power=100;game_launch(&g);update(&g,90);assert(game_load_level(&g,1) && !game_get_blast(&g,&blast));}
 game_destroy(&g);assert(!g.physics);
 printf("TNT OK: stable at rest, direct-hit blast, delayed chain reaction after %d ticks, damage/pig kill, radius limit, static immunity, single events, score, reset, silent fall-out, 20 world transitions.\n",second-first);
}
