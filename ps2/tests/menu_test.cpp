// Stage selection: episodes from the campaign titles and the 5 x 3 grid navigation.
#include "game.h"
#include "menu.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
int main(){
    int n=menu_episode_count(),total=0;
    assert(n>10 && n<=MENU_MAX_EPISODES);
    for(int e=0;e<n;e++){
        assert(menu_episode_size(e)>0 && menu_episode_first(e)==total);total+=menu_episode_size(e);
        assert(menu_episode_of(menu_episode_first(e))==e && menu_episode_of(total-1)==e);
        if(getenv("MENU_LIST"))printf("%2d %-24s %3d\n",e,menu_episode_name(e),menu_episode_size(e));
    }
    assert(total==game_level_count());
    assert(!strcmp(menu_episode_name(0),"POACHED EGGS") && !strcmp(menu_level_label(0),"1-1"));
    Menu m;menu_open(&m,0);assert(m.screen==MENU_LEVELS && m.episode==0 && m.level==0);
    assert(menu_input(&m,MENU_KEY_RIGHT,99)==MENU_MOVED && m.level==1);
    assert(menu_input(&m,MENU_KEY_DOWN,99)==MENU_MOVED && m.level==6);
    assert(menu_input(&m,MENU_KEY_NEXT,99)==MENU_MOVED && m.level==21);
    assert(menu_input(&m,MENU_KEY_OK,99)==MENU_PLAY);
    assert(menu_input(&m,MENU_KEY_OK,5)==MENU_LOCKED);
    menu_open(&m,0);assert(menu_input(&m,MENU_KEY_LEFT,99)==MENU_MOVED && m.level/MENU_PER_PAGE==(menu_episode_size(0)-1)/MENU_PER_PAGE);
    assert(menu_input(&m,MENU_KEY_BACK,99)==MENU_LEFT_SCREEN && m.screen==MENU_EPISODES);
    assert(menu_input(&m,MENU_KEY_DOWN,99)==MENU_MOVED && m.episode==1);
    assert(menu_input(&m,MENU_KEY_OK,1)==MENU_LOCKED && m.screen==MENU_EPISODES);
    assert(menu_input(&m,MENU_KEY_OK,999)==MENU_ENTERED && m.screen==MENU_LEVELS && m.level==0);
    assert(menu_input(&m,MENU_KEY_UP,999)==MENU_MOVED && m.level==10);
    // main menu (main_menu.lua): PLAY opens the episode cards; back from the cards returns to it
    m.screen=MENU_MAIN;assert(menu_input(&m,MENU_KEY_OK,999)==MENU_ENTERED && m.screen==MENU_EPISODES);
    assert(menu_input(&m,MENU_KEY_RIGHT,999)==MENU_MOVED && m.episode==2 && menu_input(&m,MENU_KEY_LEFT,999)==MENU_MOVED && m.episode==1);
    assert(menu_input(&m,MENU_KEY_BACK,999)==MENU_LEFT_SCREEN && m.screen==MENU_MAIN);
    assert(menu_input(&m,MENU_KEY_BACK,999)==MENU_LEFT_SCREEN && m.screen==MENU_MAIN);
    printf("Menu OK: %d episodes, main menu, episode cards, 5 x 3 stage grid, pages, locks and back.\n",n);
}
