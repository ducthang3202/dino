sources := $(wildcard src/*.c)
objects := $(sources:.c=.o)

# Default to macOS build
CFLAGS := -Wall -I"SDL/include" -DDESIRED_FPS=120 -DALLOW_FPS_OPTIMIZING
LDFLAGS := -L"SDL/lib/macos" -lSDL3 -lSDL3_image -lSDL3_ttf

# Platform-specific builds
buildW: CFLAGS += -DWIN
buildW: LDFLAGS = -L"SDL/lib/windows" -lSDL3 -lSDL3_image -lSDL3_ttf
buildW: dinorush.exe

buildL: CFLAGS += -DLINUX  
buildL: LDFLAGS = -L"SDL/lib/linux" -lSDL3 -lSDL3_image -lSDL3_ttf
buildL: dinorush

buildM: CFLAGS += -DMAC
LDFLAGS = -L"SDL/lib/macos" -lSDL3 -lSDL3_image -lSDL3_ttf -Wl,-rpath,@loader_path/SDL/lib/macos
buildM: dinorush

# Build targets
dinorush.exe: $(objects)
	gcc $(objects) -o $@ $(LDFLAGS)

dinorush: $(objects)
	gcc $(objects) -o $@ $(LDFLAGS)

# Object files
src/%.o: src/%.c
	gcc -c $(CFLAGS) $< -o $@

# Alternative direct build (for testing)
buildA:
	gcc $(CFLAGS) -DWIN $(sources) -o dinorush.exe -L"SDL/lib/windows" -lSDL3 -lSDL3_image -lSDL3_ttf

clean:
	rm -f $(objects) dinorush dinorush.exe

# Debug target to show variables
debug:
	@echo "CFLAGS: $(CFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "sources: $(sources)"
	@echo "objects: $(objects)"

.PHONY: buildW buildL buildM buildA clean debug