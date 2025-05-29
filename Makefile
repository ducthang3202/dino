
runW:
	gcc -Wall -DWIN -DDESIRED_FPS=10 -DALLOW_FPS_OPTIMIZING -I "SDL/include/" src/*.c  -o dinorush.exe -L SDL/lib -lSDL3 -lSDL3_image -lSDL3_ttf 
runL:
	gcc -Wall -DLINUX -DDESIRED_FPS=120 -DALLOW_FPS_OPTIMIZING -I "SDL/include/" src/*.c  -o dinorush -L SDL/lib -lSDL3 -lSDL3_image -lSDL3_ttf
runM:
	gcc -Wall -DMAC -DDESIRED_FPS=120 -DALLOW_FPS_OPTIMIZING -I "SDL/include/" src/*.c  -o dinorush -L SDL/lib -lSDL3 -lSDL3_image -lSDL3_ttf
clean:
	rm -f *.exe *.o