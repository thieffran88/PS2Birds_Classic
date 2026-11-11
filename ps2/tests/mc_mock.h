#ifndef MC_MOCK_H
#define MC_MOCK_H
#define MC_NOWAIT 1
#define MC_TYPE_MC 0
#define MC_TYPE_PS2 2
#define MC_ATTR_SUBDIR 0x20
#define FIO_O_RDONLY 1
#define FIO_O_WRONLY 2
#define FIO_O_CREAT 0x200
#define FIO_O_TRUNC 0x400
typedef struct {unsigned short AttrFile;} sceMcTblGetDir;
#ifdef __cplusplus
extern "C" {
#endif
int SifLoadModule(const char*,int,const char*);
int mcInit(int);
int mcGetInfo(int,int,int*,int*,int*);
int mcOpen(int,int,const char*,int);
int mcRead(int,void*,int);
int mcWrite(int,const void*,int);
int mcClose(int);
int mcFlush(int);
int mcGetDir(int,int,const char*,int,int,sceMcTblGetDir*);
int mcMkDir(int,int,const char*);
int mcSync(int,int*,int*);
#ifdef __cplusplus
}
#endif
#endif
