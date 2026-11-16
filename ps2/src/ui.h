/* Level-end and pause interface sprites from UI.BIN (tools/extract_ui.py): one 256 x 256 CT16 page kept
   in the VRAM left over by the stage texture slots, so it is drawn over every bank. */
#ifndef PS2BIRDS_UI_H
#define PS2BIRDS_UI_H
#include <gsKit.h>
enum {UI_STAR_1,UI_STAR_2,UI_STAR_3,UI_NEW_HIGHSCORE,UI_MENU,UI_RESTART,UI_NEXT,UI_FAILED_PIG,UI_RESUME,UI_SOUND,UI_SOUND_OFF,UI_COUNT};
void ui_set_data_path(const char *path);
int ui_init(GSGLOBAL *gs);   /* 0 without UI.BIN or VRAM: the screens fall back to text */
int ui_ready(void);
/* Centred at (x,y); rgba 0x80808080 is the original colour. */
void ui_draw(GSGLOBAL *gs,int id,float x,float y,float scale,unsigned int rgba);
#endif
