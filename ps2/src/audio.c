#include "audio.h"
#include <string.h>
#ifdef PS2BIRDS_AUDIO_TEST
#include "audio_mock.h"
#else
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <iopheap.h>
#include <sbv_patches.h>
#include <audsrv.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef PS2BIRDS_AUDIO_TEST
#define memalign(a,n) malloc(n)
#else
#include <malloc.h>
#endif
extern unsigned char audsrv_irx[];
extern unsigned int audsrv_irx_size;
static audsrv_adpcm_t samples[SFX_COUNT];
static unsigned int frame,last[SFX_CHANNELS];
static unsigned char loaded[SFX_COUNT];
static int available,enabled;
static char data_path[256]="SOUNDS.BIN";
void audio_set_data_path(const char *path){if(path){strncpy(data_path,path,sizeof(data_path)-1);data_path[sizeof(data_path)-1]=0;}}
static unsigned rd32(const unsigned char *p){return p[0]|(p[1]<<8)|(p[2]<<16)|((unsigned)p[3]<<24);}
/* SOUNDS.BIN: "PBSN", version 1, count, data start, then count x (offset,size) from the
   data start, then the SPU2 ADPCM samples. Read whole (under 1 MiB) and freed after upload. */
static unsigned char *read_sounds(unsigned *offsets,unsigned *sizes)
{
    int fd=open(data_path,O_RDONLY);if(fd<0)return 0;
    long n=(long)lseek(fd,0,SEEK_END);lseek(fd,0,SEEK_SET);
    unsigned char *data=n>16 && n<=2*1048576?(unsigned char*)memalign(64,(size_t)n):0;
    long done=0;while(data && done<n){int r=(int)read(fd,data+done,(size_t)(n-done));if(r<=0)break;done+=r;}
    close(fd);int ok=data && done==n;
    if(ok)ok=!memcmp(data,"PBSN",4) && rd32(data+4)==1 && rd32(data+8)==SFX_COUNT && 16+8*SFX_COUNT<=rd32(data+12) && rd32(data+12)<=(unsigned)n;
    for(int i=0;ok && i<SFX_COUNT;i++){offsets[i]=rd32(data+12)+rd32(data+16+8*i);sizes[i]=rd32(data+20+8*i);ok=offsets[i]+sizes[i]<=(unsigned)n && sizes[i]>16;}
    if(!ok){free(data);return 0;}
    return data;
}
void audio_init(void){
    available=enabled=0;frame=120;
    memset(last,0,sizeof(last));memset(loaded,0,sizeof(loaded));memset(samples,0,sizeof(samples));
    if(SifLoadModule("rom0:LIBSD",0,0)<0 || sbv_patch_enable_lmb()<0)return;
    SifInitIopHeap();int result=-1;
    if(SifExecModuleBuffer(audsrv_irx,audsrv_irx_size,0,0,&result)<0 || result!=0)return;
    if(audsrv_init()!=0 || audsrv_adpcm_init()!=0)return;
    unsigned offsets[SFX_COUNT],sizes[SFX_COUNT];unsigned char *bank=read_sounds(offsets,sizes);
    if(!bank)return;
    FlushCache(0);
    int count=0;
    for(int i=0;i<SFX_COUNT;i++){
        if(audsrv_load_adpcm(&samples[i],bank+offsets[i],(int)sizes[i])==0){loaded[i]=1;count++;}
    }
#ifndef PS2BIRDS_AUDIO_TEST
    free(bank); /* audsrv copied the samples into SPU2 memory */
#endif
    available=count>0;enabled=available;
}
void audio_tick(void){frame++;}
/* One of the recordings of a sound the original varies (audio_variants), picked at random (a small LCG: the pick needs no quality, only to differ from the last). */
static int pick_variant(int effect){
    static unsigned seed=12345;
    for(int g=0;g<SFX_VARIANT_GROUPS;g++)if(audio_variants[g].base==effect){
        seed=seed*1664525u+1013904223u;
        int r=(int)((seed>>16)%(unsigned)audio_variants[g].count);
        return r==0?effect:audio_variants[g].first+r-1;
    }
    return effect;
}
void audio_play(int effect){
    if(effect>=0 && effect<SFX_BASE_COUNT)effect=pick_variant(effect);
    if(!available || !enabled || effect<0 || effect>=SFX_COUNT || !loaded[effect])return;
    const AudioAsset *asset=&audio_assets[effect];
    if(frame-last[asset->channel]<(unsigned int)asset->cooldown)return;
    if(effect==SFX_WIN || effect==SFX_LOSE)audsrv_adpcm_set_volume_and_pan(audio_assets[SFX_START].channel,0,0);
    int channel=audsrv_ch_play_adpcm(asset->channel,&samples[effect]);
    if(channel>=0){audsrv_adpcm_set_volume_and_pan(channel,75,0);last[asset->channel]=frame;}
}
void audio_toggle(void){
    if(!available)return;
    enabled=!enabled;
    if(!enabled)for(int ch=0;ch<SFX_CHANNELS;ch++)audsrv_adpcm_set_volume_and_pan(ch,0,0);
}
const char *audio_status(void){return !available?"SOM INDISPONIVEL":enabled?"SOM LIGADO":"SOM DESLIGADO";}
