/* Which comic scene plays when (cutscenes.lua scenes tied to the port's stages). Pure decisions, no drawing: main.c
   passes the function that starts a scene, and the host test (tests/scenes_test.cpp) passes a recorder. */
#include "scenes.h"
#include "menu.h"
#include "game.h"
#include <string.h>
#ifdef PS2BIRDS_RIO
/* Rio (CutsceneScene.lua, tools/rio/extract_cutscene_rio.py): theme1Start opens Smugglers' Den and theme1End closes it. The other
   endings (rio_end2..6) are in CUTSCENE.BIN but their episodes are not in the port yet. */
static const char *const episode_scene[][2]={{"SMUGGLERS DEN","rio_intro"}};
static const char *const episode_done_scene[][2]={{"SMUGGLERS DEN","rio_end1"}};
static const char *const world_intro_scene[][2]={{"~","~"}};
static const char *const world_scene[][2]={{"~","~"}};
#else
static const char *const episode_scene[][2]={{"POACHED EGGS","pack1_intro"},{"MIGHTY HOAX","pack2_intro"},
    {"DANGER ABOVE","pack3_intro"},{"BIG SETUP","pack4_intro"},{"HAM EM HIGH","pack5_intro"},{"MINE AND DINE","pack6_intro"},
    {"SURF AND TURF","pack8_intro"},{"BAD PIGGIES","pack9_intro"},{"SHORT FUSE","blackbird_intro"},
    {"PIGGY FARM","intro_cutscene_farm"},{"DINO","intro_cutscene_dino"},
    {"MIGHTY FEATHERS","rmf_classic_intro"},{"EGG DEFENDER","eggdefender_intro"}};
/* Scene after the last stage of an episode gets its first score (no worlds with dashes there). */
static const char *const episode_done_scene[][2]={{"MIGHTY FEATHERS","rmf_complete"},{"EGG DEFENDER","eggdefender_complete"}};
/* The seventh Birdday has its own opening, before its first world. */
static const char *const world_intro_scene[][2]={{"BIRDDAY 7-","pack7_intro"}};
static const char *const world_scene[][2]={
    {"POACHED EGGS 1-","world1_complete"},{"POACHED EGGS 2-","world2_complete"},{"POACHED EGGS 3-","world3_complete"},
    {"MIGHTY HOAX 4-","world4_complete"},{"MIGHTY HOAX 5-","world5_complete"},
    {"DANGER ABOVE 6-","world6_complete"},{"DANGER ABOVE 7-","world7_complete"},{"DANGER ABOVE 8-","world8_complete"},
    {"BIG SETUP 9-","world9_complete"},{"BIG SETUP 10-","world10_complete"},{"BIG SETUP 11-","world11_complete"},
    {"HAM EM HIGH 12-","world12_complete"},{"HAM EM HIGH 13-","world13_complete"},{"HAM EM HIGH 14-","world14_complete"},
    {"MINE AND DINE 15-","world15_complete"},{"MINE AND DINE 16-","world16_complete"},{"MINE AND DINE 17-","world17_complete"},
    {"BIRDDAY 2-","world18_complete"},{"BIRDDAY 3-","world19_complete"},{"BIRDDAY 4-","birdday3_complete"},
    {"SURF AND TURF 1-","worldS1_complete"},{"SURF AND TURF 2-","worldS2_complete"},{"SURF AND TURF 3-","worldS3_complete"},
    {"BAD PIGGIES 1-","world20_complete"},{"BAD PIGGIES 2-","world21_complete"},{"BAD PIGGIES 3-","world22_complete"},
    {"SHORT FUSE 1-","blackbird_middle_1"},{"SHORT FUSE 2-","blackbird_middle_2"},{"SHORT FUSE 3-","blackbird_end"},
    {"PIGGY FARM 3-","outro_cutscene_farm"},{"DINO 3-","outro_cutscene_dino"}};
#endif
/* Returns 1 when the stage starts right away (no scene). */
static int episode_intro_only(int level,const int *highscores,SceneStarter play){
    int e=menu_episode_of(level);if(e<0 || level!=menu_episode_first(e))return 1;
    for(int l=level;l<level+menu_episode_size(e);l++)if(highscores[l]>0)return 1;
    for(unsigned k=0;k<sizeof(episode_scene)/sizeof(episode_scene[0]);k++)if(!strcmp(menu_episode_name(e),episode_scene[k][0]))return !play(episode_scene[k][1]);
    return 1;
}
/* The first stage of a world with no score in that world: the world's opening, if it has one. */
static int world_intro(int level,const int *highscores,SceneStarter play){
    const char *t=game_level_title(level),*dash=strrchr(t,'-');if(!dash)return 1;
    size_t prefix=(size_t)(dash-t)+1;
    if(level>0 && !strncmp(game_level_title(level-1),t,prefix))return 1;
    for(int l=level;l<game_level_count() && !strncmp(game_level_title(l),t,prefix);l++)if(highscores[l]>0)return 1;
    for(unsigned k=0;k<sizeof(world_intro_scene)/sizeof(world_intro_scene[0]);k++)
        if(strlen(world_intro_scene[k][0])==prefix && !strncmp(world_intro_scene[k][0],t,prefix))return !play(world_intro_scene[k][1]);
    return 1;
}
int scenes_episode_intro(int level,const int *highscores,SceneStarter play){
    if(!episode_intro_only(level,highscores,play))return 0;
    return world_intro(level,highscores,play);
}
/* After a stage's first score: if that completes its world, the world's scene. */
void scenes_world_complete(int level,const int *highscores,SceneStarter play){
    {int e=menu_episode_of(level);
     if(e>=0)for(unsigned k=0;k<sizeof(episode_done_scene)/sizeof(episode_done_scene[0]);k++)if(!strcmp(menu_episode_name(e),episode_done_scene[k][0])){
        int first=menu_episode_first(e);for(int l=first;l<first+menu_episode_size(e);l++)if(highscores[l]<=0)return;
        play(episode_done_scene[k][1]);return;}}
    const char *t=game_level_title(level),*dash=strrchr(t,'-');if(!dash)return;
    size_t prefix=(size_t)(dash-t)+1;
    for(int l=0;l<game_level_count();l++)if(!strncmp(game_level_title(l),t,prefix) && highscores[l]<=0)return;
    for(unsigned k=0;k<sizeof(world_scene)/sizeof(world_scene[0]);k++)
        if(strlen(world_scene[k][0])==prefix && !strncmp(world_scene[k][0],t,prefix)){play(world_scene[k][1]);return;}
}

const char *const *scenes_all(void){
    static const char *list[96];static int ready;
    if(!ready){
        int n=0;
        for(unsigned k=0;k<sizeof(episode_scene)/sizeof(episode_scene[0]);k++)list[n++]=episode_scene[k][1];
        for(unsigned k=0;k<sizeof(episode_done_scene)/sizeof(episode_done_scene[0]);k++)list[n++]=episode_done_scene[k][1];
        for(unsigned k=0;k<sizeof(world_intro_scene)/sizeof(world_intro_scene[0]);k++)list[n++]=world_intro_scene[k][1];
        for(unsigned k=0;k<sizeof(world_scene)/sizeof(world_scene[0]);k++)list[n++]=world_scene[k][1];
        list[n]=0;ready=1;
    }
    return list;
}
