#ifndef PS2BIRDS_MUSIC_MOCK_H
#define PS2BIRDS_MUSIC_MOCK_H
#ifdef __cplusplus
extern "C" {
#endif
struct audsrv_fmt_t{int freq,bits,channels;};
int audsrv_set_format(struct audsrv_fmt_t *fmt);
int audsrv_set_volume(int volume);
int audsrv_available(void);
int audsrv_play_audio(const char *chunk,int bytes);
int audsrv_stop_audio(void);
#ifdef __cplusplus
}
#endif
#endif
