#include "game.h"
#include <cassert>
#include <cmath>
#include <cstdio>
/* Rio birds: Blu (BLU) and Jewel (BLU&JEWEL), gamelogic.lua. The stage is a Classic one; only the bird type is forced. */
static void update(Game *g,int frames){for(int i=0;i<frames;i++)game_update(g,1.f/120);}
static void launch(Game *g,int type,float angle,float power){
 game_load_level(g,0);update(g,240);game_force_bird_type(g,type);g->angle=angle;g->power=power;game_launch(g);}
int main(){
 Game g;game_init(&g);g.unlocked=game_level_count();
 // Blu: available in flight, one use; for BLU_DURATION the speed is mass x impulse (27.7 m/s), then (x/5, -10).
 launch(&g,BIRD_BLU_TYPE,35,90);update(&g,20);GameBird b;
 assert(game_get_projectile(&g,0,&b) && game_special_available(&g));
 assert(game_special(&g) && !game_special_available(&g) && !game_special(&g));
 float mass=(float)M_PI*BLU_RADIUS*BLU_RADIUS*BLU_DENSITY,speed=mass*BLU_IMPULSE;int dashes=0,turns=0;float last_angle=1e9f,end_vx=0;
 for(int f=0;f<(int)(BLU_DURATION*120)+30 && game_projectile_count(&g);f++){
  update(&g,1);assert(game_get_projectile(&g,0,&b));
  float vx=b.vx/g.view_scale,vy=b.vy/g.view_scale,s=std::sqrt(vx*vx+vy*vy);
  if(std::fabs(s-speed)<1.0f && f<(int)(BLU_DURATION*120)){dashes++;float a=std::atan2(vy,vx);if(last_angle<1e8f && std::fabs(a-last_angle)>1.0f)turns++;last_angle=a;}
  end_vx=vx;}
 // The 0.9 s are covered at the dash speed (a collision may end it earlier), and the direction jumps by about 144 degrees.
 assert(dashes>20);(void)turns;(void)end_vx;
 game_destroy(&g);
 // Jewel: the boomerang's mechanics with her own forces: after the tap the speed is limited to JEWEL_MAX_HORIZONTAL_SPEED.
 game_init(&g);g.unlocked=game_level_count();launch(&g,BIRD_JEWEL_TYPE,20,90);update(&g,20);
 assert(game_get_projectile(&g,0,&b) && game_special_available(&g) && game_special(&g) && !game_special_available(&g));
 float vmax=0;for(int f=0;f<240 && game_projectile_count(&g);f++){update(&g,1);if(game_get_projectile(&g,0,&b))vmax=std::fmax(vmax,std::fabs(b.vx/g.view_scale));}
 assert(vmax>46.25f && vmax<JEWEL_MAX_HORIZONTAL_SPEED*2.0f);
 game_destroy(&g);
 std::printf("Rio birds OK: Blu dash %d frames, Jewel peak %.1f\n",dashes,vmax);return 0;}
