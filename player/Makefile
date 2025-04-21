CC = gcc
CFLAGS = -Wall -Wextra -g -Wno-switch `sdl-config --cflags` `pkg-config --cflags SDL_image SDL_ttf SDL_mixer`
LDFLAGS = `sdl-config --libs` `pkg-config --libs SDL_image SDL_ttf SDL_mixer`
SRC = main.c player.c
OBJ = $(SRC:.c=.o)
TARGET = game

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Install dependencies (Ubuntu)
install-deps:
	sudo apt-get update
	sudo apt-get install -y libsdl1.2-dev libsdl-image1.2-dev libsdl-ttf2.0-dev libsdl-mixer1.2-dev pkg-config
