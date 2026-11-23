#ifndef PS2BIRDS_SCENES_H
#define PS2BIRDS_SCENES_H
#ifdef __cplusplus
extern "C" {
#endif
/* Starts the scene of that name; 0 when CUTSCENE.BIN has no such scene (the game then just goes on). */
typedef int (*SceneStarter)(const char *name);
/* Entering a stage: the episode's opening (first stage of an episode with no score) or the world's opening (first stage of a world
   with no score in it). Returns 1 when the stage starts right away (no scene played). */
int scenes_episode_intro(int level,const int *highscores,SceneStarter play);
/* After a stage's first score: the world's (or the episode's) closing scene, if that score completes it. */
void scenes_world_complete(int level,const int *highscores,SceneStarter play);
/* Every scene name the tables can start (null terminated list), for checking CUTSCENE.BIN. */
const char *const *scenes_all(void);
#ifdef __cplusplus
}
#endif
#endif
