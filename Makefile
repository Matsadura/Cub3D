NAME 	= cub3d
CC      = cc
INCLUDES_DIR = includes
CFLAGS  = -Wall -Werror -Wextra -ggdb -I$(INCLUDES_DIR)

SRC = main.c

PARSING_SRC = parsing/parse_file.c


OBJ_DIR = objects

OBJ = $(SRC:%.c=$(OBJ_DIR)/%.o)

LIBFT = libft/libft.a

all: $(NAME)

$(NAME): $(LIBFT) $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(PARSING_SRC) $(LIBFT) -o $(NAME)

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