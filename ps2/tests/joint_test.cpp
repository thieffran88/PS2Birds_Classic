#include "game.h"
#include <cassert>
#include <cmath>
#include <cstdio>
static void update(Game *g,int n){for(int i=0;i<n;i++)game_update(g,1.f/120);}
static GameObject object(Game *g,int i){GameObject o;assert(game_get_object(g,i,&o));return o;}
static float dist(Game *g,int a,int b){GameObject x=object(g,a),y=object(g,b);return hypotf(x.x-y.x,x.y-y.y)/g->view_scale;}
int main(){
 Game g;game_init(&g);float s=g.view_scale;
 update(&g,600);
 // Distance joint: the two tires fall together and keep their 6-unit spacing.
 assert(fabsf(dist(&g,0,1)-6)<0.3f && object(&g,0).y>object(&g,0).y-1);
 // Weld joint: the upper plank stays 3 units above its base instead of falling.
 assert(fabsf((object(&g,2).y-object(&g,3).y)/s-3)<0.05f && fabsf(object(&g,3).angle)<0.02f);
 // Revolute joint to the ground: the bar swings down around its left end, which stays put.
 GameObject bar=object(&g,4);float left_x=bar.x/s-4*cosf(bar.angle)-g.view_x/s,left_y=bar.y/s-4*sinf(bar.angle)-g.view_y/s;
 assert(fabsf(left_x-46)<0.2f && fabsf(left_y+10)<0.2f && fabsf(bar.angle)>1.0f);
 // A breakable weld with a tiny threshold gives way under the plank's own weight.
 g.unlocked=2;assert(game_load_level(&g,1));update(&g,600);
 assert((object(&g,2).y-object(&g,3).y)/s<1.5f);
 // Prismatic joint (type 4) with limits, motor and backAndForth: the block moves along the
 // vertical axis only, stays within [-3,0] of its start and turns around at the limits.
 g.unlocked=3;assert(game_load_level(&g,2));s=g.view_scale;
 float lo=1e9f,hi=-1e9f;int turns=0,dir=0;
 for(int t=0;t<1200;t++){update(&g,1);GameObject b=object(&g,6);float y=(b.y-g.view_y)/s,x=(b.x-g.view_x)/s;
  assert(fabsf(x-100)<0.05f && y>-13.2f && y<-9.8f);if(y<lo)lo=y;if(y>hi)hi=y;
  static float last=-10;int d=y<last-1e-4f?-1:y>last+1e-4f?1:0;if(d && dir && d!=dir)turns++;if(d)dir=d;last=y;}
 assert(lo<-12.8f && hi>-10.2f && turns>=2);
 // Revolute motor: the bar keeps turning around its hinge (3 rad/s).
 float a0=object(&g,7).angle;update(&g,120);assert(fabsf(object(&g,7).angle-a0)>2.0f);
 // collision = false: the block drops through the scenery piece and rests on the ground.
 GameObject drop=object(&g,9);assert(fabsf((drop.y-g.view_y)/s-4.5f)<0.1f && object(&g,8).alive);
 // Joints are released with their bodies on reload and world changes.
 for(int i=0;i<30;i++){assert(game_load_level(&g,i%3));update(&g,5);}
 game_destroy(&g);assert(!g.physics);
 puts("Joint OK: distance spacing kept, weld holds a plank in the air, revolute hinge to the ground, breakable weld fails; prismatic joint with limits/motor/back-and-forth, motor-driven hinge, piece without collision; 30 world transitions.");
}
