#ifndef PS2BIRDS_SPRITES_H
#define PS2BIRDS_SPRITES_H
#include <gsKit.h>
#include "sprite_data.h"
int sprites_init(GSGLOBAL *gs);
void sprite_draw_tint(GSGLOBAL *gs,int id,float x,float y,float scale,float angle,unsigned int rgba);
void sprite_draw(GSGLOBAL *gs,int id,float x,float y,float scale,float angle);
#endif
