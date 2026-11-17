EE_BIN = build/PS2Birds_Classic.elf
BOX2D_SRC = $(shell find vendor/box2d/src -name "*.cpp")
BOX2D_OBJS = $(patsubst %.cpp,build/%.o,$(BOX2D_SRC))
EE_OBJS = build/audio.o build/audio_assets.o build/audsrv_rpc.o build/save_codec.o build/save_mc.o build/save_assets.o build/main.o build/menu.o build/level_end.o build/music.o build/ui.o build/font.o build/menu_gfx.o build/background.o build/cutscene.o build/game.o build/sprites.o $(BOX2D_OBJS)
EE_INCS = -Ivendor/box2d/include -Ivendor/box2d/src -Isrc -I$(GSKIT)/include
EE_CFLAGS = -std=gnu99 -Wextra
EE_CXXFLAGS = -std=gnu++11 -Wextra -fno-exceptions -fno-rtti
EE_LDFLAGS = -L$(GSKIT)/lib
# GAME=rio builds the Rio variant of the runtime: its own memory-card folder and save identity, so
# it never reads or writes the Classic progress. The default (classic) is unchanged.
GAME ?= classic
ifeq ($(GAME),rio)
EE_BIN = build/PS2Birds_Rio.elf
EE_CFLAGS += -DPS2BIRDS_SAVE_DIR='"/PS2BRIO"' -DPS2BIRDS_RIO -DPS2BIRDS_SAVE_ID=0x52494f31u -DPS2BIRDS_SPRITE_IDS='"rio/sprite_ids.h"' -DPS2BIRDS_DEFINITION_SPRITES='"rio/definition_sprites.h"'
EE_CXXFLAGS += -DPS2BIRDS_SPRITE_IDS='"rio/sprite_ids.h"'
endif
EE_LIBS = -lgskit -ldmakit -lpad -lmc -lpatches -lm

.PHONY: all clean test iso
# The ELF holds no original data. The data files are made from the APK (tools/gerar_iso.py);
# a developer checkout regenerates ps2/assets with that tool and packs them here.
DATA = build/SPRITES.BIN build/SOUNDS.BIN build/MUSIC.BIN build/UI.BIN build/FONT.BIN build/MENU.BIN build/THEMES.BIN build/CAMPAIGN.BIN
all: $(EE_BIN) $(DATA)

build/%.o: src/%.c src/game.h src/level_config.h src/campaign_types.h
	@mkdir -p build
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

build/main.o: src/audio.h src/audio_ids.h src/save_assets.h

build/main.o build/sprites.o: src/sprite_ids.h src/sprites.h

build/audio.o: src/audio.h src/audio_ids.h

# Each module's own header (a changed enum or struct rebuilds its users).
build/main.o: src/menu.h src/level_end.h src/music.h src/ui.h src/font.h src/menu_gfx.h src/background.h
build/menu.o: src/menu.h
build/level_end.o: src/level_end.h
build/music.o: src/music.h
build/ui.o: src/ui.h
build/font.o: src/font.h
build/menu_gfx.o: src/menu_gfx.h src/sprites.h
build/background.o: src/background.h src/sprites.h
build/cutscene.o: src/cutscene.h

build/audsrv_rpc.o: vendor/audsrv/ee/src/audsrv_rpc.c
	@mkdir -p build
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

build/audsrv.irx: $(wildcard vendor/audsrv/iop/src/*) vendor/audsrv/iop/Makefile
	$(MAKE) -C vendor/audsrv/iop

build/audio_assets.o: src/audio_assets.S build/audsrv.irx
	@mkdir -p build
	$(EE_CC) -G0 -c $< -o $@

build/save_codec.o: src/save_codec.h

build/save_mc.o: src/save_mc.h src/save_codec.h src/save_assets.h

build/save_assets.o: src/save_assets.S assets/save_icon.sys
	@mkdir -p build
	$(EE_CC) -G0 -c $< -o $@

build/SPRITES.BIN: assets/manifest.json assets/sprites.rgba assets/save.icn src/sprite_ids.h ../tools/pack_sprites.py
	@mkdir -p build
	python3 ../tools/pack_sprites.py assets/manifest.json assets/sprites.rgba assets/save.icn $@

build/SOUNDS.BIN: assets/SOUNDS.BIN
	@mkdir -p build
	cp $< $@

build/THEMES.BIN: assets/THEMES.BIN
	@mkdir -p build
	cp $< $@

build/MENU.BIN: assets/MENU.BIN
	@mkdir -p build
	cp $< $@

build/FONT.BIN: assets/FONT.BIN
	@mkdir -p build
	cp $< $@

build/UI.BIN: assets/UI.BIN
	@mkdir -p build
	cp $< $@

build/MUSIC.BIN: assets/MUSIC.BIN
	@mkdir -p build
	cp $< $@

build/CAMPAIGN.BIN: assets/CAMPAIGN.BIN
	@mkdir -p build
	cp $< $@

build/game.o: src/game.cpp src/egg_vehicles.cpp src/egg_vehicles.h src/game.h src/level_config.h src/campaign_types.h
	@mkdir -p build
	$(EE_CXX) $(EE_CXXFLAGS) $(EE_INCS) -c $< -o $@

build/vendor/%.o: vendor/%.cpp
	@mkdir -p $(@D)
	$(EE_CXX) $(EE_CXXFLAGS) $(EE_INCS) -c $< -o $@

HOST_BOX2D_OBJS = $(patsubst %.cpp,build/host/%.o,$(BOX2D_SRC))

build/host/vendor/%.o: vendor/%.cpp
	@mkdir -p $(@D)
	$(CXX) -O2 -std=c++11 -Ivendor/box2d/include -Ivendor/box2d/src -c $< -o $@

build/host/game.o: src/game.cpp src/egg_vehicles.cpp src/egg_vehicles.h src/game.h src/level_config.h src/campaign_types.h
	@mkdir -p $(@D)
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Ivendor/box2d/include -Isrc -c $< -o $@

build/host/game_test.o: tests/game_test.cpp src/game.h src/level_config.h src/campaign_types.h
	@mkdir -p $(@D)
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc -c $< -o $@

build/host/campaign_test.o: tests/campaign_test.cpp src/game.h src/campaign_types.h
	@mkdir -p $(@D)
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc -c $< -o $@

test: assets/CAMPAIGN.BIN assets/SOUNDS.BIN assets/manifest.json joint-test triangle-test tnt-test white-test black-test audio-test save-test build/host/game.o build/host/game_test.o $(HOST_BOX2D_OBJS) build/host/campaign_test.o
	$(CXX) build/host/game.o build/host/game_test.o $(HOST_BOX2D_OBJS) -lm -o build/game_test
	./build/game_test
	$(CXX) build/host/game.o build/host/campaign_test.o $(HOST_BOX2D_OBJS) -lm -o build/campaign_test
	./build/campaign_test
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc tests/blue_test.cpp build/host/game.o $(HOST_BOX2D_OBJS) -lm -o build/blue_test
	./build/blue_test
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc tests/yellow_test.cpp build/host/game.o $(HOST_BOX2D_OBJS) -lm -o build/yellow_test
	./build/yellow_test
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc tests/green_test.cpp build/host/game.o $(HOST_BOX2D_OBJS) -lm -o build/green_test
	./build/green_test
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc tests/globe_test.cpp build/host/game.o $(HOST_BOX2D_OBJS) -lm -o build/globe_test
	./build/globe_test
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc tests/red_target_test.cpp build/host/game.o $(HOST_BOX2D_OBJS) -lm -o build/red_target_test
	./build/red_target_test
	$(CC) -O2 -std=gnu99 -Wall -Wextra -Werror -Isrc -c src/menu.c -o build/host/menu.o
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc tests/menu_test.cpp build/host/menu.o build/host/game.o $(HOST_BOX2D_OBJS) -lm -o build/menu_test
	./build/menu_test
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc tests/popup_test.cpp build/host/game.o $(HOST_BOX2D_OBJS) -lm -o build/popup_test
	./build/popup_test
	$(CC) -O2 -std=gnu99 -Wall -Wextra -Werror -Isrc -c src/level_end.c -o build/host/level_end.o
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc tests/level_end_test.cpp build/host/level_end.o -lm -o build/level_end_test
	./build/level_end_test
	$(CC) -O2 -std=gnu99 -Wall -Wextra -Werror -DPS2BIRDS_MUSIC_TEST -Isrc -Itests -c src/music.c -o build/host/music.o
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc -Itests tests/music_test.cpp build/host/music.o -o build/music_test
	./build/music_test
	python3 tests/sprite_test.py
	python3 tests/ui_assets_test.py
	python3 tests/font_assets_test.py
	python3 tests/menu_assets_test.py
	python3 tests/themes_assets_test.py
	python3 tests/profile_test.py
	python3 tests/tools_test.py

iso: $(EE_BIN) $(DATA)
	python3 ../tools/make_iso.py $(EE_BIN) build/PS2Birds_Classic.iso $(addprefix --extra ,$(DATA))

clean:
	rm -rf build

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal_cpp

.PHONY: save-test
save-test:
	@mkdir -p build/host
	$(CC) -std=c99 -Wall -Wextra -Werror -Isrc -c src/save_codec.c -o build/host/save_codec.o
	$(CC) -std=c99 -Wall -Wextra -Werror -DPS2BIRDS_SAVE_TEST -Isrc -Itests -c src/save_mc.c -o build/host/save_mc.o
	$(CC) -c src/save_assets.S -o build/host/save_assets.o
	$(CXX) -std=c++11 -Wall -Wextra -Werror -Isrc -Itests tests/save_test.cpp build/host/save_codec.o build/host/save_mc.o build/host/save_assets.o -o build/save_test
	./build/save_test
	$(CC) -std=c99 -Wall -Wextra -Werror -Isrc tests/save_id_test.c src/save_codec.c -o build/save_id_classic
	$(CC) -std=c99 -Wall -Wextra -Werror -DPS2BIRDS_SAVE_ID=0x52494f31u -Isrc tests/save_id_test.c src/save_codec.c -o build/save_id_rio
	./build/save_id_classic write build/save_id.tmp && ./build/save_id_classic accept build/save_id.tmp && ./build/save_id_rio refuse build/save_id.tmp
	./build/save_id_rio write build/save_id.tmp && ./build/save_id_rio accept build/save_id.tmp && ./build/save_id_classic refuse build/save_id.tmp
	@rm -f build/save_id.tmp
	@echo "Save identity OK: Classic and Rio saves refuse each other."

.PHONY: audio-test
audio-test:
	@mkdir -p build/host
	$(CC) -std=c99 -Wall -Wextra -Werror -DPS2BIRDS_AUDIO_TEST -Isrc -Itests -c src/audio.c -o build/host/audio.o
	$(CXX) -std=c++11 -Wall -Wextra -Werror -Isrc -Itests tests/audio_test.cpp build/host/audio.o -o build/audio_test
	./build/audio_test
	python3 tests/audio_assets_test.py

.PHONY: black-test
black-test: $(HOST_BOX2D_OBJS)
	@mkdir -p build/host
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -DPS2BIRDS_BLAST_TEST -Isrc -Itests -Ivendor/box2d/include -c src/game.cpp -o build/host/game_blast.o
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc tests/black_test.cpp build/host/game_blast.o $(HOST_BOX2D_OBJS) -lm -o build/black_test
	./build/black_test

.PHONY: white-test
white-test: $(HOST_BOX2D_OBJS)
	@mkdir -p build/host
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -DPS2BIRDS_WHITE_TEST -Isrc -Itests -Ivendor/box2d/include -c src/game.cpp -o build/host/game_white.o
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc tests/white_test.cpp build/host/game_white.o $(HOST_BOX2D_OBJS) -lm -o build/white_test
	./build/white_test

.PHONY: tnt-test
tnt-test: $(HOST_BOX2D_OBJS)
	@mkdir -p build/host
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -DPS2BIRDS_TNT_TEST -Isrc -Itests -Ivendor/box2d/include -c src/game.cpp -o build/host/game_tnt.o
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc tests/tnt_test.cpp build/host/game_tnt.o $(HOST_BOX2D_OBJS) -lm -o build/tnt_test
	./build/tnt_test

.PHONY: triangle-test
triangle-test: $(HOST_BOX2D_OBJS)
	@mkdir -p build/host
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -DPS2BIRDS_TRIANGLE_TEST -Isrc -Itests -Ivendor/box2d/include -c src/game.cpp -o build/host/game_triangle.o
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc -Ivendor/box2d/include tests/triangle_test.cpp build/host/game_triangle.o $(HOST_BOX2D_OBJS) -lm -o build/triangle_test
	./build/triangle_test

.PHONY: joint-test
joint-test: $(HOST_BOX2D_OBJS)
	@mkdir -p build/host
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -DPS2BIRDS_JOINT_TEST -Isrc -Itests -Ivendor/box2d/include -c src/game.cpp -o build/host/game_joint.o
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc tests/joint_test.cpp build/host/game_joint.o $(HOST_BOX2D_OBJS) -lm -o build/joint_test
	./build/joint_test

assets/CAMPAIGN.BIN assets/SOUNDS.BIN assets/MUSIC.BIN assets/UI.BIN assets/FONT.BIN assets/MENU.BIN assets/THEMES.BIN assets/manifest.json assets/sprites.rgba assets/save.icn:
	@echo "Dados ausentes: gere-os a partir do APK com  python3 ../tools/gerar_iso.py CAMINHO/DO/APK --dev"; exit 1
