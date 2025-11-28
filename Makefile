NAME     = cub3d
CC      = cc
INCLUDES_DIR = includes
CFLAGS  = -Wall -Werror -Wextra -ggdb -I$(INCLUDES_DIR)

SRC = main.c $(PARSING_SRC) $(UTILS_SRC) $(RENDERING_SRC)

PARSING_SRC = parsing/parse_file.c \
              parsing/parse_file_utils.c \
              parsing/parse_textures_colors.c \
              parsing/parse_textures_colors_utils.c \
              parsing/parse_textures_colors_utils2.c \
              parsing/parse_map.c \
              parsing/parse_map_utils.c \
              parsing/parse_map_utils2.c \

RENDERING_SRC = rendering/init.c rendering/window.c \
                rendering/drawing.c rendering/drawing_utils.c \
                rendering/dda_algo.c rendering/player_moves.c \
                rendering/moves_utils.c rendering/clean_up.c \
                rendering/raycasting.c rendering/raycasting_utils.c \
                rendering/projection.c

UTILS_SRC = utils/arrays.c \
            utils/extras.c \

OBJ_DIR = objects

OBJ = $(SRC:%.c=$(OBJ_DIR)/%.o)

MLX = minilibx-linux/libmlx.a

LIBFT = libft/libft.a

all: $(NAME)

$(NAME): $(LIBFT) $(MLX) $(OBJ)
	$(CC) $(CFLAGS) $(OBJ)  $(LIBFT) $(MLX) -lX11 -lXext -lm -o $(NAME)

$(LIBFT):
	make -C libft

$(MLX):
	make -C minilibx-linux

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	make -C libft clean
	make -C minilibx-linux clean

fclean: clean
	rm -f $(NAME)
	make -C libft fclean

re: fclean all

.PHONY: all clean fclean re