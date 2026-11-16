#include "level_end.h"
#include <math.h>

void level_end_start(LevelEnd *e,int score,int old_score,int stars,int silver){
    e->timer=0;e->score=score;e->old_score=old_score;e->stars=stars;e->silver=silver;
}
void level_end_update(LevelEnd *e,float seconds){e->timer+=seconds;}
float level_end_count_time(const LevelEnd *e){
    float t=e->silver>0?(float)e->score/e->silver*2-0.5f:2.7f;
    if(t>2.7f)t=2.7f;
    return t;
}
int level_end_shown_score(const LevelEnd *e){
    float d=level_end_count_time(e);
    if(d>e->timer)return (int)floorf(e->score*(e->timer<d?e->timer:d)/d);
    return e->score;
}
int level_end_shown_stars(const LevelEnd *e){
    float reach=e->timer*1.25f+0.5f;if(reach>3)reach=3;
    int n=(int)floorf(reach);
    return n<e->stars?n:e->stars;
}
int level_end_new_highscore(const LevelEnd *e){return e->score>e->old_score && e->timer>=level_end_count_time(e)+1;}
int level_end_counting(const LevelEnd *e){return e->timer<level_end_count_time(e);}
float level_end_shade(const LevelEnd *e){return e->timer<0.65f?e->timer:0.65f;}
