/* Comic cutscenes from CUTSCENE.BIN (tools/extract_cutscene.py): one wide picture per scene,
   448 rows tall, scrolled from its left edge to its right edge with cubic_in_out easing, as
   cutscenes.lua does (pack1_intro: 9.6 s of scroll, the scene ends at 15.6 s). The visible 640
   columns are copied into the frame each frame, like the stage backgrounds: no VRAM is taken. */
#ifndef PS2BIRDS_CUTSCENE_H
#define PS2BIRDS_CUTSCENE_H
#include <gsKit.h>
void cutscene_set_data_path(const char *path);
/* Loads the scene named as in cutscenes.lua ("pack1_intro", "world1_complete"...); 0 when
   CUTSCENE.BIN is missing or has no such scene (the game then just goes on). */
int cutscene_start(const char *name);
int cutscene_draw(GSGLOBAL *gs,float time);  /* queues the frame at that time; 0 once the scene is over */
void cutscene_stop(void);                    /* frees the picture */
#endif
