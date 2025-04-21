prog: main.o puzzle.o
	gcc main.o puzzle.o -o puzzle -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -lm

main.o: main.c
	gcc -c main.c -o main.o -lm

puzzle.o: puzzle.c
	gcc -c puzzle.c -o puzzle.o -lm
