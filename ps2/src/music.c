#include "music.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#ifdef PS2BIRDS_MUSIC_TEST
#include "music_mock.h"
#else
#include <audsrv.h>
#endif

static char path[256]="MUSIC.BIN";
#define MUSIC_VOLUME 70
static int current=MUSIC_NONE,enabled=1,count,loaded_ambience=MUSIC_NONE;
static unsigned start,offsets[MUSIC_MAX],sizes[MUSIC_MAX],loops[MUSIC_MAX],position;
/* Title theme and level complete (about 3 MiB) stay resident; the current theme ambience
   (up to about 4 MiB) is read when a stage of another theme starts, never while playing. */
static unsigned char *resident,*ambience;static unsigned ambience_capacity;
/* The PCM ring replays stale data as a buzz once it runs dry (a stopped or finished track): the ring is emptied when a playing track is stopped,
   and after a track that ends by itself, as soon as the ring has played out (ring_free_max: the ring's size, seen while it was empty). */
static int draining;static int ring_free_max;
static char chunk[MUSIC_CHUNK] __attribute__((aligned(64)));

static unsigned rd32(const unsigned char *p){return p[0]|(p[1]<<8)|(p[2]<<16)|((unsigned)p[3]<<24);}
void music_set_data_path(const char *p){if(p){strncpy(path,p,sizeof(path)-1);path[sizeof(path)-1]=0;}}
static int read_at(int fd,unsigned at,unsigned char *to,unsigned n){
    lseek(fd,(off_t)at,SEEK_SET);unsigned done=0;
    while(done<n){int r=(int)read(fd,to+done,n-done);if(r<=0)return 0;done+=(unsigned)r;}
    return 1;
}
int music_init(void){
    free(resident);free(ambience);resident=ambience=0;ambience_capacity=0;loaded_ambience=MUSIC_NONE;count=0;current=MUSIC_NONE;
    int fd=open(path,O_RDONLY);if(fd<0)return 0;
    long n=(long)lseek(fd,0,SEEK_END);
    unsigned char head[16+12*MUSIC_MAX];
    int ok=n>16 && read_at(fd,0,head,16) && !memcmp(head,"PBMU",4) && rd32(head+4)==1;
    unsigned c=ok?rd32(head+8):0;start=ok?rd32(head+12):0;
    ok=ok && c>=MUSIC_RESIDENT && c<=MUSIC_MAX && read_at(fd,16,head+16,12*c);
    for(unsigned i=0;ok && i<c;i++){
        offsets[i]=rd32(head+16+12*i);sizes[i]=rd32(head+20+12*i);loops[i]=rd32(head+24+12*i);
        ok=sizes[i]>0 && (long)start+offsets[i]+sizes[i]<=n && (i==0 || offsets[i]>=offsets[i-1]);
    }
    /* Resident tracks are stored first: one read of [offsets[0], end of track 1). */
    unsigned resident_bytes=ok?offsets[1]+sizes[1]-offsets[0]:0;
    if(ok)ok=(resident=(unsigned char*)malloc(resident_bytes))!=0 && read_at(fd,start+offsets[0],resident,resident_bytes);
    close(fd);
    struct audsrv_fmt_t format={22050,16,1};
    if(ok)ok=audsrv_set_format(&format)==0;
    if(!ok){free(resident);resident=0;return 0;}
    audsrv_set_volume(enabled?MUSIC_VOLUME:0);
    count=(int)c;return 1;
}
int music_count(void){return count;}
int music_current(void){return current;}
static const unsigned char *track_data(int track){
    return track<MUSIC_RESIDENT?resident+offsets[track]-offsets[0]:ambience;
}
void music_play(int track){
    if(current!=MUSIC_NONE && (track<0 || track>=count) && resident){audsrv_stop_audio();draining=0;}   /* stopped on purpose: empty the ring, no stale replay */
    current=MUSIC_NONE;
    if(track<0 || track>=count)return;
    draining=0;
    if(track>=MUSIC_RESIDENT && track!=loaded_ambience){
        loaded_ambience=MUSIC_NONE;
        if(sizes[track]>ambience_capacity){free(ambience);ambience_capacity=0;ambience=(unsigned char*)malloc(sizes[track]);if(!ambience)return;ambience_capacity=sizes[track];}
        int fd=open(path,O_RDONLY);if(fd<0)return;
        int ok=read_at(fd,start+offsets[track],ambience,sizes[track]);close(fd);
        if(!ok)return;
        loaded_ambience=track;
    }
    current=track;position=0;
}
/* Sound off: the PCM ring must be stopped and emptied. Only withholding new chunks leaves the
   ring to run dry, and an underrun replays stale data as a hiss. Sound on: the volume comes back
   and the stream restarts from the next chunk (the emptied ring is refilled by music_tick). */
void music_set_enabled(int on){
    on=on!=0;
    if(on==enabled)return;
    enabled=on;
    if(!resident)return;
    if(!on){audsrv_stop_audio();audsrv_set_volume(0);}
    else audsrv_set_volume(MUSIC_VOLUME);
}
void music_tick(void){
    if(resident && enabled){int free_now=audsrv_available();if(free_now>ring_free_max)ring_free_max=free_now;
        if(draining && current==MUSIC_NONE && free_now>=ring_free_max){audsrv_stop_audio();draining=0;}}
    if(!resident || current==MUSIC_NONE || !enabled)return;
    const unsigned char *data=track_data(current);
    /* Fill what the ring can take, a chunk at a time, without blocking the frame. */
    for(int guard=0;guard<8 && audsrv_available()>=MUSIC_CHUNK;guard++){
        unsigned left=sizes[current]-position,n=left<MUSIC_CHUNK?left:MUSIC_CHUNK;
        memcpy(chunk,data+position,n);
        if(n<MUSIC_CHUNK)memset(chunk+n,0,MUSIC_CHUNK-n);
        audsrv_play_audio(chunk,MUSIC_CHUNK);
        position+=n;
        if(position>=sizes[current]){
            if(loops[current])position=0;
            else{current=MUSIC_NONE;draining=1;return;}
        }
    }
}
