// Host test of the MUSIC.BIN streamer with a mocked audsrv PCM ring.
#include "music.h"
#include "music_mock.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>
static int freq,bits,channels,ring,volume=-1,stops;static std::vector<char> played;
extern "C" int audsrv_set_format(struct audsrv_fmt_t *f){freq=f->freq;bits=f->bits;channels=f->channels;return 0;}
extern "C" int audsrv_set_volume(int v){volume=v;return 0;}
extern "C" int audsrv_stop_audio(void){stops++;ring=1<<20;return 0;}   // the stopped stream leaves the ring empty
extern "C" int audsrv_available(void){return ring;}
extern "C" int audsrv_play_audio(const char *c,int n){played.insert(played.end(),c,c+n);ring-=n;return n;}
static void put32(FILE *f,unsigned v){unsigned char b[4]={(unsigned char)v,(unsigned char)(v>>8),(unsigned char)(v>>16),(unsigned char)(v>>24)};fwrite(b,1,4,f);}
int main(){
    const char *p="build/host/MUSIC.BIN";remove(p);music_set_data_path(p);
    assert(!music_init());music_play(MUSIC_TITLE);music_tick();assert(music_current()==MUSIC_NONE); // missing file: silent
    // title: 5000 bytes of 'T' (loops); level complete: 3000 bytes of 'L' (once); ambience: 2500 bytes of 'A' (loops)
    FILE *f=fopen(p,"wb");fwrite("PBMU",1,4,f);put32(f,1);put32(f,3);put32(f,2048);
    put32(f,0);put32(f,5000);put32(f,1);put32(f,6144);put32(f,3000);put32(f,0);put32(f,10240);put32(f,2500);put32(f,1);
    for(long i=ftell(f);i<2048;i++)fputc(0,f);
    for(int i=0;i<6144;i++)fputc(i<5000?'T':0,f);
    for(int i=0;i<4096;i++)fputc(i<3000?'L':0,f);
    for(int i=0;i<2500;i++)fputc('A',f);
    fclose(f);
    assert(music_init() && music_count()==3 && freq==22050 && bits==16 && channels==1);
    music_play(MUSIC_TITLE);ring=0;music_tick();assert(played.empty());           // full ring: nothing sent
    ring=MUSIC_CHUNK*4;music_tick();assert(played.size()==4*MUSIC_CHUNK);          // 8192 bytes: wrapped once
    assert(played[0]=='T' && played[4999]=='T' && played[5000]==0 && played[6143]==0 && played[6144]=='T');
    assert(music_current()==MUSIC_TITLE);
    played.clear();music_play(MUSIC_LEVEL_COMPLETE);ring=MUSIC_CHUNK*8;music_tick();
    assert(played.size()==2*MUSIC_CHUNK && played[0]=='L' && played[2999]=='L' && played[3000]==0 && music_current()==MUSIC_NONE);
    // theme ambience: read from the file on demand, loops; an index past the table stays silent
    played.clear();music_play(2);assert(music_current()==2);ring=MUSIC_CHUNK*2;music_tick();
    assert(played.size()==2*MUSIC_CHUNK && played[0]=='A' && played[2047]=='A' && played[2048]=='A' && played[2499]=='A' && played[2500]==0 && music_current()==2);
    music_play(3);assert(music_current()==MUSIC_NONE);
    assert(volume==70 && stops==1);                                            // stopping a playing track on purpose emptied the ring once
    played.clear();music_play(MUSIC_TITLE);
    music_set_enabled(0);assert(stops==2 && volume==0);                       // sound off: stream stopped and emptied, not just starved
    ring=MUSIC_CHUNK*8;music_tick();assert(played.empty());                    // nothing is sent while muted
    music_set_enabled(0);assert(stops==2);                                     // already off: no second stop
    music_set_enabled(1);assert(volume==70 && stops==2);                       // sound on: volume back, no stop
    ring=MUSIC_CHUNK*2;music_tick();assert(played.size()==2*MUSIC_CHUNK);      // the stream is fed again
    music_play(MUSIC_NONE);assert(stops==3);played.clear();music_tick();assert(played.empty());
    // a track that ends by itself: once the ring has played out it is emptied, not left to replay stale data
    music_play(MUSIC_LEVEL_COMPLETE);ring=MUSIC_CHUNK*8;music_tick();assert(music_current()==MUSIC_NONE);
    ring=MUSIC_CHUNK*4;music_tick();assert(stops==3);                          // still playing out
    ring=1<<20;music_tick();assert(stops==4);                                  // played out: emptied
    puts("Music OK: MUSIC.BIN header, 22050 Hz mono 16-bit, title theme loops, level complete plays once, theme ambience read on demand and looped, sound off stops and empties the stream (no hiss from a starved ring) and sound on resumes it; a missing file stays silent.");
}
