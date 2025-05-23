
run:
	gcc -I "SDL/include/" src/main.c src/DinoRush.c src/window.c -o dinodash.exe -L SDL/lib -lSDL3 
clean:
	rm -f *.exe *.o