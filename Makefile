sources := $(wildcard src/*.c)
objects := $(sources:.c=.o)

# Remove main.o from test objects
test_objects := $(filter-out src/main.o, $(objects))

# Default to macOS build
CFLAGS := -Wall -I"SDL/include" -I"header" -DDESIRED_FPS=120 -DALLOW_FPS_OPTIMIZING
LDFLAGS := -L"SDL/lib/macos" -lSDL3 -lSDL3_image -lSDL3_ttf

# Platform-specific builds
buildW: CFLAGS += -DWIN
buildW: LDFLAGS = -L"SDL/lib/windows" -lSDL3 -lSDL3_image -lSDL3_ttf
buildW: dinorush.exe

buildL: CFLAGS += -DLINUX
buildL: LDFLAGS = -L"SDL/lib/linux" -lSDL3 -lSDL3_image -lSDL3_ttf
buildL: dinorush

buildM: CFLAGS += -DMAC
buildM: LDFLAGS = -L"SDL/lib/macos" -lSDL3 -lSDL3_image -lSDL3_ttf -Wl,-rpath,@loader_path/SDL/lib/macos
buildM: dinorush

# Build targets
dinorush.exe: $(objects)
	gcc $(objects) -o $@ $(LDFLAGS)

dinorush: $(objects)
	gcc $(objects) -o $@ $(LDFLAGS)

# Object files
src/%.o: src/%.c
	gcc -c $(CFLAGS) $< -o $@

# Simple test without GoogleTest
build/test_game_simple: tests/test_game_simple.cpp $(test_objects)
	@mkdir -p build
	g++ -std=c++17 tests/test_game_simple.cpp $(test_objects) \
		$(CFLAGS) $(LDFLAGS) \
		-Wl,-rpath,@loader_path/../SDL/lib/macos \
		-lm -o $@

# Simple test for Windows
build/test_game_simple.exe: tests/test_game_simple.cpp $(test_objects)
	@if not exist build mkdir build
	g++ -std=c++17 tests/test_game_simple.cpp $(test_objects) \
		$(CFLAGS) $(LDFLAGS) \
		-lm -o $@

# Run simple tests
# Run simple tests
testM: buildM build/test_game_simple
	@echo "Running tests..."
	@cd build && ./test_game_simple

testW: buildW build/test_game_simple.exe
	@echo "Running tests..."
	@cd build && test_game_simple.exe

# You could also add Linux tests for completeness
testL: buildL build/test_game_simple
	@echo "Running tests..."
	@cd build && ./test_game_simple

# Alternative direct build (for testing)
buildA:
	gcc $(CFLAGS) -DWIN $(sources) -o dinorush.exe -L"SDL/lib/windows" -lSDL3 -lSDL3_image -lSDL3_ttf

# Clean everything including tests
clean:
	rm -f $(objects) dinorush dinorush.exe build/test_game_simple build/test_game_simple.exe
	rm -rf build

# Build everything (game + tests)
all: buildM test

# Debug target to show variables
debug:
	@echo "CFLAGS: $(CFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "sources: $(sources)"
	@echo "objects: $(objects)"
	@echo "test_objects: $(test_objects)"

.PHONY: buildW buildL buildM buildA clean debug test testW all