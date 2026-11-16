/* Stage selection in the original's order: episode list (episode_selection.lua), then a
   5 x 3 grid of stages per page (level_selection.lua: 15 buttons, page dots, locks, stars). */
#ifndef PS2BIRDS_MENU_H
#define PS2BIRDS_MENU_H
#ifdef __cplusplus
extern "C" {
#endif

#define MENU_COLUMNS 5
#define MENU_ROWS 3
#define MENU_PER_PAGE (MENU_COLUMNS*MENU_ROWS)
#define MENU_EPISODES_PER_PAGE 6
#define MENU_MAX_EPISODES 64

/* MENU_MAIN: main_menu.lua (logo, PLAY); MENU_EPISODES: one card per episode, left/right. */
enum {MENU_EPISODES=0,MENU_LEVELS=1,MENU_MAIN=2};
enum {MENU_KEY_UP=1,MENU_KEY_DOWN=2,MENU_KEY_LEFT=4,MENU_KEY_RIGHT=8,MENU_KEY_OK=16,MENU_KEY_BACK=32,MENU_KEY_PREV=64,MENU_KEY_NEXT=128};
enum {MENU_NONE=0,MENU_MOVED=1,MENU_ENTERED=2,MENU_LEFT_SCREEN=3,MENU_PLAY=4,MENU_LOCKED=5};

typedef struct {int screen,episode,level;} Menu;

/* Episodes are runs of consecutive stages sharing the title prefix ("POACHED EGGS 1-1" -> "POACHED EGGS"). */
int menu_episode_count(void);
int menu_episode_first(int episode);
int menu_episode_size(int episode);
const char *menu_episode_name(int episode);
int menu_episode_of(int level);
const char *menu_level_label(int level);   /* last word of the title: "1-1", "6" */

void menu_open(Menu *m,int level);         /* level grid of the stage's episode, cursor on it */
/* keys: MENU_KEY_* bits; unlocked: stages playable (index < unlocked). Returns MENU_*. */
int menu_input(Menu *m,unsigned keys,int unlocked);

#ifdef __cplusplus
}
#endif
#endif
