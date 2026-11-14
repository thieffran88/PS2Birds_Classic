#ifndef PS2BIRDS_WHITE_FIXTURE_H
#define PS2BIRDS_WHITE_FIXTURE_H
#include "campaign_types.h"
static const CampaignObject egg_objects[]={
 {2,8,0,1,1,2,20,1,4,1,"SmallPiglette",0.7f,0.05f,0,0},
 {5,8,0,1,1,2,500,1,1,0,"WoodBlock1",4.0f,0.2f,0,0},
 {9,8,0,1,1,2,500,1,1,0,"WoodBlock1",4.0f,0.2f,0,0},
 {4,8,0,1,1,0,500,100000,3,0,"StaticBlock_01",0.8f,0.0f,0,0},
 {30,8,0,1,1,2,10000,100,4,1,"SmallPiglette",0.7f,0.05f,0,0},
};
static const CampaignObject distant_pig[]={
 {50,8,0,1,1,2,10000,100,4,1,"SmallPiglette",0.7f,0.05f,0,0},
};
static const CampaignObject dynamic_target[]={
 {0,6,0,1,1,2,200,1,1,0,"WoodBlock1",4.0f,0.2f,0,0},
 {30,6,0,1,1,2,10000,100,4,1,"SmallPiglette",0.7f,0.05f,0,0},
};
static const CampaignLevel campaign_levels[]={
 {egg_objects,5,2,{4,4},20,0,0,10,20,100,200,5,"EGG DAMAGE","fixture-egg",0,0,0,0,0},
 {distant_pig,1,2,{4,4},20,-30,0,10,20,100,200,5,"WHITE FLIGHT","fixture-white",0,0,0,0,0},
 {dynamic_target,2,1,{4},20,0,0,100,20,100,200,5,"DYNAMIC IMPACT","fixture-dynamic",0,0,0,0,0},
};
#define CAMPAIGN_COUNT (sizeof(campaign_levels)/sizeof(campaign_levels[0]))
static const CampaignPolygon campaign_polygons[]={{0,0,{0}}};
static const CampaignForce campaign_forces[]={{0,0,0,0,0,0,0,0}};
#endif
