#include "level_end.h"
#include <cassert>
#include <cstdio>
int main(){
    LevelEnd e;level_end_start(&e,40000,30000,3,30000);
    // 40000/30000*2-0.5 = 2.1667 s of counting
    assert(level_end_count_time(&e)>2.16f && level_end_count_time(&e)<2.17f);
    assert(level_end_shown_score(&e)==0 && level_end_shown_stars(&e)==0 && level_end_counting(&e));
    level_end_update(&e,0.4f);assert(level_end_shown_stars(&e)==1);
    level_end_update(&e,0.8f);assert(level_end_shown_stars(&e)==2);   // 1.2 s
    int mid=level_end_shown_score(&e);assert(mid>20000 && mid<25000);
    level_end_update(&e,0.8f);assert(level_end_shown_stars(&e)==3);   // 2.0 s
    level_end_update(&e,0.2f);assert(level_end_shown_score(&e)==40000 && !level_end_counting(&e));
    assert(!level_end_new_highscore(&e));
    level_end_update(&e,1.0f);assert(level_end_new_highscore(&e) && level_end_shade(&e)==0.65f);
    // high scores cap the count at 2.7 s; stars never exceed the earned ones; no badge without a record
    level_end_start(&e,900000,900000,1,30000);assert(level_end_count_time(&e)==2.7f);
    level_end_update(&e,10);assert(level_end_shown_stars(&e)==1 && !level_end_new_highscore(&e));
    puts("Level end OK: score count-up (min(score/silver*2-0.5, 2.7) s), star timing, new-highscore badge and shade from level_end.lua.");
    return 0;
}
