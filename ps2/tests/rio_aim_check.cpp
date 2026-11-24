/* Developer check, not part of `make test` (needs Rio data): the aim of the Rio against the original slingshot.
   Rio gamelogic.lua (launchBird): impulse = unit(displacement) * (-defaultForce * physicsScale * mass) * min(1, rubberBandLength / rubberBandMaximumLength),
   defaultForce -925 (updateValues), physicsScale 1/20 (physicsToWorld 20 in all 459 stages), rubberBandMaximumLength = shootRange (2.2) + 3.2 = 5.4;
   RocketBird x1.42. So the launch speed is 46.25 * stretch ratio whatever the bird's mass, the same as the Classic.
   Usage: PS2BIRDS_CAMPAIGN=CAMPAIGN.BIN rio_aim_check (see AUXILIARES.md). Checks every 10th stage:
   the speed after the launch for several powers and angles, and that the preview dots lie on the real flight. */
#include <cmath>
#include <initializer_list>
#include <cstdio>
#include <cstdlib>
#include "game.h"
int main(){
    Game g;game_init(&g);int n=game_level_count(),checked=0,bad=0;float worst_speed=0,worst_preview=0;
    for(int l=0;l<n;l+=(n>45?10:1)){
        g.unlocked=n;if(!game_load_level(&g,l))continue;
        for(int i=0;i<240;i++)game_update(&g,1.0f/120);
        for(float power : {20.0f,50.0f,75.0f,100.0f})for(float angle : {5.0f,30.0f,60.0f}){
            game_load_level(&g,l);for(int i=0;i<240;i++)game_update(&g,1.0f/120);
            g.angle=angle;g.power=power;
            float px,py;game_preview(&g,0.5f,&px,&py);
            game_launch(&g);GameBird b;if(!game_get_projectile(&g,0,&b))continue;
            float speed=hypotf(b.vx,b.vy)/g.view_scale,want=LAUNCH_SPEED_MAX*power/POWER_MAX;
            if(fabsf(speed-want)/want>worst_speed)worst_speed=fabsf(speed-want)/want;
            if(fabsf(speed-want)/want>0.01f){bad++;printf("stage %d power %.0f angle %.0f: speed %.2f, expected %.2f\n",l,power,angle,speed,want);}
            for(int t=0;t<60;t++)game_update(&g,1.0f/120);   /* 0.5 s of flight */
            if(game_get_projectile(&g,0,&b) && !b.collided){float d=hypotf(b.x-px,b.y-py)/g.view_scale;if(d>worst_preview)worst_preview=d;}
            checked++;
        }
    }
    printf("%d shots checked: worst speed error %.3f %%, worst preview distance after 0.5 s of free flight %.3f units, %d outside 1 %%\n",checked,worst_speed*100,worst_preview,bad);
    return bad?1:0;
}
