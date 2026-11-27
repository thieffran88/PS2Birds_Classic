#include "game.h"
#include <cassert>
#include <cmath>
#include <cstdio>
/* Rio monkeys (animations.destroyJoints): a monkey welded to a block holds on while the block rests, and lets go once a hit makes it move faster than 3 for 0.3 s. */
static void update(Game *g,int count){for(int i=0;i<count;i++)game_update(g,1.f/120);}
static GameObject object(Game *g,int i){GameObject o;assert(game_get_object(g,i,&o));return o;}
int main(){
 Game g;game_init(&g);assert(game_object_count(&g)==4);
 const float s=g.view_scale;
 // At rest the weld keeps the monkey 2.9 units right of its block and nothing is released.
 update(&g,600);
 float rest=(object(&g,1).x-object(&g,0).x)/s;assert(fabsf(rest-2.9f)<0.05f && !(g.events&GAME_EVENT_MONKEY_SLIP));
 // A bird hits the block (as in the TNT fixture): the pair is shoved; after 0.3 s above speed 3 the weld is destroyed and the cry plays once.
 g.angle=10;g.power=75.7f;game_launch(&g);
 int slips=0;float widest=0;
 for(int t=0;t<1200 && g.state==FLYING;t++){
  update(&g,1);if(g.events&GAME_EVENT_MONKEY_SLIP)slips++;
  float d=fabsf((object(&g,1).x-object(&g,0).x)/s-2.9f);if(d>widest)widest=d;
 }
 update(&g,240);
 float after=fabsf((object(&g,1).x-object(&g,0).x)/s-2.9f);if(after>widest)widest=after;
 assert(slips==1);        // it lets go exactly once
 assert(widest>0.5f);     // and the monkey is no longer held to the block
 printf("Monkey OK: weld holds at rest, the monkey lets go after a hit (%d cry, drift %.1f).\n",slips,widest);
 return 0;
}
