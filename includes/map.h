#ifndef MAP_H
# define MAP_H

# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>

/*
** .cub scene loader. Pure C (no SDL): reads the whole file once, validates
** it and returns an error code instead of exiting, so the game can retry,
** advance levels or report problems in the browser.
*/

# define MAP_MAX_FILE_SIZE	(1 << 20)
# define MAP_MAX_W			256
# define MAP_MAX_H			256
# define MAP_MAX_PATH		1024
# define MAP_MAX_ITEMS		64
# define MAP_MAX_STALKERS	8
/* Stalkers may not spawn closer than this (cells) to the player. */
# define MAP_STALKER_MIN_DIST	4

typedef enum e_map_error
{
	MAP_OK,
	MAP_ERR_NOMEM,
	MAP_ERR_EXTENSION,
	MAP_ERR_OPEN,
	MAP_ERR_NOT_FILE,
	MAP_ERR_READ,
	MAP_ERR_FILE_TOO_BIG,
	MAP_ERR_BINARY,
	MAP_ERR_EMPTY,
	MAP_ERR_UNKNOWN_ELEMENT,
	MAP_ERR_DUPLICATE_ELEMENT,
	MAP_ERR_MISSING_ELEMENT,
	MAP_ERR_TEXTURE_SYNTAX,
	MAP_ERR_TEXTURE_EXT,
	MAP_ERR_TEXTURE_MISSING,
	MAP_ERR_COLOR,
	MAP_ERR_NO_MAP,
	MAP_ERR_BLANK_LINE,
	MAP_ERR_BAD_CHAR,
	MAP_ERR_TOO_LARGE,
	MAP_ERR_NO_PLAYER,
	MAP_ERR_MULTI_PLAYER,
	MAP_ERR_OPEN_WALL,
	MAP_ERR_TOO_MANY_ITEMS,
	MAP_ERR_TOO_MANY_STALKERS,
	MAP_ERR_KEYS_NO_EXIT,
	MAP_ERR_UNREACHABLE,
	MAP_ERR_STALKER_TOO_CLOSE,
	MAP_ERR_COUNT
}	t_map_error;

typedef enum e_side
{
	SIDE_NO,
	SIDE_SO,
	SIDE_WE,
	SIDE_EA,
	SIDE_COUNT
}	t_side;

/*
** Horror-maze extensions (old maps stay valid):
**   X  exit door (a wall that opens once every key is held)
**   K  key        B  flashlight battery        M  stalker spawn
*/
typedef struct s_spawn
{
	int		x;
	int		y;
	char	kind;
}	t_spawn;

typedef struct s_map
{
	/* Texture paths, resolved so that they exist (see map_header.c). */
	char		*tex[SIDE_COUNT];
	uint32_t	floor_rgb;
	uint32_t	ceil_rgb;
	int			w;
	int			h;
	/* w * h cells: ' ' void, '1' wall, 'X' exit door, '0' floor (spawn,
	** item and stalker letters are cleared to '0' and listed below). */
	char		*cells;
	int			spawn_x;
	int			spawn_y;
	char		spawn_dir;
	t_spawn		items[MAP_MAX_ITEMS];
	int			item_count;
	int			key_count;
	t_spawn		stalkers[MAP_MAX_STALKERS];
	int			stalker_count;
	int			exit_count;
	/* File line (0-based) of the grid's first row, for error messages. */
	int			grid_line;
	/* Where the first error was found (1-based; 0 = not applicable). */
	int			err_line;
	int			err_col;
	char		err_char;
}	t_map;

t_map_error	map_load(const char *path, t_map *out);
void		map_free(t_map *m);
const char	*map_error_str(t_map_error err);
/* "line 12, column 4: invalid character '#' in map" */
void		map_describe_error(const t_map *m, t_map_error err, char *buf,
				size_t size);

/* Cell at (x, y); ' ' (void) outside the grid. */
char		map_cell(const t_map *m, int x, int y);
/* Walls, doors, void and everything outside the grid block movement and
** rays. */
bool		map_is_solid(const t_map *m, int x, int y);

#endif
