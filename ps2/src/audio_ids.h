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
#define SFX_HIT_WOOD 37           /* wood collision a1 */
#define SFX_HIT_GLASS 38          /* ice light collision a1 */
#define SFX_HIT_STONE 39          /* rock collision a1 */
#define SFX_DMG_WOOD 40           /* wood damage a1 (a piece that took damage and held) */
#define SFX_DMG_GLASS 41          /* light damage a1 */
#define SFX_DMG_STONE 42          /* rock damage a1 */
#define SFX_NEXT_BIRD 43          /* bird next military a1: the next bird hops onto the slingshot */
#define SFX_LAUNCH_PINK SFX_LAUNCH_RED   /* the Rio has no pink-bird sounds in its bank */
#define SFX_BASE_COUNT 44
#define SFX_COUNT 62      /* 44 effects + 18 variants (audio_variants) */
#else
/* Classic-only ids after the shared ones: block impact sounds and each bird's collision sound (extract_audio.py CLASSIC_ONLY). */
#define SFX_HIT_WOOD 25
#define SFX_HIT_GLASS 26
#define SFX_HIT_STONE 27
#define SFX_HIT_RED 28
#define SFX_HIT_BLUE 29
#define SFX_HIT_YELLOW 30
#define SFX_HIT_BLACK 31
#define SFX_HIT_WHITE 32
#define SFX_DMG_WOOD 33
#define SFX_DMG_GLASS 34
#define SFX_DMG_STONE 35
#define SFX_NEXT_BIRD 36
#define SFX_HIT_PIG 37            /* piglette collision a1 */
#define SFX_DMG_PIG 38            /* piglette damage a1 */
#define SFX_OINK 39               /* piglette oink a1 */
#define SFX_LAUNCH_BOOMERANG 40   /* bird_06_flying (the green boomerang bird) */
#define SFX_LAUNCH_BIG 41         /* bigbrother_fly */
#define SFX_LAUNCH_PINK 42        /* pink_launch_01 */
#define SFX_SPECIAL_BOOMERANG 43  /* boomerang_activate */
#define SFX_SPECIAL_BIG 44        /* bigbrother_yell */
#define SFX_SPECIAL_GLOBE 45      /* Globe_Bird_Special_Activation_1 */
#define SFX_SPECIAL_PINK 46       /* pink_activate_01 */
#define SFX_BASE_COUNT 47
#define SFX_COUNT 71      /* 47 effects + 24 variants (audio_variants) */
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
 {5,8},   /* HIT_WOOD */
 {5,8},   /* HIT_GLASS */
 {5,8},   /* HIT_STONE */
 {5,10},  /* DMG_WOOD */
 {5,10},  /* DMG_GLASS */
 {5,10},  /* DMG_STONE */
 {7,0},   /* NEXT_BIRD */
#else
 {5,8},   /* HIT_WOOD */
 {5,8},   /* HIT_GLASS */
 {5,8},   /* HIT_STONE */
 {6,6},   /* HIT_RED */
 {6,6},   /* HIT_BLUE */
 {6,6},   /* HIT_YELLOW */
 {6,6},   /* HIT_BLACK */
 {6,6},   /* HIT_WHITE */
 {5,10},  /* DMG_WOOD */
 {5,10},  /* DMG_GLASS */
 {5,10},  /* DMG_STONE */
 {7,0},   /* NEXT_BIRD */
#ifndef PS2BIRDS_RIO
 {5,8},   /* HIT_PIG */
 {5,10},  /* DMG_PIG */
 {2,0},   /* OINK */
 {1,0},   /* LAUNCH_BOOMERANG */
 {1,0},   /* LAUNCH_BIG */
 {1,0},   /* LAUNCH_PINK */
 {2,0},   /* SPECIAL_BOOMERANG */
 {2,0},   /* SPECIAL_BIG */
 {2,0},   /* SPECIAL_GLOBE */
 {2,0},   /* SPECIAL_PINK */
#endif
#endif
 {4,12},   /* WOOD variant 2 */
 {4,12},   /* WOOD variant 3 */
 {4,12},   /* GLASS variant 2 */
 {4,12},   /* GLASS variant 3 */
 {4,12},   /* STONE variant 2 */
 {4,12},   /* STONE variant 3 */
 {5,8},   /* HIT_WOOD variant 2 */
 {5,8},   /* HIT_WOOD variant 3 */
 {5,8},   /* HIT_GLASS variant 2 */
 {5,8},   /* HIT_GLASS variant 3 */
 {5,8},   /* HIT_STONE variant 2 */
 {5,8},   /* HIT_STONE variant 3 */
 {5,10},   /* DMG_WOOD variant 2 */
 {5,10},   /* DMG_WOOD variant 3 */
 {5,10},   /* DMG_GLASS variant 2 */
 {5,10},   /* DMG_GLASS variant 3 */
 {5,10},   /* DMG_STONE variant 2 */
 {5,10},   /* DMG_STONE variant 3 */
#ifndef PS2BIRDS_RIO
 {5,8},   /* HIT_PIG variant 2 */
 {5,8},   /* HIT_PIG variant 3 */
 {5,10},  /* DMG_PIG variant 2 */
 {5,10},  /* DMG_PIG variant 3 */
 {2,0},   /* OINK variant 2 */
 {2,0},   /* OINK variant 3 */
#endif
};
/* Sounds the originals pick at random among several recordings: the base effect (a1) and `count`-1 extra effects from `first` on (extract_audio.py VARIANT_GROUPS, same order). */
typedef struct {int base,first,count;} AudioVariant;
#ifdef PS2BIRDS_RIO
#define SFX_VARIANT_GROUPS 9
#else
#define SFX_VARIANT_GROUPS 12
#endif
static const AudioVariant audio_variants[SFX_VARIANT_GROUPS]={
 {SFX_WOOD,SFX_BASE_COUNT+0,3},{SFX_GLASS,SFX_BASE_COUNT+2,3},{SFX_STONE,SFX_BASE_COUNT+4,3},
 {SFX_HIT_WOOD,SFX_BASE_COUNT+6,3},{SFX_HIT_GLASS,SFX_BASE_COUNT+8,3},{SFX_HIT_STONE,SFX_BASE_COUNT+10,3},
 {SFX_DMG_WOOD,SFX_BASE_COUNT+12,3},{SFX_DMG_GLASS,SFX_BASE_COUNT+14,3},{SFX_DMG_STONE,SFX_BASE_COUNT+16,3}
#ifndef PS2BIRDS_RIO
 ,{SFX_HIT_PIG,SFX_BASE_COUNT+18,3},{SFX_DMG_PIG,SFX_BASE_COUNT+20,3},{SFX_OINK,SFX_BASE_COUNT+22,3}
#endif
};
#endif
