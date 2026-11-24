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
#ifdef PS2BIRDS_RIO
/* The Rio's own sounds (gamelogic.lua birds' `sounds` and the materials' destroyedSound), after the Classic's list. */
#define SFX_LAUNCH_BOOMERANG 25   /* bird_06_flying */
#define SFX_LAUNCH_BIG 26         /* big_brother_flying */
#define SFX_SPECIAL_RED 27        /* red_special_1 */
#define SFX_SPECIAL_BOOMERANG 28  /* boomerang_activate */
#define SFX_SPECIAL_BIG 29        /* big_brother_yell */
#define SFX_METAL 30              /* steel_destroyed */
#define SFX_HIT_RED 31            /* bird_01_collision .. bird_05_collision */
#define SFX_HIT_BLUE 32
#define SFX_HIT_YELLOW 33
#define SFX_HIT_BLACK 34
#define SFX_HIT_WHITE 35
#define SFX_SPECIAL_GLOBE 36      /* Globe_Bird_Special_Activation_1 */
#define SFX_COUNT 37
#else
#define SFX_COUNT 25
#endif
#define SFX_CHANNELS 10
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
#ifdef PS2BIRDS_RIO
 {1,0},   /* LAUNCH_BOOMERANG */
 {1,0},   /* LAUNCH_BIG */
 {2,0},   /* SPECIAL_RED */
 {2,0},   /* SPECIAL_BOOMERANG */
 {2,0},   /* SPECIAL_BIG */
 {4,12},  /* METAL */
 {5,6},   /* HIT_RED */
 {5,6},   /* HIT_BLUE */
 {5,6},   /* HIT_YELLOW */
 {5,6},   /* HIT_BLACK */
 {5,6},   /* HIT_WHITE */
 {2,0},   /* SPECIAL_GLOBE */
#endif
};
#endif
