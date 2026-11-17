/* Game identity of the save: a file written by one build must be refused by a build with another
   PS2BIRDS_SAVE_ID. usage: save_id_test write FILE | save_id_test refuse FILE | save_id_test accept FILE */
#include <stdio.h>
#include <string.h>
#include "save_codec.h"
int main(int argc,char **argv){
    unsigned char buf[SAVE_BYTES];SaveRecord r,out;memset(&r,0,sizeof r);
    if(argc!=3)return 2;
    if(!strcmp(argv[1],"write")){
        r.sequence=1;r.count=3;r.unlocked=2;r.scores[0]=1000;
        if(!save_encode(&r,buf))return 1;
        FILE *f=fopen(argv[2],"wb");if(!f)return 1;fwrite(buf,1,SAVE_BYTES,f);fclose(f);return 0;
    }
    FILE *f=fopen(argv[2],"rb");if(!f)return 1;size_t n=fread(buf,1,SAVE_BYTES,f);fclose(f);
    int ok=save_decode(buf,n,&out);
    if(!strcmp(argv[1],"accept"))return ok&&out.scores[0]==1000?0:1;
    if(!strcmp(argv[1],"refuse"))return ok?1:0;
    return 2;
}
