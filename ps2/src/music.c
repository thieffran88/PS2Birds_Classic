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
static int current=MUSIC_NONE,enabled=1,count;
static unsigned char *data;  /* whole MUSIC.BIN (about 3 MiB): no disc access while playing */
static unsigned start,offsets[MUSIC_COUNT],sizes[MUSIC_COUNT],loops[MUSIC_COUNT],position;
static char chunk[MUSIC_CHUNK] __attribute__((aligned(64)));

static unsigned rd32(const unsigned char *p){return p[0]|(p[1]<<8)|(p[2]<<16)|((unsigned)p[3]<<24);}
void music_set_data_path(const char *p){if(p){strncpy(path,p,sizeof(path)-1);path[sizeof(path)-1]=0;}}
int music_init(void){
    free(data);data=0;count=0;current=MUSIC_NONE;
    int fd=open(path,O_RDONLY);if(fd<0)return 0;
    long n=(long)lseek(fd,0,SEEK_END);lseek(fd,0,SEEK_SET);
    if(n>16+12*MUSIC_COUNT && n<=8*1048576)data=(unsigned char*)malloc((size_t)n);
    long done=0;while(data && done<n){int r=(int)read(fd,data+done,(size_t)(n-done));if(r<=0)break;done+=r;}
    close(fd);
    int ok=data && done==n && !memcmp(data,"PBMU",4) && rd32(data+4)==1 && rd32(data+8)==MUSIC_COUNT;
    start=ok?rd32(data+12):0;
    for(int i=0;ok && i<MUSIC_COUNT;i++){
        offsets[i]=rd32(data+16+12*i);sizes[i]=rd32(data+20+12*i);loops[i]=rd32(data+24+12*i);
        ok=sizes[i]>0 && (long)start+offsets[i]+sizes[i]<=n;
    }
    struct audsrv_fmt_t format={22050,16,1};
    if(ok)ok=audsrv_set_format(&format)==0;
    if(!ok){free(data);data=0;return 0;}
    audsrv_set_volume(70);
    count=MUSIC_COUNT;return 1;
}
int music_current(void){return current;}
void music_play(int track){
    if(track<0 || track>=count){current=MUSIC_NONE;return;}
    current=track;position=0;
}
void music_set_enabled(int on){enabled=on;}
void music_tick(void){
    if(!data || current==MUSIC_NONE || !enabled)return;
    /* Fill what the ring can take, a chunk at a time, without blocking the frame. */
    for(int guard=0;guard<8 && audsrv_available()>=MUSIC_CHUNK;guard++){
        unsigned left=sizes[current]-position,n=left<MUSIC_CHUNK?left:MUSIC_CHUNK;
        memcpy(chunk,data+start+offsets[current]+position,n);
        if(n<MUSIC_CHUNK)memset(chunk+n,0,MUSIC_CHUNK-n);
        audsrv_play_audio(chunk,MUSIC_CHUNK);
        position+=n;
        if(position>=sizes[current]){
            if(loops[current])position=0;
            else{current=MUSIC_NONE;return;}
        }
    }
}
