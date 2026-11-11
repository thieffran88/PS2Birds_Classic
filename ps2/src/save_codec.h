#ifndef PS2BIRDS_SAVE_CODEC_H
#define PS2BIRDS_SAVE_CODEC_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
#include <stdint.h>
/* Version 1 (0.6.0-0.8.0): 96 bytes, 16 scores. Version 2 (0.9.0+): 160 bytes, 32 scores.
   Both share the signature, identifier, CRC32 trailer and field layout. */
#define SAVE_BYTES_V1 96
#define SAVE_LEVEL_CAPACITY_V1 16
#define SAVE_BYTES 160
#define SAVE_LEVEL_CAPACITY 32
#define SAVE_VERSION 2
typedef struct {uint32_t sequence,count,unlocked,scores[SAVE_LEVEL_CAPACITY];uint32_t version;} SaveRecord;
uint32_t save_crc32(const unsigned char *data,size_t size);
int save_encode(const SaveRecord *record,unsigned char output[SAVE_BYTES]);
int save_decode(const unsigned char *data,size_t size,SaveRecord *record);
int save_choose(const unsigned char *a,size_t size_a,const unsigned char *b,size_t size_b,SaveRecord *record);
#ifdef __cplusplus
}
#endif
#endif
