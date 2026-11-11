#include "game.h"
#include <cassert>
#include <cmath>
#include <cstdio>
static void update(Game *g,int frames){for(int i=0;i<frames;i++)game_update(g,1.f/120);}
static int first_level_with(int type){for(int i=0;i<game_level_count();i++){Game g;game_init(&g);g.unlocked=game_level_count();game_load_level(&g,i);int found=game_queue_type(&g,0)==type;game_destroy(&g);if(found)return i;}return -1;}
int main(){
 int level=first_level_with(BIRD_YELLOW_TYPE);assert(level>=0);
 Game g;game_init(&g);g.unlocked=game_level_count();assert(game_load_level(&g,level));
 int birds=g.birds;assert(birds>=1 && game_queue_type(&g,0)==BIRD_YELLOW_TYPE && !game_special_available(&g) && !game_special(&g));
 update(&g,240);g.angle=30;g.power=300;game_launch(&g);assert(g.birds==birds-1 && game_projectile_count(&g)==1 && game_special_available(&g));
 update(&g,30);GameBird before,after;assert(game_get_projectile(&g,0,&before) && before.type==BIRD_YELLOW_TYPE && !before.collided);
 assert(game_special(&g) && !game_special_available(&g) && !game_special(&g) && game_projectile_count(&g)==1 && g.birds==birds-1);
 assert(game_get_projectile(&g,0,&after));
 float speed_before=std::hypot(before.vx,before.vy),speed_after=std::hypot(after.vx,after.vy);
 assert(std::fabs(speed_after-speed_before*YELLOW_BOOST_FACTOR)<0.01f*speed_before && after.x==before.x && after.y==before.y);
 // Direction is preserved by the boost.
 assert(std::fabs(after.vx*before.vy-after.vy*before.vx)<1e-3f*speed_before*speed_before);
 for(int i=0;i<1320 && g.state==FLYING;i++){update(&g,1);GameBird b;assert(game_get_projectile(&g,0,&b) && std::isfinite(b.x) && std::isfinite(b.y));}
 assert(g.state!=FLYING);
 // The boost is unavailable after the first contact and after the shot ends.
 game_reset(&g);update(&g,240);g.angle=10;g.power=420;game_launch(&g);bool touched=false;
 for(int i=0;i<1320 && g.state==FLYING;i++){update(&g,1);GameBird b;assert(game_get_projectile(&g,0,&b));if(b.collided){assert(!game_special_available(&g) && !game_special(&g));touched=true;break;}}
 assert(touched);
 // A red bird never boosts; a blue one still splits.
 assert(game_load_level(&g,0) && game_queue_type(&g,0)==BIRD_RED_TYPE);game_launch(&g);update(&g,10);assert(!game_special_available(&g) && !game_special(&g));
 int blue=first_level_with(BIRD_BLUE_TYPE);assert(blue>=0 && game_load_level(&g,blue));update(&g,240);game_launch(&g);update(&g,30);assert(game_special(&g) && game_projectile_count(&g)==3);
 for(int i=0;i<30;i++){assert(game_load_level(&g,level));game_launch(&g);update(&g,20);assert(game_special(&g));assert(game_load_level(&g,0) && game_projectile_count(&g)==0);}
 game_destroy(&g);assert(!g.physics);
 printf("Yellow OK: level %d queue, boost x%.1f keeps position/direction, single activation, unavailable after contact, red rejected, blue still splits, 30 transitions.\n",level,YELLOW_BOOST_FACTOR);
}
