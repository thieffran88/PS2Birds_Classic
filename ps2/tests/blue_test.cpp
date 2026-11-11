#include "game.h"
#include <cassert>
#include <cmath>
#include <cstdio>
static void update(Game *g,int frames){for(int i=0;i<frames;i++)game_update(g,1.f/120);}
int main(){
 Game g;game_init(&g);assert(!game_special(&g) && !game_special_available(&g));
 assert(game_queue_type(&g,0)==BIRD_RED_TYPE && game_queue_type(&g,-1)==-1 && game_queue_type(&g,3)==-1);
 game_launch(&g);assert(!game_special(&g) && game_projectile_count(&g)==1);g.unlocked=8;
 assert(game_load_level(&g,7));assert(g.birds==5 && g.pig_alive==2 && game_projectile_count(&g)==0);
 for(int i=0;i<5;i++)assert(game_queue_type(&g,i)==BIRD_BLUE_TYPE);
 assert(game_queue_type(&g,5)==-1 && !game_special(&g));update(&g,240);
 game_launch(&g);assert(g.birds==4 && game_special_available(&g));update(&g,60);
 GameBird center,before,upper,lower;assert(game_get_projectile(&g,0,&before));
 assert(game_special(&g) && !game_special(&g) && !game_special_available(&g) && g.birds==4 && game_projectile_count(&g)==3);
 assert(game_get_projectile(&g,0,&center) && game_get_projectile(&g,1,&upper) && game_get_projectile(&g,2,&lower));
 assert(!game_get_projectile(&g,-1,&center) && !game_get_projectile(&g,3,&center) && !game_get_projectile(&g,0,nullptr));
 assert(center.type==BIRD_BLUE_TYPE && upper.type==BIRD_BLUE_TYPE && lower.type==BIRD_BLUE_TYPE);
 assert(center.x==before.x && center.vx==before.vx && center.vy==before.vy);
 float speed=std::hypot(center.vx,center.vy);
 assert(std::fabs(std::hypot(upper.vx,upper.vy)-speed)<0.01f && std::fabs(std::hypot(lower.vx,lower.vy)-speed)<0.01f);
 assert(std::fabs(upper.vy-lower.vy)>10 && std::hypot(upper.x-lower.x,upper.y-lower.y)>10);
 update(&g,60);for(int i=0;i<3;i++){GameBird b;assert(game_get_projectile(&g,i,&b) && std::isfinite(b.x) && std::isfinite(b.y));}
 for(int i=0;i<1320 && g.state==FLYING;i++)update(&g,1);
 assert(g.state==AIMING && !game_special(&g));game_launch(&g);assert(game_projectile_count(&g)==1 && g.birds==3);
 game_reset(&g);assert(game_projectile_count(&g)==0 && g.birds==5 && !game_special_available(&g));
 // An unused blue ability must become unavailable on the first contact.
 game_launch(&g);bool touched=false;
 for(int i=0;i<1320 && g.state==FLYING;i++){
     update(&g,1);GameBird b;assert(game_get_projectile(&g,0,&b));
     if(b.collided){assert(!game_special_available(&g) && !game_special(&g));touched=true;break;}
 }
 assert(touched);game_reset(&g);
 // Exercise ownership of all three bodies through repeated reset/load/destroy.
 for(int i=0;i<30;i++){assert(game_load_level(&g,7));game_launch(&g);update(&g,30);assert(game_special(&g));assert(game_load_level(&g,0) && game_projectile_count(&g)==0);}
 game_destroy(&g);assert(!g.physics);
 puts("Blue OK: queue, airborne split, 3 trajectories, conserved speed, one activation, no extra bird consumed, rejection on red/ground/reset, 30 world transitions.");
}
