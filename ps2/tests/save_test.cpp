#include "save_codec.h"
#include "save_mc.h"
#include "mc_mock.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <functional>
#include <map>
#include <string>
#include <vector>
static std::map<std::string,std::vector<unsigned char>> files;static bool full,directory;static std::string opened;static std::function<int()> pending;
static int submit(std::function<int()> f){pending=f;return 0;}
extern "C"{
int SifLoadModule(const char*,int,const char*){return 0;}
int mcInit(int){return 0;}
static int mock_level_count=84;
int game_level_count(){return mock_level_count;}
void game_init(Game*g){memset(g,0,sizeof(*g));g->unlocked=1;}
int mcGetInfo(int,int,int*type,int*free_blocks,int*format){*type=MC_TYPE_PS2;*free_blocks=full?0:1000;*format=1;return submit([](){return 0;});}
int mcOpen(int,int,const char*p,int flags){std::string path(p);return submit([=](){if(flags==1 && !files.count(path))return -4;if(flags!=1 && full)return -3;opened=path;if(flags&FIO_O_TRUNC)files[path].clear();return 0;});}
int mcRead(int,void*buffer,int size){return submit([=](){auto&f=files[opened];int n=(int)f.size()<size?(int)f.size():size;memcpy(buffer,f.data(),n);return n;});}
int mcWrite(int,const void*buffer,int size){return submit([=](){auto&f=files[opened];f.insert(f.end(),(const unsigned char*)buffer,(const unsigned char*)buffer+size);return size;});}
int mcClose(int){return submit([](){return 0;});}
int mcFlush(int){return submit([](){return 0;});}
int mcGetDir(int,int,const char*,int,int,sceMcTblGetDir*e){return submit([=](){if(!directory)return 0;e->AttrFile=MC_ATTR_SUBDIR;return 1;});}
int mcMkDir(int,int,const char*){return submit([](){directory=true;return 0;});}
int mcSync(int,int*,int*result){if(!pending)return 0;*result=pending();pending=nullptr;return 1;}
}
static void pump(){for(int i=0;i<200;i++)save_mc_poll();}
static std::string status(){return save_mc_status();}
static void init(Game&g){game_init(&g);save_mc_init(&g);pump();}
static std::vector<unsigned char> v1(uint32_t count,uint32_t unlocked,uint32_t sequence,std::initializer_list<uint32_t> scores){
    // Independent encoder for the 96-byte version 1 layout written by 0.6.0-0.8.0.
    std::vector<unsigned char> d(96,0);memcpy(d.data(),"PS2BIRD1",8);
    auto put=[&](size_t at,uint32_t v){for(int i=0;i<4;i++)d[at+i]=(unsigned char)(v>>(i*8));};
    put(8,1);put(12,0x434c4153u);put(16,sequence);put(20,count);put(24,unlocked);size_t i=0;
    for(uint32_t s:scores)put(28+4*i++,s);
    put(92,save_crc32(d.data(),92));return d;
}
static std::vector<unsigned char> v2(uint32_t count,uint32_t unlocked,uint32_t sequence,std::initializer_list<uint32_t> scores){
    // Independent encoder for the published 160-byte v2 layout; not the production encoder.
    std::vector<unsigned char> d(160,0);memcpy(d.data(),"PS2BIRD1",8);
    auto put=[&](size_t at,uint32_t v){for(int i=0;i<4;i++)d[at+i]=(unsigned char)(v>>(i*8));};
    put(8,2);put(12,0x434c4153u);put(16,sequence);put(20,count);put(24,unlocked);size_t i=0;
    for(uint32_t score:scores)put(28+4*i++,score);
    put(156,save_crc32(d.data(),156));return d;
}
int main(){
    unsigned char a[SAVE_BYTES],b[SAVE_BYTES];SaveRecord decoded;
    assert(save_crc32((const unsigned char*)"123456789",9)==0xcbf43926u);
    SaveRecord r{};r.count=21;r.unlocked=2;r.sequence=0xffffffffu;r.scores[0]=28000;
    assert(save_encode(&r,a) && save_decode(a,SAVE_BYTES,&decoded) && decoded.count==21 && decoded.version==SAVE_VERSION && decoded.scores[0]==28000);
    assert(!save_decode(a,SAVE_BYTES-1,&decoded) && !save_decode(a,SAVE_BYTES_V1,&decoded));
    for(int i=0;i<SAVE_BYTES;i++){memcpy(b,a,SAVE_BYTES);b[i]^=0x55;assert(!save_decode(b,SAVE_BYTES,&decoded));}
    SaveRecord bad=r;bad.unlocked=22;assert(!save_encode(&bad,b));bad=r;bad.count=513;assert(!save_encode(&bad,b));bad=r;bad.scores[21]=5;assert(!save_encode(&bad,b));
    r.sequence=0;r.scores[1]=23000;assert(save_encode(&r,b));assert(save_choose(a,SAVE_BYTES,b,SAVE_BYTES,&decoded)==1 && decoded.scores[1]==23000);
    assert(save_choose(b,SAVE_BYTES,a,SAVE_BYTES,&decoded)==0 && decoded.scores[1]==23000);
    assert(save_choose(a,SAVE_BYTES,b,SAVE_BYTES-5,&decoded)==0 && save_choose(a,1,b,2,&decoded)==-1);
    // Version 1 files still decode with their own size and capacity.
    auto old=v1(8,8,7,{28000,23000,17500,44500,22000,56000,23500,31000});
    assert(save_decode(old.data(),old.size(),&decoded) && decoded.version==1 && decoded.count==8 && decoded.scores[7]==31000);
    assert(!save_decode(old.data(),SAVE_BYTES,&decoded));
    Game g;init(g);assert(status().find("SEM SAVE")!=std::string::npos && !files.count("/PS2BIRDS/progress_a.dat"));
    g.unlocked=3;g.highscores[1]=23000;save_mc_request();pump();assert(files.count("/PS2BIRDS/progress_a.dat") && !files.count("/PS2BIRDS/progress_b.dat"));init(g);assert(g.unlocked==3 && g.highscores[1]==23000);
    assert(files["/PS2BIRDS/progress_a.dat"].size()==SAVE_BYTES);
    g.highscores[2]=18000;save_mc_request();pump();init(g);assert(g.highscores[2]==18000 && files.count("/PS2BIRDS/progress_b.dat") && files["/PS2BIRDS/progress_b.dat"].size()==SAVE_BYTES);
    auto before=files;full=true;save_mc_request();pump();assert(status().find("SEM ESPACO")!=std::string::npos && files==before);full=false;
    files["/PS2BIRDS/progress_a.dat"][40]^=1;init(g);assert(g.highscores[2]==18000 || g.highscores[1]==23000);
    files["/PS2BIRDS/progress_b.dat"].resize(10);files["/PS2BIRDS/progress_a.dat"][40]^=1;init(g);assert(status().find("INVALIDO")==std::string::npos);
    files["/PS2BIRDS/progress_a.dat"][40]^=1;before=files;init(g);assert(status().find("INVALIDO")!=std::string::npos);save_mc_request();pump();assert(files==before);
    // Legacy migration: stages were Level1..Level8 in file order; they now sit at campaign positions 1,6,4,7,8,5,9,10.
    files.clear();directory=true;files["/PS2BIRDS/progress_a.dat"]=v1(3,3,1,{28000,0,18000});init(g);
    assert(g.highscores[0]==28000 && g.highscores[5]==0 && g.highscores[3]==18000 && g.unlocked==6);
    save_mc_request();pump();assert(save_decode(files["/PS2BIRDS/progress_b.dat"].data(),SAVE_BYTES,&decoded) && decoded.count==84 && decoded.version==SAVE_VERSION && decoded.scores[3]==18000);
    files.clear();files["/PS2BIRDS/progress_a.dat"]=v1(5,5,1,{28000,23000,18000,44500,41500});init(g);
    assert(g.highscores[1]==0 && g.highscores[5]==23000 && g.highscores[7]==41500 && g.unlocked==9);
    files.clear();files["/PS2BIRDS/progress_a.dat"]=v1(8,8,1,{28000,23000,17500,44500,22000,56000,23500,31000});init(g);
    assert(g.highscores[9]==31000 && g.highscores[4]==56000 && g.unlocked==11 && g.highscores[1]==0 && g.highscores[2]==0);
    files.clear();files["/PS2BIRDS/progress_a.dat"]=v1(8,8,1,{28000,23000,17500,44500,22000,56000,23500,0});init(g);
    assert(g.unlocked==10 && g.highscores[9]==0);
    // Saves from a newer version that declare more stages are preserved untouched.
    SaveRecord newer{};newer.count=32;newer.unlocked=32;newer.sequence=3;assert(save_encode(&newer,a));a[20]=85;files.clear();files["/PS2BIRDS/progress_a.dat"].assign(a,a+SAVE_BYTES);before=files;init(g);save_mc_request();pump();assert(status().find("MAIS NOVA")!=std::string::npos && files==before);
    // A future-format slot must not be overwritten just because the other slot is readable.
    SaveRecord current{};current.count=21;current.unlocked=2;current.sequence=2;assert(save_encode(&current,a));
    std::vector<unsigned char> readable(a,a+SAVE_BYTES),future=readable;future[8]=SAVE_VERSION+1;
    assert(save_requires_newer(future.data(),future.size(),21));
    for(int slot=0;slot<2;slot++){
        files.clear();files["/PS2BIRDS/progress_a.dat"]=slot?readable:future;files["/PS2BIRDS/progress_b.dat"]=slot?future:readable;
        before=files;init(g);save_mc_request();pump();assert(status().find("MAIS NOVA")!=std::string::npos && files==before);
    }
    files.clear();files["/PS2BIRDS/progress_a.dat"]=readable;files["/PS2BIRDS/progress_b.dat"]=v1(9,9,1,{28000});before=files;
    init(g);save_mc_request();pump();assert(status().find("MAIS NOVA")!=std::string::npos && files==before);
    files.clear();current.count=21;current.unlocked=21;current.scores[20]=19000;assert(save_encode(&current,a));files["/PS2BIRDS/progress_a.dat"].assign(a,a+SAVE_BYTES);init(g);
    assert(g.unlocked==22 && g.highscores[20]==19000 && g.highscores[21]==0);save_mc_request();pump();
    assert(save_decode(files["/PS2BIRDS/progress_b.dat"].data(),SAVE_BYTES,&decoded) && decoded.count==84);
    current.scores[20]=0;assert(save_encode(&current,a));files.clear();files["/PS2BIRDS/progress_a.dat"].assign(a,a+SAVE_BYTES);init(g);assert(g.unlocked==21);
    // Expansion from the 27-stage release preserves the last score and unlocks only its successor.
    current={};current.count=27;current.unlocked=27;current.scores[26]=27500;assert(save_encode(&current,a));
    files.clear();files["/PS2BIRDS/progress_a.dat"].assign(a,a+SAVE_BYTES);init(g);
    assert(g.unlocked==28 && g.highscores[26]==27500 && g.highscores[27]==0);
    g.unlocked=32;g.highscores[31]=42000;save_mc_request();pump();init(g);
    assert(g.unlocked==32 && g.highscores[31]==42000 && g.highscores[26]==27500);
    assert(save_decode(files["/PS2BIRDS/progress_b.dat"].data(),SAVE_BYTES,&decoded) && decoded.count==84 && decoded.scores[31]==42000);
    current.scores[26]=0;assert(save_encode(&current,a));files.clear();files["/PS2BIRDS/progress_a.dat"].assign(a,a+SAVE_BYTES);init(g);assert(g.unlocked==27);
    // Maximum v3 capacity and final score survive encoding; bounds and every CRC byte are checked above.
    SaveRecord maximum{};maximum.count=64;maximum.unlocked=64;maximum.scores[63]=76500;
    assert(save_encode(&maximum,a) && save_decode(a,SAVE_BYTES,&decoded) && decoded.scores[63]==76500 && decoded.count==64);
    maximum.count=SAVE_LEVEL_CAPACITY+1;assert(!save_encode(&maximum,a));
    auto previous=v2(32,32,42,{});auto put_old=[&](size_t at,uint32_t value){for(int i=0;i<4;i++)previous[at+i]=(unsigned char)(value>>(i*8));};
    put_old(28,27000);put_old(28+31*4,42000);put_old(156,save_crc32(previous.data(),156));
    assert(save_decode(previous.data(),previous.size(),&decoded) && decoded.version==2 && decoded.count==32 && decoded.scores[31]==42000);
    assert(!save_decode(previous.data(),159,&decoded));
    for(int i=0;i<160;i++){auto corrupted=previous;corrupted[i]^=0x55;assert(!save_decode(corrupted.data(),corrupted.size(),&decoded));}
    // Migrate into the alternate slot and preserve the complete previous v2 record.
    files.clear();files["/PS2BIRDS/progress_a.dat"]=previous;init(g);
    assert(g.unlocked==33 && g.highscores[31]==42000 && g.highscores[0]==27000 && !g.highscores[32]);
    g.unlocked=34;g.highscores[33]=51500;save_mc_request();pump();
    assert(files["/PS2BIRDS/progress_a.dat"]==previous && files["/PS2BIRDS/progress_b.dat"].size()==SAVE_BYTES);
    assert(save_decode(files["/PS2BIRDS/progress_b.dat"].data(),SAVE_BYTES,&decoded) && decoded.version==SAVE_VERSION && decoded.sequence==43 && decoded.count==84);
    init(g);assert(g.unlocked==34 && g.highscores[33]==51500 && g.highscores[31]==42000);
    // Damaged/incomplete new slot falls back to the retained v2 copy.
    files["/PS2BIRDS/progress_b.dat"].resize(100);init(g);assert(g.unlocked==33 && g.highscores[31]==42000);
    put_old(28+31*4,0);put_old(156,save_crc32(previous.data(),156));files.clear();files["/PS2BIRDS/progress_a.dat"]=previous;init(g);assert(g.unlocked==32);
    current={};current.count=34;current.unlocked=34;current.scores[33]=28000;assert(save_encode(&current,a));
    files.clear();files["/PS2BIRDS/progress_a.dat"].assign(a,a+SAVE_BYTES);init(g);
    assert(g.unlocked==35 && g.highscores[33]==28000 && !g.highscores[34]);
    g.highscores[34]=37500;save_mc_request();pump();init(g);assert(g.unlocked==35 && g.highscores[34]==37500);
    current.scores[33]=0;assert(save_encode(&current,a));files.clear();files["/PS2BIRDS/progress_a.dat"].assign(a,a+SAVE_BYTES);init(g);assert(g.unlocked==34);
    // 0.14.0 saves declare 35 stages: winning the old last stage (2-14) unlocks 2-15, merely reaching it does not.
    current={};current.count=35;current.unlocked=35;current.sequence=9;current.scores[33]=28000;current.scores[34]=37500;assert(save_encode(&current,a));
    files.clear();files["/PS2BIRDS/progress_a.dat"].assign(a,a+SAVE_BYTES);init(g);
    assert(g.unlocked==36 && g.highscores[34]==37500 && !g.highscores[35]);
    g.unlocked=42;g.highscores[41]=50500;save_mc_request();pump();
    assert(save_decode(files["/PS2BIRDS/progress_b.dat"].data(),SAVE_BYTES,&decoded) && decoded.count==84 && decoded.unlocked==42 && decoded.sequence==10 && decoded.scores[41]==50500 && decoded.scores[34]==37500);
    init(g);assert(g.unlocked==42 && g.highscores[41]==50500);
    current.scores[34]=0;assert(save_encode(&current,a));files.clear();files["/PS2BIRDS/progress_a.dat"].assign(a,a+SAVE_BYTES);init(g);assert(g.unlocked==35);
    // 0.15.0 saves declare 42 stages: winning 2-21 unlocks 3-1.
    current={};current.count=42;current.unlocked=42;current.sequence=20;current.scores[41]=50500;assert(save_encode(&current,a));
    files.clear();files["/PS2BIRDS/progress_a.dat"].assign(a,a+SAVE_BYTES);init(g);assert(g.unlocked==43 && g.highscores[41]==50500 && !g.highscores[42]);
    g.unlocked=63;g.highscores[62]=77000;save_mc_request();pump();
    assert(save_decode(files["/PS2BIRDS/progress_b.dat"].data(),SAVE_BYTES,&decoded) && decoded.count==84 && decoded.unlocked==63 && decoded.scores[62]==77000);
    current.scores[41]=0;assert(save_encode(&current,a));files.clear();files["/PS2BIRDS/progress_a.dat"].assign(a,a+SAVE_BYTES);init(g);assert(g.unlocked==42);
    // Version 3 (288 bytes, 64 scores) written by 0.13.0-0.16.0, encoded independently.
    {
        std::vector<unsigned char> v3(288,0);memcpy(v3.data(),"PS2BIRD1",8);
        auto put=[&](size_t at,uint32_t v){for(int i=0;i<4;i++)v3[at+i]=(unsigned char)(v>>(i*8));};
        put(8,3);put(12,0x434c4153u);put(16,30);put(20,63);put(24,63);put(28,27000);put(28+59*4,71500);put(28+62*4,1000);put(284,save_crc32(v3.data(),284));
        assert(save_decode(v3.data(),288,&decoded) && decoded.version==3 && decoded.count==63 && decoded.scores[59]==71500);
        for(int i=0;i<288;i++){auto c=v3;c[i]^=0x55;assert(!save_decode(c.data(),c.size(),&decoded));}
        // 0.16.0 saves: winning 3-21 unlocks 4-1; the migrated v4 copy goes to the other slot.
        files.clear();files["/PS2BIRDS/progress_a.dat"]=v3;init(g);assert(g.unlocked==64 && g.highscores[59]==71500 && g.highscores[62]==1000 && !g.highscores[63]);
        g.unlocked=84;g.highscores[83]=40000;save_mc_request();pump();
        assert(files["/PS2BIRDS/progress_a.dat"]==v3 && files["/PS2BIRDS/progress_b.dat"].size()==SAVE_BYTES && SAVE_BYTES==2080);
        assert(save_decode(files["/PS2BIRDS/progress_b.dat"].data(),SAVE_BYTES,&decoded) && decoded.version==SAVE_VERSION && decoded.count==84 && decoded.sequence==31 && decoded.scores[83]==40000 && decoded.scores[0]==27000);
        init(g);assert(g.unlocked==84 && g.highscores[83]==40000);
        put(28+62*4,0);put(284,save_crc32(v3.data(),284));files.clear();files["/PS2BIRDS/progress_a.dat"]=v3;init(g);assert(g.unlocked==63);
        SaveRecord full{};full.count=512;full.unlocked=512;full.scores[511]=5;assert(save_encode(&full,a) && save_decode(a,SAVE_BYTES,&decoded) && decoded.scores[511]==5);
        full.count=513;assert(!save_encode(&full,a));
    }
    // Version 4 (544 bytes, 128 scores) written by 0.17.0-0.22.0, encoded independently.
    {
        std::vector<unsigned char> v4(544,0);memcpy(v4.data(),"PS2BIRD1",8);
        auto put=[&](size_t at,uint32_t v){for(int i=0;i<4;i++)v4[at+i]=(unsigned char)(v>>(i*8));};
        put(8,4);put(12,0x434c4153u);put(16,50);put(20,84);put(24,84);put(28,31000);put(28+83*4,61500);put(28+127*4,0);put(540,save_crc32(v4.data(),540));
        assert(save_decode(v4.data(),544,&decoded) && decoded.version==4 && decoded.count==84 && decoded.scores[83]==61500);
        for(int i=0;i<544;i+=7){auto c=v4;c[i]^=0x55;assert(!save_decode(c.data(),c.size(),&decoded));}
        // A completed 84-stage save unlocks stage 85; the migrated v5 copy goes to the other slot.
        mock_level_count=135;
        files.clear();files["/PS2BIRDS/progress_a.dat"]=v4;init(g);assert(g.unlocked==85 && g.highscores[83]==61500 && g.highscores[0]==31000);
        g.highscores[84]=12000;save_mc_request();pump();
        assert(files["/PS2BIRDS/progress_a.dat"]==v4 && files["/PS2BIRDS/progress_b.dat"].size()==SAVE_BYTES);
        assert(save_decode(files["/PS2BIRDS/progress_b.dat"].data(),SAVE_BYTES,&decoded) && decoded.version==SAVE_VERSION && decoded.sequence==51 && decoded.scores[84]==12000 && decoded.scores[83]==61500);
    }
    // Version 5 (1056 bytes, 256 scores) written by 0.23.0-0.27.0, encoded independently.
    {
        std::vector<unsigned char> v5(1056,0);memcpy(v5.data(),"PS2BIRD1",8);
        auto put=[&](size_t at,uint32_t v){for(int i=0;i<4;i++)v5[at+i]=(unsigned char)(v>>(i*8));};
        put(8,5);put(12,0x434c4153u);put(16,70);put(20,240);put(24,240);put(28,31000);put(28+239*4,52000);put(1052,save_crc32(v5.data(),1052));
        assert(save_decode(v5.data(),1056,&decoded) && decoded.version==5 && decoded.count==240 && decoded.scores[239]==52000);
        for(int i=0;i<1056;i+=11){auto c=v5;c[i]^=0x55;assert(!save_decode(c.data(),c.size(),&decoded));}
        mock_level_count=285;files.clear();files["/PS2BIRDS/progress_a.dat"]=v5;init(g);assert(g.unlocked==241 && g.highscores[239]==52000);
        g.highscores[240]=9000;save_mc_request();pump();
        assert(files["/PS2BIRDS/progress_a.dat"]==v5 && files["/PS2BIRDS/progress_b.dat"].size()==SAVE_BYTES);
        assert(save_decode(files["/PS2BIRDS/progress_b.dat"].data(),SAVE_BYTES,&decoded) && decoded.version==6 && decoded.sequence==71 && decoded.scores[240]==9000 && decoded.count==285);
    }
    puts("Save OK: CRC, v1/v2/v3 layouts, 64-score boundary, v2 migration/fallback, corruption, slots, full card, invalid saves preserved, legacy order migration, 35-to-42 and 42-to-63 and v3-to-v4 63-to-84 and v4-to-v5 84-to-85 and v5-to-v6 240-to-241 expansion, 512-score boundary, newer saves preserved.");
}
