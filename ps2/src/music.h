/* Music streamed from MUSIC.BIN (made from the APK by tools/extract_audio.py) through the
   audsrv PCM ring: title_theme loops in the menus, level_complete plays once on a win. */
#ifndef PS2BIRDS_MUSIC_H
#define PS2BIRDS_MUSIC_H
#ifdef __cplusplus
extern "C" {
#endif
enum {MUSIC_NONE=-1,MUSIC_TITLE=0,MUSIC_LEVEL_COMPLETE=1,MUSIC_COUNT=2};
#define MUSIC_CHUNK 2048
void music_set_data_path(const char *path);
int music_init(void);            /* after audio_init; 0 when MUSIC.BIN is missing or invalid */
void music_play(int track);      /* restarts the track; MUSIC_NONE stops */
int music_current(void);         /* playing track or MUSIC_NONE */
void music_set_enabled(int on);
void music_tick(void);           /* once per frame: keeps the PCM ring filled */
#ifdef __cplusplus
}
#endif
#endif
