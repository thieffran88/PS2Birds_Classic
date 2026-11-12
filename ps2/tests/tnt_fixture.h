#ifndef PS2BIRDS_TNT_FIXTURE_H
#define PS2BIRDS_TNT_FIXTURE_H
#include "campaign_types.h"
/* Crates sit on a static shelf to the right of the sling. Kind 7 is ExtraBlockTNT. */
static const CampaignObject tnt_objects[]={
 {20,0,0,3.5f,3.45f,1,10,1,7,0,"ExtraBlockTNT"},      /* 0: first crate, hit by the bird */
 {24,0,0,3.5f,3.45f,1,10,1,7,0,"ExtraBlockTNT"},      /* 1: 4 units away, inside the damage radius: chained */
 {22,-3,0,1,1,2,50,1,1,0,"WoodBlock1"},               /* 2: wood between and above them */
 {-12,4.5f,0,1,1,2,200,1,1,0,"WoodBlock1"},           /* 3: on the ground behind the sling, outside both blast radii */
 {23,2.5f,0,12,1.5f,0,200,100000,3,0,"StaticBlock_02"},/* 4: shelf, static */
 {27.5f,0.6f,0,2.15f,2.15f,2,4,1,4,1,"SmallPiglette"},/* 5: pig next to the second crate */
 {60,3,0,2.15f,2.15f,2,10000,100,4,1,"SmallPiglette"},/* 6: far pig keeps the stage open */
};
static const CampaignObject fall_objects[]={
 {20,0,0,3.5f,3.45f,1,10,1,7,0,"ExtraBlockTNT"},      /* falls out of the world: no blast */
 {60,3,0,2.15f,2.15f,2,10000,100,4,1,"SmallPiglette"},
};
static const CampaignLevel campaign_levels[]={
 {tnt_objects,7,2,{0,0},20,0,0,5,20,100,200,5,"TNT FIXTURE","fixture-tnt",0},
 {fall_objects,2,1,{0},20,0,0,-40,20,100,200,5,"TNT FALL FIXTURE","fixture-tnt-fall",0},
};
#define CAMPAIGN_COUNT (sizeof(campaign_levels)/sizeof(campaign_levels[0]))
#endif
