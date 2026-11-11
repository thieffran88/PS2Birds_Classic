EE_BIN = build/PS2Birds_Classic.elf
BOX2D_SRC = $(shell find vendor/box2d/src -name "*.cpp")
BOX2D_OBJS = $(patsubst %.cpp,build/%.o,$(BOX2D_SRC))
EE_OBJS = build/save_codec.o build/save_mc.o build/save_assets.o build/main.o build/game.o build/sprites.o build/sprite_pixels.o $(BOX2D_OBJS)
EE_INCS = -Iassets -Ivendor/box2d/include -Ivendor/box2d/src -Isrc -I$(GSKIT)/include
EE_CFLAGS = -std=gnu99 -Wextra
EE_CXXFLAGS = -std=gnu++11 -Wextra -fno-exceptions -fno-rtti
EE_LDFLAGS = -L$(GSKIT)/lib
EE_LIBS = -lgskit -ldmakit -lpad -lmc -lm

.PHONY: all clean test iso
all: $(EE_BIN)

build/%.o: src/%.c src/game.h src/level_config.h src/campaign_types.h
	@mkdir -p build
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

build/main.o build/sprites.o: assets/sprite_data.h src/sprites.h

build/save_mc.o: src/save_mc.h src/save_codec.h assets/save_assets.h

build/save_assets.o: src/save_assets.S assets/save.icn assets/save_icon.sys
	@mkdir -p build
	$(EE_CC) -G0 -c $< -o $@

build/sprite_pixels.o: src/sprites.S assets/sprites.rgba
	@mkdir -p build
	$(EE_CC) -G0 -c $< -o $@

build/game.o: src/game.cpp src/game.h src/level_data.h src/level_config.h src/campaign_data.h src/campaign_types.h
	@mkdir -p build
	$(EE_CXX) $(EE_CXXFLAGS) $(EE_INCS) -c $< -o $@

build/vendor/%.o: vendor/%.cpp
	@mkdir -p $(@D)
	$(EE_CXX) $(EE_CXXFLAGS) $(EE_INCS) -c $< -o $@

HOST_BOX2D_OBJS = $(patsubst %.cpp,build/host/%.o,$(BOX2D_SRC))

build/host/vendor/%.o: vendor/%.cpp
	@mkdir -p $(@D)
	$(CXX) -O2 -std=c++11 -Ivendor/box2d/include -Ivendor/box2d/src -c $< -o $@

build/host/game.o: src/game.cpp src/game.h src/level_data.h src/level_config.h src/campaign_data.h src/campaign_types.h
	@mkdir -p $(@D)
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Ivendor/box2d/include -Isrc -c $< -o $@

build/host/game_test.o: tests/game_test.cpp src/game.h src/level_config.h src/campaign_types.h
	@mkdir -p $(@D)
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc -c $< -o $@

build/host/campaign_test.o: tests/campaign_test.cpp src/game.h src/campaign_types.h
	@mkdir -p $(@D)
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc -c $< -o $@

test: save-test build/host/game.o build/host/game_test.o $(HOST_BOX2D_OBJS) build/host/campaign_test.o
	$(CXX) build/host/game.o build/host/game_test.o $(HOST_BOX2D_OBJS) -lm -o build/game_test
	./build/game_test
	$(CXX) build/host/game.o build/host/campaign_test.o $(HOST_BOX2D_OBJS) -lm -o build/campaign_test
	./build/campaign_test
	$(CXX) -O2 -std=c++11 -Wall -Wextra -Werror -Isrc tests/blue_test.cpp build/host/game.o $(HOST_BOX2D_OBJS) -lm -o build/blue_test
	./build/blue_test
	python3 tests/sprite_test.py
	python3 tests/profile_test.py

iso: $(EE_BIN)
	python3 ../tools/make_iso.py $(EE_BIN) build/PS2Birds_Classic.iso

clean:
	rm -rf build

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal_cpp

.PHONY: save-test
save-test:
	@mkdir -p build/host
	$(CC) -std=c99 -Wall -Wextra -Werror -Isrc -c src/save_codec.c -o build/host/save_codec.o
	$(CC) -std=c99 -Wall -Wextra -Werror -DPS2BIRDS_SAVE_TEST -Isrc -Iassets -Itests -c src/save_mc.c -o build/host/save_mc.o
	$(CC) -c src/save_assets.S -o build/host/save_assets.o
	$(CXX) -std=c++11 -Wall -Wextra -Werror -Isrc -Itests tests/save_test.cpp build/host/save_codec.o build/host/save_mc.o build/host/save_assets.o -o build/save_test
	./build/save_test
