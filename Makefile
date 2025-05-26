
runW:
	gcc -Wall -DWIN -I "SDL/include/" src/*.c  -o dinorush.exe -L SDL/lib -lSDL3 -lSDL3_image -lSDL3_ttf 
runL:
	gcc -Wall -DLINUX -I "SDL/include/" src/*.c  -o dinorush -L SDL/lib -lSDL3 -lSDL3_image -lSDL3_ttf
runM:
	gcc -Wall -DMAC -I "SDL/include/" src/*.c  -o dinorush -L SDL/lib -lSDL3 -lSDL3_image -lSDL3_ttf
clean:
	rm -f *.exe *.o