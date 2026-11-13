#ifndef PS2BIRDS_JOINT_FIXTURE_H
#define PS2BIRDS_JOINT_FIXTURE_H
#include "campaign_types.h"
/* Ground at y=5 (surface). */
static const CampaignObject joint_objects[]={
 {10,-6,0,2,2,1.5f,10000,100,6,SHAPE_CIRCLE,"ExtraTire01",0.7f,0.4f},   /* 0 distance pair, raised so the spring pulls them */
 {16,-6,0,2,2,1.5f,10000,100,6,SHAPE_CIRCLE,"ExtraTire01",0.7f,0.4f},   /* 1 */
 {30,3.5f,0,4,1,1.5f,10000,100,1,SHAPE_BOX,"WoodBlock4",4.0f,0.2f},       /* 2 weld base on the ground */
 {30,0.5f,0,4,1,1.5f,10000,100,1,SHAPE_BOX,"WoodBlock4",4.0f,0.2f},       /* 3 welded 3 units above, nothing under it */
 {50,-10,0,8,1,1.5f,10000,100,1,SHAPE_BOX,"WoodBlock6",4.0f,0.2f},        /* 4 revolute bar hinged to the ground at its left end */
 {80,3.9f,0,2.15f,2.15f,2,10000,100,4,SHAPE_CIRCLE,"SmallPiglette",0.7f,0.05f},
};
static const CampaignJoint joint_list[]={
 {JOINT_DISTANCE,0,1,10,-6,16,-6,4,0.5f,0,0},
 {JOINT_WELD,2,3,30,3.5f,30,0.5f,0,0,0,0},
 {JOINT_REVOLUTE,-1,4,46,-10,46,-10,0,0,0,0},
};
static const CampaignJoint breakable_list[]={
 {JOINT_WELD,2,3,30,3.5f,30,0.5f,0,0,0.001f,0},
};
static const CampaignLevel campaign_levels[]={
 {joint_objects,6,1,{0},20,0,0,5,20,100,200,5,"JOINT FIXTURE","fixture-joint",0,joint_list,3,0,0},
 {joint_objects,6,1,{0},20,0,0,5,20,100,200,5,"BREAK FIXTURE","fixture-break",0,breakable_list,1,0,0},
};
#define CAMPAIGN_COUNT (sizeof(campaign_levels)/sizeof(campaign_levels[0]))
static const CampaignPolygon campaign_polygons[]={{0,0,{0}}};
#endif
