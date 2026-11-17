#include "game.h"
#include <cassert>
#include <cstdio>
#include <cstring>
/* Egg Defender (redbird_special.lua): while the red bird flies, the closest living pig becomes its
   target and the vehicle carrying it is told (pigTargeted); firing the special tells it again
   (redBirdTriggered). Outside the Egg Defender stages nothing is sent. */
static void update(Game *g,int frames){for(int i=0;i<frames;i++)game_update(g,1.f/120);}
int main(){
 Game g;game_init(&g);g.unlocked=game_level_count();int tested=0;
 for(int i=0;i<game_level_count();i++){
  bool egg=!strncmp(game_level_script(i),"EggDefender_",12);
  assert(game_load_level(&g,i));
  if(!egg){if(game_queue_type(&g,0)==BIRD_RED_TYPE && !tested){update(&g,240);g.angle=30;g.power=90;game_launch(&g);update(&g,60);game_special(&g);update(&g,120);assert(game_red_notifications(&g,0)==0 && game_red_notifications(&g,1)==0);}continue;}
  if(game_queue_type(&g,0)!=BIRD_RED_TYPE)continue;
  update(&g,600);   /* the first wave's vehicles come in with their pigs */
  g.angle=20;g.power=90;game_launch(&g);update(&g,20);
  if(game_red_notifications(&g,0)==0)continue;   /* no pig on the field yet in this stage */
  assert(game_red_notifications(&g,1)==0);
  assert(game_special(&g));update(&g,120);
  assert(game_red_notifications(&g,1)==1);
  tested++;
 }
 assert(tested>0);
 printf("Red target OK: pigTargeted and redBirdTriggered reach the vehicles in %d Egg Defender stages; none elsewhere.\n",tested);
 game_destroy(&g);return 0;
}
