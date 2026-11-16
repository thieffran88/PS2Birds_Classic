/* Sound-effect identifiers, voice channels and cooldowns (port settings). The samples
   themselves are read from SOUNDS.BIN, made from the player's APK by tools/gerar_iso.py. */
#ifndef PS2BIRDS_AUDIO_IDS_H
#define PS2BIRDS_AUDIO_IDS_H
#define SFX_MENU_MOVE 0
#define SFX_MENU_CONFIRM 1
#define SFX_MENU_BACK 2
#define SFX_LAUNCH_RED 3
#define SFX_LAUNCH_BLUE 4
#define SFX_LAUNCH_YELLOW 5
#define SFX_SPLIT 6
#define SFX_BOOST 7
#define SFX_PIG 8
#define SFX_WOOD 9
#define SFX_GLASS 10
#define SFX_STONE 11
#define SFX_WIN 12
#define SFX_LOSE 13
#define SFX_START 14
#define SFX_STRETCH 15
#define SFX_LAUNCH_BLACK 16
#define SFX_EXPLOSION 17
#define SFX_LAUNCH_WHITE 18
#define SFX_DROP_EGG 19
#define SFX_STAR_1 20
#define SFX_STAR_2 21
#define SFX_STAR_3 22
#define SFX_SCORE_COUNT 23
#define SFX_NEW_HIGHSCORE 24
#define SFX_CHANNELS 10
#define SFX_COUNT 25
typedef struct {int channel,cooldown;} AudioAsset;
static const AudioAsset audio_assets[SFX_COUNT]={
 {0,4},
 {0,4},
 {0,4},
 {1,0},
 {1,0},
 {1,0},
 {2,0},
 {2,0},
 {3,8},
 {4,12},
 {4,12},
 {4,12},
 {5,0},
 {6,0},
 {7,0},
 {8,30},
 {1,0},
 {9,0},
 {1,0},
 {2,0},
 {2,0},
 {3,0},
 {4,0},
 {7,0},
 {9,0},
};
#endif
