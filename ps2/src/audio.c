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
extern unsigned char audsrv_irx[],audio_bank[];
extern unsigned int audsrv_irx_size;
static audsrv_adpcm_t samples[SFX_COUNT];
static unsigned int frame,last[SFX_CHANNELS];
static unsigned char loaded[SFX_COUNT];
static int available,enabled;
void audio_init(void){
    available=enabled=0;frame=120;
    memset(last,0,sizeof(last));memset(loaded,0,sizeof(loaded));memset(samples,0,sizeof(samples));
    if(SifLoadModule("rom0:LIBSD",0,0)<0 || sbv_patch_enable_lmb()<0)return;
    SifInitIopHeap();int result=-1;
    if(SifExecModuleBuffer(audsrv_irx,audsrv_irx_size,0,0,&result)<0 || result!=0)return;
    if(audsrv_init()!=0 || audsrv_adpcm_init()!=0)return;
    FlushCache(0);
    int count=0;
    for(int i=0;i<SFX_COUNT;i++){
        if(audsrv_load_adpcm(&samples[i],audio_bank+audio_assets[i].offset,audio_assets[i].size)==0){loaded[i]=1;count++;}
    }
    available=count>0;enabled=available;
}
void audio_tick(void){frame++;}
void audio_play(int effect){
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
