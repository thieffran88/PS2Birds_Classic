# audsrv sources: PS2SDK, GNU Library GPL v2; see ../COPYING.LIB.
# This build wrapper uses the installed SDK instead of a full SDK source tree.
IOP_BIN = ../../../build/audsrv.irx
IOP_OBJS_DIR = ../../../build/audsrv_iop/
IOP_SRC_DIR = src/
IOP_INC_DIR = include/
IOP_INCS = -Isrc
IOP_OBJS = audsrv.o upsamplers.o hw.o rpc_server.o rpc_client.o common.o cdrom.o imports.o exports.o adpcm.o
all: $(IOP_BIN)
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.iopglobal
