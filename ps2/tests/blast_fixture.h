#ifndef PS2BIRDS_BLAST_FIXTURE_H
#define PS2BIRDS_BLAST_FIXTURE_H
#include "campaign_types.h"
static const CampaignObject blast_objects[]={
 {2.5f,0,0,1,1,2,100,1,1,0,"WoodBlock1",4.0f,0.2f,0,0,-1},
 {4.5f,0,0,1,1,2,200,1,1,0,"WoodBlock1",4.0f,0.2f,0,0,-1},
 {0,7,0,1,1,2,200,1,1,0,"WoodBlock1",4.0f,0.2f,0,0,-1},
 {16,0,0,1,1,2,200,1,1,0,"WoodBlock1",4.0f,0.2f,0,0,-1},
 {0,3,0,1,1,0,200,100000,3,0,"StaticBlock_01",0.8f,0.0f,0,0,-1},
 {0,-2.5f,0,1,1,2,10,1,4,1,"SmallPiglette",0.7f,0.05f,0,0,-1},
 {30,0,0,1,1,2,10000,100,4,1,"SmallPiglette",0.7f,0.05f,0,0,-1},
};
static const CampaignObject fuse_objects[]={
 {50,3,0,1,1,2,10000,100,4,1,"SmallPiglette",0.7f,0.05f,0,0,-1},
};
static const CampaignLevel campaign_levels[]={
 {blast_objects,7,1,{3},20,0,0,100,20,100,200,5,"BLAST FIXTURE","fixture-blast",0,0,0,0,0,0,-1,-1},
 {fuse_objects,1,2,{3,3},20,-30,0,5,20,100,200,5,"FUSE FIXTURE","fixture-fuse",0,0,0,0,0,0,-1,-1},};
#define CAMPAIGN_COUNT (sizeof(campaign_levels)/sizeof(campaign_levels[0]))
static const CampaignPolygon campaign_polygons[]={{0,0,{0}}};
static const CampaignForce campaign_forces[]={{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
#endif
