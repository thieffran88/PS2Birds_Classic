// Floating scores (gamelogic.lua addFloatingScore / updateFloatingScores).
#include "game.h"
#include <cassert>
#include <cmath>
#include <cstdio>
int main(){
    Game g;game_init(&g);
    game_add_popup(&g,100,200,500,POPUP_PIECE,1);
    const GamePopup *p=&g.popups[0];
    assert(p->life==0.6f && std::fabs(p->scale-(0.25f+500/3000.0f))<1e-5f && game_popup_scale(p)==0);
    GamePopup q=*p;q.time=0.125f;assert(std::fabs(game_popup_scale(&q)-p->scale/2)<1e-5f);   // growing
    q.time=0.3f;assert(game_popup_scale(&q)==p->scale);                                       // holding
    q.time=0.5f;assert(std::fabs(game_popup_scale(&q)-p->scale*0.4f)<1e-4f);                 // shrinking
    q.time=0.6f;assert(game_popup_scale(&q)==0);
    q.time=-0.5f;assert(game_popup_scale(&q)==0);                                           // queued (birds left, 0.5 s apart)
    game_add_popup(&g,0,0,5000,POPUP_PIG,1);assert(g.popups[1].life==0.9f && g.popups[1].scale==1.0f);
    game_add_popup(&g,0,0,0,POPUP_PIECE,1);assert(g.popup_next==2);                           // no score, no popup
    // A real win: POACHED EGGS 1-1 with its campaign shot puts a 5000 over the pig.
    game_load_level(&g,0);for(int i=0;i<240;i++)game_update(&g,1.0f/120);
    int pig=0;
    for(int shot=0;shot<3 && g.state==AIMING;shot++){   // the campaign script repeats this shot
        g.angle=-10;g.power=54;game_launch(&g);
        for(int t=0;t<2400 && g.state==FLYING;t++){game_update(&g,1.0f/120);for(int k=0;k<GAME_POPUPS;k++)if(g.popups[k].kind==POPUP_PIG && g.popups[k].score==5000)pig=1;}
    }
    assert(g.state==WON && pig);
    for(int t=0;t<240;t++)game_update(&g,1.0f/120);
    for(int k=0;k<GAME_POPUPS;k++)assert(game_popup_scale(&g.popups[k])==0);                 // all gone after 2 s, also after the win
    puts("Popups OK: floating scores grow 0.25 s, hold, shrink 0.25 s (0.6 s pieces at 0.25 + score/3000, 0.9 s pigs); a 1-1 win shows the pig's 5000.");
}
