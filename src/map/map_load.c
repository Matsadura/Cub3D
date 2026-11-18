#include "map_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char	*g_messages[MAP_ERR_COUNT] = {
	"no error",
	"out of memory",
	"file name must end in .cub",
	"cannot open file",
	"not a regular file",
	"cannot read file",
	"file is too large (max 1 MiB)",
	"file contains binary data",
	"file is empty",
	"unknown element (expected NO, SO, WE, EA, F or C)",
	"duplicate element",
	"missing element (NO, SO, WE, EA, F and C must come before the map)",
	"texture needs exactly one path",
	"texture must be a .png or .xpm file",
	"texture file not found",
	"color must be R,G,B with values 0-255",
	"map is missing",
	"empty line inside the map",
	"invalid character in map",
	"map is too large (max 256x256)",
	"no player start (N, S, E or W)",
	"more than one player start",
	"map is not enclosed by walls",
	"too many items (max 64 keys and batteries)",
	"too many stalkers (max 8)",
	"keys (K) need an exit door (X)",
	"cannot be reached from the player start",
	"stalker (M) starts too close to the player",
};

const char	*map_error_str(t_map_error err)
{
	if (err < 0 || err >= MAP_ERR_COUNT)
		return ("unknown error");
	return (g_messages[err]);
}

void	map_describe_error(const t_map *m, t_map_error err, char *buf,
		size_t size)
{
	char	where[48];
	char	what[16];

	where[0] = '\0';
	what[0] = '\0';
	if (m->err_line > 0 && m->err_col > 0)
		snprintf(where, sizeof(where), "line %d, column %d: ",
			m->err_line, m->err_col);
	else if (m->err_line > 0)
		snprintf(where, sizeof(where), "line %d: ", m->err_line);
	if (err == MAP_ERR_BAD_CHAR && m->err_char == '\t')
		snprintf(what, sizeof(what), " (tab)");
	else if (err == MAP_ERR_BAD_CHAR && m->err_char >= 32 && m->err_char < 127)
		snprintf(what, sizeof(what), " '%c'", m->err_char);
	snprintf(buf, size, "%s%s%s", where, map_error_str(err), what);
}

void	map_free(t_map *m)
{
	int	i;

	i = 0;
	while (i < SIDE_COUNT)
		free(m->tex[i++]);
	free(m->cells);
	memset(m, 0, sizeof(*m));
}

static void	release_keep_error(t_map *m)
{
	t_map	keep;

	memset(&keep, 0, sizeof(keep));
	keep.err_line = m->err_line;
	keep.err_col = m->err_col;
	keep.err_char = m->err_char;
	map_free(m);
	*m = keep;
}

/* On failure *out keeps only the error position (map_free is still safe). */
t_map_error	map_load(const char *path, t_map *out)
{
	t_lines		lines;
	t_map_error	err;
	int			next;

	memset(out, 0, sizeof(*out));
	if (path == NULL)
		return (MAP_ERR_OPEN);
	err = map_read_lines(path, &lines);
	if (err != MAP_OK)
		return (err);
	next = 0;
	err = map_parse_header(path, &lines, out, &next);
	if (err == MAP_OK)
		err = map_parse_grid(&lines, next, out);
	lines_free(&lines);
	if (err != MAP_OK)
		release_keep_error(out);
	return (err);
}
