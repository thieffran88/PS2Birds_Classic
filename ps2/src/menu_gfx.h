/* Menu graphics from MENU.BIN (tools/extract_menu.py): logo, PLAY button, episode cards,
   level buttons, stars and dots of the original, uploaded into the stage texture slots while
   a menu is open. */
#ifndef PS2BIRDS_MENU_GFX_H
#define PS2BIRDS_MENU_GFX_H
#include <gsKit.h>
enum {MG_LOGO,MG_PLAY_LEFT,MG_PLAY_MIDDLE,MG_PLAY_RIGHT,MG_SCOREBOX,MG_SCOREBOX_TEXT,MG_STARBOX,
      MG_ARROW_LEFT,MG_ARROW_RIGHT,MG_BACK,MG_LEVEL_CLOSED,MG_LEVEL_LOCK,MG_LEVEL_OPEN_1,
      MG_STARS_1=MG_LEVEL_OPEN_1+7,MG_DOT_WHITE=MG_STARS_1+3,MG_DOT_BLACK,MG_CARDS};
#define MG_CARD(e) (MG_CARDS+2*(e))
#define MG_TITLE(e) (MG_CARDS+2*(e)+1)
#define MG_MAX 96
void menu_gfx_set_data_path(const char *path);
int menu_gfx_show(GSGLOBAL *gs);    /* uploads the menu pages if needed; 0 without MENU.BIN */
int menu_gfx_active(void);
void menu_gfx_release(void);         /* frees the RAM copy once a stage bank is back */
int menu_gfx_count(void);
/* Centred at (x,y) plus the composition offset of the sprite; scale 1 = extracted size. */
void menu_gfx_draw(GSGLOBAL *gs,int id,float x,float y,float scale,unsigned int rgba);
/* Stretched between x0 and x1 (the PLAY button middle). */
void menu_gfx_draw_span(GSGLOBAL *gs,int id,float x0,float x1,float y,float scale);
int menu_gfx_width(int id);
int menu_gfx_height(int id);
#endif
