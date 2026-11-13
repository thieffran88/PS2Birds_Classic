#ifndef PS2BIRDS_SAVE_CODEC_H
#define PS2BIRDS_SAVE_CODEC_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
#include <stdint.h>
/* Version 1 (0.6.0-0.8.0): 96 bytes, 16 scores. Version 2 (0.9.0+): 160 bytes, 32 scores.
   Version 3 (0.13.0-0.16.0): 288 bytes, 64 scores. Version 4 (0.17.0-0.22.0): 544 bytes, 128 scores.
   Version 5 (0.23.0-0.27.0): 1056 bytes, 256 scores. Version 6 (0.28.0+): 2080 bytes, 512 scores.
   All share the signature, identifier, CRC32 trailer and field layout. */
#define SAVE_BYTES_V1 96
#define SAVE_LEVEL_CAPACITY_V1 16
#define SAVE_BYTES_V2 160
#define SAVE_LEVEL_CAPACITY_V2 32
#define SAVE_BYTES_V3 288
#define SAVE_LEVEL_CAPACITY_V3 64
#define SAVE_BYTES_V4 544
#define SAVE_LEVEL_CAPACITY_V4 128
#define SAVE_BYTES_V5 1056
#define SAVE_LEVEL_CAPACITY_V5 256
#define SAVE_BYTES 2080
#define SAVE_LEVEL_CAPACITY 512
#define SAVE_VERSION 6
typedef struct {uint32_t sequence,count,unlocked,scores[SAVE_LEVEL_CAPACITY];uint32_t version;} SaveRecord;
uint32_t save_crc32(const unsigned char *data,size_t size);
int save_encode(const SaveRecord *record,unsigned char output[SAVE_BYTES]);
int save_decode(const unsigned char *data,size_t size,SaveRecord *record);
int save_requires_newer(const unsigned char *data,size_t size,unsigned supported_count);
int save_choose(const unsigned char *a,size_t size_a,const unsigned char *b,size_t size_b,SaveRecord *record);
#ifdef __cplusplus
}
#endif
#endif
