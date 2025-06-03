sources   := $(wildcard src/*.c)
objects   := $(sources:.c=.o)
platform  :=
executable :=
LIB_PATH  =

CFLAGS    := -Wall -I"SDL/include/" -DDESIRED_FPS=120 -DALLOW_FPS_OPTIMIZING
LDFLAGS   = -L$(LIB_PATH) -lSDL3 -lSDL3_image -lSDL3_ttf

buildW: platform := -DWIN
buildW: executable := dinorush.exe
buildW: LIB_PATH := SDL/lib/windows
buildW: build

buildL: platform := -DLINUX
buildL: executable := dinorush
buildL: LIB_PATH := SDL/lib/linux
buildL: build

buildM: platform := -DMAC
buildM: executable := dinorush
buildM: LIB_PATH := SDL/lib/macos
buildM: build

buildA:
	gcc $(CFLAGS) -DWIN $(sources) -o dinorush.exe -LSDL/lib/windows -lSDL3 -lSDL3_image -lSDL3_ttf

build: $(objects)
	gcc $(CFLAGS) $(platform) $(objects) -o $(executable) $(LDFLAGS)

src/%.o: src/%.c
	gcc -c $(CFLAGS) $(platform) $< -o $@

clean:
	rm -f $(objects) dinorush dinorush.exe