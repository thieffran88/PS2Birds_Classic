#ifndef PS2BIRDS_SPRITES_H
#define PS2BIRDS_SPRITES_H
#include <gsKit.h>
#include "sprite_ids.h"
int sprites_init(GSGLOBAL *gs);
/* Uploads the texture pages of a bank into the shared slots; no-op if already active. */
int sprites_select_bank(GSGLOBAL *gs,int bank);
int sprites_bank(void);
/* Menu pages (MENU.BIN) uploaded into the stage texture slots; sprites_bank() is then
   SPRITES_MENU_BANK until a stage bank is selected again. */
#define SPRITES_MENU_BANK (-2)
int sprites_upload_pages(GSGLOBAL *gs,unsigned char *pixels,const unsigned *w,const unsigned *h,int count);
GSTEXTURE *sprites_slot(int i);
/* The stage bank's RAM buffer lent to a menu (NULL if smaller than need); the stage bank is
   read again afterwards. sprites_upload_page sends one page into slot i and waits for it. */
unsigned char *sprites_borrow_buffer(unsigned need,unsigned *size);
int sprites_upload_page(GSGLOBAL *gs,int i,unsigned char *pixels,unsigned w,unsigned h);
/* RGBA (alpha 0..128) of a frame of the active bank, still in RAM: size, pivot, row stride in pixels. */
const unsigned char *sprites_frame_rgba(int id,int *w,int *h,int *px,int *py,int *stride);
/* Where SPRITES.BIN (frame tables, every texture bank, save icon) is read from; set before sprites_init. */
void sprites_set_data_path(const char *path);
/* 1 if the last bank change could not read SPRITES.BIN. */
int sprites_load_failed(void);
/* Copies the memory-card icon stored in SPRITES.BIN; returns its size or -1. */
int sprites_read_icon(unsigned char *out,unsigned capacity);
/* Mirror the following draws (negative scaleX/scaleY of a piece); 0,0 restores. */
void sprites_set_flip(int x,int y);
void sprite_draw_tint(GSGLOBAL *gs,int id,float x,float y,float scale,float angle,unsigned int rgba);
void sprite_draw(GSGLOBAL *gs,int id,float x,float y,float scale,float angle);
#endif
