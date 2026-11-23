// Cutscene triggers: a whole playthrough in order, checking when each scene plays.
#include "game.h"
#include "menu.h"
#include "scenes.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <vector>
static std::set<std::string> available;static bool have_file;
static std::vector<std::string> played;
static int play(const char *name){played.push_back(name);return !have_file || available.count(name);}
static void read_cutscene_names(const char *path){
    FILE *f=fopen(path,"rb");if(!f)return;
    unsigned char head[12];if(fread(head,1,12,f)!=12 || memcmp(head,"PBCS",4)){fclose(f);return;}
    unsigned count=head[8]|(head[9]<<8)|(head[10]<<16);
    for(unsigned i=0;i<count;i++){unsigned char e[72];if(fread(e,1,72,f)!=72)break;e[23]=0;available.insert((const char*)e);}
    fclose(f);have_file=!available.empty();
}
int main(){
    read_cutscene_names("assets/CUTSCENE.BIN");
    int n=game_level_count();assert(n>700);
    static int hs[2048];memset(hs,0,sizeof(hs));
    std::map<std::string,int> played_at_level,played_after_level;   // scene -> stage index where it played
    std::map<std::string,int> times;
    int intros=0,closings=0;
    for(int level=0;level<n;level++){
        played.clear();
        int right_away=scenes_episode_intro(level,hs,play);
        assert(played.size()<=1);                                    // an opening never chains two scenes
        assert(right_away==(played.empty() || !(!have_file || available.count(played[0]))));
        if(!played.empty()){played_at_level[played[0]]=level;times[played[0]]++;intros++;}
        hs[level]=1000;
        played.clear();
        scenes_world_complete(level,hs,play);
        assert(played.size()<=1);
        if(!played.empty()){played_after_level[played[0]]=level;times[played[0]]++;closings++;}
    }
    const char *const *all=scenes_all();int count=0;
    for(;all[count];count++){
        const char *s=all[count];
        assert(times[s]==1);                                          // every scene plays exactly once in a full playthrough
        if(have_file)assert(available.count(s));                      // and is in CUTSCENE.BIN
    }
    printf("   %d scenes, %d openings and %d closings in %d stages%s\n",count,intros,closings,n,have_file?"":" (no CUTSCENE.BIN: names not checked)");
    // Playing a stage that already has a score, or an episode already started, plays no opening.
    played.clear();assert(scenes_episode_intro(0,hs,play)==1 && played.empty());
    memset(hs,0,sizeof(hs));hs[1]=500;played.clear();assert(scenes_episode_intro(0,hs,play)==1 && played.empty());
    // Specific places (the port's titles): openings at the first stage, closings at the last stage of the world.
    #define AT(scene,title) do{assert(played_at_level.count(scene));assert(!strcmp(game_level_title(played_at_level[scene]),title));}while(0)
    #define AFTER(scene,title) do{assert(played_after_level.count(scene));assert(!strcmp(game_level_title(played_after_level[scene]),title));}while(0)
    AT("pack1_intro","POACHED EGGS 1-1");AT("pack2_intro","MIGHTY HOAX 4-1");AT("pack7_intro","BIRDDAY 7-1");
    AFTER("world1_complete","POACHED EGGS 1-21");AFTER("world3_complete","POACHED EGGS 3-21");
    if(getenv("SCENES_LIST")){
        for(auto &kv:played_at_level)printf("  abre   %-22s antes de %s\n",kv.first.c_str(),game_level_title(kv.second));
        for(auto &kv:played_after_level)printf("  fecha  %-22s depois de %s\n",kv.first.c_str(),game_level_title(kv.second));
    }
    puts("Scenes OK: every scene plays exactly once in a full playthrough, openings before the first stage and closings after the last stage of their world or episode, none repeated on stages already scored, all names in CUTSCENE.BIN.");
    return 0;
}
