prog:func.o main.o
	gcc func.o main.o -o prog -lSDL -lSDL_ttf -lSDL_image -lSDL_mixer -g
main.o:main.c
	gcc -c main.c -g
func.o:func.c
	gcc -c func.c -g

