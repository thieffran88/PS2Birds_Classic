#ifndef PS2BIRDS_AUDIO_H
#define PS2BIRDS_AUDIO_H
#include "audio_data.h"
#ifdef __cplusplus
extern "C" {
#endif
void audio_init(void);
void audio_tick(void);
void audio_play(int effect);
void audio_toggle(void);
const char *audio_status(void);
#ifdef __cplusplus
}
#endif
#endif
