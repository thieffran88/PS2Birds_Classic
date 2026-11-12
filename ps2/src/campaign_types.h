#ifndef PS2BIRDS_CAMPAIGN_TYPES_H
#define PS2BIRDS_CAMPAIGN_TYPES_H
#define MAX_LEVEL_OBJECTS 160
#define MAX_CAMPAIGN_LEVELS 128
typedef struct {float x,y,angle,width,height,density,strength,defence;int kind,shape;const char *definition;} CampaignObject;
/* CampaignObject.shape: box, circle, right triangle (vertices 0,0 1,1 0,1) and
   isosceles triangle (0.5,0 1,1 0,1), normalized over the sprite extents. */
enum {SHAPE_BOX=0,SHAPE_CIRCLE=1,SHAPE_TRIANGLE_L=2,SHAPE_TRIANGLE_M=3};
typedef struct {const CampaignObject *objects;int count,birds;int bird_types[16];float gravity,sling_x,sling_y,ground_y,pixels,view_x,view_y,view_scale;const char *title,*script;int bank;} CampaignLevel;
#endif
