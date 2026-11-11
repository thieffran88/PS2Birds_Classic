#ifndef PS2BIRDS_SAVE_CODEC_H
#define PS2BIRDS_SAVE_CODEC_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
#include <stdint.h>
#define SAVE_BYTES 96
#define SAVE_LEVEL_CAPACITY 16
typedef struct {uint32_t sequence,count,unlocked,scores[SAVE_LEVEL_CAPACITY];} SaveRecord;
uint32_t save_crc32(const unsigned char *data,size_t size);
int save_encode(const SaveRecord *record,unsigned char output[SAVE_BYTES]);
int save_decode(const unsigned char *data,size_t size,SaveRecord *record);
int save_choose(const unsigned char *a,size_t size_a,const unsigned char *b,size_t size_b,SaveRecord *record);
#ifdef __cplusplus
}
#endif
#endif
