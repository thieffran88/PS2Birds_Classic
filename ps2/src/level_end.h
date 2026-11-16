/* Level-complete screen timing, translated from level_end.lua (LevelComplete.update,
   updateScoreCounter, checkHighScore): the score counts up for
   min(score / silverScore * 2 - 0.5, 2.7) s, star i appears when timer * 1.25 + 0.5 >= i,
   the shade fades in to 0.65, and the new-highscore badge shows 1 s after the count. */
#ifndef PS2BIRDS_LEVEL_END_H
#define PS2BIRDS_LEVEL_END_H
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {float timer,next_count;int score,old_score,stars,silver;} LevelEnd;  /* next_count: next score_count retrigger */
void level_end_start(LevelEnd *e,int score,int old_score,int stars,int silver);
void level_end_update(LevelEnd *e,float seconds);
float level_end_count_time(const LevelEnd *e);
int level_end_shown_score(const LevelEnd *e);
int level_end_shown_stars(const LevelEnd *e);
int level_end_new_highscore(const LevelEnd *e);  /* badge visible */
int level_end_counting(const LevelEnd *e);       /* score_count sound still playing */
float level_end_shade(const LevelEnd *e);        /* 0..0.65 */
#ifdef __cplusplus
}
#endif
#endif
