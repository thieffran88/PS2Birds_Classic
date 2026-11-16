/* Interface and particle sprites from UI.BIN (tools/extract_ui.py): one 256 x 512 PSMT8 page with a
   256-colour CT32 palette, kept in the VRAM left over by the stage texture slots. */
#ifndef PS2BIRDS_UI_H
#define PS2BIRDS_UI_H
#include <gsKit.h>
enum {UI_STAR_1,UI_STAR_2,UI_STAR_3,UI_NEW_HIGHSCORE,UI_MENU,UI_RESTART,UI_NEXT,UI_FAILED_PIG,UI_RESUME,UI_SOUND,UI_SOUND_OFF,
      /* PARTICLES_1: fragments (3 wood, 5 glass, 3 rock), 3 feathers per bird (red, blue, yellow,
         black, green, big brother, boomerang), 3 smoke puffs, score sprites. */
      UI_WOOD=11,UI_LIGHT=14,UI_ROCK=19,UI_FEATHER=22,UI_SMOKE=43,UI_SCORE_5K=46,
      UI_SCORE_10K=47,   /* red, blue, yellow, black, white, boomerang */
      UI_PSTAR=53,       /* P_STAR_1-5 (levelCompleteStars) */
      UI_CLOUD=58,       /* EXPLOSION_CLOUD_1-6 (explosionBuff), stored at 0.35 */
      UI_COUNT=64};
void ui_set_data_path(const char *path);
int ui_init(GSGLOBAL *gs);   /* 0 without UI.BIN or VRAM: the screens fall back to text */
int ui_ready(void);
/* Centred at (x,y); rgba 0x80808080 is the original colour. */
void ui_draw(GSGLOBAL *gs,int id,float x,float y,float scale,unsigned int rgba);
/* Rotated (radians) about its centre. */
void ui_draw_rotated(GSGLOBAL *gs,int id,float x,float y,float scale,float angle,unsigned int rgba);
#endif
