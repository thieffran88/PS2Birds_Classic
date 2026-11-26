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
#define SFX_MONKEY_DIE 44         /* monkey dies a1 (a monkey goal is destroyed) */
#define SFX_MONKEY_IDLE 45        /* monkey idle a1 */
#define SFX_METAL_DESTROY 46      /* metal_destroyed_01 */
#define SFX_HIT_METAL 47          /* metal_collision_01 */
#define SFX_WATER_BIRD_IN 48      /* bird_enter_water_01 */
#define SFX_WATER_BIRD_OUT 49     /* bird_exit_water_01 */
#define SFX_WATER_OBJ_IN 50       /* object_enter_water_01 */
#define SFX_WATER_OBJ_OUT 51      /* object_exit_water_01 */
#define SFX_LAUNCH_PINK SFX_LAUNCH_RED   /* the Rio has no pink-bird sounds in its bank */
#define SFX_HIT_PINK SFX_HIT_RED
#define SFX_HIT_GLOBE SFX_HIT_RED
#define SFX_BASE_COUNT 52
#define SFX_COUNT 85      /* 52 effects + 33 variants (audio_variants) */
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
#define SFX_BONE 47               /* bone_destroy_1 (a Dino bone breaks) */
#define SFX_VIAL 48               /* vial_break_01 (a vial or glass pipe breaks) */
#define SFX_STALAKTITE 49         /* stalaktite_break_01 */
#define SFX_HIT_BONE 50           /* bone_collision_1 */
#define SFX_DMG_BONE 51           /* bone_damage_1 */
#define SFX_CANNON 52             /* cannon_shot_01 */
#define SFX_ELECTRO 53            /* pigs_electrocuted_01 */
#define SFX_CAKE 54               /* birthday_cake1 */
#define SFX_PALM 55               /* palm_leaves_1 */
#define SFX_PIPE 56               /* metal_pipe_destroyed_01 */
#define SFX_HIT_PINK 57           /* pink_collision_01 */
#define SFX_HIT_GLOBE 58          /* Globe_Bird_Hit_1 */
#define SFX_BASE_COUNT 59
#define SFX_COUNT 105     /* 59 effects + 46 variants (audio_variants) */
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
 {2,0},   /* MONKEY_DIE */
 {2,0},   /* MONKEY_IDLE */
 {4,12},  /* METAL_DESTROY */
 {5,8},   /* HIT_METAL */
 {6,6},   /* WATER_BIRD_IN */
 {6,6},   /* WATER_BIRD_OUT */
 {3,10},  /* WATER_OBJ_IN */
 {3,10},  /* WATER_OBJ_OUT */
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
 {4,12},  /* BONE */
 {4,12},  /* VIAL */
 {4,12},  /* STALAKTITE */
 {5,8},   /* HIT_BONE */
 {5,10},  /* DMG_BONE */
 {4,6},   /* CANNON */
 {2,0},   /* ELECTRO */
 {4,12},  /* CAKE */
 {4,12},  /* PALM */
 {4,12},  /* PIPE */
 {6,6},   /* HIT_PINK */
 {6,6},   /* HIT_GLOBE */
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
#ifdef PS2BIRDS_RIO
 {2,0},   /* MONKEY_DIE variant 2 */
 {2,0},   /* MONKEY_IDLE variant 2 */
 {2,0},   /* MONKEY_IDLE variant 3 */
 {4,12},  /* METAL_DESTROY variant 2 */
 {4,12},  /* METAL_DESTROY variant 3 */
 {5,8},   /* HIT_METAL variant 2 */
 {5,8},   /* HIT_METAL variant 3 */
 {6,6},   /* WATER_BIRD_IN variant 2 */
 {6,6},   /* WATER_BIRD_IN variant 3 */
 {6,6},   /* WATER_BIRD_OUT variant 2 */
 {6,6},   /* WATER_BIRD_OUT variant 3 */
 {3,10},  /* WATER_OBJ_IN variant 2 */
 {3,10},  /* WATER_OBJ_IN variant 3 */
 {3,10},  /* WATER_OBJ_OUT variant 2 */
 {3,10},  /* WATER_OBJ_OUT variant 3 */
#endif
#ifndef PS2BIRDS_RIO
 {5,8},   /* HIT_PIG variant 2 */
 {5,8},   /* HIT_PIG variant 3 */
 {5,10},  /* DMG_PIG variant 2 */
 {5,10},  /* DMG_PIG variant 3 */
 {2,0},   /* OINK variant 2 */
 {2,0},   /* OINK variant 3 */
 {4,12},  /* BONE variant 2 */
 {4,12},  /* BONE variant 3 */
 {4,12},  /* VIAL variant 2 */
 {4,12},  /* VIAL variant 3 */
 {4,12},  /* STALAKTITE variant 2 */
 {4,12},  /* STALAKTITE variant 3 */
 {5,8},   /* HIT_BONE variant 2 */
 {5,8},   /* HIT_BONE variant 3 */
 {5,10},  /* DMG_BONE variant 2 */
 {5,10},  /* DMG_BONE variant 3 */
 {4,6},   /* CANNON variant 2 */
 {2,0},   /* ELECTRO variant 2 */
 {2,0},   /* ELECTRO variant 3 */
 {4,12},  /* CAKE variant 2 */
 {4,12},  /* PALM variant 2 */
 {4,12},  /* PALM variant 3 */
 {4,12},  /* PIPE variant 2 */
 {4,12},  /* PIPE variant 3 */
 {6,6},   /* HIT_PINK variant 2 */
 {6,6},   /* HIT_PINK variant 3 */
 {6,6},   /* HIT_GLOBE variant 2 */
 {6,6},   /* HIT_GLOBE variant 3 */
#endif
};
/* Sounds the originals pick at random among several recordings: the base effect (a1) and `count`-1 extra effects from `first` on (extract_audio.py VARIANT_GROUPS, same order). */
typedef struct {int base,first,count;} AudioVariant;
#ifdef PS2BIRDS_RIO
#define SFX_VARIANT_GROUPS 17
#else
#define SFX_VARIANT_GROUPS 24
#endif
static const AudioVariant audio_variants[SFX_VARIANT_GROUPS]={
 {SFX_WOOD,SFX_BASE_COUNT+0,3},{SFX_GLASS,SFX_BASE_COUNT+2,3},{SFX_STONE,SFX_BASE_COUNT+4,3},
 {SFX_HIT_WOOD,SFX_BASE_COUNT+6,3},{SFX_HIT_GLASS,SFX_BASE_COUNT+8,3},{SFX_HIT_STONE,SFX_BASE_COUNT+10,3},
 {SFX_DMG_WOOD,SFX_BASE_COUNT+12,3},{SFX_DMG_GLASS,SFX_BASE_COUNT+14,3},{SFX_DMG_STONE,SFX_BASE_COUNT+16,3}
#ifdef PS2BIRDS_RIO
 ,{SFX_MONKEY_DIE,SFX_BASE_COUNT+18,2},{SFX_MONKEY_IDLE,SFX_BASE_COUNT+19,3},{SFX_METAL_DESTROY,SFX_BASE_COUNT+21,3},{SFX_HIT_METAL,SFX_BASE_COUNT+23,3},
 {SFX_WATER_BIRD_IN,SFX_BASE_COUNT+25,3},{SFX_WATER_BIRD_OUT,SFX_BASE_COUNT+27,3},{SFX_WATER_OBJ_IN,SFX_BASE_COUNT+29,3},{SFX_WATER_OBJ_OUT,SFX_BASE_COUNT+31,3}
#endif
#ifndef PS2BIRDS_RIO
 ,{SFX_HIT_PIG,SFX_BASE_COUNT+18,3},{SFX_DMG_PIG,SFX_BASE_COUNT+20,3},{SFX_OINK,SFX_BASE_COUNT+22,3},
 {SFX_BONE,SFX_BASE_COUNT+24,3},{SFX_VIAL,SFX_BASE_COUNT+26,3},{SFX_STALAKTITE,SFX_BASE_COUNT+28,3},{SFX_HIT_BONE,SFX_BASE_COUNT+30,3},{SFX_DMG_BONE,SFX_BASE_COUNT+32,3},
 {SFX_CANNON,SFX_BASE_COUNT+34,2},{SFX_ELECTRO,SFX_BASE_COUNT+35,3},{SFX_CAKE,SFX_BASE_COUNT+37,2},{SFX_PALM,SFX_BASE_COUNT+38,3},{SFX_PIPE,SFX_BASE_COUNT+40,3},
 {SFX_HIT_PINK,SFX_BASE_COUNT+42,3},{SFX_HIT_GLOBE,SFX_BASE_COUNT+44,3}
#endif
};
#endif
