#ifndef PS2BIRDS_SAVE_MC_H
#define PS2BIRDS_SAVE_MC_H
#ifdef __cplusplus
extern "C" {
#endif
#include "game.h"
void save_mc_init(Game *game);
void save_mc_poll(void);
void save_mc_request(void);
int save_mc_busy(void);
const char *save_mc_status(void);
#ifdef __cplusplus
}
#endif
#endif
