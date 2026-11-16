#include "menu.h"
#include "game.h"
#include <string.h>

static int built=0,episodes=0,first[MENU_MAX_EPISODES+1];
static char names[MENU_MAX_EPISODES][40];

static int prefix_length(const char *title){
    const char *space=strrchr(title,' ');
    return space?(int)(space-title):(int)strlen(title);
}
static void build(void){
    if(built)return;
    built=1;episodes=0;
    int count=game_level_count();
    for(int i=0;i<count;i++){
        const char *t=game_level_title(i);int n=prefix_length(t);
        if(episodes>0 && (int)strlen(names[episodes-1])==(n<39?n:39) && !strncmp(names[episodes-1],t,n<39?n:39))continue;
        if(episodes==MENU_MAX_EPISODES)break;
        if(n>39)n=39;
        memcpy(names[episodes],t,n);names[episodes][n]=0;first[episodes++]=i;
    }
    first[episodes]=count;
}
int menu_episode_count(void){build();return episodes;}
int menu_episode_first(int e){build();return e>=0 && e<episodes?first[e]:0;}
int menu_episode_size(int e){build();return e>=0 && e<episodes?first[e+1]-first[e]:0;}
const char *menu_episode_name(int e){build();return e>=0 && e<episodes?names[e]:"";}
int menu_episode_of(int level){
    build();
    for(int e=episodes-1;e>=0;e--)if(level>=first[e])return e;
    return 0;
}
const char *menu_level_label(int level){
    const char *t=game_level_title(level),*space=strrchr(t,' ');
    return space?space+1:t;
}
void menu_open(Menu *m,int level){
    m->screen=MENU_LEVELS;m->episode=menu_episode_of(level);
    m->level=level-menu_episode_first(m->episode);
}
static int wrap(int v,int n){return n<=0?0:((v%n)+n)%n;}
int menu_input(Menu *m,unsigned keys,int unlocked){
    build();
    if(m->screen==MENU_MAIN){
        if(keys&MENU_KEY_OK){m->screen=MENU_EPISODES;return MENU_ENTERED;}
        if(keys&MENU_KEY_BACK)return MENU_LEFT_SCREEN;
        return MENU_NONE;
    }
    if(m->screen==MENU_EPISODES){
        int e=m->episode;
        if(keys&(MENU_KEY_UP|MENU_KEY_LEFT))e--;
        if(keys&(MENU_KEY_DOWN|MENU_KEY_RIGHT))e++;
        if(keys&MENU_KEY_PREV)e-=MENU_EPISODES_PER_PAGE;
        if(keys&MENU_KEY_NEXT)e+=MENU_EPISODES_PER_PAGE;
        e=wrap(e,episodes);
        if(keys&MENU_KEY_OK){
            m->episode=e;
            if(first[e]>=unlocked)return MENU_LOCKED;
            m->screen=MENU_LEVELS;m->level=0;return MENU_ENTERED;
        }
        if(keys&MENU_KEY_BACK){m->screen=MENU_MAIN;return MENU_LEFT_SCREEN;}
        if(e!=m->episode){m->episode=e;return MENU_MOVED;}
        return MENU_NONE;
    }
    int size=first[m->episode+1]-first[m->episode],l=m->level;
    int page=l/MENU_PER_PAGE,pages=(size+MENU_PER_PAGE-1)/MENU_PER_PAGE,cell=l%MENU_PER_PAGE;
    int col=cell%MENU_COLUMNS,row=cell/MENU_COLUMNS;
    if(keys&MENU_KEY_LEFT)col--;
    if(keys&MENU_KEY_RIGHT)col++;
    if(keys&MENU_KEY_UP)row--;
    if(keys&MENU_KEY_DOWN)row++;
    if(col<0){col=MENU_COLUMNS-1;page--;}
    if(col>=MENU_COLUMNS){col=0;page++;}
    row=wrap(row,MENU_ROWS);
    if(keys&MENU_KEY_PREV)page--;
    if(keys&MENU_KEY_NEXT)page++;
    page=wrap(page,pages);
    l=page*MENU_PER_PAGE+row*MENU_COLUMNS+col;
    if(l>=size)l=size-1;
    if(keys&MENU_KEY_BACK){m->screen=MENU_EPISODES;return MENU_LEFT_SCREEN;}
    if(keys&MENU_KEY_OK){m->level=l;return first[m->episode]+l<unlocked?MENU_PLAY:MENU_LOCKED;}
    if(l!=m->level){m->level=l;return MENU_MOVED;}
    return MENU_NONE;
}
