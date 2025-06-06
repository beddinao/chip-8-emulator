CC = cc
SRC = $(wildcard src/*.c)
HR = $(wildcard include/*.h)
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))
CFLAGS = -Iinclude 
LDFLAGS = -Llib -Wl,-rpath,lib -Wl,-lSDL3
SDL_PATH = ./assets/SDL3
NAME = chip8

all: dirs_set sdl $(NAME)

sdl:
	@cmake -B $(SDL_PATH)/build $(SDL_PATH) -DCMAKE_CXX_COMPILER="g++"
	@cd $(SDL_PATH)/build && make -j10
	@cp -r $(SDL_PATH)/include/SDL3 include
	@cp -r $(SDL_PATH)/build/libSDL3* lib

dirs_set:
	mkdir -p lib

dirs_rem:
	rm -rf lib

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(LDFLAGS)

build/%.o: src/%.c $(HR)
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf build

fclean: dirs_rem clean
	rm -rf include/SDL3
	rm -rf $(SDL_PATH)/build
	rm -rf $(NAME)

re: fclean all

.PHONY: clean
