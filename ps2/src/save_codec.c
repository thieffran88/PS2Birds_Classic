#include "save_codec.h"
#include <string.h>
static void put32(unsigned char *p,uint32_t v){for(int i=0;i<4;i++)p[i]=(unsigned char)(v>>(i*8));}
static uint32_t get32(const unsigned char *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
uint32_t save_crc32(const unsigned char *p,size_t size){
    uint32_t crc=0xffffffffu;
    for(size_t i=0;i<size;i++){crc^=p[i];for(int bit=0;bit<8;bit++)crc=(crc>>1)^((0u-(crc&1u))&0xedb88320u);}
    return ~crc;
}
static int valid(const SaveRecord *r){
    if(!r || r->count<1 || r->count>SAVE_LEVEL_CAPACITY || r->unlocked<1 || r->unlocked>r->count)return 0;
    for(unsigned i=0;i<SAVE_LEVEL_CAPACITY;i++)if(r->scores[i]>1000000000u || (i>=r->count && r->scores[i]))return 0;
    return 1;
}
int save_encode(const SaveRecord *r,unsigned char out[SAVE_BYTES]){
    if(!valid(r) || !out)return 0;
    memset(out,0,SAVE_BYTES);memcpy(out,"PS2BIRD1",8);put32(out+8,1);put32(out+12,0x434c4153u);
    put32(out+16,r->sequence);put32(out+20,r->count);put32(out+24,r->unlocked);
    for(int i=0;i<SAVE_LEVEL_CAPACITY;i++)put32(out+28+i*4,r->scores[i]);
    put32(out+92,save_crc32(out,92));return 1;
}
int save_decode(const unsigned char *data,size_t size,SaveRecord *out){
    if(!data || !out || size!=SAVE_BYTES || memcmp(data,"PS2BIRD1",8) || get32(data+8)!=1 || get32(data+12)!=0x434c4153u || get32(data+92)!=save_crc32(data,92))return 0;
    SaveRecord r;r.sequence=get32(data+16);r.count=get32(data+20);r.unlocked=get32(data+24);
    for(int i=0;i<SAVE_LEVEL_CAPACITY;i++)r.scores[i]=get32(data+28+i*4);
    if(!valid(&r))return 0;
    *out=r;return 1;
}
int save_choose(const unsigned char *a,size_t na,const unsigned char *b,size_t nb,SaveRecord *out){
    SaveRecord ra,rb;int va=save_decode(a,na,&ra),vb=save_decode(b,nb,&rb);
    if(!out || (!va && !vb))return -1;
    int slot=(!va || (vb && rb.sequence!=ra.sequence && (uint32_t)(rb.sequence-ra.sequence)<0x80000000u))?1:0;
    *out=slot?rb:ra;return slot;
}
