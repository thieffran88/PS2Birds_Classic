#ifndef PS2BIRDS_TRIANGLE_FIXTURE_H
#define PS2BIRDS_TRIANGLE_FIXTURE_H
#include "campaign_types.h"
/* Ground at y=5. Triangles 4x4 resting on the ground; a box above each apex column. */
static const CampaignObject triangle_objects[]={
 {10,3,0,4,4,1.5f,80,2.5f,1,SHAPE_TRIANGLE_L,"WoodBlock11",4.0f,0.2f,0,0,-1},
 {30,3,0,4,4,1.5f,70,2.5f,1,SHAPE_TRIANGLE_M,"WoodBlock12",4.0f,0.2f,0,0,-1},
 {50,3,0,4,4,1.5f,70,2.5f,1,SHAPE_BOX,"WoodBlock9",4.0f,0.2f,0,0,-1},
 {80,3,0,2.15f,2.15f,2,10000,100,4,SHAPE_CIRCLE,"SmallPiglette",0.7f,0.05f,0,0,-1},
};
static const CampaignLevel campaign_levels[]={
 {triangle_objects,4,1,{0},20,-30,0,5,20,100,200,5,"TRIANGLE FIXTURE","fixture-triangle",1,0,0,0,0,0,-1,-1},};
#define CAMPAIGN_COUNT (sizeof(campaign_levels)/sizeof(campaign_levels[0]))
static const CampaignPolygon campaign_polygons[]={{0,0,{0}}};
static const CampaignForce campaign_forces[]={{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
#endif
