/* Original bitmap fonts from FONT.BIN (tools/extract_font.py): FONT_BASIC for the HUD and
   menus, FONT_SCORE for floating scores. One PSMT4 page, one 16-colour palette per font. */
#ifndef PS2BIRDS_FONT_H
#define PS2BIRDS_FONT_H
#include <gsKit.h>
enum {FONT_BASIC=0,FONT_SCORE=1,FONT_COUNT=2};
enum {FONT_LEFT=0,FONT_CENTER=1,FONT_RIGHT=2};
void font_set_data_path(const char *path);
int font_init(GSGLOBAL *gs);     /* 0 without FONT.BIN or VRAM: the built-in pixel font is used */
int font_ready(void);
/* UTF-8 text; y is the top of the line; rgba tints the white fill (0x80808080 = original). */
float font_width(int font,const char *utf8,float scale);
float font_line(int font,float scale);
void font_draw(GSGLOBAL *gs,int font,float x,float y,float scale,int align,unsigned int rgba,const char *utf8);
#endif
