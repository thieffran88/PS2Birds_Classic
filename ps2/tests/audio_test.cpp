#include "audio.h"
#include "audio_mock.h"
#include <cassert>
#include <cstring>
#include <cstdio>
extern "C" {unsigned char audsrv_irx[64];unsigned int audsrv_irx_size=64;}
// A synthetic SOUNDS.BIN: header, table and SFX_COUNT silent 32-byte samples.
static void write_sounds(const char *path){
 FILE *f=fopen(path,"wb");unsigned start=16+8*SFX_COUNT;start=(start+63)/64*64;
 unsigned head[4]={0,1,SFX_COUNT,start};memcpy(head,"PBSN",4);fwrite(head,4,4,f);
 for(unsigned i=0;i<SFX_COUNT;i++){unsigned e[2]={i*64,32};fwrite(e,4,2,f);}
 for(long p=ftell(f);p<(long)start;p++)fputc(0,f);
 unsigned char sample[64]={0};for(int i=0;i<SFX_COUNT;i++)fwrite(sample,1,64,f);
 fclose(f);}
static bool module_fail=false,init_fail=false,busy=false;
static int loads=0,plays=0,tries=0,muted=0,volumes=0,exec_result=0,last_channel=-1,initializations=0;
extern "C" {
int SifLoadModule(const char *name,int,const char*){assert(!strcmp(name,"rom0:LIBSD"));return module_fail?-1:1;}
int sbv_patch_enable_lmb(){return 0;}
void SifInitIopHeap(){}
int SifExecModuleBuffer(void*,unsigned int size,unsigned int,const char*,int*result){assert(size==64);*result=exec_result;return 1;}
int audsrv_init(){initializations++;return init_fail?-1:0;}
int audsrv_adpcm_init(){return 0;}
void FlushCache(int){}
int audsrv_load_adpcm(audsrv_adpcm_t*s,void*p,int n){assert(n>16 && n%16==0);s->buffer=p;s->size=n;loads++;return 0;}
int audsrv_ch_play_adpcm(int ch,audsrv_adpcm_t*s){assert(ch>=0 && ch<SFX_CHANNELS && s->buffer);last_channel=ch;tries++;if(busy)return -7;plays++;return ch;}
int audsrv_adpcm_set_volume_and_pan(int ch,int volume,int pan){assert(ch>=0 && ch<SFX_CHANNELS && pan==0);assert(volume==0||volume==75);volumes++;if(!volume)muted++;return 0;}
}
int main(){
 remove("build/host/SOUNDS.BIN");audio_set_data_path("build/host/SOUNDS.BIN");audio_init();assert(!strcmp(audio_status(),"SOM INDISPONIVEL")); // missing file: no sound, no crash
 write_sounds("build/host/SOUNDS.BIN");
 module_fail=true;audio_init();audio_play(SFX_WIN);audio_toggle();assert(!loads && !plays && !strcmp(audio_status(),"SOM INDISPONIVEL"));
 module_fail=false;init_fail=true;audio_init();audio_play(SFX_WIN);assert(!loads && !plays);init_fail=false;
 exec_result=1;int initialized=initializations;audio_init();assert(initializations==initialized && !loads);exec_result=0;
 audio_init();assert(loads==SFX_COUNT && !strcmp(audio_status(),"SOM LIGADO"));
 audio_play(-1);audio_play(SFX_COUNT);assert(!plays);
 audio_play(SFX_WOOD);audio_play(SFX_WOOD);audio_play(SFX_GLASS);assert(plays==1); // shared block channel cooldown
 for(int i=0;i<12;i++)audio_tick();
 audio_play(SFX_GLASS);assert(plays==2);
 int before=plays;audio_toggle();assert(muted==SFX_CHANNELS && !strcmp(audio_status(),"SOM DESLIGADO"));audio_play(SFX_WIN);assert(plays==before);
 audio_toggle();assert(!strcmp(audio_status(),"SOM LIGADO"));audio_play(SFX_WIN);assert(plays==before+1);
 busy=true;int requests=tries;audio_play(SFX_LOSE);assert(tries==requests+1 && plays==before+1);busy=false;
 audio_play(SFX_START);assert(last_channel==audio_assets[SFX_START].channel);
 audio_play(SFX_WIN);assert(last_channel==audio_assets[SFX_WIN].channel && audio_assets[SFX_START].channel!=audio_assets[SFX_WIN].channel);
 assert(volumes>=plays && audio_assets[SFX_WIN].channel==5);
 puts("Audio OK: optional module failure, preload once, valid effects, channel throttling, mute, busy-channel handling.");
}
