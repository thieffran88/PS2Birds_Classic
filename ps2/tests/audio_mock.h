#ifndef PS2BIRDS_AUDIO_MOCK_H
#define PS2BIRDS_AUDIO_MOCK_H
typedef struct {int pitch,loop,channels;void *buffer;int size;} audsrv_adpcm_t;
#ifdef __cplusplus
extern "C" {
#endif
int SifLoadModule(const char*,int,const char*);
int sbv_patch_enable_lmb(void);
void SifInitIopHeap(void);
int SifExecModuleBuffer(void*,unsigned int,unsigned int,const char*,int*);
int audsrv_init(void);
int audsrv_adpcm_init(void);
void FlushCache(int);
int audsrv_load_adpcm(audsrv_adpcm_t*,void*,int);
int audsrv_ch_play_adpcm(int,audsrv_adpcm_t*);
int audsrv_adpcm_set_volume_and_pan(int,int,int);
#ifdef __cplusplus
}
#endif
#endif
