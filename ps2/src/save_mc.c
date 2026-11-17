#include "save_mc.h"
#include "save_codec.h"
#include "save_assets.h"
#include <string.h>
#include <stdio.h>
#ifdef PS2BIRDS_SAVE_TEST
#include "mc_mock.h"
#else
#include <libmc.h>
#include <loadfile.h>
/* mcOpen uses IOP flags directly; no fio calls are made. */
#define NEWLIB_PORT_AWARE
#include <io_common.h>
#endif
extern unsigned char save_icon_sys[];
/* The icon is drawn from the original RedBird sprite, so it lives in SPRITES.BIN and is
   copied here at start-up (main.c); without it the save still works with a blank icon. */
unsigned char save_icon[SAVE_ICON_BYTES] __attribute__((aligned(64)));
/* Memory-card folder of the game's save: Classic keeps /PS2BIRDS; another game built on this runtime
   (the Rio port) sets its own with -DPS2BIRDS_SAVE_DIR='"/PS2BRIO"' so the two never share progress. */
#ifndef PS2BIRDS_SAVE_DIR
#define PS2BIRDS_SAVE_DIR "/PS2BIRDS"
#endif
static const char *slots[]={PS2BIRDS_SAVE_DIR "/progress_a.dat",PS2BIRDS_SAVE_DIR "/progress_b.dat"};
enum {IDLE,INFO,OPEN_READ,READ,CLOSE_READ,CHECK_CARD,CHECK_DIR,MKDIR,OPEN_WRITE,WRITE,FLUSH,CLOSE_WRITE,OPEN_VERIFY,VERIFY,CLOSE_VERIFY};
typedef char save_capacity_matches_game[(MAX_CAMPAIGN_LEVELS==SAVE_LEVEL_CAPACITY)?1:-1];
#define SAVE_IO_BUFFER_BYTES (((SAVE_BYTES+1+63)/64)*64)
static struct {
    Game *game;int available,state,writing,pending,fd,read_slot,target,write_file,failed;
    int type,free_blocks,format,bytes[2],present[2],verify_size;
    unsigned char records[2][SAVE_IO_BUFFER_BYTES] __attribute__((aligned(64)));
    unsigned char tx[SAVE_IO_BUFFER_BYTES] __attribute__((aligned(64)));
    unsigned char verify[SAVE_IO_BUFFER_BYTES] __attribute__((aligned(64)));
    SaveRecord record;const char *status;
} io;
static sceMcTblGetDir entry __attribute__((aligned(64)));
static void fail(int result){
    io.state=IDLE;io.pending=0;
    io.status=result==-3?"MC1 SEM ESPACO":result==-2?"MC1 NAO FORMATADO":result<=-10?"SEM MC1 - L1: TENTAR":"ERRO MC1 - L1: TENTAR";
}
static void queue(int result,int state){if(result<0)fail(result);else io.state=state;}
static void begin(int writing){
    io.writing=writing;io.pending=0;io.failed=0;io.fd=-1;io.read_slot=0;
    memset(io.bytes,0,sizeof(io.bytes));memset(io.present,0,sizeof(io.present));memset(io.records,0,sizeof(io.records));
    io.status=writing?"SALVANDO - NAO REMOVA MC1":"LENDO MEMORY CARD 1";
    queue(mcGetInfo(0,0,&io.type,&io.free_blocks,&io.format),INFO);
}
static int card_ok(int result){
    if(result==-2){fail(-2);return 0;}
    if((result!=0 && result!=-1) || io.type!=MC_TYPE_PS2){fail(result<=-10?result:-12);return 0;}
    return 1;
}
/* Saves written by 0.6.0-0.8.0 (format version 1) numbered stages as
   Level1..Level8 in file order. The campaign now follows the order declared
   by the recovered episodes.lua, so legacy scores move to the new positions
   and every stage won before keeps unlocking the stage that follows it. */
static const int legacy_positions[8]={0,5,3,6,7,4,8,9};
static void remap_legacy(const SaveRecord *in,SaveRecord *out){
    int count=game_level_count();unsigned highest=1;
    memset(out,0,sizeof(*out));out->sequence=in->sequence;out->version=SAVE_VERSION;out->count=(unsigned)count;
    for(unsigned i=0;i<in->count && i<8;i++){
        unsigned position=(unsigned)legacy_positions[i];
        if(position>=(unsigned)count)continue;
        out->scores[position]=in->scores[i];
        if(i<in->unlocked && position+1>highest)highest=position+1;
        if(in->scores[i]>0 && position+2>highest)highest=position+2;
    }
    out->unlocked=highest>(unsigned)count?(unsigned)count:highest;
}
static void merge(const SaveRecord *in){
    SaveRecord mapped;const SaveRecord *r=in;
    if(in->version==1 && in->count<=8){remap_legacy(in,&mapped);r=&mapped;}
    int count=game_level_count();
    if(r->unlocked>(unsigned)io.game->unlocked)io.game->unlocked=r->unlocked>(unsigned)count?count:(int)r->unlocked;
    /* Older campaigns ended here. A winning last-stage score unlocks the
       first added stage; merely having reached the last stage does not. */
    if(r->count<(unsigned)count && r->unlocked==r->count && r->scores[r->count-1]>0 && io.game->unlocked<=(int)r->count)
        io.game->unlocked=(int)r->count+1;
    for(unsigned i=0;i<r->count && i<(unsigned)count;i++)if(r->scores[i]>(unsigned)io.game->highscores[i])io.game->highscores[i]=(int)r->scores[i];
}
static void open_read(void){queue(mcOpen(0,0,slots[io.read_slot],FIO_O_RDONLY),OPEN_READ);}
static const unsigned char *write_data(void){return io.write_file==0?save_icon_sys:io.write_file==1?save_icon:io.tx;}
static int write_size(void){return io.write_file==0?SAVE_SYS_BYTES:io.write_file==1?SAVE_ICON_BYTES:SAVE_BYTES;}
static void open_write(void){
    const char *path=io.write_file==0?PS2BIRDS_SAVE_DIR "/icon.sys":io.write_file==1?PS2BIRDS_SAVE_DIR "/save.icn":slots[io.target];
    io.failed=0;queue(mcOpen(0,0,path,FIO_O_WRONLY|FIO_O_CREAT|FIO_O_TRUNC),OPEN_WRITE);
}
static void finish_reads(void){
    for(int i=0;i<2;i++)if(save_requires_newer(io.records[i],io.bytes[i],(unsigned)game_level_count())){
        io.status="SAVE DE VERSAO MAIS NOVA";io.state=IDLE;io.pending=0;return;
    }
    SaveRecord best;int slot=save_choose(io.records[0],io.bytes[0],io.records[1],io.bytes[1],&best);
    if(slot>=0 && best.count>(unsigned)game_level_count()){io.status="SAVE DE VERSAO MAIS NOVA";io.state=IDLE;io.pending=0;return;}
    if(slot>=0)merge(&best);
    else if(io.present[0] || io.present[1]){io.status="SAVE INVALIDO - VERIFIQUE MC1";io.state=IDLE;io.pending=0;return;}
    if(!io.writing){io.status=slot>=0?"PROGRESSO CARREGADO DO MC1":"SEM SAVE - GRAVACAO AUTOMATICA";io.state=IDLE;return;}
    memset(&io.record,0,sizeof(io.record));io.record.sequence=slot>=0?best.sequence+1:1;io.record.version=SAVE_VERSION;
    io.record.count=game_level_count();io.record.unlocked=io.game->unlocked;
    for(unsigned i=0;i<io.record.count;i++)io.record.scores[i]=io.game->highscores[i];
    if(!save_encode(&io.record,io.tx)){fail(-5);return;}
    io.target=slot==0?1:0;
    queue(mcGetInfo(0,0,&io.type,&io.free_blocks,&io.format),CHECK_CARD);
}
static void next_read(void){if(io.read_slot++==0)open_read();else finish_reads();}
void save_mc_init(Game *game){
    memset(&io,0,sizeof(io));io.game=game;io.fd=-1;io.status="MC1 INDISPONIVEL - JOGO CONTINUA";
    int a=SifLoadModule("rom0:MCMAN",0,NULL),b=SifLoadModule("rom0:MCSERV",0,NULL);
    if(a<0 || b<0 || mcInit(MC_TYPE_MC)<0)return;
    io.available=1;begin(0);
}
void save_mc_request(void){if(!io.available)return;if(io.state!=IDLE)io.pending=1;else begin(1);}
int save_mc_busy(void){return io.state!=IDLE || io.pending;}
const char *save_mc_status(void){return io.status;}
void save_mc_poll(void){
    if(io.state==IDLE){if(io.pending)begin(1);return;}
    int result=0,sync=mcSync(MC_NOWAIT,NULL,&result);
    if(sync==0)return;
    if(sync<0){fail(-5);return;}
    switch(io.state){
    case INFO:if(card_ok(result))open_read();break;
    case OPEN_READ:
        if(result==-4){next_read();break;}
        if(result<0){fail(result);break;}
        io.fd=result;io.present[io.read_slot]=1;
        queue(mcRead(io.fd,io.records[io.read_slot],SAVE_BYTES+1),READ);break;
    case READ:
        io.failed=result<0?result:0;io.bytes[io.read_slot]=result>=0?result:0;
        queue(mcClose(io.fd),CLOSE_READ);break;
    case CLOSE_READ:
        io.fd=-1;if(result<0 || io.failed){fail(io.failed?io.failed:result);break;}
        next_read();break;
    case CHECK_CARD:
        if(!card_ok(result))break;
        if(result==-1){io.status="MC1 TROCADO - L1: TENTAR";io.state=IDLE;io.pending=0;break;}
        queue(mcGetDir(0,0,PS2BIRDS_SAVE_DIR,0,1,&entry),CHECK_DIR);break;
    case CHECK_DIR:
        if(result==1 && (entry.AttrFile&MC_ATTR_SUBDIR)){io.write_file=0;open_write();}
        else if(result==0 || result==-4)queue(mcMkDir(0,0,PS2BIRDS_SAVE_DIR),MKDIR);
        else fail(result<0?result:-5);
        break;
    case MKDIR:if(result<0)fail(result);else{io.write_file=0;open_write();}break;
    case OPEN_WRITE:
        if(result<0){fail(result);break;}io.fd=result;
        queue(mcWrite(io.fd,write_data(),write_size()),WRITE);break;
    case WRITE:
        if(result!=write_size()){io.failed=result<0?result:-5;queue(mcClose(io.fd),CLOSE_WRITE);}
        else queue(mcFlush(io.fd),FLUSH);
        break;
    case FLUSH:io.failed=result<0?result:0;queue(mcClose(io.fd),CLOSE_WRITE);break;
    case CLOSE_WRITE:
        io.fd=-1;if(result<0 || io.failed){fail(io.failed?io.failed:result);break;}
        if(++io.write_file<3)open_write();else queue(mcOpen(0,0,slots[io.target],FIO_O_RDONLY),OPEN_VERIFY);break;
    case OPEN_VERIFY:
        if(result<0){fail(result);break;}io.fd=result;queue(mcRead(io.fd,io.verify,SAVE_BYTES+1),VERIFY);break;
    case VERIFY:io.verify_size=result;queue(mcClose(io.fd),CLOSE_VERIFY);break;
    case CLOSE_VERIFY:{
        SaveRecord record;io.fd=-1;
        if(result<0 || io.verify_size!=SAVE_BYTES || memcmp(io.verify,io.tx,SAVE_BYTES) || !save_decode(io.verify,SAVE_BYTES,&record)){fail(result<0?result:-5);break;}
        io.status="PROGRESSO SALVO NO MC1";io.state=IDLE;break;
    }
    default:fail(-5);break;
    }
}
