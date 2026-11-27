#ifndef PS2BIRDS_MONKEY_FIXTURE_H
#define PS2BIRDS_MONKEY_FIXTURE_H
#include "campaign_types.h"
/* A wall-grabbing monkey (Rio) welded to a wooden block on a static shelf; the bird hits the block. */
static const CampaignObject monkey_objects[]={
 {20,0,0,3.5f,3.45f,1,10000,1,1,0,"WoodBlock1",4.0f,0.2f,0,0,-1},                       /* 0: the block the monkey holds on to */
 {22.9f,0.6f,0,2.15f,2.15f,2,10000,100,4,1,"R2_MonkeyGrabbingWall_Medium",0.7f,0.05f,0,0,-1},   /* 1: the monkey, welded to the block */
 {23,2.5f,0,12,1.5f,0,200,100000,3,0,"StaticBlock_02",0.8f,0.0f,0,0,-1},                /* 2: shelf, static */
 {60,3,0,2.15f,2.15f,2,10000,100,4,1,"SmallPiglette",0.7f,0.05f,0,0,-1},               /* 3: a far goal keeps the stage open */
};
static const CampaignJoint monkey_joints[]={
 {JOINT_WELD,0,1,21.75f,0.3f,21.75f,0.3f,0,0,0,0,0,0,0,0,0,0,0,0},
};
static const CampaignLevel campaign_levels[]={
 {monkey_objects,4,1,{0},20,0,0,5,20,100,200,5,"MONKEY FIXTURE","fixture-monkey",0,monkey_joints,1,0,0,0,-1,-1},};
#define CAMPAIGN_COUNT (sizeof(campaign_levels)/sizeof(campaign_levels[0]))
static const CampaignPolygon campaign_polygons[]={{0,0,{0}}};
static const CampaignForce campaign_forces[]={{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
#endif
