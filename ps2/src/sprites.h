#ifndef PS2BIRDS_SPRITES_H
#define PS2BIRDS_SPRITES_H
#include <gsKit.h>
#include "sprite_data.h"
int sprites_init(GSGLOBAL *gs);
/* Uploads the texture pages of a bank into the shared slots; no-op if already active. */
int sprites_select_bank(GSGLOBAL *gs,int bank);
int sprites_bank(void);
/* Where SPRITES.BIN (texture banks 1+) is read from; set before changing banks. */
void sprites_set_data_path(const char *path);
/* 1 if the last bank change could not read SPRITES.BIN. */
int sprites_load_failed(void);
void sprite_draw_tint(GSGLOBAL *gs,int id,float x,float y,float scale,float angle,unsigned int rgba);
void sprite_draw(GSGLOBAL *gs,int id,float x,float y,float scale,float angle);
#endif
