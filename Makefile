sources   := $(wildcard src/*.c)
objects   := $(sources:.c=.o)
platform  :=
executable :=

CFLAGS    := -Wall -I"SDL/include/" -DDESIRED_FPS=120 -DALLOW_FPS_OPTIMIZING
LDFLAGS   := -L SDL/lib -lSDL3 -lSDL3_image -lSDL3_ttf

buildW: platform := -DWIN
buildW: executable := dinorush.exe
buildW: build

buildL: platform := -DLINUX
buildL: executable := dinorush
buildL: build

buildM: platform := -DMAC
buildM: executable := dinorush
buildM: build

buildA:
	gcc $(CFLAGS) -DWIN $(sources) -o dinorush.exe $(LDFLAGS)	

build: $(objects)
	gcc $(CFLAGS) $(platform) $(objects) -o $(executable) $(LDFLAGS)

# compile .c to .o general rule (for each .o file)
src/%.o: src/%.c
	gcc -c $(CFLAGS) $(platform) $< -o $@

clean:
	rm -f $(objects) dinorush dinorush.exe