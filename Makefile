NAME 	= cub3d
CC      = cc
INCLUDES_DIR = includes
CFLAGS  = -Wall -Werror -Wextra -ggdb -I$(INCLUDES_DIR)
MLX_FLAGS = -Lmlx_linux -lmlx_Linux -L/usr/lib -Imlx_linux -lXext -lX11 -lm -lz

SRC = main.c $(PARSING_SRC) $(UTILS_SRC) $(RENDERING_SRC)

PARSING_SRC = parsing/parse_file.c \
			  parsing/parse_file_utils.c \
			  parsing/parse_textures_colors.c \
			  parsing/parse_textures_colors_utils.c \
			  parsing/parse_textures_colors_utils2.c \
			  parsing/parse_map.c \
			  parsing/parse_map_utils.c \
			  parsing/parse_map_utils2.c \

RENDERING_SRC = rendering/init.c rendering/window.c

UTILS_SRC = utils/arrays.c \
			utils/extras.c \

OBJ_DIR = objects

OBJ = $(SRC:%.c=$(OBJ_DIR)/%.o)

LIBFT = libft/libft.a

all: $(NAME)

$(NAME): $(LIBFT) $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(MLX_FLAGS) $(LIBFT) -o $(NAME)

$(LIBFT):
	make -C libft

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	make -C libft clean

fclean: clean
	rm -f $(NAME)
	make -C libft fclean

re: fclean all

.PHONY: all clean fclean re