#include "game.h"
#include <cassert>
#include <cmath>
#include <cstdio>
static void update(Game*g,int n){for(int i=0;i<n;i++)game_update(g,1.f/120);}
static GameObject object(Game*g,int i){GameObject o;assert(game_get_object(g,i,&o));return o;}
int main(){
 Game g;game_init(&g);assert(game_queue_type(&g,0)==BIRD_WHITE_TYPE && !game_special(&g));
 g.angle=75;g.power=160;game_launch(&g);GameBird before,white,egg;
 assert(game_get_projectile(&g,0,&before) && before.type==BIRD_WHITE_TYPE && !before.special_used);
 assert(game_special_available(&g) && game_special(&g) && !game_special(&g) && g.birds==1);
 assert(game_projectile_count(&g)==2 && game_get_projectile(&g,0,&white) && game_get_projectile(&g,1,&egg));
 assert(white.type==BIRD_WHITE_TYPE && white.special_used && egg.type==BIRD_EGG_TYPE);
 assert(white.x==before.x && white.y==before.y && egg.x==before.x && egg.y>white.y);
 assert(fabsf(white.vx-before.vx*WHITE_RECOIL_X_FACTOR)<.01f && fabsf(white.vy+WHITE_RECOIL_SPEED*g.view_scale)<.01f);
 assert(egg.vx==0 && fabsf(egg.vy-WHITE_DROP_SPEED*g.view_scale)<.01f);
 update(&g,1);assert(!(g.events&GAME_EVENT_EXPLOSION) && game_projectile_count(&g)==2);
 assert(game_get_projectile(&g,0,&white) && white.vy<0 && game_get_projectile(&g,1,&egg) && egg.vy>0);
 int explosions=0,impact=-1;
 for(int t=0;t<240;t++){
  update(&g,1);if(g.events&GAME_EVENT_EXPLOSION){explosions++;impact=t;
   GameBlast blast;assert(game_get_blast(&g,&blast) && fabsf(blast.radius-EGG_BLAST_RADIUS*g.view_scale)<.01f);
   assert(game_projectile_count(&g)==1 && game_get_projectile(&g,0,&white) && white.type==BIRD_WHITE_TYPE);
   assert(!object(&g,0).alive && object(&g,1).health>0 && object(&g,1).health<1);
   assert(object(&g,2).health==1 && object(&g,3).health==1 && !game_special(&g));
  }
 }
 assert(explosions==1 && impact>=0);GameBlast blast;assert(!game_get_blast(&g,&blast));
 // Fresh next bird has its own unused ability and no old egg.
 update(&g,1300);assert(g.state==AIMING);game_launch(&g);
 assert(game_projectile_count(&g)==1 && game_get_projectile(&g,0,&white) && !white.special_used && game_special_available(&g));
 // Reset/load destroys an airborne egg; time-limited turns also discard it.
 assert(game_special(&g));game_reset(&g);assert(!game_projectile_count(&g) && !game_get_blast(&g,&blast));
 g.unlocked=2;assert(game_load_level(&g,1));game_launch(&g);assert(game_special(&g));g.flight_time=11;update(&g,1);
 assert(g.state==AIMING && game_projectile_count(&g)==1 && !game_get_projectile(&g,1,&egg));
 // Ability is unavailable after real contact when no egg was dropped.
 game_reset(&g);g.angle=10;g.power=160;game_launch(&g);bool contact=false;
 for(int t=0;t<1200 && g.state==FLYING;t++){update(&g,1);if(game_get_projectile(&g,0,&white) && white.collided){contact=true;break;}}
 assert(contact && !game_special_available(&g) && !game_special(&g));
 for(int i=0;i<30;i++){assert(game_load_level(&g,1));game_launch(&g);assert(game_special(&g));update(&g,1);game_reset(&g);assert(!game_projectile_count(&g));}
 g.unlocked=3;assert(game_load_level(&g,2));game_launch(&g);assert(game_special(&g));bool dynamic_blast=false;
 for(int t=0;t<120;t++){update(&g,1);if(g.events&GAME_EVENT_EXPLOSION){dynamic_blast=true;assert(game_get_blast(&g,&blast) && blast.y<g.view_y+20*g.view_scale);break;}}
 assert(dynamic_blast && !object(&g,0).alive && object(&g,1).alive);
 game_destroy(&g);assert(!g.physics);
 puts("White OK: one egg, no extra queue consumption, vertical drop/recoil, parent isolation, floor/dynamic impact blast/damage/falloff/static immunity, one event, surviving parent, next bird, contact rejection, timeout/reset cleanup and 30 world transitions.");
}
