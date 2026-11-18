NAME        := cub3D

CC          := cc
CFLAGS      := -Wall -Wextra -Werror -std=c11 -D_DEFAULT_SOURCE -Iincludes
ifeq ($(DEBUG),1)
CFLAGS      += -g3 -O0
else
CFLAGS      += -O2
endif

# SDL2 + SDL2_image discovery (override on the command line if needed).
SDL_CFLAGS  ?= $(shell pkg-config --cflags sdl2 SDL2_image 2>/dev/null || sdl2-config --cflags 2>/dev/null)
SDL_LIBS    ?= $(shell pkg-config --libs sdl2 SDL2_image 2>/dev/null || echo "$$(sdl2-config --libs 2>/dev/null) -lSDL2_image")

SRC         := src/main.c \
               src/core/game.c \
               src/core/player.c \
               src/map/map_load.c \
               src/map/map_read.c \
               src/map/map_header.c \
               src/map/map_grid.c \
               src/map/map_util.c \
               src/platform/sdl_platform.c \
               src/platform/sdl_video.c \
               src/platform/sdl_input.c \
               src/render/renderer.c \
               src/render/raycast.c \
               src/render/minimap.c \
               src/render/hud.c \
               src/render/font.c

BUILD_DIR   := build/native
OBJ         := $(SRC:%.c=$(BUILD_DIR)/%.o)
DEP         := $(OBJ:.o=.d)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(SDL_LIBS) -lm -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -MMD -MP -c $< -o $@

# The original subject's "bonus" rule: everything is in the single build.
bonus: all

test: $(NAME)
	./tests/run_tests.sh

clean:
	rm -rf build

fclean: clean
	rm -f $(NAME)

re: fclean
	$(MAKE) all

-include $(DEP)

.PHONY: all bonus test clean fclean re
