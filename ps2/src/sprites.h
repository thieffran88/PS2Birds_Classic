#ifndef PS2BIRDS_SPRITES_H
#define PS2BIRDS_SPRITES_H
#include <gsKit.h>
#include "sprite_data.h"
int sprites_init(GSGLOBAL *gs);
/* Uploads the texture pages of a bank into the shared slots; no-op if already active. */
int sprites_select_bank(GSGLOBAL *gs,int bank);
int sprites_bank(void);
void sprite_draw_tint(GSGLOBAL *gs,int id,float x,float y,float scale,float angle,unsigned int rgba);
void sprite_draw(GSGLOBAL *gs,int id,float x,float y,float scale,float angle);
#endif
