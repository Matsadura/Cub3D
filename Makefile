NAME	= cub3d
BNAME	= cub3d_bonus
CC      = cc
INCLUDES_DIR = includes
CFLAGS  = -Wall -Werror -Wextra -ggdb -I$(INCLUDES_DIR)

SRC = Mandatory/main.c $(PARSING_SRC) $(UTILS_SRC) $(RENDERING_SRC)
BSRC = Bonus/main_bonus.c $(PARSING_SRC_BONUS) $(UTILS_SRC_BONUS) $(RENDERING_SRC_BONUS)

PARSING_SRC = Mandatory/parsing/parse_file.c \
			  Mandatory/parsing/parse_file_utils.c \
			  Mandatory/parsing/parse_textures_colors.c \
			  Mandatory/parsing/parse_textures_colors_utils.c \
			  Mandatory/parsing/parse_textures_colors_utils2.c \
			  Mandatory/parsing/parse_map.c \
			  Mandatory/parsing/parse_map_utils.c \
			  Mandatory/parsing/parse_map_utils2.c \

RENDERING_SRC = Mandatory/rendering/init.c Mandatory/rendering/window.c \
				Mandatory/rendering/drawing.c Mandatory/rendering/drawing_utils.c \
				Mandatory/rendering/player_moves.c \
				Mandatory/rendering/moves_utils.c Mandatory/rendering/clean_up.c \
				Mandatory/rendering/raycasting.c \
				Mandatory/rendering/raycasting_utils.c \
				Mandatory/rendering/raycasting_utils2.c \
				Mandatory/rendering/projection.c \
				Mandatory/rendering/projection_utils.c \
				Mandatory/rendering/projection_utils2.c \
				Mandatory/rendering/textures.c

UTILS_SRC = Mandatory/utils/arrays.c \
			Mandatory/utils/extras.c \
			Mandatory/utils/style.c \

PARSING_SRC_BONUS = Bonus/parsing/parse_file_bonus.c \
				Bonus/parsing/parse_file_utils_bonus.c \
				Bonus/parsing/parse_textures_colors_bonus.c \
				Bonus/parsing/parse_textures_colors_utils_bonus.c \
				Bonus/parsing/parse_textures_colors_utils2_bonus.c \
				Bonus/parsing/parse_map_bonus.c \
				Bonus/parsing/parse_map_utils_bonus.c \
				Bonus/parsing/parse_map_utils2_bonus.c

RENDERING_SRC_BONUS = Bonus/rendering/init_bonus.c \
				Bonus/rendering/window_bonus.c \
				Bonus/rendering/drawing_bonus.c \
				Bonus/rendering/drawing_utils_bonus.c \
				Bonus/rendering/dda_algo_bonus.c \
				Bonus/rendering/player_moves_bonus.c \
				Bonus/rendering/moves_utils_bonus.c \
				Bonus/rendering/clean_up_bonus.c \
				Bonus/rendering/raycasting_bonus.c \
				Bonus/rendering/raycasting_utils_bonus.c \
				Bonus/rendering/raycasting_utils2_bonus.c \
				Bonus/rendering/projection_bonus.c \
				Bonus/rendering/projection_utils_bonus.c \
				Bonus/rendering/projection_utils2_bonus.c \
				Bonus/rendering/textures_bonus.c \
				Bonus/rendering/minimap_bonus.c

UTILS_SRC_BONUS = Bonus/utils/arrays_bonus.c \
			Bonus/utils/extras_bonus.c \
			Bonus/utils/style_bonus.c \

OBJ_DIR = objects

OBJ = $(SRC:%.c=$(OBJ_DIR)/%.o)
BOBJ = $(BSRC:%.c=$(OBJ_DIR)/%.o)

MLX = minilibx-linux/libmlx.a

LIBFT = libft/libft.a

all: $(NAME)

$(NAME): $(LIBFT) $(MLX) $(OBJ)
	$(CC) $(CFLAGS) $(OBJ)  $(LIBFT) $(MLX) -lX11 -lXext -lm -o $(NAME)

bonus: $(BNAME)

$(BNAME): $(LIBFT) $(MLX) $(BOBJ)
	$(CC) $(CFLAGS) $(BOBJ)  $(LIBFT) $(MLX) -lX11 -lXext -lm -o $(BNAME)

$(LIBFT):
	make -C libft

$(MLX):
	make -C minilibx-linux

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BOBJ)
	make -C libft clean
	make -C minilibx-linux clean

fclean: clean
	rm -f $(NAME) $(BNAME)
	make -C libft fclean

re: fclean all

.PHONY: all clean fclean re