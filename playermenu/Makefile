prog: main.o player.o
	gcc main.o player.o -o player -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -g

main.o: main.c
	gcc -c main.c -o main.o -g

player.o: player.c
	gcc -c player.c -o player.o -g
