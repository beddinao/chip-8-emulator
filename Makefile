CC = cc
SRC = $(wildcard src/*.c)
HR = $(wildcard include/*.h)
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))
CFLAGS = -I include -I ./MLX42/include/MLX42 -Werror -Wextra -Wall
#CFLAGS += -fsanitize=address
LDFLAGS = ./MLX42/build/libmlx42.a
NAME = chip8
UNAME = $(shell uname)

ifeq ($(UNAME), Linux)
	LDFLAGS += -lglfw -ldl -pthread -lm
endif
ifeq ($(UNAME), Darwin)
	LDFLAGS += -lglfw -L $(shell brew --prefix glfw)/lib -framework Cocoa -framework IOKit
endif

all: mlx $(NAME)

mlx:
	@cmake -B ./MLX42/build ./MLX42
	@cmake --build ./MLX42/build -j16

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ)

build/%.o: src/%.c $(HR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

clean:
	rm -rf build
	rm -rf MLX42/build

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean
