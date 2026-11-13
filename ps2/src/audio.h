#ifndef PS2BIRDS_AUDIO_H
#define PS2BIRDS_AUDIO_H
#include "audio_ids.h"
#ifdef __cplusplus
extern "C" {
#endif
/* SOUNDS.BIN location (disc or ELF folder); set before audio_init. */
void audio_set_data_path(const char *path);
void audio_init(void);
void audio_tick(void);
void audio_play(int effect);
void audio_toggle(void);
const char *audio_status(void);
#ifdef __cplusplus
}
#endif
#endif
