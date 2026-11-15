#include "game.h"
#include "campaign_types.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
/* GlobeBird (puffer_bird.lua) and balloon material forces (blocks.lua forceX/forceY). */
static void update(Game *g,int frames){for(int i=0;i<frames;i++)game_update(g,1.f/120);}
static int first_level_with(int type){for(int i=0;i<game_level_count();i++){Game g;game_init(&g);g.unlocked=game_level_count();game_load_level(&g,i);int found=game_queue_type(&g,0)==type;game_destroy(&g);if(found)return i;}return -1;}
int main(){
 int level=first_level_with(BIRD_PUFFER_TYPE);assert(level>=0);
 Game g;game_init(&g);g.unlocked=game_level_count();assert(game_load_level(&g,level));
 assert(!game_special_available(&g));update(&g,240);g.angle=30;g.power=90;game_launch(&g);
 GameBird b;update(&g,30);assert(game_get_projectile(&g,0,&b) && b.fuse<0 && game_special_available(&g));
 assert(game_special(&g) && !game_special_available(&g) && !game_special(&g));
 assert(game_get_projectile(&g,0,&b) && std::fabs(b.fuse-1.0f)<1e-4f);
 int puffed=0,shrunk=0,gone=-1;float last=1;
 for(int f=1;f<=600;f++){update(&g,1);
  if(game_projectile_count(&g)==0){gone=f;break;}
  assert(game_get_projectile(&g,0,&b));
  if(b.fuse>0.999f)puffed++;else{assert(b.fuse<=last+1e-4f);shrunk++;}
  last=b.fuse;}
 // Puffed for GLOBE_PUFF_TIME, then shrinks for GLOBE_DEATH_TIME and disappears.
 assert(std::abs(puffed-(int)(GLOBE_PUFF_TIME*120))<=2 && std::abs(shrunk-(int)(GLOBE_DEATH_TIME*120))<=2 && gone>0);
 // The puff is still available after a contact (specialtyAvailableAfterCollision) and fires on its own after the delay.
 game_reset(&g);update(&g,240);g.angle=0;g.power=100;game_launch(&g);bool touched=false;int auto_frame=-1;
 for(int f=0;f<1400 && g.state==FLYING;f++){update(&g,1);if(!game_projectile_count(&g))break;assert(game_get_projectile(&g,0,&b));
  if(b.collided && !touched){touched=true;assert(game_special_available(&g));auto_frame=f;}
  if(touched && b.fuse>0){assert(f-auto_frame>=(int)(GLOBE_AUTO_PUFF_DELAY*120)-2);break;}}
 assert(touched);
 // Balloons: forceY -30 x mass (beats gravity 20) keeps the balloons and their ropes in place.
 int balloon_level=-1;
 for(int i=0;i<game_level_count() && balloon_level<0;i++){assert(game_load_level(&g,i));for(int k=0;k<game_object_count(&g);k++){GameObject o;game_get_object(&g,k,&o);if(!strncmp(o.definition,"BirthdayBalloon",15)){balloon_level=i;break;}}}
 assert(balloon_level>=0 && game_load_level(&g,balloon_level));
 int n=game_object_count(&g),checked=0;float y0[MAX_LEVEL_OBJECTS];for(int k=0;k<n;k++){GameObject o;game_get_object(&g,k,&o);y0[k]=o.y;}
 update(&g,240);
 for(int k=0;k<n;k++){GameObject o;game_get_object(&g,k,&o);if(strncmp(o.definition,"BirthdayBalloon",15))continue;assert(o.alive && std::fabs(o.y-y0[k])<3);checked++;}
 assert(checked>0);
 // Stella (PinkBird): tap -> hover for PINK_BOOM_TIME, bubble rises (<= BUBBLE_MAX_VELOCITY) for BUBBLE_TIME, then leaves.
 int pink=first_level_with(BIRD_PINK_TYPE);assert(pink>=0 && game_load_level(&g,pink));update(&g,240);g.angle=35;g.power=85;game_launch(&g);
 update(&g,40);assert(game_special_available(&g) && game_special(&g) && !game_special_available(&g));
 GameBird pb;update(&g,2);assert(game_get_projectile(&g,0,&pb) && pb.vx==0 && std::fabs(pb.vy)<1);
 int hover=0,rise=0,end_frame=-1;
 for(int f=0;f<900;f++){update(&g,1);if(!game_projectile_count(&g)){end_frame=f;break;}assert(game_get_projectile(&g,0,&pb));
  float vy=pb.vy/g.view_scale;if(std::fabs(vy)<0.5f && std::fabs(pb.vx)<0.5f)hover++;if(vy<-1){rise++;assert(vy>-BUBBLE_MAX_VELOCITY-1.5f);}}
 assert(end_frame>0 && rise>(int)(BUBBLE_TIME*120*0.6f) && end_frame<=(int)((PINK_BOOM_TIME+BUBBLE_TIME)*120)+4);
 game_destroy(&g);
 // deathSpawn (gamelogic.lua): the big cake releases its 10 creams where it is destroyed.
 int cake_level=-1;
 for(int i=0;i<game_level_count() && cake_level<0;i++){assert(game_load_level(&g,i));for(int k=0;k<game_object_count(&g);k++){GameObject o;game_get_object(&g,k,&o);if(!strcmp(o.definition,"Spawning_cake")){cake_level=i;break;}}}
 assert(cake_level>=0);
 bool released=false;
 for(int a=-10;a<=60 && !released;a+=5)for(int pw=50;pw<=100 && !released;pw+=10){
  assert(game_load_level(&g,cake_level));int cakes=0,creams=0,hidden=0;
  for(int k=0;k<game_object_count(&g);k++){GameObject o;game_get_object(&g,k,&o);if(!strcmp(o.definition,"Spawning_cake"))cakes++;if(!strcmp(o.definition,"CakeCream")){creams++;hidden+=!o.alive;}}
  assert(cakes>0 && creams==10*cakes && hidden==creams);
  update(&g,240);g.angle=(float)a;g.power=(float)pw;game_launch(&g);
  for(int t=0;t<1200 && g.state==FLYING;t++){
   game_update(&g,1.f/120);
   int lost=0,out=0;for(int k=0;k<game_object_count(&g);k++){GameObject o;game_get_object(&g,k,&o);if(!strcmp(o.definition,"Spawning_cake")&&!o.alive)lost++;if(!strcmp(o.definition,"CakeCream")&&o.alive)out++;}
   if(lost){assert(out>0 && out<=10*lost);released=true;break;}
  }
 }
 assert(released);
 // Short Fuse shockwave bird (BlackBirdAbility.lua): a tap starts the wave; after the 0.4 s
 // charge it grows to 15 units in 1.9 s, pushes pieces and electrocutes pigs, which die 2 s later.
 int shock_level=first_level_with(BIRD_SHOCK_TYPE);assert(shock_level>=0);
 assert(game_load_level(&g,shock_level));update(&g,240);
 g.angle=20;g.power=100;game_launch(&g);update(&g,40);
 assert(game_special_available(&g) && game_special(&g) && !game_special_available(&g));
 int pigs_before=g.pig_alive;GameBlast wave;
 update(&g,24);assert(!game_get_blast(&g,&wave));            /* still charging (0.4 s) */
 update(&g,60);assert(game_get_blast(&g,&wave) && wave.radius>0 && wave.radius<15*g.view_scale);
 float r1=wave.radius;update(&g,60);assert(game_get_blast(&g,&wave) && wave.radius>r1);
 update(&g,600);assert(g.pig_alive<=pigs_before);
 // Mirrored pieces (negative scaleX/scaleY in the level, Piggy Farm) carry the flip to the renderer.
 int mirrored=0;
 for(int lv=0;lv<game_level_count() && !mirrored;lv++){assert(game_load_level(&g,lv));for(int k=0;k<game_object_count(&g);k++){GameObject o;game_get_object(&g,k,&o);if(o.mirror){assert(o.width>0 && o.height>0);mirrored++;}}}
 assert(mirrored>0);
 // Dino cages: breaking one adds the caged bird to the end of the queue.
 int caged=0;
 for(int lv=0;lv<game_level_count() && !caged;lv++){
     assert(game_load_level(&g,lv));
     for(int k=0;k<game_object_count(&g);k++){GameObject o;game_get_object(&g,k,&o);
         if(o.definition && strstr(o.definition,"_Cage")){
             int before=g.birds,last=game_queue_type(&g,g.birds-1);game_damage_object(&g,k);game_update(&g,0.05f);
             assert(g.birds==before+1 && game_queue_type(&g,before-1)==last && game_queue_type(&g,before)>=0);caged++;break;}}
 }
 assert(caged>0);
 // Golden Eggs: the trajectory bends with the stage's gravity (upside down in LevelGE_31, straight in LevelGE_UGC1).
 int bends[3]={0,0,0};
 for(int lv=0;lv<game_level_count();lv++){
     const char *sc=game_level_script(lv);int which=!strcmp(sc,"LevelGE_31")?1:!strcmp(sc,"LevelGE_UGC1")?2:!strcmp(sc,"LevelGE_4")?0:-1;
     if(which<0)continue;
     assert(game_load_level(&g,lv));
     float x,y0,y1,y2;game_preview(&g,0,&x,&y0);game_preview(&g,0.5f,&x,&y1);game_preview(&g,1,&x,&y2);
     float c=y2-2*y1+y0;bends[which]=c>0.01f?1:c<-0.01f?-1:0;
 }
 assert(bends[0]==1 && bends[1]==-1 && bends[2]==0);
 // Short Fuse vials: an orange vial pops the pigs around it; a pink one lifts them in a bubble.
 int vial_seen[3]={0,0,0};
 for(int lv=0;lv<game_level_count() && !(vial_seen[1]&&vial_seen[2]);lv++){
     assert(game_load_level(&g,lv));
     for(int k=0;k<game_object_count(&g);k++){
         GameObject v;game_get_object(&g,k,&v);if(!v.definition)continue;
         int kind=!strcmp(v.definition,"BLACKBIRD_VIAL_ORANGE")?1:!strcmp(v.definition,"BLACKBIRD_VIAL_PINK")?2:0;
         if(!kind || vial_seen[kind])continue;
         float radius=(kind==1?6:7)*g.view_scale;int pig=-1;
         for(int m=0;m<game_object_count(&g);m++){GameObject o;game_get_object(&g,m,&o);if(o.kind==4 && o.alive && (o.x-v.x)*(o.x-v.x)+(o.y-v.y)*(o.y-v.y)<radius*radius*0.5f){pig=m;break;}}
         if(pig<0)continue;
         GameObject before;game_get_object(&g,pig,&before);
         game_damage_object(&g,k);
         float lowest=before.y;
         for(int t=0;t<120*2;t++){game_update(&g,1.0f/120);GameObject o;game_get_object(&g,pig,&o);if(o.alive && o.y<lowest)lowest=o.y;}
         GameObject mid;game_get_object(&g,pig,&mid);
         for(int t=0;t<120*3;t++)game_update(&g,1.0f/120);
         GameObject after;game_get_object(&g,pig,&after);
         if(kind==1){assert(mid.alive && !after.alive);}
         else {if(!(lowest<before.y-g.view_scale))continue; /* pig held down by the structure: try another */}
         vial_seen[kind]=1;break;
     }
 }
 assert(vial_seen[1] && vial_seen[2]);
 printf("Globe OK: level %d, puff on tap (radius %.1f), %.1f s puffed, shrinks over %.1f s and disappears, available after contact with automatic puff after %.1f s; %d balloons hold their ropes; Stella hovers, bubbles and leaves after %.1f s; mirrored pieces; a broken cage adds its bird to the queue; trajectories follow upside-down and zero gravity; orange vials pop nearby pigs and pink vials lift them in bubbles.\n",level,GLOBE_PUFFED_RADIUS,GLOBE_PUFF_TIME,GLOBE_DEATH_TIME,GLOBE_AUTO_PUFF_DELAY,checked,PINK_BOOM_TIME+BUBBLE_TIME);
}
