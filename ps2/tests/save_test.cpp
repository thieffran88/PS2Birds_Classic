#include "save_mc.h"
#include "save_codec.h"
#include "mc_mock.h"
#include <cassert>
#include <cstring>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <cstdio>
#include <algorithm>
static std::map<std::string,std::vector<unsigned char>> files;
static std::function<int()> operation;
static bool delayed=false, directory=false, partial=false,full=false,changed=false;
static int card=0,infos=0;
static std::string opened;
static int submit(std::function<int()> f){assert(!operation);operation=f;delayed=true;return 0;}
extern "C" {
int game_level_count(){return 8;}
int SifLoadModule(const char*,int,const char*){return 1;}
int mcInit(int){return 0;}
int mcGetInfo(int,int,int*t,int*f,int*fmt){return submit([=](){*t=2;*f=8000;*fmt=1;++infos;return changed && infos==2?-1:card;});}
int mcOpen(int,int,const char*p,int flags){std::string path(p);return submit([=](){if(flags==1 && !files.count(path))return -4;if(flags!=1 && full)return -3;opened=path;if(flags&FIO_O_TRUNC)files[path].clear();return 0;});}
int mcRead(int,void*p,int n){return submit([=](){auto &v=files[opened];int size=std::min(n,(int)v.size());memcpy(p,v.data(),size);return size;});}
int mcWrite(int,const void*p,int n){return submit([=](){int size=partial && n==96?32:n;auto b=(const unsigned char*)p;files[opened].assign(b,b+size);return size;});}
int mcClose(int){return submit([](){return 0;});}
int mcFlush(int){return submit([](){return 0;});}
int mcGetDir(int,int,const char*,int,int,sceMcTblGetDir*e){return submit([=](){e->AttrFile=MC_ATTR_SUBDIR;return directory?1:0;});}
int mcMkDir(int,int,const char*){return submit([](){directory=true;return 0;});}
int mcSync(int,int*,int*r){assert(operation);if(delayed){delayed=false;return 0;}auto f=operation;operation=nullptr;*r=f();return 1;}
}
static void pump(){int i=0;while(save_mc_busy() && i++<500)save_mc_poll();assert(i<500);assert(!operation);}
static void init(Game &g){g=Game{};g.unlocked=1;infos=0;save_mc_init(&g);pump();}
static std::string status(){return save_mc_status();}
int main(){
 assert(save_crc32((const unsigned char*)"123456789",9)==0xcbf43926u);
 SaveRecord r{};r.count=3;r.unlocked=2;r.sequence=0xffffffffu;r.scores[0]=28000;
 unsigned char a[96],b[96];assert(save_encode(&r,a));SaveRecord decoded{};assert(save_decode(a,96,&decoded));assert(decoded.scores[0]==28000);
 for(int i=0;i<96;i++){memcpy(b,a,96);b[i]^=1;assert(!save_decode(b,96,&decoded));}
 for(int i=0;i<96;i++)assert(!save_decode(a,i,&decoded));
 r.sequence=0;assert(save_encode(&r,b));assert(save_choose(a,96,b,96,&decoded)==1);
 r.unlocked=4;assert(!save_encode(&r,b));r.unlocked=2;r.scores[3]=1;assert(!save_encode(&r,b));
 Game g;card=-12;init(g);assert(status().find("SEM MC1")!=std::string::npos);assert(files.empty());
 card=-2;init(g);assert(status().find("NAO FORMATADO")!=std::string::npos);assert(files.empty());
 card=0;init(g);assert(g.unlocked==1);g.unlocked=2;g.highscores[0]=28000;save_mc_request();assert(save_mc_busy());pump();assert(status()=="PROGRESSO SALVO NO MC1");assert(files["/PS2BIRDS/icon.sys"].size()==964);assert(files["/PS2BIRDS/save.icn"].size()>32768);
 init(g);assert(g.unlocked==2 && g.highscores[0]==28000);
 g.unlocked=3;g.highscores[1]=23000;save_mc_request();pump();assert(files.count("/PS2BIRDS/progress_b.dat"));init(g);assert(g.unlocked==3);
 files["/PS2BIRDS/progress_b.dat"][40]^=1;init(g);assert(g.unlocked==2);
 partial=true;g.unlocked=3;save_mc_request();pump();assert(status().find("ERRO")!=std::string::npos);partial=false;init(g);assert(g.unlocked==2);
 full=true;save_mc_request();pump();assert(status()=="MC1 SEM ESPACO");full=false;
 changed=true;infos=0;save_mc_request();pump();assert(status().find("TROCADO")!=std::string::npos);changed=false;
 files["/PS2BIRDS/progress_a.dat"][0]^=1;auto before=files;init(g);save_mc_request();pump();assert(status().find("INVALIDO")!=std::string::npos);assert(files==before);
 files.clear();directory=false;g=Game{};g.unlocked=1;save_mc_init(&g);g.unlocked=2;save_mc_request();pump();init(g);assert(g.unlocked==2);
 files.clear();directory=true;SaveRecord old{};old.count=3;old.unlocked=3;old.sequence=1;old.scores[0]=28000;old.scores[2]=18000;
 assert(save_encode(&old,a));files["/PS2BIRDS/progress_a.dat"].assign(a,a+96);init(g);assert(g.unlocked==4 && g.highscores[2]==18000 && g.highscores[3]==0);
 save_mc_request();pump();assert(save_decode(files["/PS2BIRDS/progress_b.dat"].data(),96,&decoded) && decoded.count==8);
 old.scores[2]=0;assert(save_encode(&old,a));files.clear();files["/PS2BIRDS/progress_a.dat"].assign(a,a+96);init(g);assert(g.unlocked==3);
 old.count=5;old.unlocked=5;old.scores[4]=41500;assert(save_encode(&old,a));files.clear();files["/PS2BIRDS/progress_a.dat"].assign(a,a+96);init(g);assert(g.unlocked==6 && g.highscores[4]==41500);
 old.count=9;old.unlocked=9;assert(save_encode(&old,a));files.clear();files["/PS2BIRDS/progress_a.dat"].assign(a,a+96);before=files;init(g);save_mc_request();pump();assert(status().find("MAIS NOVA")!=std::string::npos && files==before);
 puts("save_test: CRC, 96 corruptions, truncation, sequence wrap, async MC, missing/unformatted/full/swapped card, alternating recovery, partial write, corrupt preservation, queued save: PASS");
}
