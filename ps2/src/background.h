/* Stage backgrounds from THEMES.BIN (tools/extract_themes.py): one opaque 640 x 648 RGBA5551
   picture per theme (sky, bgLayers, ground colour, fgLayers; ground line at row 448). For a
   stage the 448 rows around its ground line are copied into the frame buffer each frame,
   so no VRAM is taken (the texture slots are full). */
#ifndef PS2BIRDS_BACKGROUND_H
#define PS2BIRDS_BACKGROUND_H
#include <gsKit.h>
void background_set_data_path(const char *path);
int background_init(void);                    /* reads the table; 0 without THEMES.BIN */
/* A fixed terrain piece painted into the picture with the theme's ground texture. */
typedef struct {int sprite;float x,y,angle,scale;int flip_x,flip_y;} BackgroundPiece;
/* Reads the theme when it changes and rebuilds the picture when theme, ground line or stage
   change; the stage's fixed terrain pieces are painted then. 0: no picture. */
int background_prepare(int theme,int ground_y,int stage,const BackgroundPiece *pieces,int count);
void background_draw(GSGLOBAL *gs);            /* queues the copy into the frame being drawn */
#endif
