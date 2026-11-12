#include "game.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

int main() {
    Game g;game_init(&g);
    assert(game_object_count(&g)==19 && g.pig_alive==1 && g.block_alive==14 && g.birds==3);
    GameObject pig;assert(game_get_object(&g,0,&pig));
    assert(fabsf(pig.x-(VIEW_X+41.919f*VIEW_SCALE))<0.001f);
    assert(!game_get_object(&g,-1,&pig) && !game_get_object(&g,19,&pig));
    for(int i=0;i<360;i++)game_update(&g,1.0f/120);
    assert(g.state==AIMING && g.pig_alive==1 && g.score==0);
    game_aim(&g,1000,-1000);assert(g.angle==ANGLE_MAX && g.power==POWER_MIN);
    game_aim(&g,-1000,1000);assert(g.angle==ANGLE_MIN && g.power==POWER_MAX);
    game_reset(&g);float x,y;game_preview(&g,0.5f,&x,&y);game_launch(&g);game_launch(&g);
    assert(g.birds==2);
    for(int i=0;i<60;i++)game_update(&g,1.0f/120);
    assert(fabsf(g.x-x)<0.1f && fabsf(g.y-y)<0.1f);
    int wins=0,destruction=0;float winning_angle=0,winning_power=0;
    for(int angle=10;angle<=70;angle+=3)for(int power=20;power<=100;power+=3) {
        game_reset(&g);
        for(int i=0;i<120;i++)game_update(&g,1.0f/120);
        g.angle=(float)angle;g.power=(float)power;game_launch(&g);
        for(int i=0;i<1320 && g.state==FLYING;i++)game_update(&g,1.0f/120);
        assert(isfinite(g.x) && isfinite(g.y));
        if(g.block_alive<14)destruction++;
        if(g.state==WON){assert(g.score>=25000);wins++;winning_angle=angle;winning_power=power;}
    }
    assert(wins>0 && destruction>0);
    game_reset(&g);
    for(int shot=0;shot<3;shot++) {
        g.angle=85;g.power=20;game_launch(&g);
        for(int i=0;i<1320 && g.state==FLYING;i++)game_update(&g,1.0f/120);
    }
    assert(g.state==LOST && g.birds==0);
    game_launch(&g);assert(g.birds==0);
    game_reset(&g);assert(g.pig_alive==1 && g.block_alive==14 && g.score==0);
    game_destroy(&g);assert(g.physics==NULL);
    printf("Box2D OK: 19 objects, imported positions, settling, trajectory, %d wins, %d destruction shots, loss/reset. Example %.0f degrees / %.0f power\n",wins,destruction,winning_angle,winning_power);
}
