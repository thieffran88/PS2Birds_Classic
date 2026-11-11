#ifndef PS2BIRDS_CAMPAIGN_TYPES_H
#define PS2BIRDS_CAMPAIGN_TYPES_H
#define MAX_LEVEL_OBJECTS 128
#define MAX_CAMPAIGN_LEVELS 32
typedef struct {float x,y,angle,width,height,density,strength,defence;int kind,circle;const char *definition;} CampaignObject;
typedef struct {const CampaignObject *objects;int count,birds;int bird_types[16];float gravity,sling_x,sling_y,ground_y,pixels,view_x,view_y,view_scale;const char *title,*script;} CampaignLevel;
#endif
