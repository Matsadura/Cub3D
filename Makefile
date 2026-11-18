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
               src/web_glue.c \
               src/core/game.c \
               src/core/player.c \
               src/core/level.c \
               src/core/survival.c \
               src/core/stalker.c \
               src/core/sense.c \
               src/core/ray.c \
               src/map/map_load.c \
               src/map/map_read.c \
               src/map/map_header.c \
               src/map/map_grid.c \
               src/map/map_reach.c \
               src/map/map_util.c \
               src/platform/sdl_platform.c \
               src/platform/sdl_video.c \
               src/platform/sdl_input.c \
               src/platform/sdl_audio.c \
               src/ui/sound.c \
               src/render/renderer.c \
               src/render/raycast.c \
               src/render/light.c \
               src/render/sprites.c \
               src/render/effects.c \
               src/render/minimap.c \
               src/render/hud.c \
               src/render/screens.c \
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

# --- WebAssembly (requires: source ~/emsdk/emsdk_env.sh) ------------------
EMCC        ?= emcc
WEB_DIR     := web
WEB_OUT     := $(WEB_DIR)/index.html
WEB_SHELL   := $(WEB_DIR)/shell.html
WEB_ASSETS  := assets/textures assets/sprites assets/sounds assets/maps
WASM_FLAGS  := -Wall -Wextra -Werror -std=c11 -D_DEFAULT_SOURCE -Iincludes -O2 \
               -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS='["png"]' \
               -sALLOW_MEMORY_GROWTH=1 -sENVIRONMENT=web \
               -sEXPORTED_FUNCTIONS='["_main","_web_action","_web_set_held","_web_look","_web_is_muted","_web_player_angle","_web_game_state"]' \
               -sEXPORTED_RUNTIME_METHODS='["ccall"]' \
               $(foreach d,$(WEB_ASSETS),--preload-file $(d)) \
               --shell-file $(WEB_SHELL)

wasm: $(WEB_OUT)

$(WEB_OUT): $(SRC) $(wildcard includes/*.h src/*/*.h) $(WEB_SHELL) \
            $(shell find $(WEB_ASSETS) -type f 2>/dev/null)
	$(EMCC) $(WASM_FLAGS) $(SRC) -lm -o $@

test: $(NAME)
	./tests/run_tests.sh

clean:
	rm -rf build

fclean: clean
	rm -f $(NAME) $(WEB_DIR)/index.html $(WEB_DIR)/index.js \
		$(WEB_DIR)/index.wasm $(WEB_DIR)/index.data

re: fclean
	$(MAKE) all

-include $(DEP)

.PHONY: all bonus wasm test clean fclean re
