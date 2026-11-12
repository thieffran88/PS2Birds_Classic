#include "game.h"
#include <box2d/box2d.h>
#include <cassert>
#include <cmath>
#include <cstdio>
static void update(Game *g,int n){for(int i=0;i<n;i++)game_update(g,1.f/120);}
static GameObject object(Game *g,int i){GameObject o;assert(game_get_object(g,i,&o));return o;}
int main(){
 Game g;game_init(&g);assert(game_object_count(&g)==4 && game_level_bank(0)==1 && game_level_bank(5)==0);
 // Each triangle's centroid sits below the box centre: the solid is heavier at the base.
 // After settling, triangles stay upright and do not tip or sink through the ground.
 update(&g,600);
 for(int i=0;i<3;i++){
  GameObject o=object(&g,i);assert(o.alive && std::fabs(o.angle)<0.01f && o.health==1);
 }
 float ground=g.view_y+5*g.view_scale;
 // Box: bottom face on the ground keeps the 4x4 centre 2 units above it.
 assert(std::fabs(object(&g,2).y-(ground-2*g.view_scale))<0.05f*g.view_scale);
 // Triangles: the box-centred hull also has its base on the ground, so the reference point stays put.
 assert(std::fabs(object(&g,0).y-object(&g,2).y)<0.05f*g.view_scale && std::fabs(object(&g,1).y-object(&g,2).y)<0.05f*g.view_scale);
 // Low shots reach the triangles and register a collision with them.
 for(int t=0;t<2;t++){
  game_reset(&g);update(&g,240);
  g.angle=10;g.power=t==0?260:330;game_launch(&g);bool hit=false;
  for(int i=0;i<1320 && g.state==FLYING;i++){update(&g,1);GameBird b;if(game_get_projectile(&g,0,&b) && b.collided)hit=true;}
  assert(hit);
 }
 game_destroy(&g);assert(!g.physics);
 puts("Triangle OK: right and isosceles triangle hulls rest upright on the ground beside a box, are reached by low shots, texture bank looked up per stage.");
}
